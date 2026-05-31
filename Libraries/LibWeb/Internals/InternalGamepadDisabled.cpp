/*
 * Copyright (c) 2026, Ladybird contributors
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/Bindings/InternalGamepad.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/Internals/InternalGamepad.h>
#include <LibWeb/Internals/Internals.h>

namespace Web::Internals {

GC_DEFINE_ALLOCATOR(InternalGamepad);

static constexpr Array<i32, 15> BUTTONS {};
static constexpr Array<i32, 4> AXES {};
static constexpr Array<i32, 2> TRIGGERS {};

InternalGamepad::InternalGamepad(JS::Realm& realm, GC::Ref<Internals> internals)
    : PlatformObject(realm)
    , m_internals(internals)
{
}

InternalGamepad::~InternalGamepad() = default;

void InternalGamepad::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(InternalGamepad);
    Base::initialize(realm);
}

void InternalGamepad::visit_edges(Cell::Visitor& visitor)
{
    Base::visit_edges(visitor);
    visitor.visit(m_received_rumble_effects);
    visitor.visit(m_received_rumble_trigger_effects);
    visitor.visit(m_internals);
}

void InternalGamepad::finalize()
{
    Base::finalize();
}

Array<i32, 15> const& InternalGamepad::buttons()
{
    return BUTTONS;
}

Array<i32, 4> const& InternalGamepad::axes()
{
    return AXES;
}

Array<i32, 2> const& InternalGamepad::triggers()
{
    return TRIGGERS;
}

void InternalGamepad::set_button(int, bool) { }
void InternalGamepad::set_axis(int, short) { }

GC::RootVector<JS::Object*> InternalGamepad::get_received_rumble_effects() const
{
    return {};
}

GC::RootVector<JS::Object*> InternalGamepad::get_received_rumble_trigger_effects() const
{
    return {};
}

void InternalGamepad::received_rumble(u16, u16) { }
void InternalGamepad::received_rumble_triggers(u16, u16) { }

void InternalGamepad::disconnect()
{
    m_internals->disconnect_virtual_gamepad(*this);
}

}
