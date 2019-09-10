/*
 * Copyright (c) 2019 EKA2L1 Team
 * 
 * This file is part of EKA2L1 project
 * (see bentokun.github.com/EKA2L1).
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <epoc/services/window/classes/winbase.h>
#include <epoc/services/window/common.h>

#include <common/linked.h>

namespace eka2l1::epoc {
    struct graphic_context;
    struct window_group;

    struct window_user_base: public epoc::window {
        explicit window_user_base(window_server_client_ptr client, screen *scr, window *parent, const window_kind kind);
        virtual std::uint32_t redraw_priority(int *shift = nullptr) = 0;
    };

    struct window_top_user: public window_user_base {
        explicit window_top_user(window_server_client_ptr client, screen *scr, window *parent);
        std::uint32_t redraw_priority(int *shift = nullptr) override;
    };

    struct window_user : public window_user_base {
        epoc::display_mode dmode;
        epoc::window_type win_type;

        // The position
        eka2l1::vec2 pos{ 0, 0 };
        eka2l1::vec2 size{ 0, 0 };

        bool resize_needed;

        common::roundabout attached_contexts;

        std::uint32_t clear_color;
        std::uint32_t filter;

        eka2l1::vec2 cursor_pos;
        eka2l1::rect irect;

        std::uint32_t redraw_evt_id;
        std::uint64_t driver_win_id;

        int shadow_height;

        enum {
            shadow_disable = 1 << 0,
            active = 1 << 1,
            visible = 1 << 2,
            allow_pointer_grab = 1 << 3
        };

        std::uint32_t flags;

        explicit window_user(window_server_client_ptr client, screen *scr, window *parent,
            const epoc::window_type type_of_window, const epoc::display_mode dmode);

        std::uint32_t redraw_priority(int *shift = nullptr) override;

        /**
         * \brief Set window extent in screen space.
         * 
         * \param top   The position of the window on the screen coords, in pixels.
         * \param size  The size of the window, in pixel.
         */
        void set_extent(const eka2l1::vec2 &top, const eka2l1::vec2 &size);

        bool is_visible() const {
            return flags & visible;
        }

        /**
         * \brief Set window visibility.
         * 
         * This will trigger a screen redraw if the visibility is changed.
         */
        void set_visible(const bool vis);

        void queue_event(const epoc::event &evt) override;

        // ===================== OPCODE IMPLEMENTATIONS ===========================
        void begin_redraw(service::ipc_context &context, ws_cmd &cmd);
        void end_redraw(service::ipc_context &context, ws_cmd &cmd);
        void execute_command(service::ipc_context &context, ws_cmd &cmd) override;

        epoc::window_group *get_group() {
            return reinterpret_cast<epoc::window_group*>(parent);
        }
    };
}
