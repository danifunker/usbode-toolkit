/*
 * USBODE_Simple.c
 * Simplified single-file version of USBODE Disc Manager
 * 
 * This version combines all functionality into one file for easier building
 * Ideal for quick compilation or older development environments
 */

#include <Types.h>
#include <Memory.h>
#include <Quickdraw.h>
#include <Fonts.h>
#include <Events.h>
#include <Menus.h>
#include <Windows.h>
#include <TextEdit.h>
#include <Dialogs.h>
#include <OSUtils.h>
#include <ToolUtils.h>
#include <SCSI.h>

/* Constants */
#define kBaseResID          128
#define kMoveToFront        (WindowPtr)-1L
#define kSleep              20
#define kMaxDiscs           100

#define SCSI_CMD_NUM_CDS    0xDA
#define SCSI_CMD_LIST_CDS   0xD7
#define SCSI_CMD_SET_NEXT_CD 0xD8

#define rMenuBar            128
#define rAboutAlert         128
#define rMainWindow         128
#define rUserAlert          129

#define mApple              128
#define iAbout              1
#define mFile               129
#define iRefresh            1
#define iQuit               3
#define mEdit               130

/* Structures */
typedef struct {
    unsigned char index;
    unsigned char type;
    unsigned char name[33];
    unsigned char size[5];
} DiscEntry;

typedef struct {
    Boolean     done;
    Boolean     hasWNE;
    EventRecord event;
    WindowPtr   window;
    MenuHandle  appleMenu;
    MenuHandle  fileMenu;
    MenuHandle  editMenu;
    DiscEntry   discs[kMaxDiscs];
    short       discCount;
    short       scsiID;
    Boolean     deviceFound;
} Globals;

Globals gGlobals;

/* Forward declarations */
void ToolBoxInit(void);
void MenuBarInit(void);
void WindowInit(void);
Boolean FindUSBODEDevice(short *scsiID);
OSErr GetDiscCount(short scsiID, unsigned char *count);
OSErr GetDiscList(short scsiID, DiscEntry *discs, unsigned char count);
OSErr SetActiveDisc(short scsiID, unsigned char index);
void RefreshDiscList(void);
void EventLoop(void);
void DoEvent(void);
void HandleMouseDown(void);
void HandleUpdate(void);
void DoMenuCommand(long menuResult);
void DoAboutBox(void);
void DrawDiscList(void);
void ShowError(Str255 message);
void CStringToPascal(const char *cStr, Str255 pStr);
void GetDiscSizeString(DiscEntry *disc, Str255 sizeStr);

/* Main */
void main(void)
{
    ToolBoxInit();
    MenuBarInit();
    
    gGlobals.deviceFound = FindUSBODEDevice(&gGlobals.scsiID);
    
    WindowInit();
    
    if (gGlobals.deviceFound) {
        RefreshDiscList();
    } else {
        ShowError("\pUSBODE device not found");
    }
    
    EventLoop();
}

void ToolBoxInit(void)
{
    InitGraf(&qd.thePort);
    InitFonts();
    InitWindows();
    InitMenus();
    TEInit();
    InitDialogs(nil);
    InitCursor();
    
    gGlobals.hasWNE = (NGetTrapAddress(_WaitNextEvent, ToolTrap) != 
                       NGetTrapAddress(_Unimplemented, ToolTrap));
    
    gGlobals.done = false;
    gGlobals.discCount = 0;
}

void MenuBarInit(void)
{
    Handle menuBar = GetNewMBar(rMenuBar);
    SetMenuBar(menuBar);
    
    gGlobals.appleMenu = GetMenuHandle(mApple);
    AppendResMenu(gGlobals.appleMenu, 'DRVR');
    
    gGlobals.fileMenu = GetMenuHandle(mFile);
    gGlobals.editMenu = GetMenuHandle(mEdit);
    
    DrawMenuBar();
}

void WindowInit(void)
{
    gGlobals.window = GetNewWindow(rMainWindow, nil, kMoveToFront);
    SetPort(gGlobals.window);
    ShowWindow(gGlobals.window);
}

Boolean FindUSBODEDevice(short *scsiID)
{
    *scsiID = 0;
    unsigned char count;
    return (GetDiscCount(*scsiID, &count) == noErr);
}

OSErr GetDiscCount(short scsiID, unsigned char *count)
{
    OSErr err = SCSIGet();
    if (err != noErr) return err;
    
    err = SCSISelect(scsiID);
    if (err != noErr) {
        long dummy;
        SCSIComplete(&dummy, nil, 0);
        return err;
    }
    
    unsigned char cdb[12] = {SCSI_CMD_NUM_CDS, 0};
    err = SCSICmd(cdb, 12);
    if (err != noErr) {
        long dummy;
        SCSIComplete(&dummy, nil, 0);
        return err;
    }
    
    unsigned char response;
    err = SCSIRead(&response);
    
    long actualSize;
    SCSIComplete(&actualSize, nil, 0);
    
    if (err == noErr && actualSize >= 1) {
        *count = response;
        if (*count > kMaxDiscs) *count = kMaxDiscs;
    } else {
        *count = 0;
    }
    
    return err;
}

OSErr GetDiscList(short scsiID, DiscEntry *discs, unsigned char count)
{
    OSErr err = SCSIGet();
    if (err != noErr) return err;
    
    err = SCSISelect(scsiID);
    if (err != noErr) {
        long dummy;
        SCSIComplete(&dummy, nil, 0);
        return err;
    }
    
    unsigned char cdb[12] = {SCSI_CMD_LIST_CDS, 0};
    err = SCSICmd(cdb, 12);
    if (err != noErr) {
        long dummy;
        SCSIComplete(&dummy, nil, 0);
        return err;
    }
    
    err = SCSIRead(discs);
    
    long actualSize;
    SCSIComplete(&actualSize, nil, 0);
    
    return err;
}

OSErr SetActiveDisc(short scsiID, unsigned char index)
{
    OSErr err = SCSIGet();
    if (err != noErr) return err;
    
    err = SCSISelect(scsiID);
    if (err != noErr) {
        long dummy;
        SCSIComplete(&dummy, nil, 0);
        return err;
    }
    
    unsigned char cdb[12] = {SCSI_CMD_SET_NEXT_CD, index, 0};
    err = SCSICmd(cdb, 12);
    
    long actualSize;
    SCSIComplete(&actualSize, nil, 0);
    
    return err;
}

void RefreshDiscList(void)
{
    if (!gGlobals.deviceFound) return;
    
    unsigned char count;
    if (GetDiscCount(gGlobals.scsiID, &count) != noErr) {
        ShowError("\pError reading disc count");
        return;
    }
    
    gGlobals.discCount = count;
    
    if (count > 0) {
        if (GetDiscList(gGlobals.scsiID, gGlobals.discs, count) != noErr) {
            ShowError("\pError reading disc list");
            return;
        }
    }
    
    if (gGlobals.window != nil) {
        InvalRect(&gGlobals.window->portRect);
    }
}

void EventLoop(void)
{
    while (!gGlobals.done) {
        DoEvent();
    }
}

void DoEvent(void)
{
    Boolean gotEvent;
    
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
                    DoMenuCommand(MenuKey(gGlobals.event.message & charCodeMask));
                }
                break;
            case updateEvt:
                HandleUpdate();
                break;
        }
    }
}

void HandleMouseDown(void)
{
    WindowPtr window;
    short part = FindWindow(gGlobals.event.where, &window);
    
    switch (part) {
        case inMenuBar:
            DoMenuCommand(MenuSelect(gGlobals.event.where));
            break;
        case inSysWindow:
            SystemClick(&gGlobals.event, window);
            break;
        case inContent:
            if (window != FrontWindow()) {
                SelectWindow(window);
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

void DoMenuCommand(long menuResult)
{
    short menuID = HiWord(menuResult);
    short menuItem = LoWord(menuResult);
    Str255 name;
    
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
            } else if (menuItem == iQuit) {
                gGlobals.done = true;
            }
            break;
        case mEdit:
            SystemEdit(menuItem - 1);
            break;
    }
    
    HiliteMenu(0);
}

void DoAboutBox(void)
{
    Alert(rAboutAlert, nil);
}

void HandleUpdate(void)
{
    WindowPtr window = (WindowPtr)gGlobals.event.message;
    SetPort(window);
    BeginUpdate(window);
    EraseRect(&window->portRect);
    DrawDiscList();
    EndUpdate(window);
}

void DrawDiscList(void)
{
    Str255 str, sizeStr;
    short i, lineHeight = 16, topMargin = 40;
    
    MoveTo(10, 20);
    TextFont(systemFont);
    TextSize(12);
    TextFace(bold);
    DrawString("\pUSBODE Disc Manager");
    
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
    
    topMargin += 20;
    for (i = 0; i < gGlobals.discCount; i++) {
        MoveTo(20, topMargin + (i * lineHeight));
        
        NumToString(gGlobals.discs[i].index, str);
        DrawString(str);
        DrawString("\p. ");
        
        CStringToPascal((char *)gGlobals.discs[i].name, str);
        DrawString(str);
        
        GetDiscSizeString(&gGlobals.discs[i], sizeStr);
        DrawString("\p  (");
        DrawString(sizeStr);
        DrawString("\p)");
    }
}

void GetDiscSizeString(DiscEntry *disc, Str255 sizeStr)
{
    unsigned long size = ((unsigned long)disc->size[1] << 24) |
                        ((unsigned long)disc->size[2] << 16) |
                        ((unsigned long)disc->size[3] << 8) |
                        ((unsigned long)disc->size[4]);
    
    size = size / (1024 * 1024);
    NumToString(size, sizeStr);
    BlockMove("\p MB", sizeStr + sizeStr[0] + 1, 3);
    sizeStr[0] += 3;
}

void ShowError(Str255 message)
{
    ParamText(message, "\p", "\p", "\p");
    Alert(rUserAlert, nil);
}

void CStringToPascal(const char *cStr, Str255 pStr)
{
    short i, len = 0;
    while (cStr[len] != '\0' && len < 255) len++;
    pStr[0] = len;
    for (i = 0; i < len; i++) pStr[i + 1] = cStr[i];
}
