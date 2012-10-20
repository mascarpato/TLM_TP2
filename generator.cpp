/********************************************************************
 * Copyright (C) 2009 by Ensimag and Verimag                        *
 * Initial author: Matthieu Moy                                     *
 ********************************************************************/

#include "generator.h"

using namespace std;

void Generator::thread(void) {
	ensitlm::data_t val = 1;
	ensitlm::addr_t addr = 0x10000000;
	int offset = 0;
	tlm::tlm_response_status status;
	while (true) {
		cout << "Entrer un offset" << endl;
		cin >> offset;;
		cout << "je vais envoyer : " << std::dec << val << endl;
		status = socket.write(addr+offset*4, val);
		cout << "Status : " << status << endl;
	}
}

Generator::Generator(sc_core::sc_module_name name)
	: sc_core::sc_module(name) {
	
	SC_THREAD(thread);
}
