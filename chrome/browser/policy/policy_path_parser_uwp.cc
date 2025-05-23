// Copyright 2011 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/policy/policy_path_parser.h"

#include <shlobj.h>
#include <stddef.h>
#include <wtsapi32.h>

#include <memory>

#include "base/strings/utf_string_conversions.h"
#include "base/win/registry.h"
#include "chrome/common/chrome_switches.h"
#include "chrome/install_static/policy_path_parser.h"
#include "components/policy/policy_constants.h"

namespace policy {

namespace path_parser {

// Replaces all variable occurances in the policy string with the respective
// system settings values.
base::FilePath::StringType ExpandPathVariables(
    const base::FilePath::StringType& untranslated_string) {
  // This is implemented in the install_static library so that it can also be
  // used by Crashpad initialization code in chrome_elf, which has a very
  // constrained set of dependencies.
  return install_static::ExpandPathVariables(untranslated_string);
}

void CheckUserDataDirPolicy(base::FilePath* user_data_dir) {
  // GOOGAMCONS-164: TODO: Implement. Does nothing for now.
}

}  // namespace path_parser

}  // namespace policy
