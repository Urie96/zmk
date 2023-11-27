/*
 * Copyright (c) 2022 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_sys_info

#include <drivers/behavior.h>
#include <dt-bindings/zmk/keys.h>
#include <zephyr/bluetooth/services/bas.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zmk/behavior.h>
#include <zmk/behavior_queue.h>
#include <zmk/endpoints.h>
#include <zmk/keymap.h>
#include <zmk/battery.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

uint32_t get_multiple_keys_index_selected();

static inline void tap_key(uint32_t param1) {
    struct zmk_behavior_binding behavior = {
        .behavior_dev = "KEY_PRESS",
        .param1 = param1,
    };
    zmk_behavior_queue_add(0, behavior, true, 10);
    zmk_behavior_queue_add(0, behavior, false, 10);
}

const static uint32_t ascii_key_code[] = {SPACE,
                                          EXCLAMATION,
                                          DOUBLE_QUOTES,
                                          HASH,
                                          DOLLAR,
                                          PERCENT,
                                          AMPERSAND,
                                          APOSTROPHE,
                                          LEFT_PARENTHESIS,
                                          RIGHT_PARENTHESIS,
                                          ASTERISK,
                                          PLUS,
                                          COMMA,
                                          MINUS,
                                          PERIOD,
                                          SLASH,
                                          N0,
                                          N1,
                                          N2,
                                          N3,
                                          N4,
                                          N5,
                                          N6,
                                          N7,
                                          N8,
                                          N9,
                                          COLON,
                                          SEMICOLON,
                                          LESS_THAN,
                                          EQUAL,
                                          GREATER_THAN,
                                          QUESTION,
                                          AT_SIGN,
                                          LS(A),
                                          LS(B),
                                          LS(C),
                                          LS(D),
                                          LS(E),
                                          LS(F),
                                          LS(G),
                                          LS(H),
                                          LS(I),
                                          LS(J),
                                          LS(K),
                                          LS(L),
                                          LS(M),
                                          LS(N),
                                          LS(O),
                                          LS(P),
                                          LS(Q),
                                          LS(R),
                                          LS(S),
                                          LS(T),
                                          LS(U),
                                          LS(V),
                                          LS(W),
                                          LS(X),
                                          LS(Y),
                                          LS(Z),
                                          LEFT_BRACKET,
                                          BACKSLASH,
                                          RIGHT_BRACKET,
                                          CARET,
                                          UNDERSCORE,
                                          GRAVE,
                                          A,
                                          B,
                                          C,
                                          D,
                                          E,
                                          F,
                                          G,
                                          H,
                                          I,
                                          J,
                                          K,
                                          L,
                                          M,
                                          N,
                                          O,
                                          P,
                                          Q,
                                          R,
                                          S,
                                          T,
                                          U,
                                          V,
                                          W,
                                          X,
                                          Y,
                                          Z,
                                          LEFT_BRACE,
                                          PIPE,
                                          RIGHT_BRACE,
                                          TILDE,
                                          DELETE};

static void tap_string(char *text) {
    int i;
    for (i = 0; text[i] != '\0'; i++) {
        tap_key(ascii_key_code[text[i] - ' ']);
    }
}

static int on_sys_info_binding_pressed(struct zmk_behavior_binding *binding,
                                       struct zmk_behavior_binding_event event) {
    struct zmk_endpoint_instance edp = zmk_endpoints_selected();
    char text[256];

    sprintf(text, "c_lvl: %d%%, ", zmk_battery_state_of_charge());

    uint8_t p_bat_lvl = zmk_battery_state_of_peripheral_charge();
    if (p_bat_lvl > 0 && p_bat_lvl <= 100) {
        sprintf(text + strlen(text), "p_lvl: %i%%, ", p_bat_lvl);
    }

    if (edp.transport == ZMK_TRANSPORT_USB) {
        sprintf(text + strlen(text), "usb, ");
    } else if (edp.transport == ZMK_TRANSPORT_BLE) {
        sprintf(text + strlen(text), "ble, ");
    }

    sprintf(text + strlen(text), "ble: %i, ", zmk_ble_active_profile_index());
    sprintf(text + strlen(text), "key: %i, ", get_multiple_keys_index_selected());
    sprintf(text + strlen(text), "build %s %s", __DATE__, __TIME__);

    tap_string(text);
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