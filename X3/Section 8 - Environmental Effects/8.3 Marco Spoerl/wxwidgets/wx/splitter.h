#ifndef _WX_SPLITTER_H_BASE_
#define _WX_SPLITTER_H_BASE_

#include "wx/event.h"

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED, 850)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING, 851)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_SPLITTER_DOUBLECLICKED, 852)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_SPLITTER_UNSPLIT, 853)
END_DECLARE_EVENT_TYPES()

#include "wx/generic/splitter.h"

#endif
    // _WX_SPLITTER_H_BASE_
