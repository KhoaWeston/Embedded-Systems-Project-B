#ifndef _FONTS_H
#define _FONTS_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include "main.h"
//
//  Structure used to define fonts
//
typedef struct {
    const uint8_t FontWidth;    /* Font width in pixels */
    uint8_t FontHeight;         /* Font height in pixels */
    const uint16_t *data;       /* Pointer to data font data array */
} FontDef;

//
//  Export the 3 available fonts
//
extern FontDef Font_7x10;
extern FontDef Font_11x18;
extern FontDef Font_16x26;

#ifdef __cplusplus
}
#endif

#endif  // _FONTS_H
