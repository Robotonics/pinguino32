/*	----------------------------------------------------------------------------
	FILE:			ctype.c
	PROJECT:		pinguino32X
	PURPOSE:		alternative port of libc
	PROGRAMER:		regis blanchot <rblanchot@gmail.com>
	FIRST RELEASE:	7 feb. 2011
	LAST RELEASE:	7 feb. 2011
	----------------------------------------------------------------------------
	TODO :
	----------------------------------------------------------------------------
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
	--------------------------------------------------------------------------*/

/*
		int isalnum (int);
		int isalpha (int);
		int iscntrl (int);
		int isdigit (int);
		int isgraph (int);
		int islower (int);
		int isprint (int);
		int ispunct (int);
		int isspace (int);
		int isupper (int);
		int isxdigit(int);
		int tolower (int);
		int toupper (int);
		int isblank (int);
		int isascii (int);
		int toascii (int);
*/

#ifndef __CTYPE_C
	#define __CTYPE_C

int isalnum(int c)
{
	if ((isalpha(c)) || (isdigit(c)))
		return 1;
	return 0;
}

int isalpha(int c)
{
	if ((isupper(c)) || (islower(c)))
	  return 1;
	return 0;
}

/* Not sure which characters are actually control characters in Cybikoland */
int iscntrl(int c)
{
	if ((c >= 0) && (c <= 31))
		return 1;
	return 0;
}

int isdigit(int c)
{
	if ((c >= 48) && (c <= 57))
		return 1;
	return 0;
}

/*  Not quite sure this one is correct either */
int isgraph(int c)
{
	if ((c >= 33) && (c <= 255))
	  return 1;
	return 0;
}


int islower(int c)
{
	if ((c >= 97) && (c <= 122))
		return 1;
	return 0;
}

/*  Not quite sure this one is correct either */
int isprint(int c)
{
	if ((c >= 32) && (c <= 255))
	  return 1;
	return 0;
}


int ispunct(int c)
{
	if ((!isalnum(c)) && (!isspace(c)) && (isprint(c)))
		return 1;
	return 0;
}

int isspace(int c)
{
	if (c == 32)
		return 1;
	return 0;
}

int isupper(int c)
{
	if ((c >= 65) && (c <= 90))
		return 1;
	return 0;
}

/* Returns a nonzero value if c is a hex digit (0-9,a-f,A-F), zero otherwise */
int isxdigit(int c)
{
	if (((c >= 48) && (c<=57)) || ((c >= 65) && (c <= 70)) || ((c >= 97) && (c <= 102)))
		return 1;
	return 0;
}

int tolower(int c)
{
	if (!islower(c))
		return c + ('a' - 'A');
	else
		return c;
}

int toupper(int c)
{
	if (islower(c))
		return c - ('a' - 'A');
	else
		return c;
}

/*  Not quite sure this one is correct either */
int isblank(int c)
{
	//return ((__ctype_ptr__[c+1] & _B) || (c == '\t')); ???
	return (c == '\t');
}

int isascii(int c)
{
	return c >= 0 && c< 128;
}

int toascii(int c)
{
	return (c)&0177;
}

#endif
