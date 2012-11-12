/********************************************************************
 * Copyright (C) 2009 by Ensimag and Verimag                        *
 * Initial author: Matthieu Moy					    *
 * Modified by: Mauricio Altieri and João Leidens                   *
 ********************************************************************/

#include "ensitlm.h"
#include "generator.h"
#include "memory.h"
#include "bus.h"
#include "LCDC.h"
#include "ROM.h"

using namespace sc_core;

int sc_main (int argc, char ** argv) {
	(void)argc;
	(void)argv;

	sc_signal<bool,SC_MANY_WRITERS> irq_signal("IRQ");
        Generator g("Generator1");
        Bus b("Bus");
	LCDC l("LCD_Control",sc_time(1, SC_SEC));
	ROM r("ROM");

	/* Instantiating and mapping memory in the bus 
	 * 5 kB for the embedded SW + 320*240*1 Bytes for the LCD display
	 * Total = 81920 kB (0x14000) 
	 */
        Memory m("Memory",0x14000);
        b.map(m.target, 0x10000000, 0x14000);

	/* Mapping the 3 LCDC registers (4 Bytes each one) in the bus */
	b.map(l.target_socket, 0x00000000, 0x0C);
	
	/* Mapping the ROM in the bus */
	b.map(r.socket, 0x20000000, 0x9600);

        /* Connecting components to the bus
         * Generator as both master and slave 
         */
        g.socket.bind(b.target);
        b.initiator.bind(m.target);
	
	/* LCDC as both master and slave */
	l.initiator_socket.bind(b.target);
	b.initiator.bind(l.target_socket);
	
	/* ROM as slave */
	b.initiator.bind(r.socket);
	
	/* Connecting interrupt signal between LCDC (master) and generator (slave) */
	g.interrupt.bind(irq_signal);
	l.display_int.bind(irq_signal);

	sc_start();
        return 0;
}
