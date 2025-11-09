# USBODE SCSI Protocol Reference

## Overview

USBODE uses custom ATAPI/SCSI commands in the vendor-specific range (0xD0-0xD9) to manage CD-ROM disc images. These commands allow listing, selecting, and mounting virtual disc images.

## Command Summary

| Opcode | Name | Description | Data Direction |
|--------|------|-------------|----------------|
| 0xD9 | LIST DEVICES | Returns device type list | IN |
| 0xDA | NUMBER OF CDS | Returns count of available images | IN |
| 0xD0 | LIST FILES | Returns list of disc image entries | IN |
| 0xD7 | LIST CDS | Alias for LIST FILES | IN |
| 0xD8 | SET NEXT CD | Mounts disc at specified index | - |

## Command Details

### 0xD9 - LIST DEVICES

Lists the types of devices available on USBODE.

**CDB Format:**
```
Byte 0: 0xD9 (command code)
Bytes 1-11: Reserved (0x00)
```

**Response:**
```
8 bytes:
  Byte 0: Device 0 type (0x02 = CD-ROM)
  Bytes 1-7: Additional devices (0xFF = not implemented)
```

**Example:**
```c
unsigned char cdb[12] = {0xD9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned char devices[8];
SCSICmd(cdb, 12);
SCSIRead(devices);
// devices[0] == 0x02 (CD-ROM)
```

---

### 0xDA - NUMBER OF CDS

Returns the number of disc images available.

**CDB Format:**
```
Byte 0: 0xDA (command code)
Bytes 1-11: Reserved (0x00)
```

**Response:**
```
1 byte: Count of disc images (0-100)
```

**Notes:**
- Maximum supported entries: 100
- If more than 100 files exist, only first 100 are accessible

**Example:**
```c
unsigned char cdb[12] = {0xDA, 0};
unsigned char count;
SCSICmd(cdb, 12);
SCSIRead(&count);
// count now contains number of discs
```

---

### 0xD0 / 0xD7 - LIST FILES / LIST CDS

Returns detailed information about all available disc images.

**CDB Format:**
```
Byte 0: 0xD0 or 0xD7 (command code)
Bytes 1-11: Reserved (0x00)
```

**Response:**
```
Array of TUSBCDToolboxFileEntry structures (39 bytes each):

Offset | Size | Description
-------|------|------------
0      | 1    | Index (0-99)
1      | 1    | Type (0 = file/image)
2      | 33   | Name (null-terminated, max 32 chars)
35     | 5    | Size (40-bit big-endian, byte 0 always 0)

Total entry size: 39 bytes
Number of entries: value from 0xDA command
```

**Size Field Format:**
```
5 bytes, big-endian:
  Byte 0: Always 0x00
  Byte 1: Bits 31-24
  Byte 2: Bits 23-16
  Byte 3: Bits 15-8
  Byte 4: Bits 7-0

Example: For size 650 MB (681574400 bytes = 0x289E8000)
  [0x00, 0x28, 0x9E, 0x80, 0x00]
```

**Example:**
```c
typedef struct {
    unsigned char index;
    unsigned char type;
    unsigned char name[33];
    unsigned char size[5];
} DiscEntry;

unsigned char count = /* from 0xDA */;
DiscEntry *entries = malloc(count * sizeof(DiscEntry));

unsigned char cdb[12] = {0xD7, 0};
SCSICmd(cdb, 12);
SCSIRead(entries);

// Parse first entry
printf("Disc %d: %s\n", entries[0].index, entries[0].name);

// Extract size
unsigned long size = ((unsigned long)entries[0].size[1] << 24) |
                    ((unsigned long)entries[0].size[2] << 16) |
                    ((unsigned long)entries[0].size[3] << 8) |
                    ((unsigned long)entries[0].size[4]);
printf("Size: %lu bytes\n", size);
```

---

### 0xD8 - SET NEXT CD

Mounts the disc image at the specified index.

**CDB Format:**
```
Byte 0: 0xD8 (command code)
Byte 1: Index (0-99)
Bytes 2-11: Reserved (0x00)
```

**Response:**
```
None (command only)
```

**Notes:**
- Index must be valid (0 to count-1)
- Invalid indices are silently ignored (should return check condition)
- Disc change happens immediately
- UNIT ATTENTION condition may be triggered on next command

**Example:**
```c
// Mount disc at index 5
unsigned char cdb[12] = {0xD8, 5, 0};
SCSICmd(cdb, 12);
// No data phase

// Drive now contains disc 5
```

---

## Implementation Notes

### SCSI Manager Usage (Classic Mac OS)

```c
// Complete transaction example
OSErr SendUSBODECommand(unsigned char cmd, unsigned char param,
                       void *buffer, long bufferSize)
{
    OSErr err;
    
    // 1. Acquire SCSI bus
    err = SCSIGet();
    if (err != noErr) return err;
    
    // 2. Select USBODE device (ID 0 in this example)
    err = SCSISelect(0);
    if (err != noErr) {
        long dummy;
        SCSIComplete(&dummy, nil, 0);
        return err;
    }
    
    // 3. Build and send CDB
    unsigned char cdb[12] = {cmd, param, 0};
    err = SCSICmd(cdb, 12);
    if (err != noErr) {
        long dummy;
        SCSIComplete(&dummy, nil, 0);
        return err;
    }
    
    // 4. Read data if buffer provided
    if (buffer != nil && bufferSize > 0) {
        err = SCSIRead(buffer);
    }
    
    // 5. Complete transaction
    long actualSize;
    SCSIComplete(&actualSize, nil, 0);
    
    return err;
}
```

### Error Handling

- **Device Not Found:** SCSISelect() returns error
- **Command Failed:** SCSICmd() returns error
- **Data Transfer Error:** SCSIRead() returns error
- **Invalid Index:** Command succeeds but nothing happens (firmware issue)

### Best Practices

1. **Always check disc count before listing:**
   ```c
   unsigned char count;
   GetDiscCount(&count);
   if (count > 0) {
       GetDiscList(discs, count);
   }
   ```

2. **Validate index before mounting:**
   ```c
   if (index >= 0 && index < count) {
       SetActiveDisc(index);
   }
   ```

3. **Handle allocation for large lists:**
   ```c
   // Allocate exactly what's needed
   DiscEntry *discs = malloc(count * sizeof(DiscEntry));
   ```

4. **Cache the disc list:**
   Don't query on every UI update; refresh only when needed

## Protocol Extensions (Future)

Potential additions to the protocol:

- **0xDB - GET CURRENT CD:** Return currently mounted index
- **0xDC - EJECT CD:** Unmount current disc
- **0xDD - GET DEVICE INFO:** Return firmware version, capabilities
- **Error codes:** Proper SCSI sense data for invalid operations

## Example Session

```
1. Check device types:
   -> 0xD9
   <- [0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF]
   (Device 0 is CD-ROM)

2. Get disc count:
   -> 0xDA
   <- [0x03]
   (3 discs available)

3. List discs:
   -> 0xD7
   <- [Entry 0: index=0, type=0, name="System 7.5.3.iso", size=...]
      [Entry 1: index=1, type=0, name="Mac OS 8.1.iso", size=...]
      [Entry 2: index=2, type=0, name="Games.iso", size=...]

4. Mount disc 1:
   -> 0xD8, 0x01
   (Mac OS 8.1 now mounted)
```

## Compatibility

- **SCSI-2 Compatible:** Uses standard SCSI Manager calls
- **ATAPI Compatible:** Can be accessed as ATAPI device
- **Mac OS Support:** 7.0 through 9.2.2
- **Firmware Version:** Check USBODE documentation for version support

## References

- SCSI-2 Specification (ANSI X3.131-1994)
- Inside Macintosh: SCSI Manager
- USBODE Firmware Documentation
