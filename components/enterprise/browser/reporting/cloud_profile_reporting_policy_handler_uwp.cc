// Copyright 2022 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/enterprise/browser/reporting/cloud_profile_reporting_policy_handler.h"

#include "base/values.h"
#include "components/enterprise/browser/reporting/common_pref_names.h"
#include "components/policy/core/browser/policy_error_map.h"
#include "components/policy/core/common/policy_map.h"
#include "components/policy/core/common/policy_types.h"
#include "components/policy/policy_constants.h"
#include "components/prefs/pref_value_map.h"
#include "components/strings/grit/components_strings.h"

namespace enterprise_reporting {

CloudProfileReportingPolicyHandler::CloudProfileReportingPolicyHandler()
    : policy::TypeCheckingPolicyHandler(
          "",
          base::Value::Type::BOOLEAN) {}

CloudProfileReportingPolicyHandler::~CloudProfileReportingPolicyHandler() =
    default;

bool CloudProfileReportingPolicyHandler::CheckPolicySettings(
    const policy::PolicyMap& policies,
    policy::PolicyErrorMap* errors) {
  // TODO: GOOGAMCONS-164: Implement

  return true;
}

void CloudProfileReportingPolicyHandler::ApplyPolicySettings(
    const policy::PolicyMap& policies,
    PrefValueMap* prefs) {
  // TODO: GOOGAMCONS-164: Implement
}

}  // namespace enterprise_reporting
