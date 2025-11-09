# USBODE Disc Manager

Classic Mac OS application for managing disc images on USBODE devices. Provides a native Toolbox-based interface for browsing and mounting virtual CD-ROM images stored on USBODE hardware.

![Version](https://img.shields.io/badge/version-1.0-blue)
![Mac OS](https://img.shields.io/badge/Mac%20OS-7.5.3--9.2.2-green)
![CPU](https://img.shields.io/badge/CPU-PowerPC-orange)

## Compatibility

- **OS:** Mac OS 7.5.3 - 9.2.2
- **CPU:** PowerPC (601/603/604 or later)
- **Memory:** 256 KB minimum, 512 KB recommended
- **Required:** SCSI Manager (built into Classic Mac OS)

## Features

- List all available disc images on USBODE device
- Display disc names and sizes
- Mount disc images with a simple interface
- Refresh disc list on demand
- Classic Mac OS Toolbox-based UI

## USBODE SCSI Protocol

This application uses custom ATAPI/SCSI commands to communicate with USBODE:

- **0xD9 - LIST DEVICES:** Returns list of device types
- **0xDA - NUMBER OF CDS:** Returns count of available disc images (max 100)
- **0xD0/0xD7 - LIST FILES/CDS:** Returns array of disc entries with:
  - Index (1 byte)
  - Type (1 byte) 
  - Name (33 bytes, null-terminated, max 32 chars)
  - Size (5 bytes, big-endian, first byte zero)
- **0xD8 - SET NEXT CD:** Mounts the disc at specified index

## Building

### Using CodeWarrior Pro

1. Open CodeWarrior IDE
2. Create new project targeting PowerPC
3. Add `USBODE.c` to project
4. Add `USBODE.r` as resource file
5. Set project type to Application
6. Set creator code to 'USBO'
7. Build

### Using MPW (Macintosh Programmer's Workshop)

```bash
make
```

The compiled application will be in the `bin/` directory.

### Manual Build with MPW

```bash
# Compile C source
mwcppc -w on -opt speed -proc 604 -o USBODE.o USBODE.c

# Compile resources
rez USBODE.r -o USBODE.rsrc

# Link
mwlinkppc -mpwtool -c 'USBO' -t 'APPL' -o USBODE USBODE.o -lInterfaceLib -lMathLib -lStdCLib

# Add resources
rez -a USBODE.rsrc -o USBODE
```

## Usage

1. Connect USBODE device to your Mac via SCSI
2. Launch USBODE application
3. The application will automatically search for the USBODE device
4. Available disc images will be listed with their sizes
5. Select "File > Refresh List" to update the disc list
6. (Future) Double-click a disc to mount it

## Project Structure

```
usbode-toolkit/
├── USBODE.h             # Header file with constants and prototypes
├── USBODE.c             # Main implementation
├── USBODE_UI.c          # Enhanced UI implementation (optional)
├── USBODE_Simple.c      # Single-file version for easy building
├── USBODE.r             # Resource definitions (menus, windows, icons)
├── Makefile             # Build configuration for CodeWarrior
├── Build.mpw            # MPW build script
├── README.md            # This file
├── BUILD.md             # Detailed build instructions
├── PROTOCOL.md          # USBODE SCSI protocol reference
└── USERGUIDE.md         # End-user documentation
```

## Quick Start

### For Developers

1. **Choose your version:**
   - `USBODE.c` + `USBODE_UI.c` - Full-featured with enhanced UI
   - `USBODE.c` alone - Basic functional version
   - `USBODE_Simple.c` - All-in-one single file

2. **Build (CodeWarrior):**
   ```
   Open project, add sources, build
   ```

3. **Build (MPW):**
   ```
   Build.mpw
   ```

4. **Transfer to Classic Mac and run**

### For End Users

1. Download pre-built `USBODE` application
2. Copy to your Mac OS 7.5.3+ system
3. Connect USBODE device via SCSI
4. Launch and select discs to mount

See [USERGUIDE.md](USERGUIDE.md) for complete usage instructions.

## Current Limitations

- SCSI device ID is currently hardcoded to 0
- Disc mounting UI requires manual implementation (placeholder exists)
- No visual feedback during SCSI operations
- Icon is basic placeholder

## Future Enhancements

- Interactive disc selection with double-click support
- SCSI device scanning to auto-detect USBODE
- Custom icon and UI customization
- Progress indicators for long operations
- Preferences for default settings
- Support for keyboard shortcuts to mount discs
- Better error handling and user feedback
- About box with more information

## Technical Notes

### SCSI Communication

The application uses the Classic Mac OS SCSI Manager to communicate with USBODE. The SCSI Manager provides low-level access to SCSI devices through these key calls:

- `SCSIGet()` - Acquire SCSI bus
- `SCSISelect()` - Select target device
- `SCSICmd()` - Send command descriptor block
- `SCSIRead()` - Read data from device
- `SCSIComplete()` - Complete transaction and release bus

### Memory Management

The application uses standard Mac OS memory management:
- Stack-based allocation for small structures
- Handle-based resources for UI elements
- Direct pointer access for SCSI buffers

### String Handling

Classic Mac OS uses Pascal strings (length byte followed by characters), while USBODE returns C strings (null-terminated). The application includes conversion functions to handle both formats.

## Building for Different Systems

### System 7.5.3 - 7.6
No special considerations needed.

### Mac OS 8.0 - 8.6
Fully compatible with enhanced Appearance Manager support possible.

### Mac OS 9.0 - 9.2.2
Full compatibility. Carbon APIs could be used for future enhancement.

## License

This project is part of the USBODE toolkit. Please refer to the main project for licensing information.

## Credits

Developed for use with USBODE hardware project.
Based on Classic Mac OS Toolbox programming practices.
