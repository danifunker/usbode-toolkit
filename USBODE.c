/*
 * USBODE.c
 * Main implementation for USBODE Disc Manager
 * 
 * Classic Mac OS application for managing disc images on USBODE device
 */

#include "USBODE.h"

/* Global variables */
Globals gGlobals;

/*
 * Main entry point
 */
void main(void)
{
    ToolBoxInit();
    MenuBarInit();
    
    /* Find USBODE device on SCSI bus */
    gGlobals.deviceFound = FindUSBODEDevice(&gGlobals.scsiID);
    
    WindowInit();
    
    if (gGlobals.deviceFound) {
        RefreshDiscList();
    } else {
        ShowError("\pUSBODE device not found on SCSI bus");
    }
    
    EventLoop();
}

/*
 * Initialize the Macintosh Toolbox
 */
void ToolBoxInit(void)
{
    InitGraf(&qd.thePort);
    InitFonts();
    InitWindows();
    InitMenus();
    TEInit();
    InitDialogs(nil);
    InitCursor();
    
    /* Check for WaitNextEvent */
    gGlobals.hasWNE = (NGetTrapAddress(_WaitNextEvent, ToolTrap) != 
                       NGetTrapAddress(_Unimplemented, ToolTrap));
    
    gGlobals.done = false;
    gGlobals.discCount = 0;
    gGlobals.deviceFound = false;
}

/*
 * Initialize menu bar
 */
void MenuBarInit(void)
{
    Handle menuBar;
    
    menuBar = GetNewMBar(rMenuBar);
    SetMenuBar(menuBar);
    
    gGlobals.appleMenu = GetMenuHandle(mApple);
    AppendResMenu(gGlobals.appleMenu, 'DRVR');
    
    gGlobals.fileMenu = GetMenuHandle(mFile);
    gGlobals.editMenu = GetMenuHandle(mEdit);
    
    DrawMenuBar();
}

/*
 * Initialize main window
 */
void WindowInit(void)
{
    gGlobals.window = GetNewWindow(rMainWindow, nil, kMoveToFront);
    SetPort(gGlobals.window);
    ShowWindow(gGlobals.window);
}

/*
 * Find USBODE device on SCSI bus
 * Returns true if found, sets scsiID to device ID
 */
Boolean FindUSBODEDevice(short *scsiID)
{
    short id;
    
    /* Scan SCSI bus for USBODE device */
    for (id = 0; id < 7; id++) {
        if (IsUSBODEDevice(id)) {
            *scsiID = id;
            return true;
        }
    }
    
    /* Device not found */
    *scsiID = -1;
    return false;
}

/*
 * Scan SCSI bus and list all connected devices
 * Returns number of devices found
 */
short ScanSCSIBus(void)
{
    short id;
    short deviceCount;
    OSErr err;
    Str255 message;
    
    deviceCount = 0;
    
    /* Try each SCSI ID from 0 to 6 (7 is typically the Mac itself) */
    for (id = 0; id < 7; id++) {
        err = SCSIGet();
        if (err != noErr) continue;
        
        err = SCSISelect(id);
        if (err == noErr) {
            /* Device found at this ID */
            deviceCount++;
            
            /* Build message */
            message[0] = 0;
            BlockMove("\pDevice found at SCSI ID ", message, 25);
            message[0] = 24;
            message[25] = '0' + id;
            message[0] = 25;
            
            /* Check if it's USBODE */
            if (IsUSBODEDevice(id)) {
                BlockMove(" (USBODE)", message + 26, 10);
                message[0] = 35;
            }
            
            /* Show in debugger or alert */
            DebugStr(message);
        }
        
        SCSIComplete(nil, nil, 0);
    }
    
    return deviceCount;
}

/*
 * Test if a device at given SCSI ID responds to USBODE commands
 */
Boolean IsUSBODEDevice(short scsiID)
{
    unsigned char count;
    OSErr err;
    
    /* Try to get disc count - if this works, it's likely USBODE */
    err = GetDiscCount(scsiID, &count);
    
    return (err == noErr);
}

/*
 * Send a SCSI command to the USBODE device
 */
OSErr SendSCSICommand(short scsiID, unsigned char cmd, unsigned char param,
                      void *buffer, long bufferSize, long *actualSize)
{
    OSErr err;
    unsigned char cdb[12];
    int i;
    short scsiResult;
    
    /* Initialize CDB */
    for (i = 0; i < 12; i++) {
        cdb[i] = 0;
    }
    cdb[0] = cmd;
    cdb[1] = param;
    
    /* Open SCSI Manager */
    err = SCSIGet();
    if (err != noErr) return err;
    
    /* Select target device */
    err = SCSISelect(scsiID);
    if (err != noErr) {
        SCSIComplete(&scsiResult, nil, 0);
        return err;
    }
    
    /* Send command */
    err = SCSICmd((char *)cdb, 12);
    if (err != noErr) {
        SCSIComplete(&scsiResult, nil, 0);
        return err;
    }
    
    /* Read response if buffer provided */
    if (buffer != nil && bufferSize > 0) {
        err = SCSIRead(buffer);
        if (err != noErr) {
            SCSIComplete(&scsiResult, nil, 0);
            return err;
        }
    }
    
    /* Complete transaction */
    err = SCSIComplete(&scsiResult, nil, 0);
    if (actualSize != nil) {
        *actualSize = (long)scsiResult;
    }
    
    return err;
}

/*
 * Get number of discs available on USBODE
 */
OSErr GetDiscCount(short scsiID, unsigned char *count)
{
    long actualSize;
    unsigned char response;
    OSErr err;
    
    err = SendSCSICommand(scsiID, SCSI_CMD_NUM_CDS, 0, 
                         &response, (long)sizeof(response), &actualSize);
    
    if (err == noErr && actualSize >= 1) {
        *count = response;
        if (*count > kMaxDiscs) {
            *count = kMaxDiscs;
        }
    } else {
        *count = 0;
    }
    
    return err;
}

/*
 * Get list of discs from USBODE
 */
OSErr GetDiscList(short scsiID, DiscEntry *discs, unsigned char count)
{
    long actualSize;
    OSErr err;
    long bufferSize = count * sizeof(DiscEntry);
    
    err = SendSCSICommand(scsiID, SCSI_CMD_LIST_CDS, 0,
                         discs, bufferSize, &actualSize);
    
    return err;
}

/*
 * Set the active disc on USBODE
 */
OSErr SetActiveDisc(short scsiID, unsigned char index)
{
    long actualSize;
    OSErr err;
    
    err = SendSCSICommand(scsiID, SCSI_CMD_SET_NEXT_CD, index,
                         nil, 0, &actualSize);
    
    return err;
}

/*
 * Refresh the disc list from device
 */
void RefreshDiscList(void)
{
    OSErr err;
    unsigned char count;
    
    if (!gGlobals.deviceFound) {
        return;
    }
    
    /* Get count */
    err = GetDiscCount(gGlobals.scsiID, &count);
    if (err != noErr) {
        ShowError("\pError reading disc count");
        return;
    }
    
    gGlobals.discCount = count;
    
    /* Get list */
    if (count > 0) {
        err = GetDiscList(gGlobals.scsiID, gGlobals.discs, count);
        if (err != noErr) {
            ShowError("\pError reading disc list");
            return;
        }
    }
    
    /* Redraw window */
    if (gGlobals.window != nil) {
        InvalRect(&gGlobals.window->portRect);
    }
}

/*
 * Main event loop
 */
void EventLoop(void)
{
    gGlobals.done = false;
    
    while (!gGlobals.done) {
        DoEvent();
    }
}

/*
 * Handle one event
 */
void DoEvent(void)
{
    Boolean gotEvent;
    char theChar;
    
    if (gGlobals.hasWNE) {
        gotEvent = WaitNextEvent(everyEvent, &gGlobals.event, kSleep, nil);
    } else {
        SystemTask();
        gotEvent = GetNextEvent(everyEvent, &gGlobals.event);
    }
    
    if (gotEvent) {
        switch (gGlobals.event.what) {
            case mouseDown:
                HandleMouseDown();
                break;
                
            case keyDown:
            case autoKey:
                if (gGlobals.event.modifiers & cmdKey) {
                    theChar = (char)(gGlobals.event.message & charCodeMask);
                    DoMenuCommand(MenuKey(theChar));
                }
                break;
                
            case updateEvt:
                HandleUpdate();
                break;
                
            case activateEvt:
                HandleActivate();
                break;
        }
    }
}

/*
 * Handle mouse down events
 */
void HandleMouseDown(void)
{
    WindowPtr window;
    short part;
    long menuChoice;
    Point localPt;
    
    part = FindWindow(gGlobals.event.where, &window);
    
    switch (part) {
        case inMenuBar:
            menuChoice = MenuSelect(gGlobals.event.where);
            DoMenuCommand(menuChoice);
            break;
            
        case inSysWindow:
            SystemClick(&gGlobals.event, window);
            break;
            
        case inContent:
            if (window != FrontWindow()) {
                SelectWindow(window);
            } else {
                /* Handle clicks in window content */
                localPt = gGlobals.event.where;
                GlobalToLocal(&localPt);
                
                /* Check if clicking on a disc in the list */
                /* This will be enhanced with proper UI controls */
            }
            break;
            
        case inDrag:
            DragWindow(window, gGlobals.event.where, &qd.screenBits.bounds);
            break;
            
        case inGoAway:
            if (TrackGoAway(window, gGlobals.event.where)) {
                gGlobals.done = true;
            }
            break;
    }
}

/*
 * Handle menu commands
 */
void DoMenuCommand(long menuResult)
{
    short menuID, menuItem;
    Str255 name;
    
    menuID = HiWord(menuResult);
    menuItem = LoWord(menuResult);
    
    switch (menuID) {
        case mApple:
            if (menuItem == iAbout) {
                DoAboutBox();
            } else {
                GetMenuItemText(gGlobals.appleMenu, menuItem, name);
                OpenDeskAcc(name);
            }
            break;
            
        case mFile:
            if (menuItem == iRefresh) {
                RefreshDiscList();
            } else if (menuItem == iScanBus) {
                ShowScanResults();
            } else if (menuItem == iQuit) {
                gGlobals.done = true;
            }
            break;
            
        case mEdit:
            /* Standard edit menu handling for desk accessories */
            (void)SystemEdit(menuItem - 1);
            break;
    }
    
    HiliteMenu(0);
}

/*
 * Show About dialog
 */
void DoAboutBox(void)
{
    Alert(rAboutAlert, nil);
}

/*
 * Handle update events
 */
void HandleUpdate(void)
{
    WindowPtr window;
    
    window = (WindowPtr)gGlobals.event.message;
    
    SetPort(window);
    BeginUpdate(window);
    
    EraseRect(&window->portRect);
    DrawDiscList();
    
    EndUpdate(window);
}

/*
 * Handle activate events
 */
void HandleActivate(void)
{
    WindowPtr window;
    Boolean activate;
    
    window = (WindowPtr)gGlobals.event.message;
    activate = (gGlobals.event.modifiers & activeFlag) != 0;
    
    SetPort(window);
}

/*
 * Draw the list of discs
 */
void DrawDiscList(void)
{
    Str255 str;
    Str255 sizeStr;
    short i;
    short lineHeight;
    short topMargin;
    
    lineHeight = 16;
    topMargin = 40;
    
    /* Draw title */
    MoveTo(10, 20);
    TextFont(systemFont);
    TextSize(12);
    TextFace(bold);
    DrawString("\pUSBODE Disc Manager");
    
    /* Draw disc count */
    if (!gGlobals.deviceFound) {
        MoveTo(10, topMargin);
        TextFace(normal);
        DrawString("\pNo USBODE device found");
        return;
    }
    
    NumToString(gGlobals.discCount, str);
    MoveTo(10, topMargin);
    TextFace(normal);
    DrawString("\pAvailable discs: ");
    DrawString(str);
    
    /* Draw disc list */
    topMargin += 20;
    for (i = 0; i < gGlobals.discCount; i++) {
        MoveTo(20, topMargin + (i * lineHeight));
        
        /* Draw index */
        NumToString(gGlobals.discs[i].index, str);
        DrawString(str);
        DrawString("\p. ");
        
        /* Draw name */
        CStringToPascal((char *)gGlobals.discs[i].name, str);
        DrawString(str);
        
        /* Draw size */
        GetDiscSizeString(&gGlobals.discs[i], sizeStr);
        DrawString("\p  (");
        DrawString(sizeStr);
        DrawString("\p)");
    }
    
    /* Draw instructions */
    MoveTo(10, topMargin + (gGlobals.discCount * lineHeight) + 30);
    TextFace(italic);
    DrawString("\pDouble-click a disc to mount it, or use File > Refresh to update the list");
}

/*
 * Convert disc size to readable string
 */
void GetDiscSizeString(DiscEntry *disc, Str255 sizeStr)
{
    unsigned long size;
    unsigned char spaceStr[4];
    
    /* Extract size from 5-byte big endian (skip first byte) */
    size = ((unsigned long)disc->size[1] << 24) |
           ((unsigned long)disc->size[2] << 16) |
           ((unsigned long)disc->size[3] << 8) |
           ((unsigned long)disc->size[4]);
    
    /* Convert to MB */
    size = size / (1024 * 1024);
    
    NumToString(size, sizeStr);
    
    /* Append " MB" */
    spaceStr[0] = 3;
    spaceStr[1] = ' ';
    spaceStr[2] = 'M';
    spaceStr[3] = 'B';
    BlockMove(spaceStr + 1, sizeStr + sizeStr[0] + 1, 3);
    sizeStr[0] += 3;
}

/*
 * Mount selected disc
 * Note: Basic version - for enhanced UI with selection, use USBODE_UI.c
 */
void MountSelectedDisc(void)
{
    OSErr err;
    short discIndex;
    Str255 indexStr;
    Str255 discName;
    
    if (!gGlobals.deviceFound || gGlobals.discCount == 0) {
        ShowError("\pNo discs available to mount");
        return;
    }
    
    /* For basic version, mount the first disc (index 0) as a simple implementation
     * In production, you would add a dialog to ask which disc to mount,
     * or use USBODE_UI.c for full interactive selection
     */
    discIndex = 0;
    
    if (discIndex >= 0 && discIndex < gGlobals.discCount) {
        /* Get disc name for confirmation */
        CStringToPascal((char *)gGlobals.discs[discIndex].name, discName);
        
        /* Mount the disc */
        err = SetActiveDisc(gGlobals.scsiID, gGlobals.discs[discIndex].index);
        
        if (err == noErr) {
            /* Show success message */
            ParamText(discName, "\p", "\p", "\p");
            Alert(rUserAlert, nil);
        } else {
            ShowError("\pError mounting disc");
        }
    }
}

/*
 * Show error message
 */
void ShowError(Str255 message)
{
    ParamText(message, "\p", "\p", "\p");
    Alert(rUserAlert, nil);
}

/*
 * Show SCSI bus scan results in a dialog
 */
void ShowScanResults(void)
{
    short id;
    short deviceCount;
    OSErr err;
    Str255 resultMsg;
    Str255 tempStr;
    short scsiResult;
    
    deviceCount = 0;
    
    /* Build result message */
    BlockMove("\pSCSI/USB Device Scan:\r\r", resultMsg, 24);
    resultMsg[0] = 23;
    
    /* Try each SCSI ID from 0 to 6 */
    for (id = 0; id < 7; id++) {
        err = SCSIGet();
        if (err != noErr) continue;
        
        err = SCSISelect(id);
        if (err == noErr) {
            /* Device found */
            deviceCount++;
            
            /* Add to message: "ID X: " */
            tempStr[0] = 5;
            tempStr[1] = 'I';
            tempStr[2] = 'D';
            tempStr[3] = ' ';
            tempStr[4] = '0' + id;
            tempStr[5] = ':';
            tempStr[6] = ' ';
            tempStr[0] = 6;
            
            /* Append to result */
            if (resultMsg[0] + tempStr[0] < 255) {
                BlockMove(tempStr + 1, resultMsg + resultMsg[0] + 1, tempStr[0]);
                resultMsg[0] += tempStr[0];
            }
            
            /* Check if USBODE */
            if (IsUSBODEDevice(id)) {
                BlockMove("\pUSBODE Device", tempStr, 15);
                tempStr[0] = 14;
                
                /* Mark as current if it's our device */
                if (id == gGlobals.scsiID && gGlobals.deviceFound) {
                    BlockMove("\pUSBODE (active)", tempStr, 17);
                    tempStr[0] = 16;
                }
            } else {
                BlockMove("\pUnknown Device", tempStr, 16);
                tempStr[0] = 15;
            }
            
            /* Append device type and newline */
            if (resultMsg[0] + tempStr[0] + 1 < 255) {
                BlockMove(tempStr + 1, resultMsg + resultMsg[0] + 1, tempStr[0]);
                resultMsg[0] += tempStr[0];
                resultMsg[resultMsg[0] + 1] = '\r';
                resultMsg[0] += 1;
            }
        }
        
        SCSIComplete(&scsiResult, nil, 0);
    }
    
    /* Add summary */
    if (deviceCount == 0) {
        BlockMove("\rNo devices found.", tempStr, 19);
        tempStr[0] = 18;
    } else {
        tempStr[0] = 8;
        tempStr[1] = '\r';
        tempStr[2] = '\r';
        tempStr[3] = 'T';
        tempStr[4] = 'o';
        tempStr[5] = 't';
        tempStr[6] = 'a';
        tempStr[7] = 'l';
        tempStr[8] = ':';
        tempStr[9] = ' ';
        tempStr[10] = '0' + deviceCount;
        tempStr[0] = 10;
    }
    
    if (resultMsg[0] + tempStr[0] < 255) {
        BlockMove(tempStr + 1, resultMsg + resultMsg[0] + 1, tempStr[0]);
        resultMsg[0] += tempStr[0];
    }
    
    /* Add note about USB devices */
    BlockMove("\r\rNote: USB devices may appear as SCSI.", tempStr, 41);
    tempStr[0] = 40;
    if (resultMsg[0] + tempStr[0] < 255) {
        BlockMove(tempStr + 1, resultMsg + resultMsg[0] + 1, tempStr[0]);
        resultMsg[0] += tempStr[0];
    }
    
    /* Show in alert */
    ParamText(resultMsg, "\p", "\p", "\p");
    Alert(rUserAlert, nil);
}

/*
 * Convert C string to Pascal string
 */
void CStringToPascal(const char *cStr, Str255 pStr)
{
    short i;
    short len;
    
    len = 0;
    while (cStr[len] != '\0' && len < 255) {
        len++;
    }
    
    pStr[0] = (unsigned char)len;
    for (i = 0; i < len; i++) {
        pStr[i + 1] = (unsigned char)cStr[i];
    }
}

/*
 * Convert Pascal string to C string
 */
void PascalToCString(ConstStr255Param pStr, char *cStr, short maxLen)
{
    short i;
    short len;
    
    len = pStr[0];
    if (len >= maxLen) {
        len = maxLen - 1;
    }
    
    for (i = 0; i < len; i++) {
        cStr[i] = (char)pStr[i + 1];
    }
    cStr[len] = '\0';
}
