/*
 * Copyright (c) 2026, Ladybird contributors
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Platform.h>
#include <AK/String.h>
#include <LibCore/EventLoop.h>
#include <LibCore/Resource.h>
#include <LibCore/Timer.h>
#include <LibGfx/Bitmap.h>
#include <LibGfx/SharedImageBuffer.h>
#include <LibGfx/SystemTheme.h>
#include <LibWeb/CSS/PreferredColorScheme.h>
#include <LibWeb/HTML/VisibilityState.h>
#include <LibWeb/Page/InputEvent.h>
#include <LibWeb/UIEvents/KeyCode.h>
#include <LibWeb/UIEvents/MouseButton.h>
#include <LibWebView/Application.h>
#include <LibWebView/WebContentClient.h>
#include <UI/JW/WebContentView.h>
#include <stdlib.h>
#include <string.h>

namespace Ladybird {

static Web::UIEvents::MouseButton jw_mouse_button_to_web(int button)
{
    switch (button) {
    case JW_MOUSE_LEFT:
        return Web::UIEvents::MouseButton::Primary;
    case JW_MOUSE_MIDDLE:
        return Web::UIEvents::MouseButton::Middle;
    case JW_MOUSE_RIGHT:
        return Web::UIEvents::MouseButton::Secondary;
    default:
        return Web::UIEvents::MouseButton::None;
    }
}

static Web::UIEvents::MouseButton jw_mouse_buttons_to_web(u32 buttons)
{
    Web::UIEvents::MouseButton web_buttons = Web::UIEvents::MouseButton::None;
    if (buttons & (1u << JW_MOUSE_LEFT))
        web_buttons |= Web::UIEvents::MouseButton::Primary;
    if (buttons & (1u << JW_MOUSE_MIDDLE))
        web_buttons |= Web::UIEvents::MouseButton::Middle;
    if (buttons & (1u << JW_MOUSE_RIGHT))
        web_buttons |= Web::UIEvents::MouseButton::Secondary;
    return web_buttons;
}

static Web::UIEvents::KeyModifier jw_key_modifiers_to_web(u32 modifiers)
{
    Web::UIEvents::KeyModifier web_modifiers = Web::UIEvents::KeyModifier::Mod_None;
    if (modifiers & JW_KEY_MOD_SHIFT)
        web_modifiers |= Web::UIEvents::KeyModifier::Mod_Shift;
    if (modifiers & JW_KEY_MOD_CTRL)
        web_modifiers |= Web::UIEvents::KeyModifier::Mod_Ctrl;
    if (modifiers & JW_KEY_MOD_ALT)
        web_modifiers |= Web::UIEvents::KeyModifier::Mod_Alt;
    if (modifiers & JW_KEY_MOD_SUPER)
        web_modifiers |= Web::UIEvents::KeyModifier::Mod_Super;
    if (modifiers & JW_KEY_MOD_KEYPAD)
        web_modifiers |= Web::UIEvents::KeyModifier::Mod_Keypad;
    return web_modifiers;
}

static Web::UIEvents::KeyCode jw_key_to_web(int key)
{
    if (key >= 'a' && key <= 'z')
        return static_cast<Web::UIEvents::KeyCode>(Web::UIEvents::Key_A + (key - 'a'));
    if (key >= 'A' && key <= 'Z')
        return static_cast<Web::UIEvents::KeyCode>(Web::UIEvents::Key_A + (key - 'A'));
    if (key >= '0' && key <= '9')
        return static_cast<Web::UIEvents::KeyCode>(Web::UIEvents::Key_0 + (key - '0'));

    switch (key) {
    case '\b':
        return Web::UIEvents::Key_Backspace;
    case '\t':
        return Web::UIEvents::Key_Tab;
    case '\r':
    case '\n':
        return Web::UIEvents::Key_Return;
    case 27:
        return Web::UIEvents::Key_Escape;
    case ' ':
        return Web::UIEvents::Key_Space;
    default:
        return Web::UIEvents::Key_Invalid;
    }
}

static bool environment_is(char const* name, char const* expected)
{
    auto* value = getenv(name);
    return value && strcmp(value, expected) == 0;
}

static jw_proxy_t* create_jw_proxy(int width, int height, char const*& backend_name)
{
    if (environment_is("LADYBIRD_JW_BACKEND", "sdl")) {
        backend_name = "SDL";
        return jw_proxy_create_sdl(width, height, "Ladybird JW");
    }

#if defined(AK_OS_LINUX)
    backend_name = "fbdev";
    return jw_proxy_create_fbdev(getenv("LADYBIRD_JW_FBDEV"), width, height, "Ladybird JW");
#else
    backend_name = "SDL";
    return jw_proxy_create_sdl(width, height, "Ladybird JW");
#endif
}

ErrorOr<NonnullOwnPtr<WebContentView>> WebContentView::create()
{
    auto view = adopt_own(*new WebContentView);
    TRY(view->initialize());
    return view;
}

WebContentView::~WebContentView()
{
    if (m_poll_timer)
        m_poll_timer->stop();
    jw_context_destroy(m_context);
    jw_event_manager_destroy(m_mouse);
    jw_proxy_destroy(m_proxy);
}

ErrorOr<void> WebContentView::initialize()
{
    m_context = jw_context_create();
    if (!m_context)
        return Error::from_string_literal("Failed to create JingWei context");

    char const* backend_name = nullptr;
    m_proxy = create_jw_proxy(m_viewport_size.width(), m_viewport_size.height(), backend_name);
    if (!m_proxy) {
        if (strcmp(backend_name, "fbdev") == 0)
            return Error::from_string_literal("Failed to create JingWei fbdev proxy");
        return Error::from_string_literal("Failed to create JingWei SDL proxy");
    }

    auto* proxy = m_proxy;
    m_display = jw_display_create(m_viewport_size.width(), m_viewport_size.height(), proxy);
    if (!m_display) {
        if (strcmp(backend_name, "fbdev") == 0)
            return Error::from_string_literal("Failed to create JingWei fbdev display");
        return Error::from_string_literal("Failed to create JingWei SDL display");
    }

    m_proxy = nullptr;
    if (jw_context_register_display(m_context, m_display) < 0) {
        jw_display_destroy(m_display);
        m_display = nullptr;
        return Error::from_string_literal("Failed to register JingWei display");
    }

    m_mouse = jw_event_manager_create_mouse(proxy);
    if (!m_mouse || jw_display_bind_event_manager(m_display, m_mouse) != 0)
        return Error::from_string_literal("Failed to bind JingWei event manager");

    m_mouse = nullptr;
    jw_context_set_event_callback(m_context, on_jw_event, this);

    initialize_client(CreateNewClient::Yes);
    set_system_visibility_state(Web::HTML::VisibilityState::Visible);
    handle_resize();
    client().async_set_window_size(page_id(), viewport_size());

    on_ready_to_paint = [this] {
        present();
    };

    m_poll_timer = Core::Timer::create_repeating(8, [this] {
        poll_events();
    });
    m_poll_timer->start();
    return {};
}

void WebContentView::initialize_client(CreateNewClient create_new_client)
{
    ViewImplementation::initialize_client(create_new_client);
    update_palette();
    update_screen_rects();
}

void WebContentView::update_zoom()
{
    ViewImplementation::update_zoom();
    handle_resize();
}

Web::DevicePixelSize WebContentView::viewport_size() const
{
    return { m_viewport_size.width(), m_viewport_size.height() };
}

Gfx::IntPoint WebContentView::to_content_position(Gfx::IntPoint widget_position) const
{
    return widget_position;
}

Gfx::IntPoint WebContentView::to_widget_position(Gfx::IntPoint content_position) const
{
    return content_position;
}

void WebContentView::update_palette()
{
    auto theme_ini = MUST(Core::Resource::load_from_uri(MUST(String::formatted("resource://themes/Default.ini"))));
    auto theme_or_error = Gfx::load_system_theme(theme_ini->filesystem_path().to_byte_string());
    if (theme_or_error.is_error())
        return;

    set_preferred_color_scheme(Web::CSS::PreferredColorScheme::Light);
    client().async_update_system_theme(page_id(), theme_or_error.release_value());
}

void WebContentView::update_screen_rects()
{
    Vector<Web::DevicePixelRect> screen_rects;
    screen_rects.append(Web::DevicePixelRect { 0, 0, 1920, 1080 });
    client().async_update_screen_rects(page_id(), move(screen_rects), 0);
}

void WebContentView::present()
{
    Gfx::Bitmap const* bitmap = nullptr;
    Gfx::IntSize bitmap_size;

    if (m_client_state.has_usable_bitmap) {
        VERIFY(m_client_state.front_bitmap.shared_image_buffer);
        bitmap = m_client_state.front_bitmap.shared_image_buffer->bitmap().ptr();
        bitmap_size = m_client_state.front_bitmap.last_painted_size.to_type<int>();
    } else if (m_backup_shared_image_buffer) {
        bitmap = m_backup_shared_image_buffer->bitmap().ptr();
        bitmap_size = m_backup_bitmap_size.to_type<int>();
    }

    if (!bitmap || !m_display)
        return;

    auto width = bitmap_size.width() > 0 ? bitmap_size.width() : bitmap->width();
    auto height = bitmap_size.height() > 0 ? bitmap_size.height() : bitmap->height();
    auto* pixels = const_cast<u8*>(bitmap->scanline_u8(0));
    auto* buffer = jw_buffer_wrap_pixels(width,
        height,
        bitmap->pitch(),
        JW_PIXEL_FORMAT_ARGB8888,
        pixels,
        nullptr,
        nullptr);
    if (!buffer)
        return;

    jw_display_present_buffer(m_display, buffer);
    jw_buffer_destroy(buffer);
}

void WebContentView::poll_events()
{
    if (m_context)
        jw_context_poll(m_context, 0);
}

void WebContentView::resize(int width, int height)
{
    if (width <= 0 || height <= 0)
        return;
    if (m_viewport_size == Gfx::IntSize { width, height })
        return;

    m_viewport_size = { width, height };
    client().async_set_window_size(page_id(), viewport_size());
    handle_resize();
    client().async_did_update_window_rect(page_id());
}

void WebContentView::handle_jw_event(jw_event_t const& event)
{
    switch (event.type) {
    case JW_EVENT_QUIT:
        Core::EventLoop::current().quit(0);
        break;
    case JW_EVENT_MOUSE_MOVE: {
        Web::MouseEvent mouse_event {
            .type = Web::MouseEvent::Type::MouseMove,
            .position = { event.data.mouse_move.x, event.data.mouse_move.y },
            .screen_position = { event.data.mouse_move.x, event.data.mouse_move.y },
            .button = Web::UIEvents::MouseButton::None,
            .buttons = jw_mouse_buttons_to_web(event.data.mouse_move.buttons),
            .modifiers = Web::UIEvents::KeyModifier::Mod_None,
            .wheel_delta_x = 0,
            .wheel_delta_y = 0,
            .click_count = 0,
            .browser_data = {},
        };
        enqueue_input_event(move(mouse_event));
        break;
    }
    case JW_EVENT_MOUSE_LEAVE: {
        Web::MouseEvent mouse_event {
            .type = Web::MouseEvent::Type::MouseLeave,
            .position = { 0, 0 },
            .screen_position = { 0, 0 },
            .button = Web::UIEvents::MouseButton::None,
            .buttons = Web::UIEvents::MouseButton::None,
            .modifiers = Web::UIEvents::KeyModifier::Mod_None,
            .wheel_delta_x = 0,
            .wheel_delta_y = 0,
            .click_count = 0,
            .browser_data = {},
        };
        enqueue_input_event(move(mouse_event));
        break;
    }
    case JW_EVENT_MOUSE_KEY: {
        auto button = jw_mouse_button_to_web(event.data.mouse_key.button);
        Web::MouseEvent mouse_event {
            .type = event.data.mouse_key.state == JW_BUTTON_DOWN
                ? Web::MouseEvent::Type::MouseDown
                : Web::MouseEvent::Type::MouseUp,
            .position = { event.data.mouse_key.x, event.data.mouse_key.y },
            .screen_position = { event.data.mouse_key.x, event.data.mouse_key.y },
            .button = button,
            .buttons = jw_mouse_buttons_to_web(event.data.mouse_key.buttons),
            .modifiers = Web::UIEvents::KeyModifier::Mod_None,
            .wheel_delta_x = 0,
            .wheel_delta_y = 0,
            .click_count = 1,
            .browser_data = {},
        };
        enqueue_input_event(move(mouse_event));
        break;
    }
    case JW_EVENT_MOUSE_WHEEL: {
        Web::MouseEvent mouse_event {
            .type = Web::MouseEvent::Type::MouseWheel,
            .position = { event.data.mouse_wheel.mouse_x, event.data.mouse_wheel.mouse_y },
            .screen_position = { event.data.mouse_wheel.mouse_x, event.data.mouse_wheel.mouse_y },
            .button = Web::UIEvents::MouseButton::None,
            .buttons = Web::UIEvents::MouseButton::None,
            .modifiers = Web::UIEvents::KeyModifier::Mod_None,
            .wheel_delta_x = static_cast<double>(event.data.mouse_wheel.x),
            .wheel_delta_y = static_cast<double>(event.data.mouse_wheel.y),
            .click_count = 0,
            .browser_data = {},
        };
        enqueue_input_event(move(mouse_event));
        break;
    }
    case JW_EVENT_KEY: {
        Web::KeyEvent key_event {
            .type = event.data.key.state == JW_KEY_DOWN
                ? Web::KeyEvent::Type::KeyDown
                : Web::KeyEvent::Type::KeyUp,
            .key = jw_key_to_web(event.data.key.key),
            .modifiers = jw_key_modifiers_to_web(event.data.key.modifiers),
            .code_point = event.data.key.key >= 0 && event.data.key.key < 0x110000
                ? static_cast<u32>(event.data.key.key)
                : 0,
            .repeat = event.data.key.repeat != 0,
            .browser_data = {},
        };
        enqueue_input_event(move(key_event));
        break;
    }
    case JW_EVENT_RESIZE:
        resize(event.data.resize.width, event.data.resize.height);
        break;
    case JW_EVENT_FOCUS:
        client().async_set_has_focus(page_id(), event.data.focus.focused);
        break;
    case JW_EVENT_VISIBILITY:
        set_system_visibility_state(event.data.visibility.visible
                ? Web::HTML::VisibilityState::Visible
                : Web::HTML::VisibilityState::Hidden);
        break;
    default:
        break;
    }
}

void WebContentView::on_jw_event(jw_context_t*, jw_display_t*, jw_event_t const* event, void* user_data)
{
    if (!event || !user_data)
        return;
    static_cast<WebContentView*>(user_data)->handle_jw_event(*event);
}

}
