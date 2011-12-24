/*
 * Pinguino Basic Version 0.1
 * Copyright (c) 2011, Régis Blanchot
 *
 * Pinguino Basic Engine is originally based on uBasic
 * Copyright (c) 2006, Adam Dunkels
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */


#ifndef __COMMANDS_H__
#define __COMMANDS_H__

/*	TODO:

	clear [flash]           clear ram [and flash] variables
	cls                     clear terminal screen
	cont [line]             continue program from stop
	dir                     list saved programs
	memory                  print memory usage
	new                     erase code ram and flash memories
	renumber [line]         renumber program lines (and save)
	reset                   reset the MCU!
	save [name]             save code ram to flash memory

	auto [line]             automatically number program lines
	profile ([line][-[line]]|subname) display profile info
	purge name              purge saved program
	undo                    undo code changes since last save
	upgrade                 upgrade pbasic firmware!
	uptime                  print time since last reset
*/

/*---------------------------------------------------------------------------*/

//	load name               load saved program
void cmd_load(void);
//	list ([line][-[line]]|subname) list program lines
void cmd_list(void);
//	run [line]              run program
void cmd_run(void);
// debug on|off				(de)activate debug mode
void cmd_debug(void);
//	edit line               edit program line
void cmd_edit(void);
//	delete ([line][-[line]]|subname) delete program lines
void cmd_delete(void);
//	help [topic]            online help
void cmd_help(void);
/*---------------------------------------------------------------------------*/

#endif /* __COMMANDS_H__ */