// Copyright 2014 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/search_engines/default_search_policy_handler.h"

#include <stddef.h>

#include <utility>

#include "base/strings/string_number_conversions.h"
#include "base/values.h"
#include "build/build_config.h"
#include "components/policy/core/browser/policy_error_map.h"
#include "components/policy/core/common/policy_map.h"
#include "components/policy/policy_constants.h"
#include "components/prefs/pref_value_map.h"
#include "components/search_engines/default_search_manager.h"
#include "components/search_engines/search_engines_pref_names.h"
#include "components/search_engines/search_terms_data.h"
#include "components/search_engines/template_url.h"
#include "components/strings/grit/components_strings.h"

namespace policy {


// DefaultSearchPolicyHandler implementation -----------------------------------

DefaultSearchPolicyHandler::DefaultSearchPolicyHandler() {}

DefaultSearchPolicyHandler::~DefaultSearchPolicyHandler() {}

bool DefaultSearchPolicyHandler::CheckPolicySettings(const PolicyMap& policies,
                                                     PolicyErrorMap* errors) {
                                                      // TODO: GOOGAMCONS-164: Implement
  return true;
}

void DefaultSearchPolicyHandler::ApplyPolicySettings(const PolicyMap& policies,
                                                     PrefValueMap* prefs) {
  // TODO: GOOGAMCONS-164: Implement
}

bool DefaultSearchPolicyHandler::CheckIndividualPolicies(
    const PolicyMap& policies,
    PolicyErrorMap* errors) {
  bool all_ok = true;
  // TODO: GOOGAMCONS-164: Implement
  return all_ok;
}

bool DefaultSearchPolicyHandler::HasDefaultSearchPolicy(
    const PolicyMap& policies,
    const char* policy_name) {
  return policies.Get(policy_name) != nullptr;
}

bool DefaultSearchPolicyHandler::AnyDefaultSearchPoliciesSpecified(
    const PolicyMap& policies) {
  // TODO: GOOGAMCONS-164: Implement
  return false;
}

bool DefaultSearchPolicyHandler::DefaultSearchProviderIsDisabled(
    const PolicyMap& policies) {
  // TODO: GOOGAMCONS-164: Implement
  return true;
}

bool DefaultSearchPolicyHandler::DefaultSearchProviderPolicyIsSet(
    const PolicyMap& policies) {
      // TODO: GOOGAMCONS-164: Implement
  return true;
}

bool DefaultSearchPolicyHandler::DefaultSearchURLIsValid(
    const PolicyMap& policies,
    const base::Value** url_value,
    std::string* url_string) {
      // TODO: GOOGAMCONS-164: Implement
  return true;
}

void DefaultSearchPolicyHandler::EnsureStringPrefExists(
    PrefValueMap* prefs,
    const std::string& path) {
  std::string value;
  if (!prefs->GetString(path, &value))
    prefs->SetString(path, value);
}

void DefaultSearchPolicyHandler::EnsureListPrefExists(
    PrefValueMap* prefs,
    const std::string& path) {
  base::Value* value;
  if (!prefs->GetValue(path, &value) || !value->is_list())
    prefs->SetValue(path, base::Value(base::Value::Type::LIST));
}

}  // namespace policy
