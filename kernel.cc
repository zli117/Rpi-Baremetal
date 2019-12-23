#include <asm_utils.h>
#include <data.h>
#include <kernel.h>

#define PERI_BASE 0x3F000000
#define GPFSEL1 (0x200004 + PERI_BASE)
#define GPSET0 (0x20001C + PERI_BASE)
#define GPCLR0 (0x200028 + PERI_BASE)

static uint8 core_to_led[] = {16, 17, 18, 19};

static volatile uint8 led_signals[] = {0xff, 0xff, 0xff, 0xff};

static volatile uint32* const gpio_GPFSEL1 = reinterpret_cast<uint32*>(GPFSEL1);
static volatile uint32* const gpio_GPSET0 = reinterpret_cast<uint32*>(GPSET0);
static volatile uint32* const gpio_GPCLR0 = reinterpret_cast<uint32*>(GPCLR0);

static volatile __attribute__((aligned(32))) uint32 led_lock = 0;

void barrier() { asm volatile("dmb sy"); }

void led_spin_lock() {
  uint32 tmp;

  led_lock = 0;

  asm volatile(
      "clrex\n"
      "1:\n"
      "dsb sy\n"
      "ldxr %w0, [%1]\n"
      "dsb sy\n"
      "cbnz %w0, 1b\n"
      // "stxr %w0, %w2, [%1]\n"
      // "dsb sy\n"
      // "cbnz	%w0, 1b\n"
      "clrex\n"
      : "=&r"(tmp)
      : "r"(&led_lock), "r"(1)
      : "memory");
}

void led_spin_unlock() {
  // __sync_lock_release(&led_lock);
  asm volatile("stlr %w1, [%0]\n" : : "r"(&led_lock), "r"(0) : "memory");
}

void delay() {
  volatile uint32 timer = 150;

  while (timer--)
    ;
}

uint64 get_el() {
  uint64 el;
  asm("mrs %0, CurrentEL" : "=r"(el));
  return ((el >> 2) & 3);
}

void led_init() {
  uint32 var;

  var = *gpio_GPFSEL1;

  // 001 - output
  for (uint8 led_gpio : core_to_led) {
    int offset = ((led_gpio - 10) * 3);
    var &= ~(7 << offset);
    var |= (1 << offset);
  }

  // Write back updated value
  *gpio_GPFSEL1 = var;

  barrier();
}

void led_on(uint8 core_id) {
  uint32 gpio = core_to_led[core_id];
  // led_spin_lock();
  *gpio_GPSET0 |= (1 << gpio);
  *gpio_GPCLR0 &= ~(1 << gpio);
  barrier();
  // led_spin_unlock();
}

void led_off(uint8 core_id) {
  uint32 gpio = core_to_led[core_id];
  // led_spin_lock();
  *gpio_GPCLR0 |= (1 << gpio);
  *gpio_GPSET0 &= ~(1 << gpio);
  barrier();
  // led_spin_unlock();
}

void long_delay() {
  volatile uint32 timer = 1000000;

  while (--timer)
    ;
}

void release_cpu() {
  // Release other cores
  volatile uint64* spin_cpu0 = (uint64*)(0xd8);
  for (uint8 i = 1; i < 4; ++i) {
    spin_cpu0[i] = _start;
  }
  asm volatile("sev");
}

void entry() {
  uint64 cpu_id = get_cpu_id();

  if (cpu_id == 0x0) {
    led_init();
    release_cpu();
    for (uint32 i = 0; i < 4; ++i) {
      led_signals[i] = 0;
    }
  } else {
    // Wait for initialization to finish
    while (led_signals[cpu_id])
      ;
  }
  while (true) {
    long_delay();
    led_signals[cpu_id] = 1;
    if (cpu_id == 0x0) {
      for (uint32 i = 0; i < 4; ++i) {
        if (led_signals[i] > 0) {
          led_on(i);
        } else {
          led_off(i);
        }
      }
    }
    long_delay();
    led_signals[cpu_id] = 0;
    if (cpu_id == 0x0) {
      for (uint32 i = 0; i < 4; ++i) {
        if (led_signals[i] > 0) {
          led_on(i);
        } else {
          led_off(i);
        }
      }
    }
  }
}
