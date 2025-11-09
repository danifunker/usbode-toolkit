/*
 * USBODE.h
 * Header file for USBODE Disc Manager
 * 
 * Classic Mac OS application for managing disc images on USBODE device
 * Compatible with Mac OS 7.5.3 - 9.2.2 (PowerPC)
 */

#ifndef USBODE_H
#define USBODE_H

/* Ensure proper compilation environment */
#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

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
#include <Scrap.h>
#include <Traps.h>
#include <Devices.h>
#include <SCSI.h>

/* Compatibility defines for older CodeWarrior versions */
#ifndef _WaitNextEvent
#define _WaitNextEvent 0xA860
#endif

#ifndef _Unimplemented
#define _Unimplemented 0xA89F
#endif

/* Application Constants */
#define kBaseResID          128
#define kMoveToFront        (WindowPtr)-1L
#define kSleep              20
#define kMaxDiscs           100

/* SCSI Command Definitions for USBODE */
#define SCSI_CMD_LIST_DEVICES   0xD9
#define SCSI_CMD_NUM_CDS        0xDA
#define SCSI_CMD_LIST_CDS       0xD7
#define SCSI_CMD_LIST_FILES     0xD0
#define SCSI_CMD_SET_NEXT_CD    0xD8

/* Window Resource IDs */
#define rMenuBar            128
#define rAboutAlert         128
#define rMainWindow         128
#define rUserAlert          129

/* Menu Resource IDs */
#define mApple              128
#define iAbout              1

#define mFile               129
#define iRefresh            1
#define iScanBus            2
#define iQuit               4

#define mEdit               130

/* Control IDs */
#define kDiscListControl    128
#define kMountButton        129
#define kRefreshButton      130

/* Disc Entry Structure (matches USBODE protocol) */
typedef struct {
    unsigned char index;
    unsigned char type;
    unsigned char name[33];     /* 32 chars + null terminator */
    unsigned char size[5];      /* 40-bit big endian size */
} DiscEntry;

/* Application Globals */
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

/* Function Prototypes */

/* Initialization */
void ToolBoxInit(void);
void MenuBarInit(void);
void WindowInit(void);
Boolean FindUSBODEDevice(short *scsiID);
short ScanSCSIBus(void);  /* Returns number of devices found */
Boolean IsUSBODEDevice(short scsiID);  /* Test if device responds to USBODE commands */

/* SCSI Communication */
OSErr SendSCSICommand(short scsiID, unsigned char cmd, unsigned char param, 
                      void *buffer, long bufferSize, long *actualSize);
OSErr GetDiscCount(short scsiID, unsigned char *count);
OSErr GetDiscList(short scsiID, DiscEntry *discs, unsigned char count);
OSErr SetActiveDisc(short scsiID, unsigned char index);

/* Event Handling */
void EventLoop(void);
void DoEvent(void);
void DoMenuCommand(long menuResult);
void DoAboutBox(void);
void HandleMouseDown(void);
void HandleUpdate(void);
void HandleActivate(void);

/* UI Functions */
void RefreshDiscList(void);
void DrawDiscList(void);
void MountSelectedDisc(void);  /* Basic placeholder - use USBODE_UI.c for full implementation */
void ShowError(Str255 message);
void ShowScanResults(void);  /* Display SCSI bus scan results */

/* Utility Functions */
void GetDiscSizeString(DiscEntry *disc, Str255 sizeStr);
void CStringToPascal(const char *cStr, Str255 pStr);
void PascalToCString(ConstStr255Param pStr, char *cStr, short maxLen);

/* Enhanced UI Functions (in USBODE_UI.c) */
void InitUIState(void);
void DrawEnhancedDiscList(void);
void DrawButton(Rect *rect, Str255 title, Boolean enabled);
void HandleContentClick(Point localPt);
void MountSelectedDiscEnhanced(void);
void HandleKeyDown(EventRecord *event);

#endif /* USBODE_H */
