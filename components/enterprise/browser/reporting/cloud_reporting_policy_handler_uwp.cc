// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/enterprise/browser/reporting/cloud_reporting_policy_handler.h"

#include "base/values.h"
#include "build/build_config.h"
#include "components/enterprise/browser/reporting/common_pref_names.h"
#include "components/policy/core/browser/policy_error_map.h"
#include "components/policy/core/common/cloud/dm_token.h"
#include "components/policy/core/common/policy_map.h"
#include "components/policy/policy_constants.h"
#include "components/prefs/pref_value_map.h"
#include "components/strings/grit/components_strings.h"

#if !BUILDFLAG(IS_CHROMEOS)
#include "components/enterprise/browser/controller/browser_dm_token_storage.h"
#endif  // !BUILDFLAG(IS_CHROMEOS)

namespace enterprise_reporting {

CloudReportingPolicyHandler::CloudReportingPolicyHandler()
    : policy::TypeCheckingPolicyHandler("",
                                        base::Value::Type::BOOLEAN) {}

CloudReportingPolicyHandler::~CloudReportingPolicyHandler() = default;

bool CloudReportingPolicyHandler::CheckPolicySettings(
    const policy::PolicyMap& policies,
    policy::PolicyErrorMap* errors) {
      //TODO: GOOGAMCONS-164: IMPLEMENT
  return true;
}

void CloudReportingPolicyHandler::ApplyPolicySettings(
    const policy::PolicyMap& policies,
    PrefValueMap* prefs) {

      //TODO: GOOGAMCONS-164: IMPLEMENT
}

}  // namespace enterprise_reporting
