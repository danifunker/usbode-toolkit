# USBODE Disc Manager - User Guide

## Introduction

USBODE Disc Manager is a Classic Mac OS application that allows you to view and mount disc images stored on your USBODE device. It provides a simple, native interface for managing your virtual CD-ROM collection.

## System Requirements

- **Computer:** Power Macintosh with PowerPC processor
- **Operating System:** Mac OS 7.5.3 or later (tested up to 9.2.2)
- **Memory:** 256 KB minimum, 512 KB recommended
- **Hardware:** SCSI bus (built-in or via SCSI card)
- **Device:** USBODE hardware connected via SCSI

## Installation

1. **Copy to your Mac:**
   - Download `USBODE` application
   - Copy to your Applications folder or any location

2. **Connect USBODE:**
   - Connect USBODE device to SCSI chain
   - Ensure proper termination
   - Note: USBODE should appear as SCSI ID 0 (configurable in future)

3. **Launch:**
   - Double-click USBODE application icon
   - Application will search for USBODE device automatically

## Using the Application

### Main Window

The main window displays:
- **Title Bar:** "USBODE Disc Manager"
- **Disc Count:** Number of available disc images
- **Disc List:** All available discs with names and sizes
- **Buttons:** Mount and Refresh buttons (enhanced version)

### Viewing Disc Images

When you launch USBODE Disc Manager, it automatically:
1. Searches for USBODE device on SCSI bus
2. Retrieves list of available disc images
3. Displays them with their sizes

Example display:
```
USBODE Disc Manager
Available discs: 5

0. System 7.5.3.iso  (350 MB)
1. Mac OS 8.1.iso    (450 MB)
2. Mac OS 9.0.iso    (500 MB)
3. Games Collection.iso  (650 MB)
4. Utilities.iso     (200 MB)
```

### Mounting a Disc (Basic Version)

In the basic version:
1. Note the index number of the disc you want
2. Currently requires manual implementation

### Mounting a Disc (Enhanced Version)

In the enhanced version with USBODE_UI.c:

**Method 1: Mouse**
1. Click on a disc to select it (highlights)
2. Click "Mount" button
3. Confirmation dialog appears

**Method 2: Keyboard**
1. Use ↑/↓ arrow keys to select disc
2. Press Return/Enter to mount
3. Or press ⌘M

**Method 3: Double-click**
1. Double-click on a disc name
2. Disc mounts immediately

### Refreshing the List

If you modify disc images on USBODE:

**Method 1: Menu**
- Choose File > Refresh List

**Method 2: Keyboard**
- Press ⌘R

**Method 3: Button**
- Click "Refresh" button (enhanced version)

## Menu Reference

### Apple Menu (⌘)
- **About USBODE…** - Shows version and copyright info
- **Desk Accessories** - Standard Mac OS DA's

### File Menu
- **Refresh List (⌘R)** - Update disc list from device
- **Quit (⌘Q)** - Exit application

### Edit Menu
- Standard editing commands (for system use)

## Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| ⌘Q | Quit application |
| ⌘R | Refresh disc list |
| ⌘M | Mount selected disc (enhanced) |
| ↑ | Select previous disc (enhanced) |
| ↓ | Select next disc (enhanced) |
| Return | Mount selected disc (enhanced) |

## Troubleshooting

### "USBODE device not found"

**Possible causes:**
- USBODE not connected
- SCSI bus issues
- Wrong SCSI ID
- SCSI termination problems

**Solutions:**
1. Check SCSI connections
2. Verify USBODE is powered on
3. Check SCSI chain termination
4. Try different SCSI ID
5. Restart computer with USBODE connected

### "Error reading disc count"

**Possible causes:**
- SCSI communication failure
- Device busy
- Firmware issue

**Solutions:**
1. Click Refresh to retry
2. Disconnect/reconnect USBODE
3. Restart application
4. Check SCSI cable quality

### "Error reading disc list"

**Possible causes:**
- Timeout during data transfer
- Corrupted response
- Memory allocation failure

**Solutions:**
1. Refresh the list
2. Restart application
3. Check available memory
4. Reduce number of disc images if >100

### "Error mounting disc"

**Possible causes:**
- Invalid disc index
- SCSI communication error
- Disc file corrupted on USBODE

**Solutions:**
1. Refresh list and try again
2. Check USBODE disc storage
3. Try different disc
4. Restart USBODE device

### Application won't launch

**Possible causes:**
- Insufficient memory
- System version too old
- Corrupted application

**Solutions:**
1. Close other applications
2. Increase application memory (Get Info)
3. Verify Mac OS version ≥ 7.5.3
4. Re-download application

## Tips & Tricks

### Managing Large Collections

- USBODE supports up to 100 disc images
- Organize by type (System, Games, Apps, etc.)
- Use descriptive names (max 32 characters)
- Keep frequently-used discs at top of list

### Performance

- Disc list is cached after initial load
- Only refresh when you add/remove disc images
- SCSI communication is fast (<1 second)

### Organization

Suggested naming conventions:
```
00_System753.iso
01_System81.iso
02_MacOS90.iso
10_PhotoshopInstall.iso
11_OfficeInstall.iso
20_Games1.iso
21_Games2.iso
```

This keeps related discs grouped together.

### Working with Multiple Macs

- Different Macs can access same USBODE
- Only one Mac should access at a time
- Coordinate disc changes between systems

## Advanced Usage

### Automating Disc Changes

While USBODE Disc Manager provides manual control, you could:
- Script disc changes using AppleScript (future enhancement)
- Create shortcuts for common discs
- Build custom interfaces

### Integration Ideas

- Launch installer after mounting specific disc
- Auto-mount frequently used discs on startup
- Create smart playlists of related discs

## Version History

**Version 1.0**
- Initial release
- Basic disc listing
- SCSI communication
- Simple UI

**Version 1.1 (Enhanced)**
- Interactive disc selection
- Mouse and keyboard control
- Visual feedback
- Improved error handling

## Frequently Asked Questions

**Q: Can I add disc images while USBODE Disc Manager is running?**
A: Yes, but you must click Refresh to see new discs.

**Q: How many discs can USBODE hold?**
A: Up to 100 disc images (protocol limitation).

**Q: Can I rename discs from the application?**
A: Not currently. Manage disc names on USBODE directly.

**Q: Does this work with CD-ROM drives?**
A: No, this is specifically for USBODE virtual CD devices.

**Q: Can I use this on 68k Macs?**
A: No, this is PowerPC-only. A 68k version would need separate compilation.

**Q: What disc formats are supported?**
A: ISO 9660, HFS, and hybrid formats supported by USBODE firmware.

**Q: Can I eject the current disc?**
A: Not in current version. Select and mount a different disc.

## Getting Help

For issues not covered here:
- Check USBODE hardware documentation
- Verify Mac OS SCSI Manager is working
- Test with Apple's SCSI tools
- Report bugs to USBODE project

## Credits

**USBODE Disc Manager**
- Classic Mac OS Edition
- ©2025 USBODE Project
- Developed for vintage Mac enthusiasts

**Special Thanks:**
- Classic Mac OS Toolbox documentation
- SCSI Manager reference materials
- Vintage Mac community

## Legal

This software is provided "as is" without warranty of any kind. Use at your own risk. Always backup your data before modifying disc configurations.

---

*Enjoy using your classic Mac with modern convenience!*
