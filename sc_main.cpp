/********************************************************************
 * Copyright (C) 2009 by Ensimag and Verimag                        *
 * Initial author: Matthieu Moy                                     *
 ********************************************************************/

#include "ensitlm.h"
#include "generator.h"
#include "memory.h"
#include "bus.h"
#include "LCDC.h"

using namespace sc_core;

int sc_main (int argc, char ** argv) {
	(void)argc; (void)argv;

	sc_signal<bool> interrupt;
        Generator g("Generator1");
        Bus b("Bus");
	LCDC l("LCD Control",sc_time(1, SC_SEC));

	/* instantiate and map memory in the bus */
	/* 5 kB for the embedded SW + 320*240*1 Bytes for the LCD display */
	/* total = 81920 kB (0x14000) */
        Memory m("Memory",0x14000);
        b.map(m.target, 0x10000000, 0x14000);

        /* connect components to the bus */
        g.socket.bind(b.target);
        b.initiator.bind(m.target);
	l.initiator_socket.bind(b.target);
	b.initiator.bind(l.target_socket);
	
	/* connect interrupt signal between generator and LCDC */
	g.interrupt.bind(interrupt);
	l.display_int.bind(interrupt);

        /* and start simulation */
	sc_start();
        return 0;
}
