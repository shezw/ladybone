/*
 * Copyright (c) 2026, Ladybird contributors
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/Gamepad/Gamepad.h>
#include <LibWeb/Gamepad/GamepadHapticActuator.h>

namespace Web::Gamepad {

GC_DEFINE_ALLOCATOR(Gamepad);

GC::Ref<Gamepad> Gamepad::create(JS::Realm& realm, SDL_JoystickID sdl_joystick_id)
{
    return realm.create<Gamepad>(realm, sdl_joystick_id);
}

Gamepad::Gamepad(JS::Realm& realm, SDL_JoystickID sdl_joystick_id)
    : PlatformObject(realm)
    , m_sdl_joystick_id(sdl_joystick_id)
{
}

void Gamepad::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(Gamepad);
    Base::initialize(realm);
}

void Gamepad::visit_edges(Cell::Visitor& visitor)
{
    Base::visit_edges(visitor);
    visitor.visit(m_buttons);
    visitor.visit(m_vibration_actuator);
}

void Gamepad::finalize()
{
    Base::finalize();
}

GC::Ref<GamepadHapticActuator> Gamepad::vibration_actuator() const
{
    return *m_vibration_actuator;
}

void Gamepad::set_connected(Badge<NavigatorGamepadPartial>, bool value)
{
    m_connected = value;
}

void Gamepad::set_exposed(Badge<NavigatorGamepadPartial>, bool value)
{
    m_exposed = value;
}

void Gamepad::set_timestamp(Badge<NavigatorGamepadPartial>, HighResolutionTime::DOMHighResTimeStamp value)
{
    m_timestamp = value;
}

void Gamepad::initialize_axes() { }
void Gamepad::initialize_buttons() { }
void Gamepad::select_a_mapping() { }
void Gamepad::map_and_normalize_axes() { }
void Gamepad::map_and_normalize_buttons() { }
void Gamepad::update_gamepad_state(Badge<NavigatorGamepadPartial>) { }

bool Gamepad::contains_gamepad_user_gesture()
{
    return false;
}

}
