// Copyright 2013 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/policy/configuration_policy_handler_list_factory.h"

#include <limits.h>
#include <stddef.h>

#include <memory>
#include <utility>
#include <vector>

#include "base/command_line.h"
#include "base/functional/bind.h"
#include "base/memory/ptr_util.h"
#include "base/values.h"
#include "build/branding_buildflags.h"
#include "build/build_config.h"
#include "build/chromeos_buildflags.h"
#include "chrome/browser/browsing_data/browsing_data_lifetime_policy_handler.h"
#include "chrome/browser/enterprise/connectors/device_trust/prefs.h"
#include "chrome/browser/enterprise/idle/idle_timeout_policy_handler.h"
#include "chrome/browser/first_party_sets/first_party_sets_overrides_policy_handler.h"
#include "chrome/browser/net/disk_cache_dir_policy_handler.h"
#include "chrome/browser/net/explicitly_allowed_network_ports_policy_handler.h"
#include "chrome/browser/net/secure_dns_policy_handler.h"
#include "chrome/browser/policy/boolean_disabling_policy_handler.h"
#include "chrome/browser/policy/browsing_history_policy_handler.h"
#include "chrome/browser/policy/developer_tools_policy_handler.h"
#include "chrome/browser/policy/file_selection_dialogs_policy_handler.h"
#include "chrome/browser/policy/homepage_location_policy_handler.h"
#include "chrome/browser/policy/javascript_policy_handler.h"
#include "chrome/browser/policy/webhid_device_policy_handler.h"
#include "chrome/browser/policy/webusb_allow_devices_for_urls_policy_handler.h"
#include "chrome/browser/prefetch/pref_names.h"
#include "chrome/browser/profiles/force_safe_search_policy_handler.h"
#include "chrome/browser/profiles/force_youtube_safety_mode_policy_handler.h"
#include "chrome/browser/profiles/guest_mode_policy_handler.h"
#include "chrome/browser/profiles/incognito_mode_policy_handler.h"
#include "chrome/browser/search/ntp_custom_background_enabled_policy_handler.h"
#include "chrome/browser/sessions/restore_on_startup_policy_handler.h"
#include "chrome/browser/spellchecker/spellcheck_language_blocklist_policy_handler.h"
#include "chrome/browser/spellchecker/spellcheck_language_policy_handler.h"
#include "chrome/browser/ssl/secure_origin_policy_handler.h"
#include "chrome/browser/themes/theme_color_policy_handler.h"
#include "chrome/browser/ui/toolbar/chrome_labs_prefs.h"
#include "chrome/browser/webauthn/webauthn_pref_names.h"
#include "chrome/common/buildflags.h"
#include "chrome/common/channel_info.h"
#include "chrome/common/chrome_switches.h"
#include "chrome/common/pref_names.h"
#include "components/autofill/core/browser/autofill_address_policy_handler.h"
#include "components/autofill/core/browser/autofill_credit_card_policy_handler.h"
#include "components/autofill/core/browser/autofill_policy_handler.h"
#include "components/blocked_content/pref_names.h"
#include "components/bookmarks/common/bookmark_pref_names.h"
#include "components/bookmarks/managed/managed_bookmarks_policy_handler.h"
#include "components/browsing_data/core/pref_names.h"
#include "components/certificate_transparency/pref_names.h"
#include "components/commerce/core/pref_names.h"
#include "components/component_updater/pref_names.h"
#include "components/content_settings/core/browser/cookie_settings_policy_handler.h"
#include "components/content_settings/core/browser/insecure_local_network_policy_handler.h"
#include "components/content_settings/core/common/pref_names.h"
#include "components/custom_handlers/pref_names.h"
#include "components/domain_reliability/domain_reliability_prefs.h"
#include "components/embedder_support/pref_names.h"
#include "components/enterprise/browser/reporting/cloud_profile_reporting_policy_handler.h"
#include "components/enterprise/browser/reporting/cloud_reporting_frequency_policy_handler.h"
#include "components/enterprise/browser/reporting/cloud_reporting_policy_handler.h"
#include "components/enterprise/browser/reporting/common_pref_names.h"
#include "components/enterprise/content/copy_prevention_settings_policy_handler.h"
#include "components/enterprise/content/pref_names.h"
#include "components/feed/core/shared_prefs/pref_names.h"
#include "components/history/core/common/pref_names.h"
#include "components/history_clusters/core/history_clusters_prefs.h"
#include "components/language/core/browser/pref_names.h"
#include "components/metrics/metrics_pref_names.h"
#include "components/network_time/network_time_pref_names.h"
#include "components/omnibox/browser/omnibox_prefs.h"
#include "components/password_manager/core/common/password_manager_pref_names.h"
#include "components/payments/core/payment_prefs.h"
#include "components/performance_manager/public/user_tuning/prefs.h"
#include "components/policy/core/browser/configuration_policy_handler.h"
#include "components/policy/core/browser/configuration_policy_handler_list.h"
#include "components/policy/core/browser/configuration_policy_handler_parameters.h"
#include "components/policy/core/browser/url_blocklist_policy_handler.h"
#include "components/policy/core/browser/url_scheme_list_policy_handler.h"
#include "components/policy/core/common/policy_details.h"
#include "components/policy/core/common/policy_map.h"
#include "components/policy/core/common/policy_pref_names.h"
#include "components/policy/core/common/schema.h"
#include "components/policy/policy_constants.h"
#include "components/privacy_sandbox/privacy_sandbox_prefs.h"
#include "components/proxy_config/proxy_policy_handler.h"
#include "components/safe_browsing/content/common/file_type_policies_prefs.h"
#include "components/safe_browsing/core/common/safe_browsing_policy_handler.h"
#include "components/safe_browsing/core/common/safe_browsing_prefs.h"
#include "components/search_engines/default_search_policy_handler.h"
#include "components/search_engines/search_engines_pref_names.h"
#include "components/security_interstitials/core/https_only_mode_policy_handler.h"
#include "components/security_interstitials/core/pref_names.h"
#include "components/services/storage/public/cpp/storage_prefs.h"
#include "components/signin/public/base/signin_pref_names.h"
#include "components/spellcheck/spellcheck_buildflags.h"
#include "components/sync/base/pref_names.h"
#include "components/sync/driver/sync_policy_handler.h"
#include "components/translate/core/browser/translate_pref_names.h"
#include "components/unified_consent/pref_names.h"
#include "components/variations/pref_names.h"
#include "components/variations/service/variations_service.h"
#include "components/version_info/channel.h"
#include "content/public/common/content_switches.h"
#include "extensions/buildflags/buildflags.h"
#include "media/media_buildflags.h"
#include "pdf/buildflags.h"
#include "ppapi/buildflags/buildflags.h"
#include "printing/buildflags/buildflags.h"

#if BUILDFLAG(IS_ANDROID)
#include "chrome/browser/first_run/android/first_run_prefs.h"
#include "chrome/browser/lens/android/lens_prefs.h"
#include "chrome/browser/search/contextual_search_policy_handler_android.h"
#else  // BUILDFLAG(IS_ANDROID)
#include "chrome/browser/download/default_download_dir_policy_handler.h"
#include "chrome/browser/download/download_auto_open_policy_handler.h"
#include "chrome/browser/download/download_dir_policy_handler.h"
#include "chrome/browser/enterprise/connectors/connectors_prefs.h"
#include "chrome/browser/enterprise/connectors/enterprise_connectors_policy_handler.h"
#include "chrome/browser/enterprise/reporting/extension_request/extension_request_policy_handler.h"
#include "chrome/browser/media/router/discovery/access_code/access_code_cast_feature.h"
#include "chrome/browser/policy/local_sync_policy_handler.h"
#include "chrome/browser/policy/managed_account_policy_handler.h"
#include "chrome/browser/web_applications/policy/web_app_settings_policy_handler.h"
#include "components/headless/policy/headless_mode_policy_handler.h"
#include "components/media_router/common/pref_names.h"
#endif  // BUILDFLAG(IS_ANDROID)

#if defined(TOOLKIT_VIEWS)
#include "chrome/browser/ui/side_search/side_search_prefs.h"
#endif  // BUILDFLAG(TOOLKIT_VIEWS)

#if !BUILDFLAG(IS_CHROMEOS)
#include "chrome/browser/policy/browser_signin_policy_handler.h"
#else
#include "chrome/browser/policy/system_features_disable_list_policy_handler.h"
#include "chromeos/ui/wm/fullscreen/pref_names.h"
#endif  // !BUILDFLAG(IS_CHROMEOS)

#if BUILDFLAG(IS_CHROMEOS_ASH)
#include "ash/components/arc/arc_prefs.h"
#include "ash/constants/ash_pref_names.h"
#include "ash/public/cpp/ambient/ambient_prefs.h"
#include "chrome/browser/apps/app_service/webapk/webapk_prefs.h"
#include "chrome/browser/ash/accessibility/magnifier_type.h"
#include "chrome/browser/ash/app_restore/full_restore_prefs.h"
#include "chrome/browser/ash/arc/policy/arc_policy_handler.h"
#include "chrome/browser/ash/borealis/borealis_prefs.h"
#include "chrome/browser/ash/bruschetta/bruschetta_policy_handler.h"
#include "chrome/browser/ash/crostini/crostini_pref_names.h"
#include "chrome/browser/ash/login/login_pref_names.h"
#include "chrome/browser/ash/login/users/avatar/user_image_prefs.h"
#include "chrome/browser/ash/platform_keys/key_permissions/key_permissions_policy_handler.h"
#include "chrome/browser/ash/plugin_vm/plugin_vm_pref_names.h"
#include "chrome/browser/ash/policy/handlers/configuration_policy_handler_ash.h"
#include "chrome/browser/ash/policy/handlers/lacros_availability_policy_handler.h"
#include "chrome/browser/ash/policy/handlers/lacros_selection_policy_handler.h"
#include "chrome/browser/nearby_sharing/common/nearby_share_prefs.h"
#include "chrome/browser/policy/default_geolocation_policy_handler.h"
#include "chrome/browser/policy/device_login_screen_geolocation_access_level_policy_handler.h"
#include "chrome/browser/policy/os_color_mode_policy_handler.h"
#include "chromeos/ash/services/assistant/public/cpp/assistant_prefs.h"
#include "chromeos/ash/services/multidevice_setup/public/cpp/prefs.h"
#include "chromeos/components/disks/disks_prefs.h"
#include "chromeos/components/quick_answers/public/cpp/quick_answers_prefs.h"
#include "chromeos/dbus/power/power_policy_controller.h"
#include "components/account_manager_core/pref_names.h"
#include "components/drive/drive_pref_names.h"  // nogncheck crbug.com/1125897
#include "components/user_manager/user.h"
#include "components/user_manager/user_manager.h"
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if !BUILDFLAG(IS_ANDROID) && !BUILDFLAG(IS_CHROMEOS)
#include "chrome/browser/browser_switcher/browser_switcher_prefs.h"
#include "chrome/browser/external_protocol/auto_launch_protocols_policy_handler.h"
#endif  // !BUILDFLAG(IS_ANDROID) && !BUILDFLAG(IS_CHROMEOS)

#if BUILDFLAG(ENABLE_EXTENSIONS)
#include "chrome/browser/extensions/api/messaging/native_messaging_policy_handler.h"
#include "chrome/browser/extensions/extension_management_constants.h"
#include "chrome/browser/extensions/policy_handlers.h"
#include "extensions/browser/pref_names.h"
#include "extensions/common/manifest.h"
#endif  // BUILDFLAG(ENABLE_EXTENSIONS)

#if BUILDFLAG(ENABLE_PDF)
#include "chrome/browser/pdf/pdf_pref_names.h"
#endif  // BUILDFLAG(ENABLE_PDF)

#if BUILDFLAG(ENABLE_PRINTING)
#include "chrome/browser/policy/printing_restrictions_policy_handler.h"
#endif

#if BUILDFLAG(ENABLE_SPELLCHECK)
#include "components/spellcheck/browser/pref_names.h"
#endif  // BUILDFLAG(ENABLE_SPELLCHECK)

#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX)
#include "chrome/browser/enterprise/idle/action.h"
#include "components/device_signals/core/browser/pref_names.h"
#endif  // BUILDFLAG(IS_WIN) || BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX)

#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX) || \
    BUILDFLAG(IS_CHROMEOS) || BUILDFLAG(IS_FUCHSIA) || BUILDFLAG(IS_ANDROID)
#include "chrome/browser/privacy_sandbox/privacy_sandbox_policy_handler.h"
#endif  // BUILDFLAG(IS_WIN) || BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX) ||
        // BUILDFLAG(IS_CHROMEOS) || BUILDFLAG(IS_FUCHSIA) ||
        // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_CHROMEOS_ASH)
#include "chrome/browser/ash/wallpaper_handlers/wallpaper_prefs.h"
#endif

namespace policy {

namespace {

#if BUILDFLAG(IS_CHROMEOS_ASH)
using ::ash::MagnifierType;
#endif

// List of policy types to preference names. This is used for simple policies
// that directly map to a single preference.

#if BUILDFLAG(ENABLE_EXTENSIONS)
#endif  // BUILDFLAG(ENABLE_EXTENSIONS)

// Future policies are not supported on Stable and Beta by default.
bool AreFuturePoliciesEnabledByDefault() {
  // Enable future policies for branded browser tests.
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(switches::kTestType))
    return true;
  version_info::Channel channel = chrome::GetChannel();
  return channel != version_info::Channel::STABLE &&
         channel != version_info::Channel::BETA;
}

}  // namespace

void PopulatePolicyHandlerParameters(PolicyHandlerParameters* parameters) {
#if BUILDFLAG(IS_CHROMEOS_ASH)
  if (user_manager::UserManager::IsInitialized()) {
    const user_manager::User* user =
        user_manager::UserManager::Get()->GetActiveUser();
    if (user)
      parameters->user_id_hash = user->username_hash();
  }
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)
}

std::unique_ptr<ConfigurationPolicyHandlerList> BuildHandlerList(
    const Schema& chrome_schema) {
  std::unique_ptr<ConfigurationPolicyHandlerList> handlers(
      new ConfigurationPolicyHandlerList(
          base::BindRepeating(&PopulatePolicyHandlerParameters),
          base::BindRepeating(&GetChromePolicyDetails),
          AreFuturePoliciesEnabledByDefault()));


  return handlers;
}

}  // namespace policy
