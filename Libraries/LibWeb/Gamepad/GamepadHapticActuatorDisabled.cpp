/*
 * Copyright (c) 2026, Ladybird contributors
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/TypeCasts.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/DOM/DocumentObserver.h>
#include <LibWeb/Gamepad/GamepadHapticActuator.h>
#include <LibWeb/HTML/Window.h>
#include <LibWeb/WebIDL/ExceptionOr.h>
#include <LibWeb/WebIDL/Promise.h>

namespace Web::Gamepad {

GC_DEFINE_ALLOCATOR(GamepadHapticActuator);

GC::Ref<GamepadHapticActuator> GamepadHapticActuator::create(JS::Realm& realm, GC::Ref<Gamepad> gamepad)
{
    auto observer = realm.create<DOM::DocumentObserver>(realm, as<HTML::Window>(realm.global_object()).associated_document());
    return realm.create<GamepadHapticActuator>(realm, gamepad, observer);
}

GamepadHapticActuator::GamepadHapticActuator(JS::Realm& realm, GC::Ref<Gamepad> gamepad, GC::Ref<DOM::DocumentObserver> observer)
    : Bindings::PlatformObject(realm)
    , m_gamepad(gamepad)
    , m_document_became_hidden_observer(observer)
{
}

GamepadHapticActuator::~GamepadHapticActuator() = default;

void GamepadHapticActuator::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GamepadHapticActuator);
    Base::initialize(realm);
}

void GamepadHapticActuator::visit_edges(Cell::Visitor& visitor)
{
    Base::visit_edges(visitor);
    visitor.visit(m_gamepad);
    visitor.visit(m_document_became_hidden_observer);
    visitor.visit(m_playing_effect_promise);
    visitor.visit(m_playing_effect_timer);
}

GC::Ref<WebIDL::Promise> GamepadHapticActuator::play_effect(Bindings::GamepadHapticEffectType, Bindings::GamepadEffectParameters const&)
{
    return WebIDL::create_rejected_promise_from_exception(realm(), WebIDL::NotSupportedError::create(realm(), "Gamepad support is disabled"_utf16));
}

GC::Ref<WebIDL::Promise> GamepadHapticActuator::reset()
{
    return WebIDL::create_rejected_promise_from_exception(realm(), WebIDL::NotSupportedError::create(realm(), "Gamepad support is disabled"_utf16));
}

void GamepadHapticActuator::document_became_hidden() { }
void GamepadHapticActuator::issue_haptic_effect(Bindings::GamepadHapticEffectType, Bindings::GamepadEffectParameters const&, GC::Ref<GC::Function<void()>>) { }

bool GamepadHapticActuator::stop_haptic_effects()
{
    return false;
}

void GamepadHapticActuator::clear_playing_effect_timers() { }

}
