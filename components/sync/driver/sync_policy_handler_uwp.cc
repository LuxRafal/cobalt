// Copyright 2013 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/sync/driver/sync_policy_handler.h"

#include <string>

#include "base/values.h"
#include "build/chromeos_buildflags.h"
#include "components/policy/core/common/policy_map.h"
#include "components/policy/policy_constants.h"
#include "components/prefs/pref_value_map.h"
#include "components/sync/base/pref_names.h"
#include "components/sync/base/sync_prefs.h"
#include "components/sync/base/user_selectable_type.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace syncer {

SyncPolicyHandler::SyncPolicyHandler()
    : policy::TypeCheckingPolicyHandler("",
                                        base::Value::Type::BOOLEAN) {}

SyncPolicyHandler::~SyncPolicyHandler() = default;

void SyncPolicyHandler::ApplyPolicySettings(const policy::PolicyMap& policies,
                                            PrefValueMap* prefs) {
  const base::Value* disable_sync_value =
      policies.GetValue(policy_name(), base::Value::Type::BOOLEAN);
  if (disable_sync_value && disable_sync_value->GetBool()) {
    prefs->SetValue(prefs::kSyncManaged, disable_sync_value->Clone());
  }

  // TODO: GOOGAMCONS-164: Implement
  // const base::Value* disabled_sync_types_value = policies.GetValue(
  //     policy::key::kSyncTypesListDisabled, base::Value::Type::LIST);
  // if (disabled_sync_types_value) {
  //   const base::Value::List& list = disabled_sync_types_value->GetList();
  //   for (const base::Value& type_name : list) {
  //     if (!type_name.is_string())
  //       continue;
  //     DisableSyncType(type_name.GetString(), prefs);
  //   }
  // }
}

}  // namespace syncer
