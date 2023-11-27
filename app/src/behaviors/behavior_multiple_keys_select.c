/*
 * Copyright (c) 2022 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_multiple_keys_select

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>
#include <zmk/behavior.h>
#include <zmk/keymap.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static uint32_t multiple_keys_index_selected = 0;

uint32_t get_multiple_keys_index_selected() { return multiple_keys_index_selected; }

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

static int on_multiple_keys_select_binding_pressed(struct zmk_behavior_binding *binding,
                                                   struct zmk_behavior_binding_event event) {
    multiple_keys_index_selected = binding->param1;
    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_multiple_keys_select_binding_released(struct zmk_behavior_binding *binding,
                                                    struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_multiple_keys_select_driver_api = {
    .binding_pressed = on_multiple_keys_select_binding_pressed,
    .binding_released = on_multiple_keys_select_binding_released,
};

static int behavior_multiple_keys_select_init(const struct device *dev) { return 0; };

DEVICE_DT_INST_DEFINE(0, behavior_multiple_keys_select_init, NULL, NULL, NULL, APPLICATION,
                      CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
                      &behavior_multiple_keys_select_driver_api);
#endif