// Copyright 2020 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/safe_browsing/core/common/safe_browsing_policy_handler.h"

#include "base/strings/string_number_conversions.h"
#include "base/values.h"
#include "components/policy/core/browser/policy_error_map.h"
#include "components/policy/core/common/policy_map.h"
#include "components/policy/policy_constants.h"
#include "components/prefs/pref_service.h"
#include "components/prefs/pref_value_map.h"
#include "components/safe_browsing/core/common/safe_browsing_prefs.h"
#include "components/strings/grit/components_strings.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace safe_browsing {

bool SafeBrowsingPolicyHandler::CheckPolicySettings(
    const policy::PolicyMap& policies,
    policy::PolicyErrorMap* errors) {
// TODO: GOOGAMCONS-164: Implement
  // Always continue to ApplyPolicySettings which can handle invalid policy
  // values.
  return true;
}

void SafeBrowsingPolicyHandler::ApplyPolicySettings(
    const policy::PolicyMap& policies,
    PrefValueMap* prefs) {
      // TODO: GOOGAMCONS-164: Implement
}

// static
SafeBrowsingPolicyHandler::ProtectionLevel
SafeBrowsingPolicyHandler::GetSafeBrowsingProtectionLevel(
    const PrefService* pref_sevice) {
      // TODO: GOOGAMCONS-164: Implement
  return ProtectionLevel::kNoProtection;
}

// static
bool SafeBrowsingPolicyHandler::IsSafeBrowsingProtectionLevelSetByPolicy(
    const PrefService* pref_service) {
  // TODO: GOOGAMCONS-164: Implement
  return false;
}

}  // namespace safe_browsing
