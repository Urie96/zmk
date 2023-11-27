/*
 * Copyright (c) 2022 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_multiple_keys

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>
#include <zmk/behavior.h>
#include <zmk/keymap.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

struct behavior_multiple_keys_config {
    size_t behavior_count;
    struct zmk_behavior_binding *behaviors;
};

uint32_t get_multiple_keys_index_selected();

static inline int handle_multiple_keys_binding_event(struct zmk_behavior_binding *binding,
                                                     struct zmk_behavior_binding_event event,
                                                     bool pressed) {
    uint32_t index = get_multiple_keys_index_selected();

    const struct device *dev = device_get_binding(binding->behavior_dev);
    const struct behavior_multiple_keys_config *cfg = dev->config;

    struct zmk_behavior_binding selected_binding =
        cfg->behaviors[index < cfg->behavior_count ? index : 0];
    struct zmk_behavior_binding_event selected_event = {
        .position = event.position,
        .timestamp = event.timestamp,
    };

    if (pressed) {
        return behavior_keymap_binding_pressed(&selected_binding, selected_event);
    } else {
        return behavior_keymap_binding_released(&selected_binding, selected_event);
    }

    return 0;
}

static int on_multiple_keys_binding_pressed(struct zmk_behavior_binding *binding,
                                            struct zmk_behavior_binding_event event) {
    LOG_DBG("%d multiple_keys keybind pressed", event.position);
    handle_multiple_keys_binding_event(binding, event, true);
    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_multiple_keys_binding_released(struct zmk_behavior_binding *binding,
                                             struct zmk_behavior_binding_event event) {
    LOG_DBG("%d multiple_keys keybind released", event.position);
    handle_multiple_keys_binding_event(binding, event, false);
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_multiple_keys_driver_api = {
    .binding_pressed = on_multiple_keys_binding_pressed,
    .binding_released = on_multiple_keys_binding_released,
};

static int behavior_multiple_keys_init(const struct device *dev) { return 0; };

#define _TRANSFORM_ENTRY(idx, node) ZMK_KEYMAP_EXTRACT_BINDING(idx, node)

#define TRANSFORMED_BINDINGS(node)                                                                 \
    { LISTIFY(DT_INST_PROP_LEN(node, bindings), _TRANSFORM_ENTRY, (, ), DT_DRV_INST(node)) }

#define KP_INST(n)                                                                                 \
    static struct zmk_behavior_binding                                                             \
        behavior_multiple_keys_config_##n##_bindings[DT_INST_PROP_LEN(n, bindings)] =              \
            TRANSFORMED_BINDINGS(n);                                                               \
    static struct behavior_multiple_keys_config behavior_multiple_keys_config_##n = {              \
        .behaviors = behavior_multiple_keys_config_##n##_bindings,                                 \
        .behavior_count = DT_INST_PROP_LEN(n, bindings)};                                          \
    DEVICE_DT_INST_DEFINE(                                                                         \
        n, behavior_multiple_keys_init, NULL, NULL, &behavior_multiple_keys_config_##n,            \
        APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_multiple_keys_driver_api);

DT_INST_FOREACH_STATUS_OKAY(KP_INST)

#endif