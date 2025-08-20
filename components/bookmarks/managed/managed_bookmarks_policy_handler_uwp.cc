// Copyright 2014 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/bookmarks/managed/managed_bookmarks_policy_handler.h"

#include <utility>

#include "base/values.h"
#include "components/bookmarks/common/bookmark_pref_names.h"
#include "components/bookmarks/managed/managed_bookmarks_tracker.h"
#include "components/policy/core/browser/policy_error_map.h"
#include "components/policy/core/common/policy_logger.h"
#include "components/policy/core/common/policy_map.h"
#include "components/policy/policy_constants.h"
#include "components/prefs/pref_value_map.h"
#include "components/url_formatter/url_fixer.h"
#include "url/gurl.h"

using bookmarks::ManagedBookmarksTracker;

namespace bookmarks {

ManagedBookmarksPolicyHandler::ManagedBookmarksPolicyHandler(
    policy::Schema chrome_schema)
    : SchemaValidatingPolicyHandler(
      "",
      chrome_schema.GetKnownProperty(""),
      policy::SCHEMA_ALLOW_UNKNOWN) {}

ManagedBookmarksPolicyHandler::~ManagedBookmarksPolicyHandler() = default;

void ManagedBookmarksPolicyHandler::ApplyPolicySettings(
    const policy::PolicyMap& policies,
    PrefValueMap* prefs) {
      // TODO: GOOGAMCONS-164: IMPLEMENT
}

std::string ManagedBookmarksPolicyHandler::GetFolderName(
    const base::Value::List& list) {
      // TODO: GOOGAMCONS-164: IMPLEMENT
  return std::string();
}

base::Value::List ManagedBookmarksPolicyHandler::FilterBookmarks(
    base::Value::List list) {
  // Move over conforming values found.
  base::Value::List out;
  // TODO: GOOGAMCONS-164: IMPLEMENT
  return out;
}

}  // namespace bookmarks
