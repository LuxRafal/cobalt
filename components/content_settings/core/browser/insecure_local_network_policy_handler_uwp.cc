// Copyright 2020 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/content_settings/core/browser/insecure_local_network_policy_handler.h"

#include "base/values.h"
#include "components/content_settings/core/common/content_settings.h"
#include "components/content_settings/core/common/pref_names.h"
#include "components/policy/core/common/policy_map.h"
#include "components/policy/policy_constants.h"
#include "components/prefs/pref_value_map.h"

namespace content_settings {

InsecureLocalNetworkPolicyHandler::InsecureLocalNetworkPolicyHandler()
    : TypeCheckingPolicyHandler(
          "",
          base::Value::Type::BOOLEAN) {}

InsecureLocalNetworkPolicyHandler::~InsecureLocalNetworkPolicyHandler() =
    default;

void InsecureLocalNetworkPolicyHandler::ApplyPolicySettings(
    const policy::PolicyMap& policies,
    PrefValueMap* prefs) {
      // TODO: GOOGAMCONS-164: Implement
}

}  // namespace content_settings
