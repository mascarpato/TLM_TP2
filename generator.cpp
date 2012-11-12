/********************************************************************
 * Copyright (C) 2009 by Ensimag and Verimag                        *
 * Initial author: Matthieu Moy					    *
 * Modified by: Mauricio Altieri and João Leidens                   *
 ********************************************************************/

#include "generator.h"

using namespace std;
using namespace ensitlm;

void Generator::thread(void) {
	data_t from_rom = 0;
	data_t to_mem = 0;	
	data_t offset_rom = 0;
	data_t offset_mem = 0;
	
	/* Reads the image stored in the ROM (4-bits pixels), encodes it  
	 * according to the video encoding used by the LCDC (8-bits pixels) 
	 * and stores it in the memory. 
	 */
	for( ; offset_rom < ROM_SIZE; offset_rom += sizeof(data_t)) {
	 	socket.read(ADDR_ROM + offset_rom, from_rom);
		data_t mask = 0;
		
		/* Runs the word from ROM getting each one of its 8 4-bits pixels
		 * through a sequence of AND (with a mask) and shift left (4x). 
		 * At the same time, generates another word with 4 8-bits pixels
		 * through a sequence of ADD and shift right (8x).
		 */
		int j = 0;
		for( ; j < 8; j++) {
			/* Gets only the first pixel (4 bits) */
			mask = (from_rom & 0xF0000000);
			mask >>= (j % 4) * 8;
			to_mem += mask;
			from_rom <<= 4;
			
			/* After 4 executions, a word filled with 8-bits pixels
			 * is ready to be stored in the memory.
			 */
			if ((j + 1) % 4 == 0) {
				socket.write(ADDR_MEM_VIDEO + offset_mem,
								to_mem);
				offset_mem += sizeof(data_t);
				to_mem = 0;
			}		
		}
	}
	
	/* Indicating the image starting address to the LCD Controller */
	socket.write(ADDR_LCDC + LCDC_ADDR_REG, ADDR_MEM_VIDEO);
	 
	/* Starting the LCD Controller */
	socket.write(ADDR_LCDC + LCDC_START_REG, 1);
	
	while (true) {	
		/* Waiting for the LCD Controller assertion */
		sc_core::wait(interrupt.posedge_event());

		/* Deasserting the LCDC Interrupt Register */
		socket.write(ADDR_LCDC + LCDC_INT_REG, 0);
		
		/* Image size in words (width * height) */
		int width = 320/sizeof(data_t);
		int height = 240;
		
		/* This portion of code rolls the entire image in the memory
		 * one row above, moving the highest row to the lowest position.
		 * There are 2 nested lops, the outter one runs through each
		 * column while the inner one through each row.
		*/
		int i = 0;
		for( ; i < width ; i++) {
			/* Keeping the highest pixel in this column */
			data_t pixel0;
			socket.read(ADDR_MEM_VIDEO + (i * sizeof(data_t)), 
							pixel0);
			/* Shifting each pixel one row above */
			int j = 1;
			for( ; j < height; j++) {
				data_t moved_data;
				socket.read(ADDR_MEM_VIDEO + ((j * width) + i) 
						* sizeof(data_t), moved_data);
						
				socket.write(ADDR_MEM_VIDEO + 
					(((j - 1) * width) + i)	* sizeof(data_t),
					moved_data);
			}
			
			/* Storing the former highest pixel as the lowest pixel */
			socket.write(ADDR_MEM_VIDEO + ((j-1)*width + i) 
					* sizeof(data_t), pixel0);
			
		}
		
		/* Starting the LCD Controller */
		socket.write(ADDR_LCDC + LCDC_START_REG, 1);	
	}
		
}

Generator::Generator(sc_core::sc_module_name name)
	: sc_core::sc_module(name) {
	
	SC_THREAD(thread);
}
