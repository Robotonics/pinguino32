#!/usr/bin/env python
#  -*- coding: UTF-8 -*-

"""-------------------------------------------------------------------------
	pinguino

	(c) 2008-2009-2010-2011 Jean-Pierre MANDON <jp.mandon@gmail.com> 

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
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
	-------------------------------------------------------------------------"""

# ------------------------------------------------------------------------------
# changelog (see also file ChangeLog)
# ------------------------------------------------------------------------------
# x.3 regis blanchot
# TODO: pinguino universal uploader class
# TODO: pinguino debug stream class
#
# x.2 regis blanchot
# multi-architecture support (8 and 32-bit)
# multi-board support
# multi-platform support (GNU/Linux, Win32, MacOSX)
# improved command line execution
# process time calculation
# pinguino check list class (jp.mandon)
# pinguino boards list class
# pinguino version control class

# ------------------------------------------------------------------------------
# check dependencies
# ------------------------------------------------------------------------------

from check import *

EVT_RESULT_REVISION_ID = wx.NewId()

def EVT_RESULT_REVISION(win, func):
    win.Connect(-1, -1, EVT_RESULT_REVISION_ID, func)

class ResultEventRevision(wx.PyEvent):
    def __init__(self, data):
        wx.PyEvent.__init__(self)
        self.SetEventType(EVT_RESULT_REVISION_ID)
        self.data = data
        
        
# ------------------------------------------------------------------------------
# current version
# ------------------------------------------------------------------------------

pinguino_version="x.3"

# ------------------------------------------------------------------------------
# paths
# ------------------------------------------------------------------------------

HOME_DIR	= sys.path[0]
THEME_DIR	= os.path.join(HOME_DIR, 'theme')
SOURCE_DIR	= os.path.join(HOME_DIR, 'source')
LOCALE_DIR	= os.path.join(HOME_DIR, 'locale')
P32_DIR		= os.path.join(HOME_DIR, 'p32')
P8_DIR		= os.path.join(HOME_DIR, 'p8')
SVN_DIR		= 'http://pinguino32.googlecode.com/svn/trunk/'
APP_CONFIG	= os.path.join(HOME_DIR, '.config')
TEM_DIR = os.path.join(HOME_DIR, '.temp')

# ------------------------------------------------------------------------------
# default
# ------------------------------------------------------------------------------

THEME_DEFAULT = "miniregino"
BOARD_DEFAULT = Vasco2550()
gui=False

# ------------------------------------------------------------------------------
# Pinguino Class
# ------------------------------------------------------------------------------

class Pinguino(framePinguinoX, Tools, editor):

    global lang
    global gui

    osdir=""
    sdcc=""
    #gcc=""
    debug_output=0
    debug_handle=False
    debug_thread=False
    debug_flag=False
    

    noname=0
    keywordList=[]
    reservedword=[]
    libinstructions=[]
    regobject=[]
    rw=[]
    THEME=[]
    KEYWORD=[]

# ------------------------------------------------------------------------------
# id's
# ------------------------------------------------------------------------------

    # id board submenu
    ID_BOARD = wx.NewId()

    # id theme submenu
    ID_THEME = wx.NewId()
    ID_THEME1 = 1000

    # id debug submenu
    ID_DEBUG = wx.NewId()			# must be first
    ID_NODEBUG = wx.NewId()
    ID_USBCDC = wx.NewId()
    ID_USBBULK = wx.NewId()
    ID_USBHID = wx.NewId()
    ID_USBOTG = wx.NewId()
    ID_UART1 = wx.NewId()
    ID_UART2 = wx.NewId()
    ID_ENDDEBUG = wx.NewId()		# must be last

    # id Revision submenu
    ID_REVISION = wx.NewId()
    ID_UPGRADE = wx.NewId()
    ID_DIFFERENCE = wx.NewId()
    ID_CHECK = wx.NewId()

    # id help menu
    ID_KEYWORD = wx.NewId()
    ID_KEYWORD1 = 2000
    ID_WEBSITE = wx.NewId()
    ID_BLOG = wx.NewId()
    ID_FORUM = wx.NewId()
    ID_GROUP = wx.NewId()
    ID_WIKI = wx.NewId()
    ID_TUTORIAL = wx.NewId()
    ID_SHOP = wx.NewId()
    ID_ABOUT = wx.NewId()

    # other id
    ID_VERIFY = wx.NewId()
    ID_UPLOAD = wx.NewId()		

# ------------------------------------------------------------------------------
# init
# ------------------------------------------------------------------------------
    def __init__(self, parent):
        self._init_ctrls(parent)
        self.notebook1.Hide()
        
        self.debugOutMessage = None
        self.closing = False
        self.GetTheme()
        
        if os.path.isdir(TEM_DIR) == False: os.mkdir(TEM_DIR)
        
        self._mgr = wx.aui.AuiManager(self)
        
        self.loadSettings()
        self.setOSvariables()
        self.setLanguage()
        self.buildMenu()
        self.buildOutput()
        self.buildEditor()
        self.ConnectAll()
    
    
        #Threads
        EVT_RESULT_REVISION(self, self.setRevision)
        
        


        # ------------------------------------------------------------------------------
        # check new release of Pinguino
        # TODO: how to exclude compilers dir. from other OS ?
        # ------------------------------------------------------------------------------

        self.threadRevision = threading.Thread(target=self.getRevision, args=( ))
        self.threadRevision.start()
        
        self.SetTitle('Pinguino IDE ' + pinguino_version + ' rev. [loading...]')
        self.displaymsg(self.translate("Welcome to Pinguino IDE (rev. [loading...])\n"), 0)     

        self.__initEditor__()

        self.initTools()
        self.openLast()
        
        
# ------------------------------------------------------------------------------
# Decorator to debug time
# ------------------------------------------------------------------------------       
    def debugTime(function):
        DEBUG_TIME = True
        def process(*args):
            inicio = time.time()
            retorno = function(*args)
            fin = time.time()
            print function.__name__ + "\tTime: %.7fs" %(fin - inicio)
            return retorno
        if DEBUG_TIME : return process
        else: return function


# ------------------------------------------------------------------------------
# Editor
# ------------------------------------------------------------------------------
    def buildEditor(self):
        _ = self._
        self.EditorPanel = self.panel2
        #background with pinguino.cc colour and pinguino logo
        self.EditorPanel.SetBackgroundColour(wx.Colour(175, 200, 225))
        self.imageBackground = wx.Bitmap(os.path.join(THEME_DIR, 'logo.png'), wx.BITMAP_TYPE_ANY)
        if sys.platform == 'win32':
            self.imageBackground.SetSize((5000,5000)) # :)
        self.background = wx.StaticBitmap(self.EditorPanel, wx.ID_ANY, self.imageBackground)
        self.background.CentreOnParent(wx.BOTH) 

        # create a PaneInfo structure for editor window 
        # this Paneinfo will be switched when loading a file
        self.PaneEditorInfo=wx.aui.AuiPaneInfo()
        self.PaneEditorInfo.CloseButton(False)
        self.PaneEditorInfo.MaximizeButton(True)
        self.PaneEditorInfo.Caption(_("Editor"))
        self.PaneEditorInfo.Top()

        # ------------------------------------------------------------------------------
        # add the panes to the manager
        # ------------------------------------------------------------------------------

        self._mgr.AddPane(self.panelOutput, self.PaneOutputInfo, '')
        self._mgr.AddPane(self.panel1, wx.CENTER , '')


        # tell the manager to 'commit' all the changes just made
        self._mgr.Update()		

# ------------------------------------------------------------------------------
# Event Management
# ------------------------------------------------------------------------------
    def ConnectAll(self):
        self.Bind(wx.EVT_CLOSE, self.OnExit)
        self.Bind(wx.EVT_SIZE, self.OnResize)	 

        # file menu
        self.Bind(wx.EVT_MENU, self.OnNew, self.NEW)
        self.Bind(wx.EVT_MENU, self.OnOpen, self.OPEN)
        self.Bind(wx.EVT_MENU_RANGE, self.OnFileHistory, id=wx.ID_FILE1, id2=wx.ID_FILE9) 
        self.Bind(wx.EVT_MENU, self.OnSave, self.SAVE)
        self.Bind(wx.EVT_MENU, self.OnSaveAs, self.SAVEAS)
        self.Bind(wx.EVT_MENU, self.OnClose, self.CLOSE)
        self.Bind(wx.EVT_MENU, self.OnExit, self.EXIT)
        
        # edit menu
        self.Bind(wx.EVT_MENU, self.copy, self.COPY)
        self.Bind(wx.EVT_MENU, self.paste, self.PASTE)
        self.Bind(wx.EVT_MENU, self.cut, self.CUT)
        self.Bind(wx.EVT_MENU, self.clear, self.CLEAR)	   
        self.Bind(wx.EVT_MENU, self.undo, self.UNDO)	   
        self.Bind(wx.EVT_MENU, self.redo, self.REDO)
        self.Bind(wx.EVT_MENU, self.OnFind, self.FIND)
        self.Bind(wx.EVT_MENU, self.OnReplace, self.REPLACE)		
        self.Bind(wx.EVT_MENU, self.selectall, self.SELECTALL)
        self.Bind(wx.EVT_MENU, self.comentar, self.COMMENT)
        self.Bind(wx.EVT_MENU, self.OnPreferences, self.PREFERENCES)

        # pref menu
        if DEV:
            self.Bind(wx.EVT_MENU, self.OnDebug, id=self.ID_DEBUG)
            self.Bind(wx.EVT_MENU, self.OnCheck, id=self.ID_CHECK)
            self.Bind(wx.EVT_MENU, self.OnUpgrade, id=self.ID_UPGRADE)
        for b in range(len(boardlist)):
            self.Bind(wx.EVT_MENU, self.OnBoard, id = boardlist[b].id)
        #self.Bind(wx.EVT_MENU_RANGE, self.OnTheme, id=self.ID_THEME1, id2=self.ID_THEME1 + self.themeNum)
        self.Bind(wx.EVT_MENU, lambda x:self.setDebugger(mode="CDC"), self.USBCDC)
        self.Bind(wx.EVT_MENU, lambda x:self.setDebugger(mode=None), self.NODEBUG)

        

        # help menu
        #self.Bind(wx.EVT_TOOL, self.OnKeyword, id=self.ID_KEYWORD1, id2=self.ID_KEYWORD1 + self.keywordNum)# keywords
        self.Bind(wx.EVT_TOOL, self.OnKeyword, id=self.ID_KEYWORD)# keywords
        self.Bind(wx.EVT_TOOL, self.OnWeb, id=self.ID_WEBSITE)	# website
        self.Bind(wx.EVT_TOOL, self.OnWeb, id=self.ID_BLOG)		# blog   
        self.Bind(wx.EVT_TOOL, self.OnWeb, id=self.ID_FORUM)	   # forum
        self.Bind(wx.EVT_TOOL, self.OnWeb, id=self.ID_GROUP)	   # group		
        self.Bind(wx.EVT_TOOL, self.OnWeb, id=self.ID_WIKI)		# wiki
        self.Bind(wx.EVT_TOOL, self.OnWeb, id=self.ID_SHOP)		# shop
        self.Bind(wx.EVT_MENU, self.OnAbout, id=self.ID_ABOUT)	# about

        # icons bar
        self.Bind(wx.EVT_TOOL, self.OnVerify, id=self.ID_VERIFY)
        self.Bind(wx.EVT_TOOL, self.OnClose, id=wx.ID_CLOSE)
        self.Bind(wx.EVT_TOOL, self.OnNew, id=wx.ID_NEW)
        self.Bind(wx.EVT_TOOL, self.OnSave, id=wx.ID_SAVE)
        self.Bind(wx.EVT_TOOL, self.OnOpen, id=wx.ID_OPEN)
        self.Bind(wx.EVT_TOOL, self.OnUpload, id=self.ID_UPLOAD)
        self.Bind(wx.EVT_TOOL, self.OnFind, id=wx.ID_FIND)
        self.Bind(wx.EVT_TOOL, self.OnExit, id=wx.ID_EXIT)
        self.Bind(wx.EVT_TOOL, self.undo, id=wx.ID_UNDO)
        self.Bind(wx.EVT_TOOL, self.redo, id=wx.ID_REDO)
        self.Bind(wx.EVT_TOOL, self.cut, id=wx.ID_CUT)
        self.Bind(wx.EVT_TOOL, self.copy, id=wx.ID_COPY)
        self.Bind(wx.EVT_TOOL, self.paste, id=wx.ID_PASTE)
        self.Bind(wx.EVT_TOOL, self.clear, id=wx.ID_CLEAR)
        self.Bind(wx.EVT_TOOL, self.selectall, id=wx.ID_SELECTALL)

        
# ------------------------------------------------------------------------------
# Output
# ------------------------------------------------------------------------------
    def buildOutput(self):
        self.panelOutput = wx.Panel(id=wx.NewId(), name='panel1', parent=self,
              pos=wx.Point(0, 0), size=wx.Size(449, 169),
              style=wx.TAB_TRAVERSAL)

        self.logwindow = wx.TextCtrl(id=wx.NewId(), name='textCtrl1',
              parent=self.panelOutput, pos=wx.Point(0, 0), size=wx.Size(449, 148),
              style=wx.TE_READONLY | wx.TE_MULTILINE, value=u'')
        self.logwindow.SetBackgroundColour(wx.Colour(0, 0, 0))
        self.logwindow.SetForegroundColour(wx.Colour(255, 255, 255))
        self.logwindow.SetMinSize((-1, -1))

        self.debuggingLine = wx.TextCtrl(id=wx.NewId(), name='textCtrl2',
              parent=self.panelOutput, pos=wx.Point(0, 148), size=wx.Size(449, 21),
              style=0, value=u'>>>')
        self.debuggingLine.SetBackgroundColour(wx.Colour(0, 0, 0))
        self.debuggingLine.SetForegroundColour(wx.Colour(255, 255, 255))
        self.debuggingLine.SetInsertionPoint(125)
        self.debuggingLine.Hide()        
        self.debuggingLine.Bind(wx.EVT_KEY_UP, self.sendDebugging)
       
        self.boxSizer1 = wx.BoxSizer(orient=wx.VERTICAL)
        self.boxSizer1.AddWindow(self.logwindow, 1, border=0, flag=wx.EXPAND)
        self.boxSizer1.AddWindow(self.debuggingLine, 0, border=0, flag=wx.EXPAND) 
        self.panelOutput.SetSizer(self.boxSizer1)
        

        # create a PaneInfo structure for output window
        self.PaneOutputInfo=wx.aui.AuiPaneInfo()
        self.PaneOutputInfo.CloseButton(False)
        self.PaneOutputInfo.MaximizeButton(True)
        self.PaneOutputInfo.MinimizeButton(True)
        self.PaneOutputInfo.Caption("Output")
        self.PaneOutputInfo.Bottom()
        

# ----------------------------------------------------------------------
# Menus
# ----------------------------------------------------------------------
    #@debugTime
    def buildMenu(self):
        _ = self._
        self.menu = wx.MenuBar()
        
        # file menu
        self.file_menu = wx.Menu()
        self.NEW = wx.MenuItem(self.file_menu, wx.ID_NEW, _("New"), "", wx.ITEM_NORMAL)
        self.file_menu.AppendItem(self.NEW)
        self.OPEN = wx.MenuItem(self.file_menu, wx.ID_OPEN, _("Open"), "", wx.ITEM_NORMAL)
        self.file_menu.AppendItem(self.OPEN)
        self.SAVE = wx.MenuItem(self.file_menu, wx.ID_SAVE, _("Save"), "", wx.ITEM_NORMAL)
        self.file_menu.AppendItem(self.SAVE)
        self.SAVEAS = wx.MenuItem(self.file_menu, wx.ID_SAVEAS, _("Save as"), "", wx.ITEM_NORMAL)
        self.file_menu.AppendItem(self.SAVEAS)
        self.CLOSE = wx.MenuItem(self.file_menu, wx.ID_CLOSE, _("Close"), "", wx.ITEM_NORMAL)
        self.file_menu.AppendItem(self.CLOSE)
        self.EXIT = wx.MenuItem(self.file_menu, wx.ID_EXIT, _("Exit"), "", wx.ITEM_NORMAL)
        self.file_menu.AppendItem(self.EXIT)
        # ---file history-------------------------------------------------------
        self.filehistory.UseMenu(self.file_menu)
        self.filehistory.AddFilesToMenu()		
        self.menu.Append(self.file_menu, _("File"))

        # edit menu
        self.edit_menu = wx.Menu()
        self.UNDO = wx.MenuItem(self.edit_menu, wx.ID_UNDO, _("Undo"), "", wx.ITEM_NORMAL)
        self.edit_menu.AppendItem(self.UNDO)
        self.REDO = wx.MenuItem(self.edit_menu, wx.ID_REDO, _("Redo"), "", wx.ITEM_NORMAL)
        self.edit_menu.AppendItem(self.REDO)
        self.edit_menu.AppendSeparator()
        self.COMMENT_ID = wx.NewId()
        self.COMMENT = wx.MenuItem(self.edit_menu, self.COMMENT_ID, _("Comment/Uncomment\tCtrl+l"), "", wx.ITEM_NORMAL)
        self.edit_menu.AppendItem(self.COMMENT)
        self.edit_menu.AppendSeparator()        
        self.FIND = wx.MenuItem(self.edit_menu, wx.ID_FIND, _("Find"), "", wx.ITEM_NORMAL)
        self.edit_menu.AppendItem(self.FIND)		
        self.REPLACE = wx.MenuItem(self.edit_menu, wx.ID_REPLACE, _("Replace"), "", wx.ITEM_NORMAL)
        self.edit_menu.AppendItem(self.REPLACE)
        self.edit_menu.AppendSeparator()		
        self.CUT = wx.MenuItem(self.edit_menu, wx.ID_CUT, _("Cut"), "", wx.ITEM_NORMAL)
        self.edit_menu.AppendItem(self.CUT)
        self.COPY = wx.MenuItem(self.edit_menu, wx.ID_COPY, _("Copy"), "", wx.ITEM_NORMAL)
        self.edit_menu.AppendItem(self.COPY)
        self.PASTE = wx.MenuItem(self.edit_menu, wx.ID_PASTE, _("Paste"), "", wx.ITEM_NORMAL)
        self.edit_menu.AppendItem(self.PASTE)  
        self.CLEAR = wx.MenuItem(self.edit_menu, wx.ID_CLEAR, _("Clear"), "", wx.ITEM_NORMAL)
        self.edit_menu.AppendItem(self.CLEAR)
        self.SELECTALL = wx.MenuItem(self.edit_menu, wx.ID_SELECTALL, _("Select all"), "", wx.ITEM_NORMAL)
        self.edit_menu.AppendItem(self.SELECTALL)
        self.edit_menu.AppendSeparator()
        #self.COMMENT_ID = wx.NewId()
        #self.COMMENT = wx.MenuItem(self.edit_menu, self.COMMENT_ID, _("Comment/Uncomment\tCtrl+l"), "", wx.ITEM_NORMAL)
        #self.edit_menu.AppendItem(self.COMMENT)
        #self.edit_menu.AppendSeparator()
        self.PREFERENCES_ID = wx.NewId()
        self.PREFERENCES = wx.MenuItem(self.edit_menu, self.PREFERENCES_ID, _("Preferences...")+"\tCtrl+p", "", wx.ITEM_NORMAL)
        self.edit_menu.AppendItem(self.PREFERENCES)		

        self.menu.Append(self.edit_menu, _("Edit"))

        # preferences menu
        self.pref_menu = wx.Menu()

        # ---debug submenu
        if DEV:
            self.debug_menu = wx.Menu()
            self.NODEBUG = wx.MenuItem(self.debug_menu, self.ID_NODEBUG, _("None"), "", wx.ITEM_RADIO)
            self.debug_menu.AppendItem(self.NODEBUG)
            #self.NODEBUG.Enable(False)
            self.USBCDC = wx.MenuItem(self.debug_menu, self.ID_USBCDC, _("USB CDC (Serial Emulation)"), "", wx.ITEM_RADIO)
            self.debug_menu.AppendItem(self.USBCDC)
            #self.USBCDC.Enable(False)
            self.USBBULK = wx.MenuItem(self.debug_menu, self.ID_USBBULK, _("USB Bulk"), "", wx.ITEM_RADIO)
            self.debug_menu.AppendItem(self.USBBULK)
            #self.USBBULK.Enable(False)
            self.USBHID = wx.MenuItem(self.debug_menu, self.ID_USBHID, _("USB HID"), "", wx.ITEM_RADIO)
            self.debug_menu.AppendItem(self.USBHID)
            self.USBHID.Enable(False)
            self.USBOTG = wx.MenuItem(self.debug_menu, self.ID_USBOTG, _("USB OTG"), "", wx.ITEM_RADIO)
            self.debug_menu.AppendItem(self.USBOTG)
            self.USBOTG.Enable(False)

            
            
            #
            # TODO:
            # pic8  -> UART
            # pic32 -> UART1, UART2, etc
            #
            self.UART1 = wx.MenuItem(self.debug_menu, self.ID_UART1, _("UART1 (Serial 1)"), "", wx.ITEM_CHECK)
            self.debug_menu.AppendItem(self.UART1)
            self.UART2 = wx.MenuItem(self.debug_menu, self.ID_UART2, _("UART2 (Serial 2)"), "", wx.ITEM_CHECK)
            self.debug_menu.AppendItem(self.UART2)
            self.pref_menu.AppendMenu(self.ID_DEBUG, _("Debug mode"), self.debug_menu)
            # mark current debug mode
            did = self.config.ReadInt('Debug', -1)
            if did == -1:
                did = self.ID_NODEBUG
                self.pref_menu.Check(did, True)
            self.OnDebug(wx.Event)

        # --- board submenu
        self.board_menu=wx.Menu()
        for b in range(len(boardlist)):
            self.board_menu.AppendRadioItem(boardlist[b].id, boardlist[b].name, "your board")
        self.pref_menu.AppendMenu(self.ID_BOARD, "Board", self.board_menu)
        # mark current board
        bid = self.config.ReadInt('Board', -1)
        minbid = boardlist[0].id
        maxbid = boardlist[len(boardlist)-1].id
        if bid == -1 or bid < minbid or bid > maxbid:
            bid = BOARD_DEFAULT.id
        self.board_menu.Check(bid, True)
        self.OnBoard(wx.Event)

         #---theme submenu
        #self.theme_menu = wx.Menu()
        #i = 0
        #for th in self.themeList:
            #self.THEME.append(wx.MenuItem(self.theme_menu, self.ID_THEME1 + i, th, "", wx.ITEM_CHECK))
            #self.theme_menu.AppendItem(self.THEME[i])
            #i = i + 1
        #self.pref_menu.AppendMenu(self.ID_THEME, _("Themes"), self.theme_menu)
        #tid = self.theme_menu.FindItem(self.theme)
        #self.theme_menu.Check(tid, True)

        # ---revision submenu
        if DEV:
            self.revision_menu = wx.Menu()
            self.CHECK = wx.MenuItem(self.revision_menu, self.ID_CHECK, _("Check last revision"), "", wx.ITEM_NORMAL)
            self.revision_menu.AppendItem(self.CHECK)
            #self.DIFFERENCE = wx.MenuItem(self.revision_menu, self.ID_DIFFERENCE, "Check differences", "", wx.ITEM_NORMAL)
            #self.revision_menu.AppendItem(self.DIFFERENCE)
            self.UPGRADE = wx.MenuItem(self.revision_menu, self.ID_UPGRADE, _("Upgrade"), "", wx.ITEM_NORMAL)
            self.revision_menu.AppendItem(self.UPGRADE)
            self.UPGRADE.Enable(False)
            self.pref_menu.AppendMenu(self.ID_REVISION, _("Revision"), self.revision_menu)

        self.menu.Append(self.pref_menu, _("Pinguino"))

        # help menu
        self.help_menu = wx.Menu()

        self.KEYWORD = wx.MenuItem(self.help_menu, self.ID_KEYWORD, _("Keywords"), "", wx.ITEM_NORMAL)
        self.help_menu.AppendItem(self.KEYWORD)		

        self.WEBSITE = wx.MenuItem(self.help_menu, self.ID_WEBSITE, _("Website"), "", wx.ITEM_NORMAL)
        self.help_menu.AppendItem(self.WEBSITE)
        self.WIKI = wx.MenuItem(self.help_menu, self.ID_WIKI, _("Wiki"), "", wx.ITEM_NORMAL)
        self.help_menu.AppendItem(self.WIKI)
        self.FORUM = wx.MenuItem(self.help_menu, self.ID_FORUM, _("Forum"), "", wx.ITEM_NORMAL)
        self.help_menu.AppendItem(self.FORUM)
        self.BLOG = wx.MenuItem(self.help_menu, self.ID_BLOG, _("Blog"), "", wx.ITEM_NORMAL)
        self.help_menu.AppendItem(self.BLOG)									  
        self.GROUP = wx.MenuItem(self.help_menu, self.ID_GROUP, _("Group"), "", wx.ITEM_NORMAL)
        self.help_menu.AppendItem(self.GROUP) 
        self.SHOP = wx.MenuItem(self.help_menu, self.ID_SHOP, _("Shop"), "", wx.ITEM_NORMAL)
        self.help_menu.AppendItem(self.SHOP)
        self.ABOUT = wx.MenuItem(self.help_menu, self.ID_ABOUT, _("About..."), "", wx.ITEM_NORMAL)
        self.help_menu.AppendItem(self.ABOUT)												
        self.menu.Append(self.help_menu, _("Help"))

        # ------------------------------------------------------------------------------
        # Menu + Icons
        # ------------------------------------------------------------------------------

        self.SetMenuBar(self.menu)
        self.DrawToolbar()
        self.SetToolBar(self.toolbar)

        
        
# ----------------------------------------------------------------------
# load settings from config file
# ----------------------------------------------------------------------
    def loadSettings(self):
        self.config = wx.FileConfig(localFilename = APP_CONFIG, style = wx.CONFIG_USE_LOCAL_FILE)
        self.filehistory = wx.FileHistory()
        self.filehistory.Load(self.config)

        framesize = ( self.config.ReadInt('Window/Width', -1), self.config.ReadInt('Window/Height', -1))
        if framesize == (0, 0): framesize = (400, 400)
        self.SetSize(framesize)

        #framepos = (self.config.ReadInt('Window/Posx', -1), self.config.ReadInt('Window/Posy', -1))

        self.outputsize = (self.config.ReadInt('Output/Width', -1), self.config.ReadInt('Output/Height', -1))
        if self.outputsize == (0, 0): self.outputsize = (400, 250)

        self.debug =   self.config.ReadInt('Debug', -1)
        if self.debug == '': self.debug = ID_NODEBUG

        self.theme =   self.config.Read('Theme/name')
        if self.theme == '': self.theme = THEME_DEFAULT
        
        
# ----------------------------------------------------------------------
# window
# ----------------------------------------------------------------------
    def setLanguage(self):
        loc = locale.getdefaultlocale()[0][0:2]

        # pt_BR Language Check, By Wagner de Queiroz, 2010-Mar,01
        if loc == "pt":
            loc = locale.getdefaultlocale()[0][0:5]
        if loc != "pt_BR":
            loc = locale.getdefaultlocale()[0][0:2]              

        self.lang = gettext.translation('pinguino', LOCALE_DIR, languages=[loc], fallback=True)
        self._=self.lang.ugettext

        _icon = wx.EmptyIcon()
        _icon.CopyFromBitmap(wx.Bitmap(os.path.join(THEME_DIR, 'logo.png'), wx.BITMAP_TYPE_ANY))
        self.SetIcon(_icon)


        
        
# ----------------------------------------------------------------------
# get OS name and define some OS dependant variable
# ----------------------------------------------------------------------
    def setOSvariables(self):
        if sys.platform == 'darwin':
            self.osdir = 'macosx'
            self.debug_port = '/dev/tty.usbmodem1912'
            self.c8 = 'sdcc'
            self.u32 = 'ubw32'
            self.make = 'make'
        elif sys.platform == 'win32':
            self.osdir = 'win32'
            self.debug_port = 15
            self.c8 = 'sdcc.exe'
            self.u32 = 'mphidflash.exe'
            self.make = os.path.join(HOME_DIR, self.osdir, 'p32', 'bin', 'make.exe')
        else:
            self.osdir = 'linux'
            self.debug_port = '/dev/ttyACM0'
            self.c8 = 'sdcc'
            self.u32 = 'ubw32'
            self.make = 'make'       
        
        
# ------------------------------------------------------------------------------
# Get revision
# ------------------------------------------------------------------------------ 
    def getRevision(self):
        try:
            sw = SubversionWorkingCopy(HOME_DIR).current_version()
        except: sw = "unknown"
        wx.PostEvent(self, ResultEventRevision(sw))
        
        
# ------------------------------------------------------------------------------
# Revision
# ------------------------------------------------------------------------------
    def setRevision(self, event):
        self.localRev = event.data
        self.SetTitle('Pinguino IDE ' + pinguino_version + ' rev. ' + self.localRev)
        self.displaymsg(self.translate("Welcome to Pinguino IDE (rev. ") + self.localRev + ")\n", 1)
        self.statusBar1.SetStatusText(number=2, text="Rev. %s" %self.localRev)

# ------------------------------------------------------------------------------
# Update
# ------------------------------------------------------------------------------

    def updateIDE(self):
        self.panel1.Fit()
        self.panel2.Fit()
        self._mgr.Update()   

# ------------------------------------------------------------------------------
# Thread
# ------------------------------------------------------------------------------

    def dthread(self):
        while self.debug_flag is True:
            if self.debug_handle:
                print ">>>debug:"	
                print self.debug_handle.readline()
            time.sleep(0.01)

    # create the debug thread			
    if DEV:
        debug_thread = threading.Thread(target=dthread)

# ------------------------------------------------------------------------------
# OnUpgrade
# ------------------------------------------------------------------------------

    def OnUpgrade(self, event):
        self.displaymsg(self.translate("Upgrading ...\n"), 0)
        try:
            self.sr.checkout(HOME_DIR)
        except:
            self.displaymsg(self.translate("Local version is not under revision control.\n"), 0)
            return
        self.displaymsg(self.translate("Done\n"), 0)
        Pinguino.__init__(self, parent)

# ------------------------------------------------------------------------------
# OnCheck
# ------------------------------------------------------------------------------

    def OnCheck(self, event):
        self.displaymsg(self.translate("Checking repository revision number ...\n"), 0)
        try:
            self.sr = SubversionRepository(SVN_DIR)
        except:
            self.displaymsg(self.translate("Server temporarily unavailable.\n"), 0)
            return
        self.svnRev = self.sr.current_version()
        if self.svnRev == self.localRev:
            self.UPGRADE.Enable(False)
            self.displaymsg(self.translate("You have the latest version.\n"), 0)
        else:
            self.UPGRADE.Enable(True)
            self.displaymsg(self.translate("Revision ") + self.svnRev + self.translate(" is available.\n"), 0)

# ------------------------------------------------------------------------------
# OnDebug
# ------------------------------------------------------------------------------

    def OnDebug(self, event):
        for d in range(self.ID_ENDDEBUG - self.ID_DEBUG - 1):
            did = self.ID_DEBUG + d + 1
            if self.debug_menu.IsChecked(did):
                self.debug = did

        # mode Debug ?
        """
			# try to open serial port
			try:
				self.debug_handle = serial.Serial(self.debug_port)
			except:
				pass
			# is a device connected ?
			if self.debug_handle:
				# let's start our thread
				self.debug_flag = True
				self.debug_thread.start()
		else:
			# stop the thread
			self.debug_flag = False
			if self.debug_thread:
				self.debug_thread.join()
			if self.debug_handle:
				self.debug_handle.close()
		"""


# ------------------------------------------------------------------------------
# OnNew
# ------------------------------------------------------------------------------

    def OnNew(self, event):
        self.background.Hide()
        self.New("Newfile" + str(self.noname), self.reservedword, self.rw)
        self.noname+=1
        self.updatenotebook()

# ------------------------------------------------------------------------------
# OnOpen
# ------------------------------------------------------------------------------

    def OnOpen(self, event):
        self.background.Hide()
        self.OpenDialog("Pde Files",\
                        "pde",\
                        self.reservedword,\
                        self.rw,\
                        self.filehistory,\
                        self.config)
        self.updatenotebook()

# ------------------------------------------------------------------------------
# OnFileHistory : open selected history file
# ------------------------------------------------------------------------------

    def OnFileHistory(self, event):
        self.background.Hide() 
        fileNum = event.GetId() - wx.ID_FILE1
        path = self.filehistory.GetHistoryFile(fileNum)
        self.filehistory.AddFileToHistory(path)				  # move up the list
        self.Open(path,self.reservedword,self.rw, self.filehistory, self.config)
        # refresh file menu (doesn't seem to work)
        self.file_menu.UpdateUI()
        self.updatenotebook()

# ------------------------------------------------------------------------------
# OnSave : Save current file
# ------------------------------------------------------------------------------

    def OnSave(self, event): 
        self.SaveDirect()

# ------------------------------------------------------------------------------
# OnSaveAs : Save current File as ...
# ------------------------------------------------------------------------------

    def OnSaveAs(self, event): 
        self.Save("Pde Files","pde")

# ------------------------------------------------------------------------------
# OnClose : Close Editor Window
# ------------------------------------------------------------------------------

    def OnClose(self, event): 
        self.CloseTab()
        self.updatenotebook()

# ------------------------------------------------------------------------------
# OnExit : Save Settings and Exit Program
# ------------------------------------------------------------------------------

    def OnExit(self, event):
    
        self.closing = True  #Signal for Threads
        
        try:
            self.pinguino.close()
            fclose(self.debug_handle)
        except: pass
        
        # ---save settings-----------------------------------------------
        #if not self.IsIconized() and not self.IsMaximized():
        w, h = self.GetSize()
        self.config.WriteInt('Window/Width', w)
        self.config.WriteInt('Window/Height', h)
        self.config.WriteInt("frame/sashposition", self.splitterWindow1.GetSashPosition())
        #x, y = self.GetPosition()
        #self.config.WriteInt('Window/Posx', x)
        #self.config.WriteInt('Window/Posy', y)

        w, h = self.logwindow.GetSize()
        self.config.WriteInt('Output/Width', w)
        self.config.WriteInt('Output/Height', h)

        i = 0
        self.config.WriteInt("LastEdit/count", len(self.filename))
        for file in self.filename:
            self.config.Write("LastEdit/file%d" %i, file)
            i += 1

        #for t in self.themeList:
        #	tid = self.theme_menu.FindItem(t)
        #	if self.theme_menu.IsChecked(tid):
        self.config.Write('Theme/name', self.theme)

        #Save the last files in the editor
        for b in range(len(boardlist)):
            bid = boardlist[b].id
            if self.board_menu.IsChecked(bid):
                self.config.WriteInt('Board', bid)

        if DEV:
            for d in range(self.ID_ENDDEBUG - self.ID_DEBUG - 1):
                did = self.ID_DEBUG + d + 1
                if self.debug_menu.IsChecked(did):
                    self.config.WriteInt('Debug', did)

        self.config.Flush()

        # ----------------------------------------------------------------------
        # deinitialize the frame manager
        self._mgr.UnInit()
        # delete the frame
        self.Destroy()
        sys.exit(0)

# ------------------------------------------------------------------------------
# GetTheme : get all the theme (directory) name
# ------------------------------------------------------------------------------

    def GetTheme(self):
        self.themeList = [f for f in os.listdir(THEME_DIR)
                          if os.path.isdir(os.path.join(THEME_DIR, f))]
        self.themeNum = len(self.themeList)

# ------------------------------------------------------------------------------
# OnTheme : delete current theme and load a new one
# ------------------------------------------------------------------------------

    #def OnTheme(self, event):
        ## uncheck all
        #for f in self.themeList:
            #tid = self.theme_menu.FindItem(f)
            #self.menu.Check(tid, False)
        ## check selected only
        #curid = event.GetId()
        #tid = curid - self.ID_THEME1
        #self.menu.Check(curid, True)
        #self.theme = self.themeList[tid]
        #self.DrawToolbar()

# ------------------------------------------------------------------------------
# OnBoard : load boards specificities
# ------------------------------------------------------------------------------

    def OnBoard(self, event):
        for b in range(len(boardlist)):
            #clef = BOARD_DEFAULT_ID + b
            bid = boardlist[b].id
            if self.board_menu.IsChecked(bid):
                self.curBoard = boardlist[b]
                break
        # clear all the lists before rebuild them
        del self.rw[:]
        del self.regobject[:]
        del self.keywordList[:]
        del self.reservedword[:]
        del self.libinstructions[:]
        self.readlib(self.curBoard)

        self.statusBar1.SetStatusText(number=3, text=self.curBoard.name)


# ------------------------------------------------------------------------------
# OnResize:
# ------------------------------------------------------------------------------

    def OnResize(self,event):
        try: self.background.CentreOnParent(wx.BOTH)
        except: pass	
        self._mgr.Update	   
        event.Skip()

    #----------------------------------------------------------------------
    def openLast(self):
        for i in range(self.config.ReadInt('LastEdit/count')):
            file = self.config.Read('LastEdit/file%d' %i)
            if os.path.isfile(file):
                self.Open(file, self.reservedword, self.rw, self.filehistory, self.config)		


# ------------------------------------------------------------------------------
# OnAbout:
# ------------------------------------------------------------------------------

    def OnAbout(self, event):
        description = """Pinguino is an Open Software and Open Hardware Arduino-like project. Boards are based on 8 or 32-bit USB built-in Microchip microcontrollers. The main goal is to build a real USB system without USB to serial converter.
		"""

        licence = """Pinguino is free software; you can redistribute it and/or modify it 
		under the terms of the GNU General Public License as published by the Free Software Foundation; 
		either version 2 of the License, or (at your option) any later version.

		Pinguino is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
		without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
		See the GNU General Public License for more details. You should have received a copy of 
		the GNU General Public License along with File Hunter; if not, write to 
		the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA"""


        info = wx.AboutDialogInfo()
        #bmp = wx.Icon(os.path.join(THEME_DIR, 'logoX3.png'), wx.BITMAP_TYPE_PNG)
        image = wx.Image(os.path.join(THEME_DIR, 'logoX3.png'), wx.BITMAP_TYPE_PNG)
        image = image.Scale(500, 375, wx.IMAGE_QUALITY_HIGH)
        bmp = wx.BitmapFromImage(image)
        #bmp = image.ConvertToBitmap()
        icon = wx.EmptyIcon()
        icon.CopyFromBitmap(bmp)
        info.SetIcon(icon)
        info.SetName('Pinguino')
        info.SetVersion("rev. " + self.localRev)
        info.SetDescription(description)
        # LGPL compatibility ?
        #info.SetCopyright('2008, 2009, 2010, 2011 jean-pierre mandon')
        info.SetWebSite('http://www.pinguino.cc')
        info.SetLicence(licence)

        info.AddDeveloper('Jean-Pierre Mandon')
        info.AddDeveloper('Régis Blanchot')
        info.AddDeveloper('Marcus Fazzi')
        info.AddDeveloper('Jesús Carmona Esteban')		
        info.AddDeveloper('Ivan Ricondo')
        info.AddDeveloper('Joan Espinoza')
        info.AddDeveloper('Yeison Cardona')									

        info.AddDocWriter('Benoit Espinola')
        info.AddDocWriter('Sebastien Koechlin')
        info.AddDocWriter('Ivan Ricondo')
        info.AddDocWriter('Jesús Carmona Esteban')
        info.AddDocWriter('Marcus Fazzi')
        info.AddDocWriter('Régis Blanchot')

        info.AddArtist('France Cadet')
        info.AddArtist('Laurent Costes')
        info.AddArtist('Daniel Rodríguez')

        info.AddTranslator('Joan Espinoza: Spanish, Portuguese')
        info.AddTranslator('Marin Purgar: Croatian')
        info.AddTranslator('Wim Heirman: Dutch')
        info.AddTranslator('Vasile Guta Ciucur: Romanian')
        info.AddTranslator('Wagner de Queiroz: Brazilian portuguese')

        wx.AboutBox(info)  

# ------------------------------------------------------------------------------
# OnVerify:
# ------------------------------------------------------------------------------

    def OnVerify(self, event):
        global lang
        t0 = time.time()
        if self.GetPath()==-1:
            dlg = wx.MessageDialog(self,
                                   self.translate('Open file first !!'),
                                   self.translate('Warning'),
                                   wx.OK | wx.ICON_WARNING)
            result=dlg.ShowModal()
            dlg.Destroy()		   
            return
        self.displaymsg("Board:\t" + self.curBoard.name + "\n", 1)
        self.displaymsg("Proc: \t" + self.curBoard.proc + "\n", 0)
        self.SaveDirect()
        filename=self.GetPath()
        filename,extension=os.path.splitext(filename)
        if os.path.exists(filename+".hex"):
            os.remove(filename+".hex")
        if os.path.exists(os.path.join(SOURCE_DIR, "user.c")):
            os.remove(os.path.join(SOURCE_DIR, "user.c"))
        retour=self.preprocess(filename, self.curBoard)
        if retour=="error":
            return
        # compilation
        if self.curBoard.arch == 8:
            MAIN_FILE="main.hex"
        else:
            MAIN_FILE="main32.hex"
        retour=self.compile(filename, self.curBoard)
        if retour!=0:
            self.displaymsg(self.translate("error while compiling file ")+filename,0)
        else:
            retour=self.link(filename, self.curBoard)
            if os.path.exists(os.path.join(SOURCE_DIR, MAIN_FILE))!=True:
                self.displaymsg(self.translate("error while linking")+" "+filename+".o",0)
            else:
                shutil.copy(os.path.join(SOURCE_DIR, MAIN_FILE), filename+".hex")
                self.displaymsg(self.translate("compilation done")+"\n",0)
                self.displaymsg(self.getCodeSize(filename, self.curBoard)+"\n",0)
                self.displaymsg(str(time.time() - t0) + " seconds process time\n",0)
                os.remove(os.path.join(SOURCE_DIR, MAIN_FILE))
                #os.remove(filename+".c")

# ------------------------------------------------------------------------------
# OnUpload:
# ------------------------------------------------------------------------------

    def OnUpload(self, event):
        if self.GetPath() != -1:
            filename = self.GetPath()
            filename, extension = os.path.splitext(filename)
            if os.path.exists(filename + '.hex'):
                if self.curBoard.arch == 8:
                    u = Uploader(self.logwindow, filename, self.curBoard)
                else:
                    fichier = open(os.path.join(SOURCE_DIR, 'stdout'), 'w+')
                    sortie=Popen([os.path.join(HOME_DIR, self.osdir, 'p32', 'bin', self.u32),
                                  "-w",
                                  filename+".hex",
                                  "-r",
                                  "-n"],
                                 stdout=fichier, stderr=STDOUT)
                    sortie.communicate()
                    fichier.seek(0)
                    self.displaymsg(fichier.read(),0)
                    fichier.close()
            else:# no file
                dlg = wx.MessageDialog(self,
                                       self.translate('File must be verified/compiled before upload'),
                                       self.translate('Warning!'),
                                       wx.OK | wx.ICON_WARNING)
                result=dlg.ShowModal()
                dlg.Destroy()					 
        else:# not saved
            dlg = wx.MessageDialog(self,
                                   self.translate('File must be saved before upload'),
                                   self.translate('Warning!'),
                                   wx.OK | wx.ICON_WARNING)
            result=dlg.ShowModal()
            dlg.Destroy()		   
        event.Skip()

# ------------------------------------------------------------------------------
# OnFind:
# ------------------------------------------------------------------------------

    def OnFind(self,event):
        self.panel_replace.Hide()		
        if self.panel_find.IsShown():
            self.panel_find.Hide()
            self.updateIDE()
            return
        self.Bind(wx.EVT_BUTTON, self.findnext, self.FindNext)  
        self.Bind(wx.EVT_BUTTON, self.findprev, self.FindPrevious)			   
        self.FindText.SetFocus()
        self.panel_find.Show()
        self.updateIDE()		

    def findprev(self,event):
        chaine=self.FindText.GetString(0,self.FindText.GetLastPosition())
        trouve, position=self.find(chaine,0)
        textEdit = self.stcpage[self.notebook1.GetSelection()]		
        if trouve!=-1:
            self.highlightline(trouve,'yellow')
            self.focus()
            textEdit.SetSelectionStart(position)
            textEdit.SetSelectionEnd(position+len(chaine))
            self.Replace.Enable()
            self.ReplaceAll.Enable()			

    def findnext(self,event):
        chaine=self.FindText.GetString(0,self.FindText.GetLastPosition())
        trouve, position=self.find(chaine,1)
        textEdit = self.stcpage[self.notebook1.GetSelection()]
        if trouve!=-1:
            self.highlightline(trouve,'yellow')
            self.focus()
            textEdit.SetSelectionStart(position)
            textEdit.SetSelectionEnd(position+len(chaine))
            self.Replace.Enable()
            self.ReplaceAll.Enable()	
        return trouve

# ------------------------------------------------------------------------------
# OnReplace:
# ------------------------------------------------------------------------------

    def OnReplace(self,event):
        if self.panel_find.IsShown():
            self.panel_find.Hide()
            self.panel_replace.Hide()
            self.ReplaceText.Clear()
            self.updateIDE()
            return
        self.Bind(wx.EVT_BUTTON, self.findnext, self.FindNext)  
        self.Bind(wx.EVT_BUTTON, self.findprev, self.FindPrevious)
        self.Bind(wx.EVT_BUTTON, self.replacetext, self.Replace)  
        self.Bind(wx.EVT_BUTTON, self.replacealltext, self.ReplaceAll)
        self.FindText.SetFocus()
        self.panel_find.Show()
        self.panel_replace.Show()
        self.Replace.Disable()
        self.ReplaceAll.Disable()
        self.updateIDE()

    #----------------------------------------------------------------------
    def replacetext(self, event=None):
        word = self.FindText.GetString(0, self.FindText.GetLastPosition())
        wordReplace = self.ReplaceText.GetString(0, self.ReplaceText.GetLastPosition())
        print word, wordReplace
        if word == "": return False
        textEdit = self.stcpage[self.notebook1.GetSelection()]
        textEdit.Clear()
        textEdit.InsertText(textEdit.CurrentPos, wordReplace)
        if self.findnext(event) == -1:
            self.gotostart()
            self.Replace.Disable()
            self.ReplaceAll.Disable()
            return False
        return True

    #----------------------------------------------------------------------
    def replacealltext(self, event):
        textEdit = self.stcpage[self.notebook1.GetSelection()]
        self.gotostart()
        self.findnext(event)
        while self.replacetext(): pass


# ------------------------------------------------------------------------------
# OnWeb: Open Web page
# ------------------------------------------------------------------------------

    def OnWeb(self,event):
        id=event.GetId()
        if id==self.ID_WEBSITE:
            webbrowser.open("http://www.pinguino.cc")
        if id==self.ID_WIKI:
            webbrowser.open("http://wiki.pinguino.cc")
        if id==self.ID_FORUM:
            webbrowser.open("http://forum.pinguino.cc")
        if id==self.ID_BLOG:
            webbrowser.open("http://blog.pinguino.cc")
        if id==self.ID_SHOP:
            webbrowser.open("http://shop.pinguino.cc")
        if id==self.ID_GROUP:
            webbrowser.open("http://groups.google.fr/group/pinguinocard?pli=1")

# ------------------------------------------------------------------------------
# Draw toolbar icons
# ------------------------------------------------------------------------------
    
    def DrawToolbar(self):
        try:
            # Deletes all the tools in the current toolbar
            self.toolbar.ClearTools()
        except:
            # Create toolbar
            self.toolbar = wx.ToolBar(self, -1, wx.DefaultPosition, wx.DefaultSize, wx.TB_FLAT | wx.TB_NODIVIDER)
            pass 
        # Get size of new theme's icons
        icon = wx.Bitmap(os.path.join(THEME_DIR, self.theme, "new.png"), wx.BITMAP_TYPE_ANY)
        iconSize = icon.GetSize()
        # Update Bitmap size to fit new icons (not sure that it works !)
        self.toolbar.SetToolBitmapSize(iconSize)
        if (os.path.exists(os.path.join(THEME_DIR, self.theme, "new.png"))!=False):
            self.toolbar.AddLabelTool(wx.ID_NEW, "&New", wx.Bitmap(os.path.join(THEME_DIR, self.theme, "new.png"), wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "New File", "")
        if (os.path.exists(os.path.join(THEME_DIR, self.theme, "open.png"))!=False):			
            self.toolbar.AddLabelTool(wx.ID_OPEN, "&Open", wx.Bitmap(os.path.join(THEME_DIR, self.theme, "open.png"), wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "Open File", "")
        if (os.path.exists(os.path.join(THEME_DIR, self.theme, "save.png"))!=False):			
            self.toolbar.AddLabelTool(wx.ID_SAVE, "&Save", wx.Bitmap(os.path.join(THEME_DIR, self.theme, "save.png"), wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "Save File", "")
        if (os.path.exists(os.path.join(THEME_DIR, self.theme, "stop.png"))!=False):			
            self.toolbar.AddLabelTool(wx.ID_CLOSE, "&Close", wx.Bitmap(os.path.join(THEME_DIR, self.theme, "stop.png"), wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "Close File", "")
        self.toolbar.AddSeparator()
        if (os.path.exists(os.path.join(THEME_DIR, self.theme, "undo.png"))!=False):		
            self.toolbar.AddLabelTool(wx.ID_UNDO, "&Undo", wx.Bitmap(os.path.join(THEME_DIR, self.theme, "undo.png"), wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "Undo", "")
        if (os.path.exists(os.path.join(THEME_DIR, self.theme, "redo.png"))!=False):
            self.toolbar.AddLabelTool(wx.ID_REDO, "&Redo", wx.Bitmap(os.path.join(THEME_DIR, self.theme, "redo.png"), wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "Redo", "")
        self.toolbar.AddSeparator()
        if (os.path.exists(os.path.join(THEME_DIR, self.theme, "cut.png"))!=False):		
            self.toolbar.AddLabelTool(wx.ID_CUT, "&Cut", wx.Bitmap(os.path.join(THEME_DIR, self.theme, "cut.png"), wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "Cut", "")
        if (os.path.exists(os.path.join(THEME_DIR, self.theme, "copy.png"))!=False):
            self.toolbar.AddLabelTool(wx.ID_COPY, "&Copy", wx.Bitmap(os.path.join(THEME_DIR, self.theme, "copy.png"), wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "Copy", "")
        if (os.path.exists(os.path.join(THEME_DIR, self.theme, "paste.png"))!=False):		
            self.toolbar.AddLabelTool(wx.ID_PASTE, "&Paste", wx.Bitmap(os.path.join(THEME_DIR, self.theme, "paste.png"), wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "Paste", "")
        if (os.path.exists(os.path.join(THEME_DIR, self.theme, "clear.png"))!=False):
            self.toolbar.AddLabelTool(wx.ID_CLEAR, "&Clear", wx.Bitmap(os.path.join(THEME_DIR, self.theme, "clear.png"), wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "Clear", "")
        if (os.path.exists(os.path.join(THEME_DIR, self.theme, "select.png"))!=False):
            self.toolbar.AddLabelTool(wx.ID_SELECTALL, "&Select all", wx.Bitmap(os.path.join(THEME_DIR, self.theme, "select.png"), wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "Select all", "")
        self.toolbar.AddSeparator()
        if (os.path.exists(os.path.join(THEME_DIR, self.theme, "find.png"))!=False):
            self.toolbar.AddLabelTool(wx.ID_FIND, "&Find", wx.Bitmap(os.path.join(THEME_DIR, self.theme, "find.png"), wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "Search in File", "")
        if (os.path.exists(os.path.join(THEME_DIR, self.theme, "replace.png"))!=False):
            self.toolbar.AddLabelTool(wx.ID_REPLACE, "&Replace", wx.Bitmap(os.path.join(THEME_DIR, self.theme, "replace.png"), wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "Replace in File", "")
        self.toolbar.AddSeparator()
        if (os.path.exists(os.path.join(THEME_DIR, self.theme, "runw.png"))!=False):
            self.toolbar.AddLabelTool(self.ID_VERIFY, "&Verify", wx.Bitmap(os.path.join(THEME_DIR, self.theme, "runw.png"), wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "Compile", "")
        if (os.path.exists(os.path.join(THEME_DIR, self.theme, "dwn.png"))!=False):
            self.toolbar.AddLabelTool(self.ID_UPLOAD, "&Upload", wx.Bitmap(os.path.join(THEME_DIR, self.theme, "dwn.png"), wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "Upload to Pinguino", "")				   
        if (os.path.exists(os.path.join(THEME_DIR, self.theme, "debug.png"))!=False):
            self.toolbar.AddLabelTool(self.ID_DEBUG, "&Debug On/Off", wx.Bitmap(os.path.join(THEME_DIR, self.theme, "debug.png"), wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_CHECK, "USB Connexion with Pinguino", "")				   
        if (os.path.exists(os.path.join(THEME_DIR, self.theme, "preferences.png"))!=False):
                    self.toolbar.AddLabelTool(self.PREFERENCES_ID, "&preferences", wx.Bitmap(os.path.join(THEME_DIR, self.theme, "preferences.png"), wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "set preferences of Pinguino IDE", "")				   
        self.toolbar.AddSeparator()
        if (os.path.exists(os.path.join(THEME_DIR, self.theme, "exit.png"))!=False):		
            self.toolbar.AddLabelTool(wx.ID_EXIT, "&Exit", wx.Bitmap(os.path.join(THEME_DIR, self.theme, "exit.png"), wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "Exit Pinguino IDE", "")

        self.toolbar.Realize()		 

# ------------------------------------------------------------------------------
# readlib:
# ------------------------------------------------------------------------------

    def readlib(self, board):
        # trying to find PDL files to store reserved words
        self.keywordList = []
        if board.arch == 8:
            libext='.pdl'
            libdir=P8_DIR
        else:
            libext='.pdl32'		
            libdir=P32_DIR
        #for fichier in os.listdir(os.path.join(libdir, 'lib')):
        for fichier in os.listdir(os.path.join(libdir, 'pdl')):
            filename,extension=os.path.splitext(fichier)
            if extension==libext:
                # check content of the PDL file
                #libfile=open(os.path.join(libdir, 'lib', fichier),'r')
                libfile=open(os.path.join(libdir, 'pdl', fichier),'r')
                for line in libfile:
                    if line!="\n":
                        # arduino's instruction
                        instruction=line[0:line.find(" ")]
                        self.keywordList.append(instruction)
                        # library's instruction
                        cnvinstruction=line[line.find(" ")+1:line.find("#")]
                        # find #include & #define
                        #include=line[line.find("#")+1:len(line)]
                        include=""
                        define=""
                        explode=line.split("#")
                        if len(explode)>=2:
                            include=explode[1]
                        if len(explode)==3:
                            define=explode[2]
                        # append to the list	
                        self.libinstructions.append([instruction,cnvinstruction,include,define])
                        #regex = re.compile(r"(^|[' ']|['=']|['{']|[',']|[\t]|['(']|['!'])"+str(instruction))+"[ ]*\(")
                        #regex = re.compile(r"(^|[' ']|['=']|['{']|[',']|[\t]|['(']|['!'])"+str(instruction)+r"([' ']|['=']|['}']|[',']|[';']|[\t]|[')'].*)")
                        #regex = re.compile(r"(^|[' ']|['=']|['{']|[',']|[\t]|['(']|['!'])"+str(instruction)+".*")
                        #regex = re.compile(r'\W%s\W' % re.escape(str(instruction)))
                        regex = re.compile(r"(^|[' ']|['=']|['{']|[',']|[\t]|['(']|['!'])%s\W" % re.escape(str(instruction)))
                        self.regobject.append(regex)
                libfile.close()
        # clean up the keyword list
        self.keywordList.sort()
        self.keywordList = self.ClearRedundancy(self.keywordList)
        self.keywordNum = len(self.keywordList)
        # make reserved words list
        if len(self.libinstructions)!=0:	
            for i in range(len(self.libinstructions)):
                chaine=self.libinstructions[i][0]
                self.rw.append(chaine)
                pos = chaine.find(".")
                if pos != -1:
                    self.reservedword.append(chaine[0:pos])
                    self.reservedword.append(chaine[pos+1:len(chaine)])
                else:
                    self.reservedword.append(chaine)
            # sort keywords for short key help
            self.rw.sort(key=lambda x: x.lower())
        # adding fixed reserved word
        fixed_rw=("setup","loop","HIGH","LOW","INPUT","OUTPUT","void","FOSC","MIPS","ON","OFF","TRUE","FALSE")
        for i in range(len(fixed_rw)):
            self.reservedword.append(fixed_rw[i])

# ------------------------------------------------------------------------------
# ClearRedundancy:
# ------------------------------------------------------------------------------

    def ClearRedundancy(self, myList):
        seen = set()
        out = []
        for item in myList:
            if item not in seen:
                seen.add(item)
                out.append(item)
        return out

# ------------------------------------------------------------------------------
# displaymsg
# ------------------------------------------------------------------------------

    def displaymsg(self, message, clearpanel):
        """ display message in the log window """
        if gui==True:
            if clearpanel==1:
                self.logwindow.Clear()
            self.logwindow.WriteText(message)
        else:
            if message!="":
                print message
        return  

# ------------------------------------------------------------------------------
# translate
# ------------------------------------------------------------------------------

    def translate(self, message):
        """ translate message using gettext according current OS """
        # Checking if the host platform is a mac
        if sys.platform == 'darwin':
            return message
        else:
            # assume it's a posix or win32 platform
            return self.lang.ugettext(message)

# ------------------------------------------------------------------------------
# preprocess
# ------------------------------------------------------------------------------

    def preprocess(self, filename, board):
        defineword={}
        index=0
        fileline={}

        # delete old define.h and create a new one
        if os.path.exists(os.path.join(SOURCE_DIR, 'define.h')):
            os.remove(os.path.join(SOURCE_DIR, 'define.h'))
        fichier=open(os.path.join(SOURCE_DIR, 'define.h'), 'a')
        fichier.close()

        # rename .pde in user.c
        path,name = os.path.split(filename)
        shutil.copy(filename + '.pde', os.path.join(SOURCE_DIR, 'user.c'))
        fichier = open(os.path.join(SOURCE_DIR, 'user.c'), 'a')
        #fichier.writelines("\n")
        fichier.close()

        # debug mode		
        DEBUG_STR = "NODEBUG"
        if self.debug == self.ID_USBCDC:
            DEBUG_STR = "USBCDCDEBUG"
        if self.debug == self.ID_USBBULK:
            DEBUG_STR = "USBBULKDEBUG"
        if self.debug == self.ID_USBHID:
            DEBUG_STR = "USBHIDDEBUG"
        if self.debug == self.ID_USBOTG:
            DEBUG_STR = "USBOTGDEBUG"
        if self.debug == self.ID_UART1:
            DEBUG_STR = "UART1DEBUG"
        if self.debug == self.ID_UART2:
            DEBUG_STR = "UART2DEBUG"
        self.adddefine("#define DEBUG_MODE " + DEBUG_STR)

        # board
        #if board.arch == 8:
        #	self.adddefine("#define " + board.board)

        # add #include and #define from user.c to define.h
        fichier = open(os.path.join(SOURCE_DIR, 'user.c'), 'r')
        i=0
        for line in fichier:
            if line.find("#include")!=-1 or line.find("#define")!=-1:
                self.adddefine(line)	# add to define.h
                fileline[i]="\r\n";	# delete from user.c
                i=i+1
            else:
                fileline[i]=line
                i=i+1
        fichier.close()

        # rewrite file user.c without #include and #define
        fichier = open(os.path.join(SOURCE_DIR, 'user.c'), 'w')
        for cpt in range(i):
            fichier.write(fileline[cpt])
        fichier.close()

        # search and replace arduino keywords in file
        fichier = open(os.path.join(SOURCE_DIR, 'user.c'), 'r')
        content = fichier.read()
        content = self.removecomments(content)
        content = content.split('\n')
        nblines = 0
        for line in content:
            resultline = self.replaceword(line)
            if resultline.find("error")==1:
                line = resultline
                print "error " + resultline
                self.displaymsg("error "+resultline,1)
                return "error"
            fileline[nblines] = resultline
            nblines += 1	  
        fichier.close()

        # save new tmp file
        fichier = open(os.path.join(SOURCE_DIR, 'user.c'), 'w')
        for i in range(0,nblines):
            fichier.writelines(fileline[i])
        fichier.writelines("\r\n")
        fichier.close()

        # sort define.h
        fichier = open(os.path.join(SOURCE_DIR, 'define.h'), "r")
        lignes = fichier.readlines()
        lignes.sort()
        fichier.close()

        # save sorted lines
        fichier = open(os.path.join(SOURCE_DIR, 'define.h'), "w")
        fichier.writelines(lignes)
        fichier.close()
        return

# ------------------------------------------------------------------------------
# adddefine
# ------------------------------------------------------------------------------

    def adddefine(self,chaine):
        """ add #define in define.h file """
        fichier = open(os.path.join(SOURCE_DIR, 'define.h'), 'a')
        fichier.writelines(chaine+"\n")
        fichier.close()

# ------------------------------------------------------------------------------
# notindefine
# ------------------------------------------------------------------------------

    def notindefine(self,chaine):
        """ verify if #define exists in define.h file """
        fichier = open(os.path.join(SOURCE_DIR, 'define.h'), 'r') 
        for line in fichier:
            # chaine has been found ?
            if line.find(chaine)!=-1:
                fichier.close()
                return(0)
        fichier.close()
        return(1)		 

# ------------------------------------------------------------------------------
# replaceword
# ------------------------------------------------------------------------------

    def replaceword(self,line):
        """ convert pinguino language in C language """
        # replace arduino/pinguino language and add #define or #include to define.h
        for i in range(len(self.libinstructions)):
            if re.search(self.regobject[i], line):
                line = line.replace(str(self.libinstructions[i][0]), str(self.libinstructions[i][1]))
                if self.notindefine("#"+str(self.libinstructions[i][2])) == 1:
                    self.adddefine("#"+str(self.libinstructions[i][2]))
                if self.notindefine("#"+str(self.libinstructions[i][3])) == 1:
                    self.adddefine("#"+str(self.libinstructions[i][3]))
        return line+"\n"

# ------------------------------------------------------------------------------
# removecomment
# ------------------------------------------------------------------------------

    def removecomments(self, text):
        def replacer(match):
            s = match.group(0)
            if s.startswith('/'):
                return ""
            else:
                return s
        pattern = re.compile(
            r'//.*?$|/\*.*?\*/|\'(?:\\.|[^\\\'])*\'|"(?:\\.|[^\\"])*"',
            re.DOTALL | re.MULTILINE
        )
        return re.sub(pattern, replacer, text)

# ------------------------------------------------------------------------------
# compile
# ------------------------------------------------------------------------------

    def compile(self, filename, board):
        if (self.debug_output == 1):
            print("compile " + board.proc)
        else:
            if board.arch == 8:
                fichier = open(os.path.join(SOURCE_DIR, 'stdout'), 'w+')
                if board.bldr == 'vasco':
                    sortie = Popen([os.path.join(HOME_DIR, self.osdir, 'p8', 'bin', self.c8),\
                                    "-mpic16",\
                                    "--denable-peeps",\
                                    "--obanksel=9",\
                                    "--opt-code-size",\
                                    "--optimize-cmp",\
                                    "--optimize-df",\
                                    "-p" + board.proc,\
                                    "-D" + board.board,\
                                    "-D" + board.bldr,\
                                    "-I" + os.path.join(P8_DIR, 'include'),\
                                    "-I" + os.path.join(P8_DIR, 'include', 'non-free', 'pic16'),\
                                    "-I" + os.path.join(P8_DIR, 'include', 'pinguino', 'core'),\
                                    "-I" + os.path.join(P8_DIR, 'include', 'pinguino', 'libraries'),\
                                    "-I" + os.path.dirname(filename),\
                                    "--compile-only",\
                                    "-o" + os.path.join(SOURCE_DIR, 'main.o'),\
                                    os.path.join(SOURCE_DIR, 'main.c')],\
                                   stdout=fichier, stderr=STDOUT)
                else:# if board.bldr == 'diolan'
#							"--extended",\
                    sortie = Popen([os.path.join(HOME_DIR, self.osdir, 'p8', 'bin2', self.c8),\
                                    "-mpic16",\
                                    "--denable-peeps",\
                                    "--obanksel=9",\
                                    "--opt-code-size",\
                                    "--optimize-cmp",\
                                    "--optimize-df",\
                                    "-p" + board.proc,\
                                    "-D" + board.board,\
                                    "-D" + board.bldr,\
                                    "-I" + os.path.join(P8_DIR, 'include'),\
                                    "-I" + os.path.join(P8_DIR, 'include', 'non-free', 'pic16'),\
                                    "-I" + os.path.join(P8_DIR, 'include', 'pinguino', 'core'),\
                                    "-I" + os.path.join(P8_DIR, 'include', 'pinguino', 'libraries'),\
                                    "-I" + os.path.dirname(filename),\
                                    "--compile-only",\
                                    "-o" + os.path.join(SOURCE_DIR, 'main.o'),\
                                    os.path.join(SOURCE_DIR, 'main.c')],\
                                   stdout=fichier, stderr=STDOUT)
                sortie.communicate()
                if sortie.poll()!=0:
                    fichier.seek(0)
                    for ligne in fichier:
                        if ligne.find('error')!=-1:
                            self.displaymsg(ligne, 0)
                fichier.seek(0)
                line=fichier.readline()
                if line.find("error")!=-1:
                    number=line[line.find(":")+1:line.find("error")-2]
                    #self.highlightline(int(number)-1,'pink')
                    self.displaymsg("error line " + number + "\n", 1)
                    self.displaymsg(line[line.find("error")+7:len(line)],0)
                fichier.close()
                return sortie.poll()
            else:
                return 0

# ------------------------------------------------------------------------------
# link
# ------------------------------------------------------------------------------

    def link(self, filename, board):
        if (self.debug_output == 1):
            print("link " + board.proc)
        else:
            fichier = open(os.path.join(SOURCE_DIR, 'stdout'), 'w+')
            if board.arch == 8:
                if board.bldr == 'vasco':
                    sortie=Popen([os.path.join(HOME_DIR, self.osdir, 'p8', 'bin', self.c8),\
                                  "-o" + os.path.join(SOURCE_DIR, 'main.hex'),\
                                  "--denable-peeps",\
                                  "--obanksel=9",\
                                  "--opt-code-size",\
                                  "--optimize-cmp",\
                                  "--optimize-df",\
                                  "--no-crt",\
                                  "-Wl-s" + os.path.join(P8_DIR, 'lkr', board.bldr + board.proc + '.lkr') + ",-m",\
                                  "-mpic16",\
                                  "-p" + board.proc,\
                                  "-D" + board.bldr,\
                                  "-I" + os.path.join(P8_DIR, 'include'),\
                                  "-I" + os.path.join(P8_DIR, 'include', 'non-free', 'pic16'),\
                                  "-I" + os.path.join(P8_DIR, 'include', 'pinguino', 'core'),\
                                  "-I" + os.path.join(P8_DIR, 'include', 'pinguino', 'libraries'),\
                                  "-L" + os.path.join(P8_DIR, 'lib', 'pic16'),\
                                  '-llibio' + board.proc + '.lib',\
                                  '-llibc18f.lib',\
                                  '-llibm18f.lib',\
                                  '-llibsdcc.lib',\
                                  '-llibpuf.lib',\
                                  os.path.join(P8_DIR, 'obj', 'application_iface.o'),\
                                  os.path.join(P8_DIR, 'obj', 'usb_descriptors.o'),\
                                  os.path.join(P8_DIR, 'obj', 'crt0ipinguino.o'),\
                                  os.path.join(SOURCE_DIR, 'main.o')],\
                                 stdout=fichier, stderr=STDOUT)
                else:# if board.bldr == 'diolan'
#							"--extended",\
                    sortie=Popen([os.path.join(HOME_DIR, self.osdir, 'p8', 'bin2', self.c8),\
                                  "-o" + os.path.join(SOURCE_DIR, 'main.hex'),\
                                  "-mpic16",\
                                  "--denable-peeps",\
                                  "--obanksel=9",\
                                  "--opt-code-size",\
                                  "--optimize-cmp",\
                                  "--optimize-df",\
                                  "--no-crt",\
                                  "-Wl-s" + os.path.join(P8_DIR, 'lkr', board.bldr + board.proc + '.lkr') + ",-m",\
                                  "-p" + board.proc,\
                                  "-D" + board.bldr,\
                                  "-I" + os.path.join(P8_DIR, 'include'),\
                                  "-I" + os.path.join(P8_DIR, 'include', 'non-free', 'pic16'),\
                                  "-I" + os.path.join(P8_DIR, 'include', 'pinguino', 'core'),\
                                  "-I" + os.path.join(P8_DIR, 'include', 'pinguino', 'libraries'),\
                                  "-L" + os.path.join(P8_DIR, 'lib', 'pic16'),\
                                  '-llibio' + board.proc + '.lib',\
                                  '-llibc18f.lib',\
                                  '-llibm18f.lib',\
                                  '-llibsdcc.lib',\
                                  os.path.join(P8_DIR, 'obj', 'crt0i' + board.proc + '.o'),\
                                  os.path.join(SOURCE_DIR, 'main.o')],\
                                 stdout=fichier, stderr=STDOUT)
            else:
                # "PDEDIR=" + os.path.dirname(self.GetPath()),\
                # can't be used with Command Line version since editor isn't used
                sortie=Popen([self.make,\
                              "--makefile=" + os.path.join(SOURCE_DIR, 'Makefile32.'+self.osdir),\
                              "HOME=" + HOME_DIR,\
                              "PDEDIR=" + os.path.dirname(filename),\
                              "PROC=" + board.proc,\
                              "BOARD=" + board.board],\
                             stdout=fichier, stderr=STDOUT)
            sortie.communicate()
            fichier.seek(0)
            # Check if child process has terminated
            if sortie.poll()!=0:
                for ligne in fichier:
                    if ligne.find('error')!=-1:
                        self.displaymsg(ligne, 0)
            fichier.close()
            if sys.platform=='win32':
                if os.path.exists(os.path.join(SOURCE_DIR,"main32tmp.hex")):
                    fichiersource=open(os.path.join(SOURCE_DIR,"main32tmp.hex"),'r')
                    fichierdest=open(os.path.join(SOURCE_DIR,"main32.hex"),'w+')
                    for line in fichiersource:
                        if line!=":040000059D006000FA\n":
                            fichierdest.writelines(line)
                    fichiersource.close()
                    fichierdest.close()
                    os.remove(os.path.join(SOURCE_DIR,"main32tmp.hex"))
            return sortie.poll()

# ------------------------------------------------------------------------------
# getCodeSize
# ------------------------------------------------------------------------------

    def getCodeSize(self, filename, board):
        codesize = 0
        totalsize = board.memend - board.memstart
        fichier = open(filename + ".hex", 'r')
        lines = fichier.readlines()
        for line in lines:
            # count only data record
            if line[7:9:1] == "00":
                if board.arch == 8:
                    # filter records below 0x2000 (by RASM)
                    #if int(line[3:7:1],16) >= int("2000", 16): 
                    if int(line[3:7:1],16) >= board.memstart:
                        codesize = codesize + int(line[1:3:1], 16)
                else:
                    codesize = codesize + int(line[1:3:1], 16)
        fichier.close()
        return "code size: " + str(codesize) + " / " + str(totalsize) + " bytes" + " (" + str(100*codesize/totalsize) + "% used)"

    #----------------------------------------------------------------------
    def OnPreferences(self, event=None):
        app = wx.PySimpleApp(0)
        wx.InitAllImageHandlers()
        frame_1 = Preferences(self)
        app.SetTopWindow(frame_1)
        frame_1.CenterOnParent()
        frame_1.Show()
        app.MainLoop()
        
    #----------------------------------------------------------------------
    def OnDrop(self, event):
        file = event.GetDragText().replace("file://", "").replace("%20", " ")
        paths = file.split("\n")
        for path in paths:
            print path
            if os.path.isfile(path):
                self.Open(path,
                          self.reservedword,
                          self.rw,
                          self.filehistory,
                          self.config)
        event.SetDragText("")
        
        
    #----------------------------------------------------------------------
    def appyPreferences(self):
        self.DrawToolbar()
        


# ------------------------------------------------------------------------------
# getOptions
# ------------------------------------------------------------------------------

def getOptions():
    parser = argparse.ArgumentParser(description='*** Pinguino IDE ***')
    parser.add_argument('-v', '--version', dest='version', action='store_true', default=False, help='show Pinguino IDE version and exit')
    parser.add_argument('-a', '--author', dest='author', action='store_true', default=False, help='show authors of this Pinguino IDE version and exit')
    parser.add_argument('-f', '--filename', dest='filename', nargs=1, default=False, help='filename to process')
    for b in range(len(boardlist)):
        parser.add_argument(	boardlist[b].shortarg,
                                    boardlist[b].longarg,
                                    dest='board',
                                    const=b,
                                    action='store_const',
                                    default=False,
                                    help='compile code for ' + boardlist[b].board + ' board')
    return parser.parse_args()

#----------------------------------------------------------------------
def setGui(bool):
    global gui
    gui=bool