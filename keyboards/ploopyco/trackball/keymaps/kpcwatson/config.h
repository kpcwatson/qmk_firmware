#pragma once

#define PLOOPY_DPI_OPTIONS { 800, 1200, 1600, 2400 }
#define PLOOPY_DPI_DEFAULT 2

// invert dragscroll so rolling the ball down scrolls up
#define PLOOPY_DRAGSCROLL_INVERT 1

// Fix dragscroll being too fast/chaotic
// #define PLOOPY_DRAGSCROLL_MULTIPLIER 0.70 // Variable-DPI drag scroll
// #define PLOOPY_DRAGSCROLL_FIXED 1
// #define PLOOPY_DRAGSCROLL_DPI 100    // Fixed-DPI drag scroll (100 is lowest)
#define PLOOPY_DRAGSCROLL_DIVISOR_H 64.0
#define PLOOPY_DRAGSCROLL_DIVISOR_V 64.0
