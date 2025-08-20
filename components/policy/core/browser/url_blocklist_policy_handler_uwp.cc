// Copyright 2014 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/policy/core/browser/url_blocklist_policy_handler.h"

#include <memory>
#include <utility>
#include <vector>

#include "base/logging.h"
#include "base/strings/strcat.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/values.h"
#include "build/build_config.h"
#include "components/policy/core/browser/policy_error_map.h"
#include "components/policy/core/common/policy_logger.h"
#include "components/policy/core/common/policy_map.h"
#include "components/policy/core/common/policy_pref_names.h"
#include "components/policy/policy_constants.h"
#include "components/prefs/pref_value_map.h"
#include "components/strings/grit/components_strings.h"
#include "components/url_matcher/url_util.h"
#include "third_party/abseil-cpp/absl/types/optional.h"


namespace policy {

URLBlocklistPolicyHandler::URLBlocklistPolicyHandler(const char* policy_name)
    : TypeCheckingPolicyHandler(policy_name, base::Value::Type::LIST) {}

URLBlocklistPolicyHandler::~URLBlocklistPolicyHandler() = default;

bool URLBlocklistPolicyHandler::CheckPolicySettings(const PolicyMap& policies,
                                                    PolicyErrorMap* errors) {
  // TODO: GOOGAMCONS-164: Impelment
  return true;
}

void URLBlocklistPolicyHandler::ApplyPolicySettings(const PolicyMap& policies,
                                                    PrefValueMap* prefs) {
  // TODO: GOOGAMCONS-164: Implement
}

bool URLBlocklistPolicyHandler::ValidatePolicy(const std::string& url_pattern) {
  // TODO: GOOGAMCONS-164: Implement
  return true;
}

}  // namespace policy
