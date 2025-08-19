
// Copyright 2011 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/app/chrome_exe_main_uwp.h"

#include <windows.h>
#include <malloc.h>
#include <stddef.h>

#include <algorithm>
#include <array>
#include <string>

#include "base/at_exit.h"
#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/containers/cxx20_erase.h"
#include "base/debug/alias.h"
#include "base/feature_list.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/logging.h"
#include "base/path_service.h"
#include "base/process/memory.h"
#include "base/process/process.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/time/time.h"
#include "base/win/current_module.h"
#include "base/win/win_util.h"
#include "base/win/windows_version.h"
#include "build/build_config.h"
#include "chrome/app/main_dll_loader_win.h"
#include "chrome/app/packed_resources_integrity.h"
#include "chrome/common/chrome_paths_internal.h"
#include "chrome/common/chrome_switches.h"
#include "chrome/install_static/initialize_from_primary_module.h"
#include "chrome/install_static/install_util.h"
#include "chrome/install_static/user_data_dir.h"
#include "components/crash/core/app/crash_switches.h"
#include "components/crash/core/app/crashpad.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/result_codes.h"

#include "winrt/Windows.ApplicationModel.h"
#include "winrt/Windows.Storage.h"

using namespace winrt;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::Storage;

namespace {

// UWP equivalent of setting current working directory
void SetUwpWorkingDirectory() {
    // In UWP, we use the app's local folder as the working directory equivalent
    auto localFolder = ApplicationData::Current().LocalFolder();
    // Note: UWP doesn't allow setting CWD, but we can use this path for file operations
}

// UWP command line simulation from activation args
std::wstring GetUwpCommandLine(IActivatedEventArgs const& args) {
    std::wstring command_line;
    
    if (args.Kind() == ActivationKind::Launch) {
        auto launch_args = args.as<LaunchActivatedEventArgs>();
        if (!launch_args.Arguments().empty()) {
            command_line = launch_args.Arguments().c_str();
        }
    }
    
    return command_line;
}

// UWP-compatible process type detection
std::string GetProcessType() {
    // In UWP, we're always the main browser process
    // Child processes are handled differently through app services
    return std::string();
}

} // namespace

void GetPakFileHashes(
    const uint8_t** resources_pak,
    const uint8_t** chrome_100_pak,
    const uint8_t** chrome_200_pak) {
    *resources_pak = kSha256_resources_pak.data();
    *chrome_100_pak = kSha256_chrome_100_percent_pak.data();
    *chrome_200_pak = kSha256_chrome_200_percent_pak.data();
}

// UWP entry point
int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) {
    try {
        CoreApplication::Run(winrt::make<ChromeUwpAppSource>());
        return 0;
    } catch (...) {
        return -1;
    }
}

void ChromeUwpApp::Initialize(CoreApplicationView const& applicationView) {
    m_applicationView = applicationView;
    
    // Register for application lifecycle events
    applicationView.Activated({ this, &ChromeUwpApp::OnActivated });
    
    // Register for suspend/resume events
    auto app = Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().Dispatcher();
    Windows::ApplicationModel::Core::CoreApplication::Suspending({ this, &ChromeUwpApp::OnSuspending });
    Windows::ApplicationModel::Core::CoreApplication::Resuming({ this, &ChromeUwpApp::OnResuming });
}

void ChromeUwpApp::SetWindow(CoreWindow const& window) {
    m_window = window;
    
    // Register window events
    m_window.Closed({ this, &ChromeUwpApp::OnWindowClosed });
    
    // Additional window events specific to Chrome
    // m_window.SizeChanged, m_window.VisibilityChanged, etc.
}

void ChromeUwpApp::Load(winrt::hstring const& entryPoint) {
    // Initialize Chrome components that don't require activation
    SetUwpWorkingDirectory();
    install_static::InitializeFromPrimaryModule();
    
    // Initialize crash reporting for UWP
    // Note: UWP has restrictions on crash handling
    
    // Enable OOM termination
    base::EnableTerminationOnOutOfMemory();
    
#if !defined(COMPONENT_BUILD) && DCHECK_IS_ON()
    // Feature list initialization for UWP
    base::FeatureList::FailOnFeatureAccessWithoutFeatureList();
#endif
}

void ChromeUwpApp::Run() {
    // Activate the window
    m_window.Activate();
    
    // Initialize Chrome if not already done
    if (!m_chromeInitialized) {
        if (!InitializeChrome()) {
            m_exitCode = -1;
            return;
        }
    }
    
    // Run the main message loop
    CoreDispatcher dispatcher = m_window.Dispatcher();
    while (!m_windowClosed) {
        dispatcher.ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit);
    }
}

void ChromeUwpApp::Uninitialize() {
    UninitializeChrome();
}

bool ChromeUwpApp::InitializeChrome() {
    try {
        // Initialize command line for UWP context
        base::CommandLine::Init(0, nullptr);
        
        const base::TimeTicks exe_entry_point_ticks = base::TimeTicks::Now();
        
        // The exit manager is in charge of calling the dtors of singletons
        static base::AtExitManager exit_manager;
        
        // UWP doesn't support multiple Chrome instances in the same way
        // Skip the fast notify check
        
        // Load and launch the chrome dll
        VLOG(1) << "About to load main DLL in UWP context.";
        MainDllLoader* loader = MakeMainDllLoader();
        
        // Get the current module handle (UWP app)
        HINSTANCE instance = reinterpret_cast<HINSTANCE>(&__ImageBase);
        
        m_exitCode = loader->Launch(instance, exe_entry_point_ticks);
        
        // Note: In UWP, we can't relaunch with new command line
        // loader->RelaunchChromeBrowserWithNewCommandLineIfNeeded();
        
        delete loader;
        
        m_chromeInitialized = true;
        return true;
        
    } catch (...) {
        return false;
    }
}

void ChromeUwpApp::UninitializeChrome() {
    if (m_chromeInitialized) {
        // Perform Chrome cleanup
        m_chromeInitialized = false;
    }
}

int ChromeUwpApp::LaunchChrome() {
    if (InitializeChrome()) {
        return m_exitCode;
    }
    return -1;
}

void ChromeUwpApp::OnActivated(CoreApplicationView const& /*applicationView*/, 
                               IActivatedEventArgs const& args) {
    // Handle different activation types
    switch (args.Kind()) {
        case ActivationKind::Launch: {
            auto launch_args = args.as<LaunchActivatedEventArgs>();
            
            // Process launch arguments
            std::wstring command_line_args = launch_args.Arguments().c_str();
            
            // Initialize Chrome with these arguments if needed
            if (!m_chromeInitialized) {
                InitializeChrome();
            }
            break;
        }
        
        case ActivationKind::Protocol: {
            auto protocol_args = args.as<ProtocolActivatedEventArgs>();
            // Handle protocol activation (e.g., opening URLs)
            break;
        }
        
        case ActivationKind::File: {
            auto file_args = args.as<FileActivatedEventArgs>();
            // Handle file activation
            break;
        }
        
        default:
            break;
    }
    
    // Activate the main window
    m_window.Activate();
}

void ChromeUwpApp::OnWindowClosed(CoreWindow const& /*sender*/, 
                                  CoreWindowEventArgs const& /*args*/) {
    m_windowClosed = true;
}

void ChromeUwpApp::OnSuspending(Windows::ApplicationModel::ISuspendingEventArgs const& args) {
    // Handle application suspension
    // Get a deferral to perform async operations
    auto deferral = args.SuspendingOperation().GetDeferral();
    
    // Perform Chrome suspension logic here
    // Save state, pause operations, etc.
    
    deferral.Complete();
}

void ChromeUwpApp::OnResuming() {
    // Handle application resumption
    // Restore state, resume operations, etc.
}
