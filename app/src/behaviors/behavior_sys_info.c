/*
 * Copyright (c) 2022 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_sys_info

#include <drivers/behavior.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zmk/behavior.h>
#include <zmk/behavior_queue.h>
#include <zmk/keymap.h>
#include <zephyr/bluetooth/services/bas.h>
#include <zmk/endpoints.h>
#include <dt-bindings/zmk/keys.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

static inline void tap_key(uint32_t param1) {
    struct zmk_behavior_binding behavior = {
        .behavior_dev = "KEY_PRESS",
        .param1 = param1,
    };
    zmk_behavior_queue_add(0, behavior, true, 20);
    zmk_behavior_queue_add(0, behavior, false, 20);
}

const static uint32_t num_key_code[] = {N0, N1, N2, N3, N4, N5, N6, N7, N8, N9};

static inline void tap_number(uint8_t num) {
    int n = 1;
    while (num >= n * 10) {
        n *= 10;
    }
    while (n > 0) {
        int p = num / n % 10;
        tap_key(num_key_code[p]);
        n /= 10;
    }
}

uint8_t get_peripheral_battery_level(void);
uint8_t get_battery_level(void);

static int on_sys_info_binding_pressed(struct zmk_behavior_binding *binding,
                                       struct zmk_behavior_binding_event event) {
    tap_number(get_battery_level());
    tap_key(SPACE);
    tap_number(get_peripheral_battery_level());
    tap_key(SPACE);

    struct zmk_endpoint_instance edp = zmk_endpoints_selected();
    tap_number(edp.ble.profile_index + 1);
    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_sys_info_binding_released(struct zmk_behavior_binding *binding,
                                        struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_sys_info_driver_api = {
    .binding_pressed = on_sys_info_binding_pressed,
    .binding_released = on_sys_info_binding_released,
};

static int behavior_sys_info_init(const struct device *dev) { return 0; };

DEVICE_DT_INST_DEFINE(0, behavior_sys_info_init, NULL, NULL, NULL, APPLICATION,
                      CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_sys_info_driver_api);
#endif