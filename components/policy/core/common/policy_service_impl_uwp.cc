// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/policy/core/common/policy_service_impl.h"

#include <stddef.h>

#include <algorithm>
#include <utility>

#include "base/containers/contains.h"
#include "base/containers/flat_set.h"
#include "base/functional/bind.h"
#include "base/location.h"
#include "base/memory/ptr_util.h"
#include "base/observer_list.h"
#include "base/ranges/algorithm.h"
#include "base/strings/utf_string_conversions.h"
#include "base/task/sequenced_task_runner.h"
#include "base/values.h"
#include "build/build_config.h"
#include "build/chromeos_buildflags.h"
#include "components/policy/core/common/policy_bundle.h"
#include "components/policy/core/common/policy_logger.h"
#include "components/policy/core/common/policy_map.h"
#include "components/policy/core/common/policy_merger.h"
#include "components/policy/core/common/policy_types.h"
#include "components/policy/core/common/proxy_settings_constants.h"
#include "components/policy/core/common/values_util.h"
#include "components/policy/policy_constants.h"
#include "components/strings/grit/components_strings.h"
#include "extensions/buildflags/buildflags.h"

#if BUILDFLAG(IS_ANDROID)
#include "components/policy/core/common/android/policy_service_android.h"
#endif

#if BUILDFLAG(IS_CHROMEOS)
#include "components/policy/core/common/default_chrome_apps_migrator.h"
#endif

namespace policy {

PolicyServiceImpl::PolicyServiceImpl(Providers providers, Migrators migrators)
    : PolicyServiceImpl(std::move(providers),
                        std::move(migrators),
                        /*initialization_throttled=*/false) {}

PolicyServiceImpl::PolicyServiceImpl(Providers providers,
                                     Migrators migrators,
                                     bool initialization_throttled)
    : providers_(std::move(providers)),
      migrators_(std::move(migrators)),
      initialization_throttled_(initialization_throttled) {
  for (int domain = 0; domain < POLICY_DOMAIN_SIZE; ++domain)
    policy_domain_status_[domain] = PolicyDomainStatus::kUninitialized;

  for (auto* provider : providers_)
    provider->AddObserver(this);
  // There are no observers yet, but calls to GetPolicies() should already get
  // the processed policy values.
  MergeAndTriggerUpdates();
}

// static
std::unique_ptr<PolicyServiceImpl>
PolicyServiceImpl::CreateWithThrottledInitialization(Providers providers,
                                                     Migrators migrators) {
  return base::WrapUnique(
      new PolicyServiceImpl(std::move(providers), std::move(migrators),
                            /*initialization_throttled=*/true));
}

PolicyServiceImpl::~PolicyServiceImpl() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  for (auto* provider : providers_)
    provider->RemoveObserver(this);
}

void PolicyServiceImpl::AddObserver(PolicyDomain domain,
                                    PolicyService::Observer* observer) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  observers_[domain].AddObserver(observer);
}

void PolicyServiceImpl::RemoveObserver(PolicyDomain domain,
                                       PolicyService::Observer* observer) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  auto it = observers_.find(domain);
  if (it == observers_.end())
    return;
  it->second.RemoveObserver(observer);
  if (it->second.empty()) {
    observers_.erase(it);
  }
}

void PolicyServiceImpl::AddProviderUpdateObserver(
    ProviderUpdateObserver* observer) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  provider_update_observers_.AddObserver(observer);
}

void PolicyServiceImpl::RemoveProviderUpdateObserver(
    ProviderUpdateObserver* observer) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  provider_update_observers_.RemoveObserver(observer);
}

bool PolicyServiceImpl::HasProvider(
    ConfigurationPolicyProvider* provider) const {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  return base::Contains(providers_, provider);
}

const PolicyMap& PolicyServiceImpl::GetPolicies(
    const PolicyNamespace& ns) const {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  return policy_bundle_.Get(ns);
}

bool PolicyServiceImpl::IsInitializationComplete(PolicyDomain domain) const {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  DCHECK(domain >= 0 && domain < POLICY_DOMAIN_SIZE);
  return !initialization_throttled_ &&
         policy_domain_status_[domain] != PolicyDomainStatus::kUninitialized;
}

bool PolicyServiceImpl::IsFirstPolicyLoadComplete(PolicyDomain domain) const {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  DCHECK(domain >= 0 && domain < POLICY_DOMAIN_SIZE);
  return !initialization_throttled_ &&
         policy_domain_status_[domain] == PolicyDomainStatus::kPolicyReady;
}

void PolicyServiceImpl::RefreshPolicies(base::OnceClosure callback) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);

  VLOG_POLICY(2, POLICY_PROCESSING) << "Policy refresh starting";

  if (!callback.is_null())
    refresh_callbacks_.push_back(std::move(callback));

  if (providers_.empty()) {
    // Refresh is immediately complete if there are no providers. See the note
    // on OnUpdatePolicy() about why this is a posted task.
    update_task_ptr_factory_.InvalidateWeakPtrs();
    base::SequencedTaskRunner::GetCurrentDefault()->PostTask(
        FROM_HERE, base::BindOnce(&PolicyServiceImpl::MergeAndTriggerUpdates,
                                  update_task_ptr_factory_.GetWeakPtr()));

    VLOG_POLICY(2, POLICY_PROCESSING) << "Policy refresh has no providers";
  } else {
    // Some providers might invoke OnUpdatePolicy synchronously while handling
    // RefreshPolicies. Mark all as pending before refreshing.
    for (auto* provider : providers_)
      refresh_pending_.insert(provider);
    for (auto* provider : providers_)
      provider->RefreshPolicies();
  }
}

#if BUILDFLAG(IS_ANDROID)
android::PolicyServiceAndroid* PolicyServiceImpl::GetPolicyServiceAndroid() {
  if (!policy_service_android_)
    policy_service_android_ =
        std::make_unique<android::PolicyServiceAndroid>(this);
  return policy_service_android_.get();
}
#endif

void PolicyServiceImpl::UnthrottleInitialization() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  if (!initialization_throttled_)
    return;

  initialization_throttled_ = false;
  std::vector<PolicyDomain> updated_domains;
  for (int domain = 0; domain < POLICY_DOMAIN_SIZE; ++domain)
    updated_domains.push_back(static_cast<PolicyDomain>(domain));
  MaybeNotifyPolicyDomainStatusChange(updated_domains);
}

void PolicyServiceImpl::OnUpdatePolicy(ConfigurationPolicyProvider* provider) {
  DCHECK_EQ(1, base::ranges::count(providers_, provider));
  refresh_pending_.erase(provider);
  provider_update_pending_.insert(provider);

  // Note: a policy change may trigger further policy changes in some providers.
  // For example, disabling SigninAllowed would cause the CloudPolicyManager to
  // drop all its policies, which makes this method enter again for that
  // provider.
  //
  // Therefore this update is posted asynchronously, to prevent reentrancy in
  // MergeAndTriggerUpdates. Also, cancel a pending update if there is any,
  // since both will produce the same PolicyBundle.
  update_task_ptr_factory_.InvalidateWeakPtrs();
  base::SequencedTaskRunner::GetCurrentDefault()->PostTask(
      FROM_HERE, base::BindOnce(&PolicyServiceImpl::MergeAndTriggerUpdates,
                                update_task_ptr_factory_.GetWeakPtr()));
}

void PolicyServiceImpl::NotifyNamespaceUpdated(const PolicyNamespace& ns,
                                               const PolicyMap& previous,
                                               const PolicyMap& current) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  auto iterator = observers_.find(ns.domain);
  if (iterator != observers_.end()) {
    for (auto& observer : iterator->second)
      observer.OnPolicyUpdated(ns, previous, current);
  }
}

void PolicyServiceImpl::NotifyProviderUpdatesPropagated() {
  if (provider_update_pending_.empty())
    return;

  for (auto& provider_update_observer : provider_update_observers_) {
    for (ConfigurationPolicyProvider* provider : provider_update_pending_) {
      provider_update_observer.OnProviderUpdatePropagated(provider);
    }
  }
  provider_update_pending_.clear();
}

void PolicyServiceImpl::MergeAndTriggerUpdates() {
  // TODO: GOOGAMCONS-164: Implement
}

std::vector<PolicyDomain> PolicyServiceImpl::UpdatePolicyDomainStatus() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);

  std::vector<PolicyDomain> updated_domains;

  // Check if all the providers just became initialized for each domain; if so,
  // notify that domain's observers. If they were initialized, check if they had
  // their first policies loaded.
  for (int domain = 0; domain < POLICY_DOMAIN_SIZE; ++domain) {
    PolicyDomain policy_domain = static_cast<PolicyDomain>(domain);
    if (policy_domain_status_[domain] == PolicyDomainStatus::kPolicyReady)
      continue;

    PolicyDomainStatus new_status = PolicyDomainStatus::kPolicyReady;

    for (auto* provider : providers_) {
      if (!provider->IsInitializationComplete(policy_domain)) {
        new_status = PolicyDomainStatus::kUninitialized;
        break;
      } else if (!provider->IsFirstPolicyLoadComplete(policy_domain)) {
        new_status = PolicyDomainStatus::kInitialized;
      }
    }

    if (new_status == policy_domain_status_[domain])
      continue;

    policy_domain_status_[domain] = new_status;
    updated_domains.push_back(static_cast<PolicyDomain>(domain));
  }
  return updated_domains;
}

void PolicyServiceImpl::MaybeNotifyPolicyDomainStatusChange(
    const std::vector<PolicyDomain>& updated_domains) {
  if (initialization_throttled_)
    return;

  for (const auto policy_domain : updated_domains) {
    if (policy_domain_status_[policy_domain] ==
        PolicyDomainStatus::kUninitialized) {
      continue;
    }

    auto iter = observers_.find(policy_domain);
    if (iter == observers_.end())
      continue;

    // If and when crbug.com/1221454 gets fixed, we should drop the WeakPtr
    // construction and checks here.
    const auto weak_this = weak_ptr_factory_.GetWeakPtr();
    for (auto& observer : iter->second) {
      observer.OnPolicyServiceInitialized(policy_domain);
      if (!weak_this) {
        VLOG_POLICY(1, POLICY_PROCESSING)
            << "PolicyService destroyed while notifying observers.";
        return;
      }
      if (policy_domain_status_[policy_domain] ==
          PolicyDomainStatus::kPolicyReady) {
        observer.OnFirstPoliciesLoaded(policy_domain);
        // If this gets hit, it implies that some OnFirstPoliciesLoaded()
        // observer was changed to trigger the deletion of |this|. See
        // crbug.com/1221454 for a similar problem with
        // OnPolicyServiceInitialized().
        CHECK(weak_this);
      }
    }
  }
}

void PolicyServiceImpl::CheckRefreshComplete() {
  if (refresh_pending_.empty()) {
    VLOG(2) << "Policy refresh complete";
  }

  // Invoke all the callbacks if a refresh has just fully completed.
  if (refresh_pending_.empty() && !refresh_callbacks_.empty()) {
    std::vector<base::OnceClosure> callbacks;
    callbacks.swap(refresh_callbacks_);
    for (auto& callback : callbacks)
      std::move(callback).Run();
  }
}

}  // namespace policy
