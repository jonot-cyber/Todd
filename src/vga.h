#pragma once

#include "common.h"

/* 0:black, 1:blue, 2:green, 3:cyan, 4:red, 5:magenta, 6:brown, 7:light grey, 8:dark grey, 9:light blue, 10:light green, 
11:light cyan, 12:light red, 13:light magneta, 14: light brown, 15: white.*/

enum class VGAColor: u8 {
    Black,
    Blue,
    Green,
    Cyan,
    Red,
    Magenta,
    Brown,
    LightGrey,
    DarkGrey,
    LightBlue,
    LightGreen,
    LightCyan,
    LightRed,
    LightMagenta,
    LightBrown,
    White,
};

u16 colorCharacter(u8 c, VGAColor bg = VGAColor::Black, VGAColor fg = VGAColor::White);