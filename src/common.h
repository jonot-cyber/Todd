#pragma once

typedef unsigned int u32;
typedef int i32;
typedef unsigned short u16;
typedef short i16;
typedef unsigned char u8;
typedef char i8;

void memset(u8* start, u8 v, u32 sz);
void halt();

struct MultiBootModule {
    u32 modStart;
    u32 modEnd;
    u32 string;
    u32 reserved;
} __attribute__ ((packed));

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

    bool memPresent();
    bool bootDevicePresent();
    bool cmdLinePresent();
    bool modsPresent();
} __attribute__ ((packed));