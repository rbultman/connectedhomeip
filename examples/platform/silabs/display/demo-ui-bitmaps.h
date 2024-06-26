/*******************************************************************************
 * @file
 * @brief User Interface bitmaps for demo.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc.
 *www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon
 *Laboratories Inc. Your use of this software is
 *governed by the terms of Silicon Labs Master
 *Software License Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 *This software is distributed to you in Source Code
 *format and is governed by the sections of the MSLA
 *applicable to Source Code.
 *
 ******************************************************************************/

#include <AppConfig.h>

#ifndef SILABS_DEMO_UI_BITMAPS_H
#define SILABS_DEMO_UI_BITMAPS_H

#define SILICONLABS_BITMAP_WIDTH 128
#define SILICONLABS_BITMAP_HEIGHT 45

#define ZIGBEE_BITMAP_WIDTH 16
#define ZIGBEE_BITMAP_HEIGHT 16

#define ZIGBEE_BITMAP                                                                                                              \
    0x3f, 0xfc, 0x07, 0xf0, 0xff, 0xc7, 0xff, 0x9f, 0x01, 0x9c, 0x00, 0x0e, 0x00, 0x07, 0x80, 0x03, 0xc0, 0x01, 0xe0, 0x00, 0x70,  \
        0x80, 0x39, 0x80, 0xf9, 0xff, 0xfb, 0xff, 0x07, 0xe0, 0x1f, 0xfc

#define RAIL_BITMAP_WIDTH 16
#define RAIL_BITMAP_HEIGHT 16

#define CONNECT_BITMAP_WIDTH 16
#define CONNECT_BITMAP_HEIGHT 16

#define BLUETOOTH_BITMAP_WIDTH 16
#define BLUETOOTH_BITMAP_HEIGHT 18

#define SILABS_BITMAP                                                                                                              \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  \
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,    \
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,    \
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xf0, 0xff, 0xff, 0xff, 0xf7, 0xff, 0xff,    \
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x00, 0x00, 0xf8, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,    \
        0xfd, 0xff, 0xff, 0x01, 0x00, 0x00, 0xc0, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xff, 0xff, 0x3f, 0x00,    \
        0x00, 0x00, 0x00, 0xff, 0xff, 0x1f, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0xfe,    \
        0xff, 0x1f, 0xf8, 0xff, 0xff, 0xff, 0x7f, 0xfc, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0x1f, 0xf0, 0xff,    \
        0xff, 0xff, 0x1f, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0x1f, 0xe0, 0xff, 0xff, 0xff, 0xc3, 0xff,    \
        0xff, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0x0f, 0xc0, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0x07, 0x00, 0x00,    \
        0x00, 0x00, 0x00, 0xff, 0xff, 0x07, 0xc0, 0xff, 0xff, 0x1f, 0xf8, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x80, 0xff,    \
        0xff, 0x01, 0xc0, 0xff, 0xff, 0x07, 0xfc, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0x7f, 0x00, 0x80, 0xff,    \
        0xff, 0x01, 0xfe, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x0f, 0x00, 0x80, 0xff, 0x7f, 0x00, 0xff, 0xff,    \
        0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0x3f, 0x80, 0xff, 0xff, 0xff, 0x00, 0x00, 0xf0,    \
        0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0xc0, 0xff, 0x0f, 0x80, 0xff, 0xff, 0xff, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0x01,    \
        0x00, 0x00, 0xc0, 0xff, 0x03, 0xc0, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x00, 0x00, 0xe0, 0xff,    \
        0x01, 0xc0, 0xff, 0xff, 0xff, 0xf1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xe0, 0xff, 0x00, 0xc0, 0xff, 0xff,    \
        0xff, 0xc3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0xf0, 0x7f, 0x00, 0x80, 0xff, 0xff, 0xff, 0x0f, 0xfc, 0xff,    \
        0xff, 0xff, 0xff, 0xff, 0x07, 0x00, 0xf8, 0x3f, 0x00, 0x80, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff,    \
        0x0f, 0x00, 0xfc, 0x1f, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0xc0, 0xff, 0xff, 0xff, 0x0f, 0x00, 0xfe, 0x0f,    \
        0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xff, 0x1f, 0x00, 0xff, 0x07, 0x00, 0x00, 0xf0, 0xff,    \
        0xff, 0xff, 0xff, 0xff, 0x3f, 0xfc, 0xff, 0xff, 0x1f, 0x80, 0xff, 0x07, 0x00, 0x00, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff,    \
        0x3f, 0xf8, 0xff, 0xff, 0x1f, 0xc0, 0xff, 0x03, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0x00, 0xf0, 0xff, 0xff,    \
        0x0f, 0xf0, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0x3f, 0x00, 0x00, 0xf0, 0xff, 0xff, 0x0f, 0xfc, 0xff, 0x03,    \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0x07, 0xfe, 0xff, 0x01, 0x00, 0x00, 0x3f, 0x00,    \
        0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0x83, 0xff, 0xff, 0x01, 0x00, 0xc0, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00,    \
        0x00, 0xfc, 0xff, 0xff, 0xe1, 0xff, 0xff, 0x03, 0x00, 0xf0, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff,    \
        0xf8, 0xff, 0xff, 0x03, 0x00, 0xf8, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x03,    \
        0x00, 0xfe, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0xdf, 0xff, 0xff, 0xff, 0x07, 0x00, 0xff, 0xff, 0x01,    \
        0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x80, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00,    \
        0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xc0, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff,    \
        0xff, 0xff, 0xff, 0x3f, 0xc0, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,    \
        0xc0, 0xff, 0xff, 0xff, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff,    \
        0x1f, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,    \
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,    \
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff

#define THREAD_BITMAP_WIDTH 16
#define THREAD_BITMAP_HEIGHT 18

#define THREAD_BITMAP                                                                                                              \
    0x1F, 0xF8, 0x07, 0xE0, 0x03, 0xCE, 0x01, 0x9F, 0x01, 0xB3, 0x00, 0x33, 0xF0, 0x3F, 0xF8, 0x1F, 0x0C, 0x03, 0x0C, 0x03, 0x18,  \
        0x03, 0x11, 0x83, 0x01, 0x83, 0x03, 0xC3, 0x07, 0xE3, 0x1F, 0xFB, 0x7f, 0xff, 0xff, 0xff

#define WIFI_BITMAP_WIDTH 18
#define WIFI_BITMAP_HEIGHT 18

#define WIFI_BITMAP                                                                                                                \
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03, 0xFF, 0x01, 0xE0, 0x03, 0x00, 0xC7, 0xFF, 0xB8, 0xFF, 0xF7, 0x07, 0xF8, 0x0F, 0xC0, 0x3F,  \
        0x3F, 0xFF, 0xFF, 0xFF, 0x3F, 0xFF, 0x7F, 0xF8, 0xFF, 0xE1, 0xFF, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F

#define MATTER_LOGO_WIDTH 18
#define MATTER_LOGO_HEIGHT 17

#define MATTER_LOGO_BITMAP                                                                                                         \
    0xFF, 0xFF, 0xFF, 0xF3, 0xFF, 0xCF, 0xFF, 0x3F, 0xFF, 0xFF, 0xFC, 0x3F, 0x12, 0xFF, 0x01, 0xFE, 0xFF, 0xFF, 0xF3, 0x3F, 0x8F,  \
        0x7F, 0xFC, 0xFC, 0xFC, 0xE3, 0xF1, 0x87, 0x87, 0xC7, 0xDE, 0x88, 0x33, 0xC7, 0xCF, 0xFC, 0xFF, 0xFF, 0x03

#ifndef ON_DEMO_BITMAP // Unknown demo....

#define ON_DEMO_BITMAP                                                                                                             \
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  \
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    \
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    \
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    \
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    \
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    \
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    \
        0xFF, 0xFF, 0xFF, 0xB9, 0xFF, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xB1, 0xFF, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xB5,    \
        0xF9, 0x0D, 0x30, 0x0C, 0xC3, 0xFF, 0xFF, 0xB5, 0xF6, 0x6D, 0xF6, 0x6D, 0x9B, 0xFF, 0xFF, 0xAD, 0xF6, 0x6D, 0x36, 0x6C,    \
        0x83, 0xFF, 0xFF, 0x8D, 0xF6, 0x6D, 0xB6, 0x6D, 0xFB, 0xFF, 0xFF, 0x9D, 0xF9, 0x6D, 0x36, 0x0C, 0x83, 0xFF, 0xFF, 0xFF,    \
        0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    \
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    \
        0xFF, 0xFF, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xE1, 0xFF, 0xFF, 0xE7, 0x99, 0xF9, 0xFF, 0xFF, 0xFD, 0xFF, 0xFF, 0xE7, 0x99,    \
        0xF9, 0xFF, 0xFF, 0x7D, 0x6E, 0xC3, 0xE0, 0x99, 0xF9, 0xFF, 0xFF, 0xA1, 0x6D, 0xDB, 0xE6, 0x99, 0xF9, 0xFF, 0xFF, 0xBD,    \
        0x6D, 0xDB, 0xE6, 0x99, 0xF9, 0xFF, 0xFF, 0xBD, 0x6D, 0xDB, 0xE6, 0xFF, 0xFF, 0xFF, 0xFF, 0x7D, 0x9E, 0xDB, 0xE0, 0x99,    \
        0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    \
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    \
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    \
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    \
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    \
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    \
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    \
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    \
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    \
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    \
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
#endif

#ifndef OFF_DEMO_BITMAP // Unknown demo....
#define OFF_DEMO_BITMAP ON_DEMO_BITMAP
#endif

#endif // SILABS_DEMO_UI_BITMAPS_H
