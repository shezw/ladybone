/*
 * Copyright (c) 2026, Ladybird contributors
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Error.h>
#include <AK/NonnullOwnPtr.h>
#include <AK/OwnPtr.h>
#include <AK/RefPtr.h>
#include <LibCore/Forward.h>
#include <LibGfx/Rect.h>
#include <LibWebView/ViewImplementation.h>

extern "C" {
#include <jingwei.h>
}

namespace Ladybird {

class WebContentView final : public WebView::ViewImplementation {
public:
    static ErrorOr<NonnullOwnPtr<WebContentView>> create();
    virtual ~WebContentView() override;

    void present();
    void poll_events();
    void resize(int width, int height);
    void handle_jw_event(jw_event_t const&);

private:
    WebContentView() = default;

    ErrorOr<void> initialize();
    void update_palette();
    void update_screen_rects();

    virtual void initialize_client(CreateNewClient = CreateNewClient::Yes) override;
    virtual void update_zoom() override;
    virtual Web::DevicePixelSize viewport_size() const override;
    virtual Gfx::IntPoint to_content_position(Gfx::IntPoint widget_position) const override;
    virtual Gfx::IntPoint to_widget_position(Gfx::IntPoint content_position) const override;

    static void on_jw_event(jw_context_t*, jw_display_t*, jw_event_t const*, void*);

    jw_context_t* m_context { nullptr };
    jw_proxy_t* m_proxy { nullptr };
    jw_display_t* m_display { nullptr };
    jw_event_manager_t* m_mouse { nullptr };
    RefPtr<Core::Timer> m_poll_timer;
    Gfx::IntSize m_viewport_size { 800, 600 };
};

}
