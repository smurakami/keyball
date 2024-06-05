/*
Copyright 2022 @Yowkees
Copyright 2022 MURAOKA Taro (aka KoRoN, @kaoriya)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H

#include "quantum.h"
#ifdef CONSOLE_ENABLE
  #include <print.h>
#endif


// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // keymap for default (VIA)
  [0] = LAYOUT_universal(
    KC_ESC   , KC_Q     , KC_W     , KC_E     , KC_R     , KC_T     ,                                        KC_Y     , KC_U     , KC_I     , KC_O     , KC_P     , KC_DEL   ,
    KC_TAB   , KC_A     , KC_S     , KC_D     , KC_F     , KC_G     ,                                        KC_H     , KC_J     , KC_K     , KC_L     , KC_SCLN  , S(KC_7)  ,
    KC_LSFT  , KC_Z     , KC_X     , KC_C     , KC_V     , KC_B     ,                                        KC_N     , KC_M     , KC_COMM  , KC_DOT   , KC_SLSH  , KC_INT1  ,
              KC_LALT,KC_LGUI,LCTL_T(KC_LNG2)     ,LT(1,KC_SPC),LT(3,KC_LNG1),                  KC_BSPC,LT(2,KC_ENT), RCTL_T(KC_LNG2),     KC_RALT  , KC_PSCR
  ),

  [1] = LAYOUT_universal(
    SSNP_FRE ,  KC_F1   , KC_F2    , KC_F3   , KC_F4    , KC_F5    ,                                         KC_F6    , KC_F7    , KC_F8    , KC_F9    , KC_F10   , KC_F11   ,
    SSNP_VRT ,  _______ , _______  , KC_UP   , KC_ENT   , KC_DEL   ,                                         KC_PGUP  , KC_BTN1  , KC_UP    , KC_BTN2  , KC_BTN3  , KC_F12   ,
    SSNP_HOR ,  _______ , KC_LEFT  , KC_DOWN , KC_RGHT  , KC_BSPC  ,                                         KC_PGDN  , KC_LEFT  , KC_DOWN  , KC_RGHT  , _______  , _______  ,
                  _______  , _______ , _______  ,         _______  , _______  ,                   _______  , _______  , _______       , _______  , _______
  ),

  [2] = LAYOUT_universal(
    _______  ,S(KC_QUOT), KC_7     , KC_8    , KC_9     , S(KC_8)  ,                                         S(KC_9)  , S(KC_1)  , S(KC_6)  , KC_LBRC  , S(KC_4)  , _______  ,
    _______  ,S(KC_SCLN), KC_4     , KC_5    , KC_6     , KC_RBRC  ,                                         KC_NUHS  , KC_MINS  , S(KC_EQL), S(KC_3)  , KC_QUOT  , S(KC_2)  ,
    _______  ,S(KC_MINS), KC_1     , KC_2    , KC_3     ,S(KC_RBRC),                                        S(KC_NUHS),S(KC_INT1), KC_EQL   ,S(KC_LBRC),S(KC_SLSH),S(KC_INT3),
                  KC_0     , KC_DOT  , _______  ,         _______  , _______  ,                   KC_DEL   , _______  , _______       , _______  , _______
  ),

  [3] = LAYOUT_universal(
    RGB_TOG  , AML_TO   , AML_I50  , AML_D50  , _______  , _______  ,                                        RGB_M_P  , RGB_M_B  , RGB_M_R  , RGB_M_SW , RGB_M_SN , RGB_M_K  ,
    RGB_MOD  , RGB_HUI  , RGB_SAI  , RGB_VAI  , _______  , SCRL_DVI ,                                        RGB_M_X  , RGB_M_G  , RGB_M_T  , RGB_M_TW , _______  , _______  ,
    RGB_RMOD , RGB_HUD  , RGB_SAD  , RGB_VAD  , _______  , SCRL_DVD ,                                        CPI_D1K  , CPI_D100 , CPI_I100 , CPI_I1K  , _______  , KBC_SAVE ,
                  QK_BOOT  , KBC_RST  , _______  ,        _______  , _______  ,                   _______  , _______  , _______       , KBC_RST  , QK_BOOT
  ),
};
// clang-format on

#ifdef CONSOLE_ENABLE
void matrix_init_kb(void) {
    debug_enable = true;
    debug_matrix = true;
    debug_mouse  = true;
}
#endif


// 状態管理に使う変数
bool is_alt_tab_enabled = false;
bool is_alt_tab_active = false;
uint16_t esc_timer = 0;
uint16_t enter_timer = 0;
uint16_t lang2_timer = 0;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  bool resume = true;

#ifdef CONSOLE_ENABLE
    uprintf("%d %d\n", keycode, record->event.pressed);
#endif

    switch (keycode) {

    case LT(5,KC_SPC):
      if (record->event.pressed) {
        // alttabをレイヤーキーで発動できるように
        is_alt_tab_enabled = true;
      } else {
        // 手動マウス用のレイヤーを抜けた時は、だいたい文字を打ちたい時なので、オートマウスレイヤも抜ける。
        layer_off(AUTO_MOUSE_DEFAULT_LAYER);

        // alttabをレイヤーキーで発動できるように
        is_alt_tab_enabled = false;
        if (is_alt_tab_active) {
          unregister_code(227); // L command
          is_alt_tab_active = false;
        }
      }
      break;

    case KC_TAB: // Tab
        // alttabをレイヤーキーで発動できるように
      if (record->event.pressed && is_alt_tab_enabled) {
        is_alt_tab_enabled = false;

        register_code(227); // L command
        is_alt_tab_active = true;
      }
      break;
    
    case KC_RGUI:
      if (record->event.pressed) {
        // 右コマンドを押した時はショートカットきーを打ちたい時なのでマウスレイヤを抜ける。
        layer_off(AUTO_MOUSE_DEFAULT_LAYER);
        enter_timer = timer_read();
      } else {
        // タップでエンターキー発動。
        // 右コマンド認識までのタイムラグを作りたくないので、RGUI(kc)ではなく自前でタップを実装
        resume = false;
        unregister_code(KC_RGUI); // R command
        uint16_t timer = timer_read();
        if (timer - enter_timer <= TAPPING_TERM) {
          tap_code(KC_ENT);
        }
      }
      break;

    case 20576: // MO(3)
      // タップでESC発動
      // LT(layer, kc)だとレイヤが有効になるのにタイムラグがある。
      // 結果スクロールの発動にタイムラグが生まれるので、自前でタップを実装しタイムラグをなくす。
      if (record->event.pressed) {
        esc_timer = timer_read();
      } else {
        uint16_t timer = timer_read();
        if (timer - esc_timer <= TAPPING_TERM) {
          tap_code(KC_ESC);
          tap_code(KC_LNG2);
        }
      }
      break;

    case 20544: // MO(2)
      // タップで英数発動
      // LT(layer, kc)だとレイヤが有効になるのにタイムラグがある。
      // 度々数字の入力に失敗するので、挙動を改善
      if (record->event.pressed) {
        lang2_timer = timer_read();
      } else {
        uint16_t timer = timer_read();
        if (timer - lang2_timer <= TAPPING_TERM) {
          tap_code(KC_LNG2);
        }
      }
      break;
    break;

    case 0x5F50:
      if (record->event.pressed) {
        register_code(KC_SPC); // L command
        register_code(KC_MS_BTN1); // L command
        // defer_exec(30, down_mouse1, NULL);
      } else {
        unregister_code(KC_SPC); // L command
        unregister_code(KC_MS_BTN1); // L command
        // defer_exec(30, up_mouse1, NULL);
      }
      break;

    case 0x5F51:
      if (record->event.pressed) {
        register_code(KC_LALT); // L command
        register_code(KC_MS_BTN1); // L command
      } else {
        unregister_code(KC_MS_BTN1); // L command
        unregister_code(KC_LALT); // L command
      }
      break;

    default:
      break;
    }

  return resume;
}



layer_state_t layer_state_set_user(layer_state_t state) {
    // Auto enable scroll mode when the highest layer is 3
    // keyball_set_scroll_mode(get_highest_layer(state) == 3);
    keyball_set_scroll_mode(IS_LAYER_ON_STATE(state, 3));
    return state;
}

#ifdef OLED_ENABLE

#    include "lib/oledkit/oledkit.h"

void oledkit_render_info_user(void) {
    keyball_oled_render_keyinfo();
    keyball_oled_render_ballinfo();
    keyball_oled_render_layerinfo();
}
#endif
