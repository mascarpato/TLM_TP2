/********************************************************************
 * Copyright (C) 2009 by Ensimag and Verimag                        *
 * Initial author: Matthieu Moy                                     *
 ********************************************************************/

#ifndef GENERATOR_H
#define GENERATOR_H

#include "ensitlm.h"
#include "bus.h"

SC_MODULE(Generator) {
        ensitlm::initiator_socket<Generator> socket;
	sc_core::sc_in<bool> interrupt;
        void thread(void);

        SC_HAS_PROCESS(Generator);
        Generator(sc_core::sc_module_name name);
        
   //     ~Generator();
 
};

#endif
