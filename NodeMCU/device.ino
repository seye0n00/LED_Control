#include <Arduino.h>
#include "device.h"
#include "neopixel_controller.h"
#include "log.h"
#include "config.h"

static int8_t current_seg = -1;

void set_segment(uint8_t seg_id, bool state)
{
  if (seg_id >= NUM_SEGMENT) {
    Serial.printf("[SEG] invalid seg_id=%u\n", seg_id);
    return;
  }

  int prev_seg = current_seg;

  if (state) {
    // ON 요청
    if (current_seg != -1 && current_seg != seg_id) {
      neopixel_set_segment(current_seg, 0, 0, 0);
    }

    neopixel_set_segment(seg_id, 0x20, 0x20, 0x20);
    current_seg = seg_id;

    Serial.printf(
      "[SEG] input=%u, prev=%d, next=%d, state=ON\n",
      seg_id, prev_seg, current_seg
    );
  }
  else {
    // OFF 요청
    neopixel_set_segment(seg_id, 0, 0, 0);

    if (current_seg == seg_id) {
      current_seg = -1;
    }

    Serial.printf(
      "[SEG] input=%u, prev=%d, next=%d, state=OFF\n",
      seg_id, prev_seg, current_seg
    );
  }

  neopixel_show();
}
