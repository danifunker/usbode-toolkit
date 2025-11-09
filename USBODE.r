/* USBODE.r - Resource definitions for USBODE Disc Manager */

#include "Types.r"
#include "SysTypes.r"

/* Application signature */
type 'USBO' as 'STR ';

/* Version resource */
resource 'vers' (1) {
    0x01,
    0x00,
    release,
    0x00,
    verUS,
    "1.0",
    "1.0, ©2025 USBODE Project"
};

/* Size resource for System 7 */
resource 'SIZE' (-1) {
    reserved,
    acceptSuspendResumeEvents,
    reserved,
    canBackground,
    doesActivateOnFGSwitch,
    backgroundAndForeground,
    dontGetFrontClicks,
    ignoreAppDiedEvents,
    is32BitCompatible,
    isHighLevelEventAware,
    localAndRemoteHLEvents,
    isStationeryAware,
    useTextEditServices,
    reserved,
    reserved,
    reserved,
    512 * 1024,     /* Preferred size: 512K */
    256 * 1024      /* Minimum size: 256K */
};

/* Menu bar */
resource 'MBAR' (128) {
    { 128, 129, 130 }  /* Apple, File, Edit */
};

/* Apple menu */
resource 'MENU' (128, preload) {
    128,
    textMenuProc,
    allEnabled,
    enabled,
    apple,
    {
        "About USBODE…",
            noIcon, noKey, noMark, plain;
        "-",
            noIcon, noKey, noMark, plain
    }
};

/* File menu */
resource 'MENU' (129, preload) {
    129,
    textMenuProc,
    0b1111111111111011,  /* Items 1,2,4 enabled (separator at 3 disabled) */
    enabled,
    "File",
    {
        "Refresh List",
            noIcon, "R", noMark, plain;
        "Scan SCSI Bus",
            noIcon, "S", noMark, plain;
        "-",
            noIcon, noKey, noMark, plain;
        "Quit",
            noIcon, "Q", noMark, plain
    }
};

/* Edit menu */
resource 'MENU' (130, preload) {
    130,
    textMenuProc,
    0b0000000000000000,  /* All disabled */
    enabled,
    "Edit",
    {
        "Undo",
            noIcon, "Z", noMark, plain;
        "-",
            noIcon, noKey, noMark, plain;
        "Cut",
            noIcon, "X", noMark, plain;
        "Copy",
            noIcon, "C", noMark, plain;
        "Paste",
            noIcon, "V", noMark, plain;
        "Clear",
            noIcon, noKey, noMark, plain
    }
};

/* Main window */
resource 'WIND' (128, preload, purgeable) {
    {50, 50, 400, 600},
    documentProc,
    visible,
    goAway,
    0x0,
    "USBODE Disc Manager",
    centerMainScreen
};

/* About alert */
resource 'ALRT' (128, purgeable) {
    {40, 40, 180, 340},
    128,
    {
        OK, visible, silent,
        OK, visible, silent,
        OK, visible, silent,
        OK, visible, silent
    },
    centerMainScreen
};

resource 'DITL' (128, purgeable) {
    {
        {100, 120, 120, 180},
        Button {
            enabled,
            "OK"
        };
        {10, 70, 90, 280},
        StaticText {
            disabled,
            "USBODE Disc Manager\n"
            "Version 1.0\n\n"
            "©2025 USBODE Project"
        }
    }
};

/* User alert (for errors) */
resource 'ALRT' (129, purgeable) {
    {40, 40, 140, 340},
    129,
    {
        OK, visible, silent,
        OK, visible, silent,
        OK, visible, silent,
        OK, visible, silent
    },
    centerMainScreen
};

resource 'DITL' (129, purgeable) {
    {
        {70, 120, 90, 180},
        Button {
            enabled,
            "OK"
        };
        {10, 70, 60, 280},
        StaticText {
            disabled,
            "^0"
        }
    }
};

/* Bundle resource for Finder */
resource 'BNDL' (128) {
    'USBO',
    0,
    {
        'ICN#',
        {
            0, 128
        },
        'FREF',
        {
            0, 128
        }
    }
};

resource 'FREF' (128) {
    'APPL',
    0,
    ""
};

/* Application icon */
resource 'ICN#' (128) {
    {
        /* Icon bitmap */
        $"00000000 00000000"
        $"00000000 00000000"
        $"00000000 00000000"
        $"00000000 00000000"
        $"00FFFF00 00FFFF00"
        $"00F00F00 00F00F00"
        $"00F00F00 00FFFF00"
        $"00F00000 00F00000"
        $"00F00000 00000000"
        $"00000000 00000000"
        $"00F00F00 00F00F00"
        $"00F00F00 00F00F00"
        $"00FFFF00 00000000"
        $"00000000 00000000"
        $"00FFFF00 00F00F00"
        $"00FFFF00 00F00F00"
        $"00F00F00 00000000"
        $"00000000 00000000"
        $"00FFFF00 00F00F00"
        $"00FFFF00 00F00F00"
        $"00F00F00 00000000"
        $"00000000 00000000"
        $"00FFFF00 00F00000"
        $"00FFFF00 00F00000"
        $"00FFFF00 00000000"
        $"00000000 00000000",
        
        /* Mask */
        $"00000000 00000000"
        $"00000000 00000000"
        $"00000000 00000000"
        $"00000000 00000000"
        $"00FFFF00 00FFFF00"
        $"00FFFF00 00FFFF00"
        $"00FFFF00 00FFFF00"
        $"00F00000 00F00000"
        $"00F00000 00000000"
        $"00000000 00000000"
        $"00FFFF00 00FFFF00"
        $"00FFFF00 00FFFF00"
        $"00FFFF00 00000000"
        $"00000000 00000000"
        $"00FFFF00 00FFFF00"
        $"00FFFF00 00FFFF00"
        $"00FFFF00 00000000"
        $"00000000 00000000"
        $"00FFFF00 00FFFF00"
        $"00FFFF00 00FFFF00"
        $"00FFFF00 00000000"
        $"00000000 00000000"
        $"00FFFF00 00FFFF00"
        $"00FFFF00 00FFFF00"
        $"00FFFF00 00000000"
        $"00000000 00000000"
    }
};

/* Signature resource */
resource 'USBO' (0) {
    "USBODE Manager"
};
