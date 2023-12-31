#pragma once

// Some types to be used for consistency
typedef unsigned int u32;
typedef int i32;
typedef unsigned short u16;
typedef short i16;
typedef unsigned char u8;
typedef char i8;

/**
   Clears a section of memory to a value

   @param start is the memory address to start filling at
   @param value is what value to fill the range with
   @param size is how many bytes to fill with value
 */
void memset(u8* start, u8 value, u32 size);

/**
   Triggers an infinite loop. Used in case of errors.
 */
void halt();

/**
   Aligns a value to a 4k page boundry. I put this here because it's
   used a bit

   @param in the addr to take in
 */
u32 align4k(u32 in);

/**
   A module for use in multiboot. As of now unused
 */
struct MultiBootModule {
	u32 modStart;
	u32 modEnd;
	u32 string;
	u32 reserved;
} __attribute__ ((packed));

/**
   Multiboot header information
*/
struct MultiBoot {
	u32 flags;
	u32 memLower;
	u32 memUpper;
	u32 bootDevice;
	u32 cmdLine;
	u32 modsCount;
	u32 modsAddr;
	u32 syms[3];
	u32 mmapLength;
	u32 mmapAddr;
	u32 drivesLength;
	u32 drivesAddr;
	u32 configTable;
	u32 bootLoaderName;
	u32 apmTable;
	u32 vbeControlInfo;
	u32 vbeModeInfo;
	u16 vbeMode;
	u16 vbeInterfaceSeg;
	u16 vbeInterfaceOff;
	u16 vbeInterfaceLen;
	u32 framebufferAddr;
	u32 none;
	u32 framebufferPitch;
	u32 framebufferWidth;
	u32 framebufferHeight;
	u32 framebufferBPP;
	u32 framebufferType;
	u8 colorInfo[5];

	/** Tells if memory information is present */
	bool memPresent();
	/** Tells if boot device information is present */
	bool bootDevicePresent();

	/** Tells if command line information is present */
	bool cmdLinePresent();

	/** Tells if module information is present */
	bool modsPresent();
} __attribute__ ((packed));
