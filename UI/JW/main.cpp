/*
 * Copyright (c) 2026, Ladybird contributors
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibMain/Main.h>
#include <UI/JW/Application.h>

ErrorOr<int> ladybird_main(Main::Arguments arguments)
{
    AK::set_rich_debug_enabled(true);

    auto app = TRY(Ladybird::Application::create(arguments));
    if (!Ladybird::Application::browser_options().headless_mode.has_value())
        TRY(app->create_window(Ladybird::Application::browser_options().urls));

    return app->execute();
}
