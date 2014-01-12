#include "winutil.h"

#include "main.h"

// most of the enum-related functions in here are auto-generated

const char* SystemMetricToString(int metric) {
	switch(metric) {
	case SM_CXSCREEN: return "SM_CXSCREEN";
	case SM_CYSCREEN: return "SM_CYSCREEN";
	case SM_CXVSCROLL: return "SM_CXVSCROLL";
	case SM_CYHSCROLL: return "SM_CYHSCROLL";
	case SM_CYCAPTION: return "SM_CYCAPTION";
	case SM_CXBORDER: return "SM_CXBORDER";
	case SM_CYBORDER: return "SM_CYBORDER";
	case SM_CXDLGFRAME: return "SM_CXDLGFRAME";
	case SM_CYDLGFRAME: return "SM_CYDLGFRAME";
	case SM_CYVTHUMB: return "SM_CYVTHUMB";
	case SM_CXHTHUMB: return "SM_CXHTHUMB";
	case SM_CXICON: return "SM_CXICON";
	case SM_CYICON: return "SM_CYICON";
	case SM_CXCURSOR: return "SM_CXCURSOR";
	case SM_CYCURSOR: return "SM_CYCURSOR";
	case SM_CYMENU: return "SM_CYMENU";
	case SM_CXFULLSCREEN: return "SM_CXFULLSCREEN";
	case SM_CYFULLSCREEN: return "SM_CYFULLSCREEN";
	case SM_CYKANJIWINDOW: return "SM_CYKANJIWINDOW";
	case SM_MOUSEPRESENT: return "SM_MOUSEPRESENT";
	case SM_CYVSCROLL: return "SM_CYVSCROLL";
	case SM_CXHSCROLL: return "SM_CXHSCROLL";
	case SM_DEBUG: return "SM_DEBUG";
	case SM_SWAPBUTTON: return "SM_SWAPBUTTON";
	case SM_RESERVED1: return "SM_RESERVED1";
	case SM_RESERVED2: return "SM_RESERVED2";
	case SM_RESERVED3: return "SM_RESERVED3";
	case SM_RESERVED4: return "SM_RESERVED4";
	case SM_CXMIN: return "SM_CXMIN";
	case SM_CYMIN: return "SM_CYMIN";
	case SM_CXSIZE: return "SM_CXSIZE";
	case SM_CYSIZE: return "SM_CYSIZE";
	case SM_CXFRAME: return "SM_CXFRAME";
	case SM_CYFRAME: return "SM_CYFRAME";
	case SM_CXMINTRACK: return "SM_CXMINTRACK";
	case SM_CYMINTRACK: return "SM_CYMINTRACK";
	case SM_CXDOUBLECLK: return "SM_CXDOUBLECLK";
	case SM_CYDOUBLECLK: return "SM_CYDOUBLECLK";
	case SM_CXICONSPACING: return "SM_CXICONSPACING";
	case SM_CYICONSPACING: return "SM_CYICONSPACING";
	case SM_MENUDROPALIGNMENT: return "SM_MENUDROPALIGNMENT";
	case SM_PENWINDOWS: return "SM_PENWINDOWS";
	case SM_DBCSENABLED: return "SM_DBCSENABLED";
	case SM_CMOUSEBUTTONS: return "SM_CMOUSEBUTTONS";
	case SM_SECURE: return "SM_SECURE";
	case SM_CXEDGE: return "SM_CXEDGE";
	case SM_CYEDGE: return "SM_CYEDGE";
	case SM_CXMINSPACING: return "SM_CXMINSPACING";
	case SM_CYMINSPACING: return "SM_CYMINSPACING";
	case SM_CXSMICON: return "SM_CXSMICON";
	case SM_CYSMICON: return "SM_CYSMICON";
	case SM_CYSMCAPTION: return "SM_CYSMCAPTION";
	case SM_CXSMSIZE: return "SM_CXSMSIZE";
	case SM_CYSMSIZE: return "SM_CYSMSIZE";
	case SM_CXMENUSIZE: return "SM_CXMENUSIZE";
	case SM_CYMENUSIZE: return "SM_CYMENUSIZE";
	case SM_ARRANGE: return "SM_ARRANGE";
	case SM_CXMINIMIZED: return "SM_CXMINIMIZED";
	case SM_CYMINIMIZED: return "SM_CYMINIMIZED";
	case SM_CXMAXTRACK: return "SM_CXMAXTRACK";
	case SM_CYMAXTRACK: return "SM_CYMAXTRACK";
	case SM_CXMAXIMIZED: return "SM_CXMAXIMIZED";
	case SM_CYMAXIMIZED: return "SM_CYMAXIMIZED";
	case SM_NETWORK: return "SM_NETWORK";
	case SM_CLEANBOOT: return "SM_CLEANBOOT";
	case SM_CXDRAG: return "SM_CXDRAG";
	case SM_CYDRAG: return "SM_CYDRAG";
	case SM_SHOWSOUNDS: return "SM_SHOWSOUNDS";
	case SM_CXMENUCHECK: return "SM_CXMENUCHECK";
	case SM_CYMENUCHECK: return "SM_CYMENUCHECK";
	case SM_SLOWMACHINE: return "SM_SLOWMACHINE";
	case SM_MIDEASTENABLED: return "SM_MIDEASTENABLED";
	case SM_MOUSEWHEELPRESENT: return "SM_MOUSEWHEELPRESENT";
	case SM_XVIRTUALSCREEN: return "SM_XVIRTUALSCREEN";
	case SM_YVIRTUALSCREEN: return "SM_YVIRTUALSCREEN";
	case SM_CXVIRTUALSCREEN: return "SM_CXVIRTUALSCREEN";
	case SM_CYVIRTUALSCREEN: return "SM_CYVIRTUALSCREEN";
	case SM_CMONITORS: return "SM_CMONITORS";
	case SM_SAMEDISPLAYFORMAT: return "SM_SAMEDISPLAYFORMAT";
	case SM_IMMENABLED: return "SM_IMMENABLED";
	case SM_CXFOCUSBORDER: return "SM_CXFOCUSBORDER";
	case SM_CYFOCUSBORDER: return "SM_CYFOCUSBORDER";
	case SM_TABLETPC: return "SM_TABLETPC";
	case SM_MEDIACENTER: return "SM_MEDIACENTER";
	case SM_STARTER: return "SM_STARTER";
	case SM_SERVERR2: return "SM_SERVERR2";
	case SM_MOUSEHORIZONTALWHEELPRESENT: return "SM_MOUSEHORIZONTALWHEELPRESENT";
	case SM_CXPADDEDBORDER: return "SM_CXPADDEDBORDER";
	case SM_REMOTESESSION: return "SM_REMOTESESSION";
	case SM_SHUTTINGDOWN: return "SM_SHUTTINGDOWN";
	}
	return "Unknown Metric!";
}

string RectToString(const RECT* rect) {
	if(!rect) return string("NULL_RECT");
	return format("RECT[%4ld/%4ld/%4ld/%4ld]", rect->left, rect->top, rect->right, rect->bottom);
}

