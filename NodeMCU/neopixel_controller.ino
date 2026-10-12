#include "neopixel_controller.h"

static uint8_t spi_bits[NUM_LED * 3 * 3];
static uint8_t reset_bits[RESET_PULSE] = {0};
Segment col[NUM_SEGMENT];

static void set_spi_bits(uint8_t* buf, uint8_t val) {
  uint32_t pulse = 0;

  for (unsigned i = 0; i < 8; i++) {
    uint8_t bit = (val >> (7 - i)) & 1;
    pulse = (pulse << 3) | (bit == 0 ? 0b100 : 0b110);
  }

  buf[0] = (pulse >> 16) & 0xff;
  buf[1] = (pulse >> 8)  & 0xff;
  buf[2] = (pulse >> 0)  & 0xff;
}

void set_led(uint8_t idx, uint8_t r, uint8_t g, uint8_t b) {
  set_spi_bits(spi_bits + idx * 9 + 0 * 3, g);
  set_spi_bits(spi_bits + idx * 9 + 1 * 3, r);
  set_spi_bits(spi_bits + idx * 9 + 2 * 3, b);
}

void neopixel_segments_init() {
  int base_len = NUM_LED / NUM_SEGMENT;
  int idx = 0;

  for (int i = 0; i < NUM_SEGMENT; i++) {
    col[i].start = idx;
    col[i].len   = base_len;
    idx += col[i].len;
  }
}

void neopixel_clear(void) {
  for (uint16_t i = 0; i < NUM_LED; i++) {
    set_led(i, 0, 0, 0);
  }
  neopixel_show();
}

void neopixel_init() {
  SPI.begin();
  SPI.beginTransaction(SPISettings(2400000, MSBFIRST, SPI_MODE0));

  neopixel_segments_init();
  neopixel_clear();
}

void neopixel_set_segment(uint8_t seg_id, uint8_t r, uint8_t g, uint8_t b){
    if(seg_id >= NUM_SEGMENT) return;

    Segment *seg = &col[seg_id];
    for(uint16_t i=0;i<seg->len;i++){
        set_led(seg->start+i,r,g,b);
    }
}

void neopixel_show() {
  SPI.transferBytes(spi_bits, nullptr, sizeof(spi_bits));
  SPI.transferBytes(reset_bits, nullptr, sizeof(reset_bits));
}