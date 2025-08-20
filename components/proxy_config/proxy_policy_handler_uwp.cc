// Copyright 2013 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/proxy_config/proxy_policy_handler.h"

#include <stddef.h>

#include "base/check.h"
#include "base/notreached.h"
#include "base/strings/string_number_conversions.h"
#include "base/values.h"
#include "components/policy/core/browser/configuration_policy_handler.h"
#include "components/policy/core/browser/policy_error_map.h"
#include "components/policy/core/common/policy_map.h"
#include "components/policy/core/common/proxy_settings_constants.h"
#include "components/policy/core/common/schema.h"
#include "components/policy/policy_constants.h"
#include "components/prefs/pref_value_map.h"
#include "components/proxy_config/proxy_config_dictionary.h"
#include "components/proxy_config/proxy_config_pref_names.h"
#include "components/strings/grit/components_strings.h"

namespace {

using policy::kProxyPacMandatory;
using policy::PolicyErrorMap;
using policy::PolicyErrorPath;
using policy::PolicyMap;

// This is used to check whether for a given ProxyMode value, the ProxyPacUrl,
// the ProxyBypassList and the ProxyServer policies are allowed to be specified.
// |error_message_id| is the message id of the localized error message to show
// when the policies are not specified as allowed. Each value of ProxyMode
// has a ProxyModeValidationEntry in the |kProxyModeValidationMap| below.
struct ProxyModeValidationEntry {
  const char* mode_value;
  bool pac_url_allowed;
  bool pac_mandatory_allowed;
  bool bypass_list_allowed;
  bool server_allowed;
  int error_message_id;
};

// Maps the separate deprecated policies for proxy settings into a single
// Dictionary policy. This allows to keep the logic of merging policies from
// different sources simple, as all separate proxy policies should be considered
// as a single whole during merging. Returns proxy_settings value.

}  // namespace

namespace proxy_config {

// The proxy policies have the peculiarity that they are loaded from individual
// policies, but the providers then expose them through a unified
// DictionaryValue.

ProxyPolicyHandler::ProxyPolicyHandler() {}

ProxyPolicyHandler::~ProxyPolicyHandler() {}

bool ProxyPolicyHandler::CheckPolicySettings(const PolicyMap& policies,
                                             PolicyErrorMap* errors) {
  // TODO: GOOGAMCONS-164: Implement
  return true;
}

void ProxyPolicyHandler::ApplyPolicySettings(const PolicyMap& policies,
                                             PrefValueMap* prefs) {
  // TODO: GOOGAMCONS-164: Implement
}

const base::Value* ProxyPolicyHandler::GetProxyPolicyValue(
    const base::Value* value,
    const char* policy_name) {
  if (!value)
    return nullptr;
  const base::Value::Dict* settings = value->GetIfDict();
  if (!settings)
    return nullptr;

  const base::Value* policy_value = settings->Find(policy_name);
  if (!policy_value || policy_value->is_none())
    return nullptr;
  const std::string* tmp = policy_value->GetIfString();
  if (tmp && tmp->empty())
    return nullptr;
  return policy_value;
}

bool ProxyPolicyHandler::CheckProxyModeAndServerMode(
    const base::Value* proxy_settings,
    PolicyErrorMap* errors,
    std::string* mode_value) {
      // TODO: GOOGAMCONS-164: Implement
  return true;
}

}  // namespace proxy_config
