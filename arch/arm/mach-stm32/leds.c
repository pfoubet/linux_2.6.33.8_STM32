#include <mach/leds.h>
#include <mach/platform.h>
#include <linux/leds.h>
#include <linux/platform_device.h>

// déclaration des broches et noms
static struct gpio_led stm32f429idisco_led_pins[] = {
  {
    .name = "LD3",
    .gpio = 109,
  },
  {
    .name
= "LD4",
    .gpio
= 110,
    },
};

// déclaration du périphérique leds
static struct gpio_led_platform_data stm32f429idisco_led_data = {
  .num_leds = ARRAY_SIZE(stm32f429idisco_led_pins),
  .leds = stm32f429idisco_led_pins,
};

// déclaration du périphérique
static struct platform_device stm32f429idisco_leds = {
  .name = "leds-gpio",
  .id = -1,
  .dev.platform_data = &stm32f429idisco_led_data,
};

// intégration
static struct platform_device *stm32f429idisco_devices[] __initdata = {
  &stm32f429idisco_leds,
};

// initialisation
void __init stm32_led_init(void)
{
int platform;
  printk("INFO: Call stm32_led_init() !");
  platform = stm32_platform_get();
  if(platform == PLATFORM_STM32_STM32429_DISCO)
    platform_add_devices(stm32f429idisco_devices,
    ARRAY_SIZE(stm32f429idisco_devices));
}

