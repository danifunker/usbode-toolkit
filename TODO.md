# USBODE Disc Manager - TODO & Future Enhancements

## High Priority

### Core Functionality
- [ ] Auto-detect USBODE SCSI ID (scan bus 0-6)
- [ ] Add bounds checking for SET NEXT CD command
- [ ] Implement proper SCSI error handling with sense data
- [ ] Add timeout handling for SCSI operations
- [ ] Cache disc list to avoid unnecessary SCSI queries

### User Interface
- [ ] Add visual feedback during SCSI operations (spinning cursor)
- [ ] Implement scroll bars for disc lists > 15 items
- [ ] Add status bar showing current mounted disc
- [ ] Show progress indicator for long operations
- [ ] Implement drag-and-drop reordering (visual only)

### Error Handling
- [ ] Better error messages with specific codes
- [ ] Recovery suggestions in error dialogs
- [ ] Log errors to file for debugging
- [ ] Retry mechanism for transient failures

## Medium Priority

### Features
- [ ] Get current disc index (requires firmware update: 0xDB)
- [ ] Eject/unmount current disc (requires firmware: 0xDC)
- [ ] Search/filter disc list
- [ ] Favorites/bookmarks system
- [ ] Recent discs history
- [ ] Disc information details window
- [ ] Multiple USBODE device support

### UI Enhancements
- [ ] Custom icons for different disc types (System, Games, Apps)
- [ ] List view with columns (Name, Size, Index, Type)
- [ ] Icon view option
- [ ] Appearance Manager support (Mac OS 8+)
- [ ] Platinum look on compatible systems
- [ ] Custom color schemes

### Preferences
- [ ] Preferences dialog
- [ ] Remember window position/size
- [ ] Default SCSI ID setting
- [ ] Auto-refresh interval option
- [ ] Confirmation options (mount, quit)
- [ ] Sound effects toggle

### Keyboard Shortcuts
- [ ] Number keys (0-9) for quick disc selection
- [ ] Page Up/Down for scrolling
- [ ] Home/End for first/last disc
- [ ] Type-ahead search

## Low Priority

### Advanced Features
- [ ] AppleScript support for automation
- [ ] Batch operations (mount sequence)
- [ ] Disc image information (ISO9660 metadata)
- [ ] Export disc list to text file
- [ ] Print disc catalog
- [ ] Compare disc contents

### Integration
- [ ] Finder integration (mount from Finder)
- [ ] Control Strip module (System 7.5+)
- [ ] Contextual menu plugin (Mac OS 8+)
- [ ] Startup item for auto-mount
- [ ] Background daemon mode

### Documentation
- [ ] Balloon help (System 7.5+)
- [ ] Apple Guide help book
- [ ] Tutorial for first launch
- [ ] Keyboard shortcut reference card
- [ ] Video tutorial (QuickTime)

### Internationalization
- [ ] Japanese localization
- [ ] French localization
- [ ] German localization
- [ ] String resources externalization
- [ ] Right-to-left language support

## Technical Improvements

### Code Quality
- [ ] Unit tests for SCSI commands
- [ ] Integration tests with mock device
- [ ] Code documentation (Doxygen-style)
- [ ] Static analysis cleanup
- [ ] Memory leak detection
- [ ] Performance profiling

### Build System
- [ ] Automated build scripts
- [ ] Version number automation
- [ ] Release packaging script
- [ ] StuffIt archive creation
- [ ] Disk image (.img) builder
- [ ] Installer creation (Installer VISE)

### Platform Support
- [ ] 68k version for older Macs
- [ ] Carbon version for Mac OS X Classic
- [ ] Fat binary (PPC + 68k)
- [ ] Optimize for specific CPUs (G3, G4)
- [ ] Mac OS X native port (Cocoa)

### Device Support
- [ ] SCSI-1 compatibility mode
- [ ] USB-to-SCSI adapter support
- [ ] Network SCSI over IP
- [ ] Multiple simultaneous USBODE devices
- [ ] Hot-plug detection

## Protocol Enhancements

### New Commands (requires firmware updates)
- [ ] 0xDB - GET CURRENT CD: Return mounted index
- [ ] 0xDC - EJECT CD: Unmount current disc
- [ ] 0xDD - GET DEVICE INFO: Firmware version, capabilities
- [ ] 0xDE - SET DEVICE CONFIG: Change settings
- [ ] 0xDF - DEVICE STATUS: Detailed status info

### Protocol Improvements
- [ ] Proper SCSI sense data for errors
- [ ] Extended LUN support
- [ ] Larger disc counts (>100)
- [ ] Long filename support (>32 chars)
- [ ] Unicode filenames
- [ ] Disc metadata (tags, categories)

## UI Mockups

### Potential Features to Mock Up
- [ ] Preferences window layout
- [ ] Detailed disc info dialog
- [ ] Search/filter interface
- [ ] Multi-column list view
- [ ] Icon view grid
- [ ] Status bar design
- [ ] Toolbar buttons (if space permits)

## Known Issues to Fix

### Bugs
- [ ] Handle case where device disconnects during operation
- [ ] Fix potential buffer overflow in name parsing
- [ ] Validate all SCSI buffer sizes
- [ ] Handle allocation failures gracefully
- [ ] Fix redraw issues on older systems

### Compatibility
- [ ] Test on Mac OS 7.5.3 (oldest supported)
- [ ] Test on Mac OS 9.2.2 (newest supported)
- [ ] Test on various PowerPC models (601, 603, 604, G3, G4)
- [ ] Test with different SCSI adapters
- [ ] Test in Mac OS X Classic environment

### Performance
- [ ] Optimize large disc list rendering
- [ ] Reduce SCSI transaction overhead
- [ ] Cache more aggressively
- [ ] Background thread for SCSI operations (if supported)

## Documentation TODO

- [ ] Add screenshots (when running on emulator)
- [ ] Create video demonstration
- [ ] Write developer guide for extending
- [ ] API documentation for SCSI functions
- [ ] Troubleshooting flowcharts
- [ ] FAQ expansion

## Community Features

- [ ] GitHub repository setup
- [ ] Issue tracking
- [ ] Contribution guidelines
- [ ] Code of conduct
- [ ] Release notes template
- [ ] Changelog automation

## Testing TODO

### Manual Testing
- [ ] Test on Basilisk II (if 68k version created)
- [ ] Test on SheepShaver emulator
- [ ] Test on real PowerPC hardware
- [ ] Test with real USBODE device
- [ ] Test all menu items
- [ ] Test all keyboard shortcuts
- [ ] Test error conditions

### Automated Testing
- [ ] SCSI mock framework
- [ ] Unit test harness
- [ ] UI automation tests
- [ ] Memory leak tests
- [ ] Performance benchmarks

## Release Checklist

- [ ] Version number updated
- [ ] Copyright year current
- [ ] All resources compiled
- [ ] Icon finalized
- [ ] About box accurate
- [ ] Documentation complete
- [ ] Build on clean system
- [ ] Test on target systems
- [ ] Create installer
- [ ] Package for distribution
- [ ] Update website
- [ ] Announce release

## Future Versions

### Version 1.1
- Auto-detect SCSI ID
- Enhanced UI with click-to-mount
- Visual feedback
- Better error handling

### Version 1.5
- Preferences system
- Favorites
- Search/filter
- Appearance Manager support

### Version 2.0
- AppleScript support
- Multiple device support
- Advanced disc management
- Metadata support

### Version 3.0 (Vision)
- Mac OS X native version
- Network support
- Cloud integration
- Modern UI paradigms

## Ideas for Consideration

- Integration with other vintage Mac tools
- Plugin architecture for extensibility
- Remote control via network
- Disc image creation/editing
- Disc burning support
- Virtual disc encryption
- Disc image conversion utilities

---

**Priority Legend:**
- High: Core functionality, critical fixes
- Medium: Important enhancements, nice-to-have
- Low: Future considerations, experimental

**Status Tracking:**
- [ ] Not started
- [→] In progress
- [✓] Completed
- [✗] Cancelled/Not feasible

Last updated: 2025-11-09
