
// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_APP_CHROME_EXE_MAIN_UWP_H_
#define CHROME_APP_CHROME_EXE_MAIN_UWP_H_

#include <stdint.h>
#include "winrt/Windows.ApplicationModel.Core.h"
#include "winrt/Windows.UI.Core.h"
#include "winrt/Windows.ApplicationModel.Activation.h"

// Returns the SHA-256 hashes of the specified .pak files.
void GetPakFileHashes(
    const uint8_t** resources_pak,
    const uint8_t** chrome_100_pak,
    const uint8_t** chrome_200_pak);

// UWP Application class that implements the Chrome browser lifecycle
struct ChromeUwpApp : winrt::implements<ChromeUwpApp, winrt::Windows::ApplicationModel::Core::IFrameworkView>
{
public:
    // IFrameworkView interface
    void Initialize(winrt::Windows::ApplicationModel::Core::CoreApplicationView const& applicationView);
    void SetWindow(winrt::Windows::UI::Core::CoreWindow const& window);
    void Load(winrt::hstring const& entryPoint);
    void Run();
    void Uninitialize();

private:
    // Event handlers
    void OnActivated(winrt::Windows::ApplicationModel::Core::CoreApplicationView const& applicationView,
        winrt::Windows::ApplicationModel::Activation::IActivatedEventArgs const& args);
    void OnWindowClosed(winrt::Windows::UI::Core::CoreWindow const& sender,
        winrt::Windows::UI::Core::CoreWindowEventArgs const& args);
    void OnSuspending(winrt::Windows::ApplicationModel::ISuspendingEventArgs const& args);
    void OnResuming();

    // Chrome initialization and cleanup
    bool InitializeChrome();
    void UninitializeChrome();
    int LaunchChrome();

    winrt::Windows::UI::Core::CoreWindow m_window{ nullptr };
    winrt::Windows::ApplicationModel::Core::CoreApplicationView m_applicationView{ nullptr };
    bool m_windowClosed{ false };
    bool m_chromeInitialized{ false };
    int m_exitCode{ 0 };
};

// Factory for creating ChromeUwpApp instances
struct ChromeUwpAppSource : winrt::implements<ChromeUwpAppSource, winrt::Windows::ApplicationModel::Core::IFrameworkViewSource>
{
    winrt::Windows::ApplicationModel::Core::IFrameworkView CreateView()
    {
        return winrt::make<ChromeUwpApp>();
    }
};

#endif  // CHROME_APP_CHROME_EXE_MAIN_UWP_H_
