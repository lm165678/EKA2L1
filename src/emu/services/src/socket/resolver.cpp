/*
 * Copyright (c) 2020 EKA2L1 Team
 * 
 * This file is part of EKA2L1 project.
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

#include <services/socket/resolver.h>
#include <services/socket/socket.h>

#include <utils/err.h>

namespace eka2l1::epoc::socket {
    socket_host_resolver::socket_host_resolver(socket_client_session *parent, const std::uint32_t addr_fam,
        const std::uint32_t protocol, connection *conn)
        : socket_subsession(parent)
        , addr_fam_(addr_fam)
        , protocol_(protocol)
        , conn_(conn) {
    }

    epoc::socket::host &socket_host_resolver::lookup_host() {
        socket_server *grand_parent = parent_->server<socket_server>();
        return grand_parent->host_by_info(addr_fam_, protocol_);
    }

    std::u16string socket_host_resolver::host_name() {
        return lookup_host().name_;
    }

    void socket_host_resolver::set_host_name(const std::u16string &new_name) {
        lookup_host().name_ = new_name;
    }
    
    void socket_host_resolver::get_host_name(service::ipc_context *ctx) {
        ctx->write_arg(0, host_name());
        ctx->complete(epoc::error_none);
    }

    void socket_host_resolver::set_host_name(service::ipc_context *ctx) {
        std::optional<std::u16string> new_name = ctx->get_argument_value<std::u16string>(0);

        if (!new_name) {
            ctx->complete(epoc::error_argument);
            return;
        }

        set_host_name(new_name.value());
        ctx->complete(epoc::error_none);
    }

    void socket_host_resolver::close(service::ipc_context *ctx) {
        parent_->subsessions_.remove(id_);
        ctx->complete(epoc::error_none);
    }
    
    void socket_host_resolver::dispatch(service::ipc_context *ctx) {
        if (parent_->is_oldarch()) {
            switch (ctx->msg->function) {
            case socket_old_hr_get_host_name:
                get_host_name(ctx);
                return;

            case socket_old_hr_set_host_name:
                set_host_name(ctx);
                return;

            case socket_old_hr_close:
                close(ctx);
                return;

            default:
                break;
            }
        } else {
            switch (ctx->msg->function) {
            default:
                break;
            }
        }

        LOG_ERROR(SERVICE_ESOCK, "Unimplemented socket host resolver opcode: {}", ctx->msg->function);
    }
}