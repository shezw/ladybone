/*
 * Copyright (c) 2026, Ladybird contributors
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibCore/EventLoop.h>
#include <UI/JW/Application.h>
#include <UI/JW/WebContentView.h>

namespace Ladybird {

Application::Application() = default;
Application::~Application() = default;

void Application::create_platform_options(WebView::BrowserOptions&, WebView::RequestServerOptions&, WebView::WebContentOptions& web_content_options)
{
    web_content_options.force_cpu_painting = WebView::ForceCPUPainting::Yes;
    web_content_options.enable_async_scrolling = WebView::EnableAsyncScrolling::No;
}

NonnullOwnPtr<Core::EventLoop> Application::create_platform_event_loop()
{
    return WebView::Application::create_platform_event_loop();
}

ErrorOr<void> Application::create_window(Vector<URL::URL> const& initial_urls)
{
    auto view = TRY(WebContentView::create());
    m_view = move(view);

    if (!initial_urls.is_empty())
        m_view->load(initial_urls.first());

    return {};
}

Optional<WebView::ViewImplementation&> Application::active_web_view() const
{
    if (!m_view)
        return {};
    return static_cast<WebView::ViewImplementation&>(*m_view);
}

Optional<WebView::ViewImplementation&> Application::open_blank_new_tab(Web::HTML::ActivateTab) const
{
    if (!m_view)
        return {};
    return static_cast<WebView::ViewImplementation&>(*m_view);
}

void Application::open_url_in_new_window(URL::URL const& url)
{
    if (m_view)
        m_view->load(url);
}

}
