/*
 * Copyright (c) 2019 EKA2L1 Team.
 * 
 * This file is part of EKA2L1 project 
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

#include <console/state.h>
#include <common/log.h>
#include <gdbstub/gdbstub.h>

namespace eka2l1::desktop {
    void emulator::stage_one() {
        // Initialize the logger
        logger = std::make_shared<imgui_logger>();
        log::setup_log(logger);
    
        // Start to read the configs
        conf.deserialize();

        // Initialize an empty root.
        symsys = std::make_unique<eka2l1::system>(nullptr, nullptr, &conf);

        // Make debugger. Go watch Case Closed.
        debugger = std::make_unique<eka2l1::imgui_debugger>(symsys.get(), logger);

        // Set up system.
        symsys->set_jit_type(conf.cpu_backend == 0 ? arm_emulator_type::unicorn
                                                   : arm_emulator_type::dynarmic);
        
        // TODO
        // symsys->set_debugger(debugger);

        symsys->init();

        symsys->set_device(conf.device);
        symsys->mount(drive_c, drive_media::physical, conf.storage + "/drives/c/", io_attrib::internal);
        symsys->mount(drive_e, drive_media::physical, conf.storage + "/drives/e/", io_attrib::removeable);

        if (conf.enable_gdbstub) {
            symsys->get_gdb_stub()->set_server_port(conf.gdb_port);
            symsys->get_gdb_stub()->init(symsys.get());
            symsys->get_gdb_stub()->toggle_server(true);
        }
    }

    void emulator::stage_two() {
        bool res = symsys->load_rom(conf.storage + conf.rom_path);

        // Mount the drive Z after the ROM was loaded. The ROM load than a new FS will be
        // created for ROM purpose.
        symsys->mount(drive_z, drive_media::rom,
            conf.storage + "/drives/e/", io_attrib::internal | io_attrib::write_protected);
    }
}