#ifndef NEOPIXEL_CONTROLLER
#define NEOPIXEL_CONTROLLER

#include <SPI.h>

/* Configuration */
#define NUM_SEGMENT 5
#define LED_PER_SEGMENT 3
#define NUM_LED         (NUM_SEGMENT * LED_PER_SEGMENT)

#define RESET_PULSE 16   // 50us reset

// WS2812B GRB order
#define GRB_GREEN   0
#define GRB_RED     1
#define GRB_BLUE    2

typedef struct {
  uint16_t start;
  uint16_t len;
} Segment;

extern Segment col[NUM_SEGMENT];

void set_led(uint8_t idx, uint8_t r, uint8_t g, uint8_t b);
void neopixel_init();
void neopixel_clear();
void neopixel_segments_init();
void neopixel_set_segment(uint8_t idx, uint8_t r, uint8_t g, uint8_t b);
void neopixel_show();

#endif