// Copyright 2022 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/enterprise/browser/reporting/cloud_reporting_frequency_policy_handler.h"

#include "base/json/values_util.h"
#include "base/time/time.h"
#include "components/enterprise/browser/reporting/common_pref_names.h"
#include "components/policy/core/browser/policy_error_map.h"
#include "components/policy/core/common/schema.h"
#include "components/policy/policy_constants.h"
#include "components/prefs/pref_value_map.h"
#include "components/strings/grit/components_strings.h"

namespace enterprise_reporting {
namespace {
constexpr int kMinimumReportFrequencyInHour = 3;
constexpr int kMaximumReportFrequencyInour = 24;

}  // namespace

CloudReportingFrequencyPolicyHandler::CloudReportingFrequencyPolicyHandler()
    : policy::IntRangePolicyHandlerBase(
          "",
          kMinimumReportFrequencyInHour,
          kMaximumReportFrequencyInour,
          /*clamp=*/true) {}
CloudReportingFrequencyPolicyHandler::~CloudReportingFrequencyPolicyHandler() =
    default;

bool CloudReportingFrequencyPolicyHandler::CheckPolicySettings(
    const policy::PolicyMap& policies,
    policy::PolicyErrorMap* errors) {
      // TODO: GOOGAMCONS-164: Implement
  return true;
}

void CloudReportingFrequencyPolicyHandler::ApplyPolicySettings(
    const policy::PolicyMap& policies,
    PrefValueMap* prefs) {
  // TODO: GOOGAMCONS-164: Implement
}

}  // namespace enterprise_reporting
