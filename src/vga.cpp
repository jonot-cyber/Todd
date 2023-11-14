#include "vga.h"

u16 colorCharacter(u8 c, VGAColor bg = VGAColor::Black, VGAColor fg = VGAColor::White) {
    u8 attr = ((u8)bg << 4) | ((u8)fg & 0x0F);
    u16 wc = c | (attr << 8);
    return wc;
}