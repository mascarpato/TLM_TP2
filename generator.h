/********************************************************************
 * Copyright (C) 2009 by Ensimag and Verimag                        *
 * Initial author: Matthieu Moy					    *
 * Modified by: Mauricio Altieri and João Leidens                   *
 ********************************************************************/

#ifndef GENERATOR_H
#define GENERATOR_H

#include "ensitlm.h"
#include "bus.h"
#include "LCDC_registermap.h"

#define ADDR_ROM	0x20000000
#define ADDR_MEM_SW 	0x10000000
#define ADDR_MEM_VIDEO 	0x10001400
#define ADDR_LCDC 	0x00000000
#define IMG_SIZE 	320*240
#define ROM_SIZE	0x9600
#define WHITE_CODE 	0xFFFFFFFF

SC_MODULE(Generator) {
        ensitlm::initiator_socket<Generator> socket;
	sc_core::sc_in<bool> interrupt;
        void thread(void);

        SC_HAS_PROCESS(Generator);
        Generator(sc_core::sc_module_name name);
 
};

#endif
