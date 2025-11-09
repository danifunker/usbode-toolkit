# Build Notes for USBODE Disc Manager

## Building on Classic Mac OS

### Method 1: CodeWarrior Pro 4 or later (Recommended)

#### For CodeWarrior 8:

1. **Create New Project:**
   - File > New Project
   - Project name: `USBODE`
   - Select **"C Toolbox Classic"** (under "Mac OS Toolbox" section)
   - Click OK

2. **Remove default files:**
   - Delete any starter files CW adds (like `main.c`)

3. **Add Files:**
   - Add `USBODE.c` to project
   - Add `USBODE_UI.c` to project (optional, for enhanced UI)
   - Add `USBODE.r` to project

#### For CodeWarrior Pro 4-7:

1. **Create New Project:**
   - File > New Project
   - Select "MacOS:PPC:PPC Application"
   - Save as "USBODE.mcp"

2. **Add Files:**
   - Add `USBODE.c` to project
   - Add `USBODE_UI.c` to project (optional, for enhanced UI)
   - Add `USBODE.r` to project

3. **Project Settings:**
   - Target: PowerPC
   - Code Generation: 4-byte alignment
   - Processor: PowerPC 604 or higher
   - OS Version: Mac OS 7.5 or later

4. **PPC Target Settings (Edit > Target Settings):**
   - File Type: 'APPL'
   - Creator: 'USBO'
   - Preferred Size: 512K
   - Minimum Size: 256K

5. **Build:**
   - Project > Make (⌘-M)

### Method 2: MPW (Macintosh Programmer's Workshop)

**Using the Build Script:**

```
Build.mpw
```

**Manual Build:**

```bash
# Set up environment
Set Echo 1

# Compile main source
SC USBODE.c -w 2 -opt speed -b 4 -o :obj:USBODE.c.o

# Compile UI (optional)
SC USBODE_UI.c -w 2 -opt speed -b 4 -o :obj:USBODE_UI.c.o

# Compile resources
Rez USBODE.r -o USBODE.rsrc -i :

# Link
Link -w -c 'USBO' -t 'APPL' ¶
    :obj:USBODE.c.o ¶
    :obj:USBODE_UI.c.o ¶
    "{SharedLibraries}InterfaceLib" ¶
    "{SharedLibraries}StdCLib" ¶
    "{SharedLibraries}MathLib" ¶
    "{PPCLibraries}StdCRuntime.o" ¶
    "{PPCLibraries}PPCCRuntime.o" ¶
    "{PPCLibraries}PPCToolLibs.o" ¶
    -o :bin:USBODE

# Add resources
Rez -a USBODE.rsrc -o :bin:USBODE

# Set file attributes
SetFile -c 'USBO' -t 'APPL' :bin:USBODE
```

### Method 3: THINK C / Symantec C++ (Older systems)

For System 7.5.3 on older PowerPCs:

1. Create new project
2. Add source files
3. Set PowerPC target
4. Include MacHeaders library
5. Build

## Building on Modern Systems

### Using Retro68 (Cross-compilation from Linux/macOS)

```bash
# Install Retro68 first
git clone https://github.com/autc04/Retro68.git

# Set up Retro68 environment
export RETRO68=/path/to/Retro68-build/toolchain

# Compile
powerpc-apple-macos-gcc -O2 -o USBODE USBODE.c -lInterfaceLib

# Add resources
Rez USBODE.r -o USBODE
```

## Testing

### On Real Hardware

1. Copy `USBODE` to a Mac running OS 7.5.3 - 9.2.2
2. Ensure USBODE device is connected via SCSI
3. Double-click to launch
4. Grant SCSI access if prompted

### On Emulators

**Basilisk II (68k emulator, won't work - PPC only!):**
Not compatible - this is PowerPC code

**SheepShaver (PowerPC emulator):**

1. Set up Mac OS 8.6 or 9.0.4
2. Configure SCSI emulation
3. Copy USBODE to emulated system
4. Run application

**QEMU with Mac-on-Linux:**

1. Boot Mac OS 9.2.2 in QEMU
2. Mount shared folder with USBODE
3. Test application

## Debugging

### Using CodeWarrior Debugger

1. Build with symbols enabled
2. Set breakpoints in source
3. Run in debugger
4. Use Register and Memory windows to inspect SCSI buffers

### Using MacsBug

1. Install MacsBug extension
2. Launch USBODE
3. Press Command-Power to enter debugger
4. Use `sc6` to view SCSI Manager calls
5. Use `dm` to dump memory

### Debug Output

Add debugging code:

```c
#ifdef DEBUG
    DebugStr("\pSCSI Command sent");
#endif
```

Compile with `-DDEBUG` flag for debug builds.

## Common Build Issues

### Error: "Can't find InterfaceLib"

**Solution:** Ensure Universal Interfaces are installed:
- CodeWarrior: Include Universal Interfaces 3.4 or later
- MPW: Install MPW GM or later

### Error: "Unknown pragma"

**Solution:** Using wrong compiler. This code is for:
- CodeWarrior Pro 4-7
- MPW SC 8.9 or later
- Not compatible with THINK C

### Error: "SCSI.h not found"

**Solution:** 
- Add Universal Interfaces to include path
- Ensure you're targeting Mac OS, not Carbon

### Resource Compilation Fails

**Solution:**
- Check Rez include path
- Ensure Types.r is accessible
- Use `-i :` to add current directory

## Optimization Tips

### For System 7.5.3

- Use 4-byte alignment
- Disable 604e-specific optimizations
- Test on 601/603 if possible

### For Mac OS 8-9

- Can use Appearance Manager
- Navigation Services available
- CarbonLib possible for OS 9

### Performance

- Current implementation is synchronous
- SCSI calls may block briefly
- For large disc lists, consider caching

## File Attributes

After building, ensure these are set:

```bash
Type: 'APPL'
Creator: 'USBO'
Attributes: Bundle bit set
```

Use ResEdit or SetFile to verify.

## Distribution

To distribute:

1. Build application
2. Test on target systems
3. Create StuffIt archive or disk image
4. Include README
5. Consider creating installer with Installer VISE

## Version Compatibility

| OS Version | Compatibility | Notes |
|------------|---------------|-------|
| 7.5.3      | ✓ Full       | Minimum requirement |
| 7.6        | ✓ Full       | Enhanced features |
| 8.0-8.6    | ✓ Full       | Appearance Manager |
| 9.0-9.2.2  | ✓ Full       | Carbon possible |
| X (Classic)| ? Untested   | May work in Classic |

## Additional Resources

- Inside Macintosh: SCSI Manager
- Inside Macintosh: Toolbox Essentials
- CodeWarrior Documentation
- MPW Reference (Apple)
