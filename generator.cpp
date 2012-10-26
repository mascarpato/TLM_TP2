/********************************************************************
 * Copyright (C) 2009 by Ensimag and Verimag                        *
 * Initial author: Matthieu Moy                                     *
 ********************************************************************/

#include "generator.h"

using namespace std;
using namespace ensitlm;

			/* COMMENTS and IDENTATION!!!!!!!!!!!!!!!!! */

void Generator::thread(void) {
	data_t from_rom = 0;
	data_t to_mem = 0;
	addr_t addr_mem = ADDR_MEM_VIDEO;
	addr_t addr_rom = ADDR_ROM;
	
	
	data_t offset_rom = 0;
	data_t offset_mem = 0;
	for( ; offset_rom < ROM_SIZE; offset_rom += sizeof(data_t)){
	 	socket.read(addr_rom + offset_rom, from_rom);
		data_t mask = 0;
		for(int j=0; j < 8; j++){
			mask = (from_rom & 0xF0000000);
			mask >>= (j % 4) * 8;
			to_mem += mask;
			from_rom <<= 4;
			
			if((j+1)%4 == 0){
				socket.write(addr_mem + offset_mem,to_mem);
				offset_mem += sizeof(data_t);
				to_mem = 0;
				}
				
		}
	}
	 
	 socket.write(ADDR_LCDC + LCDC_ADDR_REG, ADDR_MEM_VIDEO);
	 
	//start the LCD Controller
	socket.write(ADDR_LCDC + LCDC_START_REG, 1);
	
	while(true){
	
		//wait for the interruption be asserted
		sc_core::wait(interrupt.posedge_event());
		socket.write(ADDR_LCDC + LCDC_INT_REG, 0);
		
		int width = 320/sizeof(data_t);
		int height = 240;
		
		int i=0;
		for(; i < width ; i++){
			//keep the lowest pixel
			data_t pixel0;
			socket.read(addr_mem + i*sizeof(data_t),pixel0);
			
			int j=1;
			for(; j < height ; j++){
				data_t moved_data;
				socket.read(addr_mem + ((j)*width + i)*sizeof(data_t), moved_data);
				socket.write(addr_mem + ((j-1)*width + i)*sizeof(data_t), moved_data);
			}
			socket.write(addr_mem + ((j-1)*width + i)*sizeof(data_t), pixel0);
			
		}
		//start the LCD Controller
		socket.write(ADDR_LCDC + LCDC_START_REG, 1);
	
	
	}
	
	
	
}

Generator::Generator(sc_core::sc_module_name name)
	: sc_core::sc_module(name) {
	
	SC_THREAD(thread);
}
