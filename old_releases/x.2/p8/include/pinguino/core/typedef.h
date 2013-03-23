/*	----------------------------------------------------------------------------
	typedef.c
	Régis Blanchot
	--------------------------------------------------------------------------*/

#ifndef __TYPEDEF_H
#define __TYPEDEF_H

/*	----------------------------------------------------------------------------
	pinguino types
	--------------------------------------------------------------------------*/

	typedef signed char				s8;
	typedef signed short int		s16;
	typedef signed long int			s32;
	typedef signed long long 		s64;

	typedef unsigned char			u8;
	typedef unsigned short int		u16;
	typedef unsigned long int		u32;
	typedef unsigned long long 	u64;
	
/*	----------------------------------------------------------------------------
	avr-gcc types
	--------------------------------------------------------------------------*/

	typedef unsigned char			byte;
	typedef unsigned char			BOOL;//bool;					// not compatible with c++
	typedef unsigned char			boolean;	
	typedef signed char 				int8_t;
	typedef unsigned char 			uint8_t;

	typedef unsigned int				word;	
	typedef int 						int16_t;
	typedef unsigned int 			uint16_t;

	typedef long 						int32_t;
	typedef unsigned long 			uint32_t;

	typedef long long 				int64_t;
	typedef unsigned long long 	uint64_t;

#endif
