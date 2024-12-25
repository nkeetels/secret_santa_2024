#include "demo.h"
#include "script.h"
#include "effects.h"

#define NUM_EFFECTS 6

uint16_t effect_frame = 0;
uint8_t current_effect = 0;

extern uint16_t *framebuffer;

const static struct scene_t scene_list[] = { 
  effect_piano,
  effect_shack,
  effect_city2,
  effect_city,
  effect_globe,
  effect_end,
 // effect_end,
};

// 7.785 sec per overgang
const static int transitions[] = 
{
  0,          // piano
  226 * 4,    // shack
  232 * 8,    // checkitout
  232 * 12,    // city
  232 * 16,   // globe
  232 * 20,   // end
  232 * 24,   // end
};

void reset_script()
{
  effect_frame = 0;
  current_effect = 0;
}

void start_script()
{
  reset_script();

  scene_list[current_effect].init();
}

void advance_script()
{
  effect_frame = 0;
  ++current_effect;

  current_effect %= NUM_EFFECTS;
}

void tick_script(uint32_t pulse)
{ 
  if (pulse >= transitions[current_effect + 1]) 
  {
    next_effect();
  }

  scene_list[current_effect].update(framebuffer, effect_frame++, pulse);  
}

void next_effect()
{
  scene_list[current_effect].destroy();

  advance_script();

  scene_list[current_effect].init();
}