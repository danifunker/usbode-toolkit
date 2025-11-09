/*
 * USBODE_UI.c
 * Enhanced UI functionality for USBODE Disc Manager
 * 
 * This file contains enhanced UI features including:
 * - Interactive disc list with click-to-select
 * - Mount button functionality
 * - Keyboard shortcuts
 */

#include "USBODE.h"

/* UI State */
typedef struct {
    short selectedDisc;
    Boolean hasSelection;
    Rect listRect;
    Rect mountButtonRect;
    Rect refreshButtonRect;
    short scrollPosition;
} UIState;

static UIState gUIState;

/* Constants for UI layout */
#define kTopMargin      60
#define kLeftMargin     20
#define kLineHeight     20
#define kButtonHeight   20
#define kButtonWidth    80
#define kRightMargin    20
#define kBottomMargin   20

/*
 * Initialize UI state
 */
void InitUIState(void)
{
    gUIState.selectedDisc = -1;
    gUIState.hasSelection = false;
    gUIState.scrollPosition = 0;
    
    /* Calculate layout rects */
    if (gGlobals.window != nil) {
        Rect *bounds = &gGlobals.window->portRect;
        
        /* Disc list area */
        gUIState.listRect.top = kTopMargin;
        gUIState.listRect.left = kLeftMargin;
        gUIState.listRect.right = bounds->right - kRightMargin;
        gUIState.listRect.bottom = bounds->bottom - kBottomMargin - kButtonHeight - 10;
        
        /* Mount button */
        gUIState.mountButtonRect.left = kLeftMargin;
        gUIState.mountButtonRect.top = bounds->bottom - kBottomMargin - kButtonHeight;
        gUIState.mountButtonRect.right = kLeftMargin + kButtonWidth;
        gUIState.mountButtonRect.bottom = bounds->bottom - kBottomMargin;
        
        /* Refresh button */
        gUIState.refreshButtonRect.left = kLeftMargin + kButtonWidth + 10;
        gUIState.refreshButtonRect.top = bounds->bottom - kBottomMargin - kButtonHeight;
        gUIState.refreshButtonRect.right = kLeftMargin + kButtonWidth * 2 + 10;
        gUIState.refreshButtonRect.bottom = bounds->bottom - kBottomMargin;
    }
}

/*
 * Draw enhanced disc list with selection
 */
void DrawEnhancedDiscList(void)
{
    Rect textRect;
    Str255 str;
    Str255 sizeStr;
    short i;
    short yPos;
    
    /* Draw title */
    MoveTo(10, 20);
    TextFont(systemFont);
    TextSize(12);
    TextFace(bold);
    DrawString("\pUSBODE Disc Manager");
    
    /* Draw disc count */
    if (!gGlobals.deviceFound) {
        MoveTo(10, 40);
        TextFace(normal);
        DrawString("\pNo USBODE device found");
        return;
    }
    
    NumToString(gGlobals.discCount, str);
    MoveTo(10, 40);
    TextFace(normal);
    DrawString("\pAvailable discs: ");
    DrawString(str);
    
    /* Draw disc list with selection highlighting */
    TextFace(normal);
    for (i = 0; i < gGlobals.discCount; i++) {
        yPos = kTopMargin + (i * kLineHeight);
        
        /* Skip if out of visible area */
        if (yPos < gUIState.listRect.top || 
            yPos + kLineHeight > gUIState.listRect.bottom) {
            continue;
        }
        
        /* Highlight selected item */
        if (i == gUIState.selectedDisc && gUIState.hasSelection) {
            Rect highlightRect;
            highlightRect.top = yPos - 2;
            highlightRect.bottom = yPos + kLineHeight - 2;
            highlightRect.left = kLeftMargin - 5;
            highlightRect.right = gUIState.listRect.right;
            
            /* Invert for selection */
            InvertRect(&highlightRect);
        }
        
        MoveTo(kLeftMargin, yPos + 12);
        
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
    
    /* Draw buttons */
    DrawButton(&gUIState.mountButtonRect, "\pMount", gUIState.hasSelection);
    DrawButton(&gUIState.refreshButtonRect, "\pRefresh", true);
    
    /* Draw instructions */
    MoveTo(10, gUIState.listRect.bottom + 30);
    TextFace(italic);
    TextSize(10);
    DrawString("\pClick to select, then click Mount. ⌘R to refresh.");
}

/*
 * Draw a simple button
 */
void DrawButton(Rect *rect, Str255 title, Boolean enabled)
{
    PenNormal();
    
    /* Draw frame */
    FrameRoundRect(rect, 8, 8);
    
    if (!enabled) {
        /* Gray out if disabled */
        PenPat(&qd.gray);
    }
    
    /* Draw title centered */
    short strWidth = StringWidth(title);
    short strHeight = 10;
    short x = rect->left + (rect->right - rect->left - strWidth) / 2;
    short y = rect->top + (rect->bottom - rect->top + strHeight) / 2;
    
    MoveTo(x, y);
    DrawString(title);
    
    PenNormal();
}

/*
 * Handle content clicks with enhanced UI
 */
void HandleContentClick(Point localPt)
{
    short i;
    short yPos;
    
    /* Check if clicking on mount button */
    if (PtInRect(localPt, &gUIState.mountButtonRect)) {
        if (gUIState.hasSelection) {
            /* Flash button */
            InvertRect(&gUIState.mountButtonRect);
            Delay(8, nil);
            InvertRect(&gUIState.mountButtonRect);
            
            /* Mount disc */
            MountSelectedDiscEnhanced();
        }
        return;
    }
    
    /* Check if clicking on refresh button */
    if (PtInRect(localPt, &gUIState.refreshButtonRect)) {
        /* Flash button */
        InvertRect(&gUIState.refreshButtonRect);
        Delay(8, nil);
        InvertRect(&gUIState.refreshButtonRect);
        
        /* Refresh list */
        RefreshDiscList();
        return;
    }
    
    /* Check if clicking in disc list */
    if (PtInRect(localPt, &gUIState.listRect)) {
        for (i = 0; i < gGlobals.discCount; i++) {
            yPos = kTopMargin + (i * kLineHeight);
            
            if (localPt.v >= yPos - 2 && localPt.v < yPos + kLineHeight - 2) {
                /* Item clicked */
                if (gUIState.hasSelection && gUIState.selectedDisc == i) {
                    /* Double-click: mount immediately */
                    MountSelectedDiscEnhanced();
                } else {
                    /* Single click: select */
                    gUIState.selectedDisc = i;
                    gUIState.hasSelection = true;
                    InvalRect(&gGlobals.window->portRect);
                }
                return;
            }
        }
        
        /* Clicked in list but not on an item: deselect */
        gUIState.hasSelection = false;
        InvalRect(&gGlobals.window->portRect);
    }
}

/*
 * Mount the currently selected disc with feedback
 */
void MountSelectedDiscEnhanced(void)
{
    OSErr err;
    Str255 message;
    Str255 discName;
    
    if (!gUIState.hasSelection || gUIState.selectedDisc < 0 || 
        gUIState.selectedDisc >= gGlobals.discCount) {
        return;
    }
    
    /* Get disc name for feedback */
    CStringToPascal((char *)gGlobals.discs[gUIState.selectedDisc].name, discName);
    
    /* Show mounting message */
    BlockMove("\pMounting: ", message, 11);
    BlockMove(discName + 1, message + 11, discName[0]);
    message[0] = 10 + discName[0];
    
    /* Send mount command */
    err = SetActiveDisc(gGlobals.scsiID, gGlobals.discs[gUIState.selectedDisc].index);
    
    if (err == noErr) {
        /* Success */
        BlockMove("\pDisc mounted successfully!", message, 26);
        message[0] = 25;
        ParamText(message, "\p", "\p", "\p");
        Alert(rUserAlert, nil);
    } else {
        /* Error */
        ShowError("\pError mounting disc. Please check SCSI connection.");
    }
}

/*
 * Handle keyboard shortcuts
 */
void HandleKeyDown(EventRecord *event)
{
    char key = event->message & charCodeMask;
    
    if (event->modifiers & cmdKey) {
        switch (key) {
            case 'r':
            case 'R':
                /* Command-R: Refresh */
                RefreshDiscList();
                break;
                
            case 'm':
            case 'M':
                /* Command-M: Mount selected */
                if (gUIState.hasSelection) {
                    MountSelectedDiscEnhanced();
                }
                break;
        }
    } else {
        /* Arrow keys for navigation */
        switch (key) {
            case 0x1E:  /* Up arrow */
                if (gUIState.hasSelection && gUIState.selectedDisc > 0) {
                    gUIState.selectedDisc--;
                    InvalRect(&gGlobals.window->portRect);
                } else if (!gUIState.hasSelection && gGlobals.discCount > 0) {
                    gUIState.selectedDisc = 0;
                    gUIState.hasSelection = true;
                    InvalRect(&gGlobals.window->portRect);
                }
                break;
                
            case 0x1F:  /* Down arrow */
                if (gUIState.hasSelection && gUIState.selectedDisc < gGlobals.discCount - 1) {
                    gUIState.selectedDisc++;
                    InvalRect(&gGlobals.window->portRect);
                } else if (!gUIState.hasSelection && gGlobals.discCount > 0) {
                    gUIState.selectedDisc = 0;
                    gUIState.hasSelection = true;
                    InvalRect(&gGlobals.window->portRect);
                }
                break;
                
            case 0x0D:  /* Return/Enter */
                if (gUIState.hasSelection) {
                    MountSelectedDiscEnhanced();
                }
                break;
        }
    }
}
