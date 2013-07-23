#!/usr/bin/env python
#-*- coding: utf-8 -*-

"""-------------------------------------------------------------------------
    To implement translations in all files.

    author:		Yeison Cardona
    contact:		yeison.eng@gmail.com 
    first release:	03/March/2012
    last release:	31/March/2012
    
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
-------------------------------------------------------------------------"""

import locale, gettext, os, sys

loc = locale.getdefaultlocale()[0][0:2]


if "--dev" in sys.argv and sys.argv[sys.argv.index("--dev")+1].lower() == "true":
    _ = lambda x:x
    
else:
    # pt_BR Language Check, By Wagner de Queiroz, 2010-Mar,01
    if loc == "pt": loc = locale.getdefaultlocale()[0][0:5]
    if loc != "pt_BR": loc = locale.getdefaultlocale()[0][0:2]              
    
    if sys.platform == 'win32': # needed for for py2exe on [EasyPack]
        lang = gettext.translation('pinguino', os.path.join('.', 'locale'), languages=[loc], fallback=True)
    else:
        lang = gettext.translation('pinguino', os.path.join(sys.path[0], 'locale'), languages=[loc], fallback=True)
    _=lang.ugettext
