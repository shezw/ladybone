/*
 * Copyright (c) 2026, Ladybird contributors
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/OwnPtr.h>
#include <AK/Vector.h>
#include <LibURL/URL.h>
#include <LibWebView/Application.h>

namespace Ladybird {

class WebContentView;

class Application final : public WebView::Application {
    WEB_VIEW_APPLICATION(Application)

public:
    virtual ~Application() override;

    ErrorOr<void> create_window(Vector<URL::URL> const&);
    WebContentView* view() const { return m_view.ptr(); }

private:
    explicit Application();

    virtual void create_platform_options(WebView::BrowserOptions&, WebView::RequestServerOptions&, WebView::WebContentOptions&) override;
    virtual NonnullOwnPtr<Core::EventLoop> create_platform_event_loop() override;
    virtual Optional<WebView::ViewImplementation&> active_web_view() const override;
    virtual Optional<WebView::ViewImplementation&> open_blank_new_tab(Web::HTML::ActivateTab) const override;
    virtual void open_url_in_new_window(URL::URL const&) override;

    OwnPtr<WebContentView> m_view;
};

}
