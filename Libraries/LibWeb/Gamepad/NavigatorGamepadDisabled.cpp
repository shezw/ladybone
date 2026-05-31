/*
 * Copyright (c) 2026, Ladybird contributors
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/TypeCasts.h>
#include <LibWeb/Gamepad/NavigatorGamepad.h>
#include <LibWeb/HTML/Navigator.h>

namespace Web::Gamepad {

WebIDL::ExceptionOr<GC::RootVector<GC::Ptr<Gamepad>>> NavigatorGamepadPartial::get_gamepads()
{
    return GC::RootVector<GC::Ptr<Gamepad>> {};
}

void NavigatorGamepadPartial::visit_edges(GC::Cell::Visitor& visitor)
{
    visitor.visit(m_gamepads);
}

size_t NavigatorGamepadPartial::select_an_unused_gamepad_index(Badge<Gamepad>)
{
    return 0;
}

void NavigatorGamepadPartial::handle_gamepad_connected(SDL_JoystickID) { }
void NavigatorGamepadPartial::handle_gamepad_updated(Badge<EventHandler>, SDL_JoystickID) { }
void NavigatorGamepadPartial::handle_gamepad_disconnected(Badge<EventHandler>, SDL_JoystickID) { }
void NavigatorGamepadPartial::check_for_connected_gamepads() { }

void NavigatorGamepadPartial::set_has_gamepad_gesture(Badge<Gamepad>, bool value)
{
    m_has_gamepad_gesture = value;
}

GC::RootVector<GC::Ptr<Gamepad>> NavigatorGamepadPartial::gamepads(Badge<Gamepad>) const
{
    return GC::RootVector<GC::Ptr<Gamepad>> { as<HTML::Navigator>(*this).m_gamepads };
}

}
