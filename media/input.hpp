#ifndef IMATHLIB_H_MEDIA_INPUT_HPP
#define IMATHLIB_H_MEDIA_INPUT_HPP

#include "IMathLib/math/liner_algebra/vector.hpp"

namespace iml {
	namespace ml {

		// ボタン入力の基本クラス
		class button_input {
			friend class mouse;
			friend class keyboard;

			bool	push_m;			// 押している
			bool	trigger_m;		// 押した瞬間
			bool	release_m;		// 離した瞬間

			// 次の状態に遷移するための更新処理
			void update() {
				// トリガー系の変数を初期化
				if (this->trigger_m) this->trigger_m = false;
				else if (this->release_m) this->release_m = false;
			}
			// key downイベント
			void key_down() {
				this->trigger_m = true;
				this->push_m = true;
			}
			// key upイベント
			void key_up() {
				this->release_m = true;
				this->push_m = false;
			}
		public:
			constexpr button_input() : push_m(false), trigger_m(false), release_m(false) {}

			// 各種状態の取得
			bool push() const { return this->push_m; }
			bool trigger() const { return this->trigger_m; }
			bool release() const { return this->release_m; }

			// 2つのボタンに関する判定
			friend constexpr button_input operator+(const button_input& lhs, const button_input& rhs) {
				button_input temp;
				temp.push_m = lhs.push_m && rhs.push_m;
				temp.trigger_m = (lhs.push_m && rhs.trigger_m) || (lhs.trigger_m && rhs.push_m);
				temp.trigger_m = (!lhs.push_m && rhs.release_m) || (lhs.release_m && !rhs.push_m);
				return temp;
			}

			friend bool event();
		};


		// マウスのクラス
		class mouse {
		public:
			static inline button_input		l;				// 左ボタン
			static inline button_input		r;				// 右ボタン
			static inline button_input		m;				// 中央ボタン
		private:
			static inline vector2<int_t>	pos_m;			// 現在のマウスポインタの座標
			static inline vector2<int_t>	mov_m;			// 前回からの移動量
			static inline int_t				wheel_h_m;		// 水平ホイール
			static inline int_t				wheel_v_m;		// 垂直ホイール

			// ボタン情報の更新受付を開始する
			static void update() {
				mouse::l.update();
				mouse::r.update();
				mouse::m.update();
				mouse::mov_m[0] = 0;
				mouse::mov_m[1] = 0;
				mouse::wheel_h_m = 0;
				mouse::wheel_v_m = 0;
			}
		public:
			static const vector2<int_t>& pos() { return mouse::pos_m; }
			static const vector2<int_t>& mov() { return mouse::mov_m; }
			static const int_t& wheel_h() { return mouse::wheel_h_m; }
			static const int_t& wheel_v() { return mouse::wheel_v_m; }

			friend bool event();
		};

		// キーボードの構造体
		class keyboard {
			static inline bool				scancode_m = true;		// スキャンコードを用いるかのフラグ
			static inline std::string		input_text_m;			// 入力されたテキスト
			static inline std::string		editting_text_m;		// 編集中のテキスト
			static inline Sint32			editting_cursor_m;		// 編集中のテキストへのカーソル
		public:
			// 各種キーボード入力
			static inline button_input		key_a;				// 各英語
			static inline button_input		key_b;
			static inline button_input		key_c;
			static inline button_input		key_d;
			static inline button_input		key_e;
			static inline button_input		key_f;
			static inline button_input		key_g;
			static inline button_input		key_h;
			static inline button_input		key_i;
			static inline button_input		key_j;
			static inline button_input		key_k;
			static inline button_input		key_l;
			static inline button_input		key_m;
			static inline button_input		key_n;
			static inline button_input		key_o;
			static inline button_input		key_p;
			static inline button_input		key_q;
			static inline button_input		key_r;
			static inline button_input		key_s;
			static inline button_input		key_t;
			static inline button_input		key_u;
			static inline button_input		key_v;
			static inline button_input		key_w;
			static inline button_input		key_x;
			static inline button_input		key_y;
			static inline button_input		key_z;
			static inline button_input		key_1;				// 各数字
			static inline button_input		key_2;
			static inline button_input		key_3;
			static inline button_input		key_4;
			static inline button_input		key_5;
			static inline button_input		key_6;
			static inline button_input		key_7;
			static inline button_input		key_8;
			static inline button_input		key_9;
			static inline button_input		key_0;
			static inline button_input		key_f1;				// ファンクションキー
			static inline button_input		key_f2;
			static inline button_input		key_f3;
			static inline button_input		key_f4;
			static inline button_input		key_f5;
			static inline button_input		key_f6;
			static inline button_input		key_f7;
			static inline button_input		key_f8;
			static inline button_input		key_f9;
			static inline button_input		key_f10;
			static inline button_input		key_f11;
			static inline button_input		key_f12;
			static inline button_input		key_enter;			// ENTER
			static inline button_input		key_escape;			// ESCAPE
			static inline button_input		key_backspace;		// BACKSPACE
			static inline button_input		key_tab;			// TAB
			static inline button_input		key_space;			// SPACE
			static inline button_input		key_right;			// ←
			static inline button_input		key_left;			// →
			static inline button_input		key_down;			// ↓
			static inline button_input		key_up;				// ↑
			static inline button_input		key_lctrl;			// LEFT CONTROL
			static inline button_input		key_rctrl;			// RIGHT CONTROL
			static inline button_input		key_lshift;			// LEFT SHIFT
			static inline button_input		key_rshift;			// RIGHT SHIFT
			static inline button_input		key_lalt;			// LEFT ALT
			static inline button_input		key_ralt;			// RIGHT ALT

			// キーコードかスキャンコードかのセット
			static void set_scancode() { keyboard::scancode_m = true; }
			static void set_keycode() { keyboard::scancode_m = false; }
		private:
			// ボタン情報の更新受付を開始する
			static void update() {
				keyboard::key_a.update();
				keyboard::key_b.update();
				keyboard::key_c.update();
				keyboard::key_d.update();
				keyboard::key_e.update();
				keyboard::key_f.update();
				keyboard::key_g.update();
				keyboard::key_h.update();
				keyboard::key_i.update();
				keyboard::key_j.update();
				keyboard::key_k.update();
				keyboard::key_l.update();
				keyboard::key_m.update();
				keyboard::key_n.update();
				keyboard::key_o.update();
				keyboard::key_p.update();
				keyboard::key_q.update();
				keyboard::key_r.update();
				keyboard::key_s.update();
				keyboard::key_t.update();
				keyboard::key_u.update();
				keyboard::key_v.update();
				keyboard::key_w.update();
				keyboard::key_x.update();
				keyboard::key_y.update();
				keyboard::key_z.update();
				keyboard::key_1.update();
				keyboard::key_2.update();
				keyboard::key_3.update();
				keyboard::key_4.update();
				keyboard::key_5.update();
				keyboard::key_6.update();
				keyboard::key_7.update();
				keyboard::key_8.update();
				keyboard::key_9.update();
				keyboard::key_0.update();
				keyboard::key_f1.update();
				keyboard::key_f2.update();
				keyboard::key_f3.update();
				keyboard::key_f4.update();
				keyboard::key_f5.update();
				keyboard::key_f6.update();
				keyboard::key_f7.update();
				keyboard::key_f8.update();
				keyboard::key_f9.update();
				keyboard::key_f10.update();
				keyboard::key_f11.update();
				keyboard::key_f12.update();
				keyboard::key_enter.update();
				keyboard::key_escape.update();
				keyboard::key_backspace.update();
				keyboard::key_tab.update();
				keyboard::key_space.update();
				keyboard::key_right.update();
				keyboard::key_left.update();
				keyboard::key_down.update();
				keyboard::key_up.update();
				keyboard::key_lctrl.update();
				keyboard::key_rctrl.update();
				keyboard::key_lshift.update();
				keyboard::key_rshift.update();
				keyboard::key_lalt.update();
				keyboard::key_ralt.update();
			}
			// キーボードの各々の入力の反映
			static void key_down_ev(const SDL_KeyboardEvent& ev) {
				// スキャンコードの処理をするとき
				if (keyboard::scancode_m) {
					switch (ev.keysym.scancode) {
					case SDL_SCANCODE_A:
						keyboard::key_a.key_down();
						break;
					case SDL_SCANCODE_B:
						keyboard::key_b.key_down();
						break;
					case SDL_SCANCODE_C:
						keyboard::key_c.key_down();
						break;
					case SDL_SCANCODE_D:
						keyboard::key_d.key_down();
						break;
					case SDL_SCANCODE_E:
						keyboard::key_e.key_down();
						break;
					case SDL_SCANCODE_F:
						keyboard::key_f.key_down();
						break;
					case SDL_SCANCODE_G:
						keyboard::key_g.key_down();
						break;
					case SDL_SCANCODE_H:
						keyboard::key_h.key_down();
						break;
					case SDL_SCANCODE_I:
						keyboard::key_i.key_down();
						break;
					case SDL_SCANCODE_J:
						keyboard::key_j.key_down();
						break;
					case SDL_SCANCODE_K:
						keyboard::key_k.key_down();
						break;
					case SDL_SCANCODE_L:
						keyboard::key_l.key_down();
						break;
					case SDL_SCANCODE_M:
						keyboard::key_m.key_down();
						break;
					case SDL_SCANCODE_N:
						keyboard::key_n.key_down();
						break;
					case SDL_SCANCODE_O:
						keyboard::key_o.key_down();
						break;
					case SDL_SCANCODE_P:
						keyboard::key_p.key_down();
						break;
					case SDL_SCANCODE_Q:
						keyboard::key_q.key_down();
						break;
					case SDL_SCANCODE_R:
						keyboard::key_r.key_down();
						break;
					case SDL_SCANCODE_S:
						keyboard::key_s.key_down();
						break;
					case SDL_SCANCODE_T:
						keyboard::key_t.key_down();
						break;
					case SDL_SCANCODE_U:
						keyboard::key_u.key_down();
						break;
					case SDL_SCANCODE_V:
						keyboard::key_v.key_down();
						break;
					case SDL_SCANCODE_W:
						keyboard::key_w.key_down();
						break;
					case SDL_SCANCODE_X:
						keyboard::key_x.key_down();
						break;
					case SDL_SCANCODE_Y:
						keyboard::key_y.key_down();
						break;
					case SDL_SCANCODE_Z:
						keyboard::key_z.key_down();
						break;
					case SDL_SCANCODE_1:
						keyboard::key_1.key_down();
						break;
					case SDL_SCANCODE_2:
						keyboard::key_2.key_down();
						break;
					case SDL_SCANCODE_3:
						keyboard::key_3.key_down();
						break;
					case SDL_SCANCODE_4:
						keyboard::key_4.key_down();
						break;
					case SDL_SCANCODE_5:
						keyboard::key_5.key_down();
						break;
					case SDL_SCANCODE_6:
						keyboard::key_6.key_down();
						break;
					case SDL_SCANCODE_7:
						keyboard::key_7.key_down();
						break;
					case SDL_SCANCODE_8:
						keyboard::key_8.key_down();
						break;
					case SDL_SCANCODE_9:
						keyboard::key_9.key_down();
						break;
					case SDL_SCANCODE_0:
						keyboard::key_0.key_down();
						break;
					case SDL_SCANCODE_F1:
						keyboard::key_f1.key_down();
						break;
					case SDL_SCANCODE_F2:
						keyboard::key_f2.key_down();
						break;
					case SDL_SCANCODE_F3:
						keyboard::key_f3.key_down();
						break;
					case SDL_SCANCODE_F4:
						keyboard::key_f4.key_down();
						break;
					case SDL_SCANCODE_F5:
						keyboard::key_f5.key_down();
						break;
					case SDL_SCANCODE_F6:
						keyboard::key_f6.key_down();
						break;
					case SDL_SCANCODE_F7:
						keyboard::key_f7.key_down();
						break;
					case SDL_SCANCODE_F8:
						keyboard::key_f8.key_down();
						break;
					case SDL_SCANCODE_F9:
						keyboard::key_f9.key_down();
						break;
					case SDL_SCANCODE_F10:
						keyboard::key_f10.key_down();
						break;
					case SDL_SCANCODE_F11:
						keyboard::key_f11.key_down();
						break;
					case SDL_SCANCODE_F12:
						keyboard::key_f12.key_down();
						break;
					case SDL_SCANCODE_RETURN:
						keyboard::key_enter.key_down();
						break;
					case SDL_SCANCODE_ESCAPE:
						keyboard::key_escape.key_down();
						break;
					case SDL_SCANCODE_BACKSPACE:
						keyboard::key_backspace.key_down();
						break;
					case SDL_SCANCODE_TAB:
						keyboard::key_tab.key_down();
						break;
					case SDL_SCANCODE_SPACE:
						keyboard::key_space.key_down();
						break;
					case SDL_SCANCODE_RIGHT:
						keyboard::key_right.key_down();
						break;
					case SDL_SCANCODE_LEFT:
						keyboard::key_left.key_down();
						break;
					case SDL_SCANCODE_DOWN:
						keyboard::key_down.key_down();
						break;
					case SDL_SCANCODE_UP:
						keyboard::key_up.key_down();
						break;
					case SDL_SCANCODE_LCTRL:
						keyboard::key_lctrl.key_down();
						break;
					case SDL_SCANCODE_RCTRL:
						keyboard::key_rctrl.key_down();
						break;
					case SDL_SCANCODE_LSHIFT:
						keyboard::key_lshift.key_down();
						break;
					case SDL_SCANCODE_RSHIFT:
						keyboard::key_rshift.key_down();
						break;
					case SDL_SCANCODE_LALT:
						keyboard::key_lalt.key_down();
						break;
					case SDL_SCANCODE_RALT:
						keyboard::key_ralt.key_down();
						break;
					}
				}
				else {
					switch (ev.keysym.sym) {
					case SDLK_a:
						keyboard::key_a.key_down();
						break;
					case SDLK_b:
						keyboard::key_b.key_down();
						break;
					case SDLK_c:
						keyboard::key_c.key_down();
						break;
					case SDLK_d:
						keyboard::key_d.key_down();
						break;
					case SDLK_e:
						keyboard::key_e.key_down();
						break;
					case SDLK_f:
						keyboard::key_f.key_down();
						break;
					case SDLK_g:
						keyboard::key_g.key_down();
						break;
					case SDLK_h:
						keyboard::key_h.key_down();
						break;
					case SDLK_i:
						keyboard::key_i.key_down();
						break;
					case SDLK_j:
						keyboard::key_j.key_down();
						break;
					case SDLK_k:
						keyboard::key_k.key_down();
						break;
					case SDLK_l:
						keyboard::key_l.key_down();
						break;
					case SDLK_m:
						keyboard::key_m.key_down();
						break;
					case SDLK_n:
						keyboard::key_n.key_down();
						break;
					case SDLK_o:
						keyboard::key_o.key_down();
						break;
					case SDLK_p:
						keyboard::key_p.key_down();
						break;
					case SDLK_q:
						keyboard::key_q.key_down();
						break;
					case SDLK_r:
						keyboard::key_r.key_down();
						break;
					case SDLK_s:
						keyboard::key_s.key_down();
						break;
					case SDLK_t:
						keyboard::key_t.key_down();
						break;
					case SDLK_u:
						keyboard::key_u.key_down();
						break;
					case SDLK_v:
						keyboard::key_v.key_down();
						break;
					case SDLK_w:
						keyboard::key_w.key_down();
						break;
					case SDLK_x:
						keyboard::key_x.key_down();
						break;
					case SDLK_y:
						keyboard::key_y.key_down();
						break;
					case SDLK_z:
						keyboard::key_z.key_down();
						break;
					case SDLK_1:
						keyboard::key_1.key_down();
						break;
					case SDLK_2:
						keyboard::key_2.key_down();
						break;
					case SDLK_3:
						keyboard::key_3.key_down();
						break;
					case SDLK_4:
						keyboard::key_4.key_down();
						break;
					case SDLK_5:
						keyboard::key_5.key_down();
						break;
					case SDLK_6:
						keyboard::key_6.key_down();
						break;
					case SDLK_7:
						keyboard::key_7.key_down();
						break;
					case SDLK_8:
						keyboard::key_8.key_down();
						break;
					case SDLK_9:
						keyboard::key_9.key_down();
						break;
					case SDLK_0:
						keyboard::key_0.key_down();
						break;
					case SDLK_F1:
						keyboard::key_f1.key_down();
						break;
					case SDLK_F2:
						keyboard::key_f2.key_down();
						break;
					case SDLK_F3:
						keyboard::key_f3.key_down();
						break;
					case SDLK_F4:
						keyboard::key_f4.key_down();
						break;
					case SDLK_F5:
						keyboard::key_f5.key_down();
						break;
					case SDLK_F6:
						keyboard::key_f6.key_down();
						break;
					case SDLK_F7:
						keyboard::key_f7.key_down();
						break;
					case SDLK_F8:
						keyboard::key_f8.key_down();
						break;
					case SDLK_F9:
						keyboard::key_f9.key_down();
						break;
					case SDLK_F10:
						keyboard::key_f10.key_down();
						break;
					case SDLK_F11:
						keyboard::key_f11.key_down();
						break;
					case SDLK_F12:
						keyboard::key_f12.key_down();
						break;
					case SDLK_RETURN:
						keyboard::key_enter.key_down();
						break;
					case SDLK_ESCAPE:
						keyboard::key_escape.key_down();
						break;
					case SDLK_BACKSPACE:
						keyboard::key_backspace.key_down();
						break;
					case SDLK_TAB:
						keyboard::key_tab.key_down();
						break;
					case SDLK_SPACE:
						keyboard::key_space.key_down();
						break;
					case SDLK_RIGHT:
						keyboard::key_right.key_down();
						break;
					case SDLK_LEFT:
						keyboard::key_left.key_down();
						break;
					case SDLK_DOWN:
						keyboard::key_down.key_down();
						break;
					case SDLK_UP:
						keyboard::key_up.key_down();
						break;
					case SDLK_LCTRL:
						keyboard::key_lctrl.key_down();
						break;
					case SDLK_RCTRL:
						keyboard::key_rctrl.key_down();
						break;
					case SDLK_LSHIFT:
						keyboard::key_lshift.key_down();
						break;
					case SDLK_RSHIFT:
						keyboard::key_rshift.key_down();
						break;
					case SDLK_LALT:
						keyboard::key_lalt.key_down();
						break;
					case SDLK_RALT:
						keyboard::key_ralt.key_down();
						break;
					}
				}
			}
			static void key_up_ev(const SDL_KeyboardEvent& ev) {
				// スキャンコードの処理をするとき
				if (keyboard::scancode_m) {
					switch (ev.keysym.scancode) {
					case SDL_SCANCODE_A:
						keyboard::key_a.key_up();
						break;
					case SDL_SCANCODE_B:
						keyboard::key_b.key_up();
						break;
					case SDL_SCANCODE_C:
						keyboard::key_c.key_up();
						break;
					case SDL_SCANCODE_D:
						keyboard::key_d.key_up();
						break;
					case SDL_SCANCODE_E:
						keyboard::key_e.key_up();
						break;
					case SDL_SCANCODE_F:
						keyboard::key_f.key_up();
						break;
					case SDL_SCANCODE_G:
						keyboard::key_g.key_up();
						break;
					case SDL_SCANCODE_H:
						keyboard::key_h.key_up();
						break;
					case SDL_SCANCODE_I:
						keyboard::key_i.key_up();
						break;
					case SDL_SCANCODE_J:
						keyboard::key_j.key_up();
						break;
					case SDL_SCANCODE_K:
						keyboard::key_k.key_up();
						break;
					case SDL_SCANCODE_L:
						keyboard::key_l.key_up();
						break;
					case SDL_SCANCODE_M:
						keyboard::key_m.key_up();
						break;
					case SDL_SCANCODE_N:
						keyboard::key_n.key_up();
						break;
					case SDL_SCANCODE_O:
						keyboard::key_o.key_up();
						break;
					case SDL_SCANCODE_P:
						keyboard::key_p.key_up();
						break;
					case SDL_SCANCODE_Q:
						keyboard::key_q.key_up();
						break;
					case SDL_SCANCODE_R:
						keyboard::key_r.key_up();
						break;
					case SDL_SCANCODE_S:
						keyboard::key_s.key_up();
						break;
					case SDL_SCANCODE_T:
						keyboard::key_t.key_up();
						break;
					case SDL_SCANCODE_U:
						keyboard::key_u.key_up();
						break;
					case SDL_SCANCODE_V:
						keyboard::key_v.key_up();
						break;
					case SDL_SCANCODE_W:
						keyboard::key_w.key_up();
						break;
					case SDL_SCANCODE_X:
						keyboard::key_x.key_up();
						break;
					case SDL_SCANCODE_Y:
						keyboard::key_y.key_up();
						break;
					case SDL_SCANCODE_Z:
						keyboard::key_z.key_up();
						break;
					case SDL_SCANCODE_1:
						keyboard::key_1.key_up();
						break;
					case SDL_SCANCODE_2:
						keyboard::key_2.key_up();
						break;
					case SDL_SCANCODE_3:
						keyboard::key_3.key_up();
						break;
					case SDL_SCANCODE_4:
						keyboard::key_4.key_up();
						break;
					case SDL_SCANCODE_5:
						keyboard::key_5.key_up();
						break;
					case SDL_SCANCODE_6:
						keyboard::key_6.key_up();
						break;
					case SDL_SCANCODE_7:
						keyboard::key_7.key_up();
						break;
					case SDL_SCANCODE_8:
						keyboard::key_8.key_up();
						break;
					case SDL_SCANCODE_9:
						keyboard::key_9.key_up();
						break;
					case SDL_SCANCODE_0:
						keyboard::key_0.key_up();
						break;
					case SDL_SCANCODE_F1:
						keyboard::key_f1.key_up();
						break;
					case SDL_SCANCODE_F2:
						keyboard::key_f2.key_up();
						break;
					case SDL_SCANCODE_F3:
						keyboard::key_f3.key_up();
						break;
					case SDL_SCANCODE_F4:
						keyboard::key_f4.key_up();
						break;
					case SDL_SCANCODE_F5:
						keyboard::key_f5.key_up();
						break;
					case SDL_SCANCODE_F6:
						keyboard::key_f6.key_up();
						break;
					case SDL_SCANCODE_F7:
						keyboard::key_f7.key_up();
						break;
					case SDL_SCANCODE_F8:
						keyboard::key_f8.key_up();
						break;
					case SDL_SCANCODE_F9:
						keyboard::key_f9.key_up();
						break;
					case SDL_SCANCODE_F10:
						keyboard::key_f10.key_up();
						break;
					case SDL_SCANCODE_F11:
						keyboard::key_f11.key_up();
						break;
					case SDL_SCANCODE_F12:
						keyboard::key_f12.key_up();
						break;
					case SDL_SCANCODE_RETURN:
						keyboard::key_enter.key_up();
						break;
					case SDL_SCANCODE_ESCAPE:
						keyboard::key_escape.key_up();
						break;
					case SDL_SCANCODE_BACKSPACE:
						keyboard::key_backspace.key_up();
						break;
					case SDL_SCANCODE_TAB:
						keyboard::key_tab.key_up();
						break;
					case SDL_SCANCODE_SPACE:
						keyboard::key_space.key_up();
						break;
					case SDL_SCANCODE_RIGHT:
						keyboard::key_right.key_up();
						break;
					case SDL_SCANCODE_LEFT:
						keyboard::key_left.key_up();
						break;
					case SDL_SCANCODE_DOWN:
						keyboard::key_down.key_up();
						break;
					case SDL_SCANCODE_UP:
						keyboard::key_up.key_up();
						break;
					case SDL_SCANCODE_LCTRL:
						keyboard::key_lctrl.key_up();
						break;
					case SDL_SCANCODE_RCTRL:
						keyboard::key_rctrl.key_up();
						break;
					case SDL_SCANCODE_LSHIFT:
						keyboard::key_lshift.key_up();
						break;
					case SDL_SCANCODE_RSHIFT:
						keyboard::key_rshift.key_up();
						break;
					case SDL_SCANCODE_LALT:
						keyboard::key_lalt.key_up();
						break;
					case SDL_SCANCODE_RALT:
						keyboard::key_ralt.key_up();
						break;
					}
				}
				else {
					switch (ev.keysym.sym) {
					case SDLK_a:
						keyboard::key_a.key_up();
						break;
					case SDLK_b:
						keyboard::key_b.key_up();
						break;
					case SDLK_c:
						keyboard::key_c.key_up();
						break;
					case SDLK_d:
						keyboard::key_d.key_up();
						break;
					case SDLK_e:
						keyboard::key_e.key_up();
						break;
					case SDLK_f:
						keyboard::key_f.key_up();
						break;
					case SDLK_g:
						keyboard::key_g.key_up();
						break;
					case SDLK_h:
						keyboard::key_h.key_up();
						break;
					case SDLK_i:
						keyboard::key_i.key_up();
						break;
					case SDLK_j:
						keyboard::key_j.key_up();
						break;
					case SDLK_k:
						keyboard::key_k.key_up();
						break;
					case SDLK_l:
						keyboard::key_l.key_up();
						break;
					case SDLK_m:
						keyboard::key_m.key_up();
						break;
					case SDLK_n:
						keyboard::key_n.key_up();
						break;
					case SDLK_o:
						keyboard::key_o.key_up();
						break;
					case SDLK_p:
						keyboard::key_p.key_up();
						break;
					case SDLK_q:
						keyboard::key_q.key_up();
						break;
					case SDLK_r:
						keyboard::key_r.key_up();
						break;
					case SDLK_s:
						keyboard::key_s.key_up();
						break;
					case SDLK_t:
						keyboard::key_t.key_up();
						break;
					case SDLK_u:
						keyboard::key_u.key_up();
						break;
					case SDLK_v:
						keyboard::key_v.key_up();
						break;
					case SDLK_w:
						keyboard::key_w.key_up();
						break;
					case SDLK_x:
						keyboard::key_x.key_up();
						break;
					case SDLK_y:
						keyboard::key_y.key_up();
						break;
					case SDLK_z:
						keyboard::key_z.key_up();
						break;
					case SDLK_1:
						keyboard::key_1.key_up();
						break;
					case SDLK_2:
						keyboard::key_2.key_up();
						break;
					case SDLK_3:
						keyboard::key_3.key_up();
						break;
					case SDLK_4:
						keyboard::key_4.key_up();
						break;
					case SDLK_5:
						keyboard::key_5.key_up();
						break;
					case SDLK_6:
						keyboard::key_6.key_up();
						break;
					case SDLK_7:
						keyboard::key_7.key_up();
						break;
					case SDLK_8:
						keyboard::key_8.key_up();
						break;
					case SDLK_9:
						keyboard::key_9.key_up();
						break;
					case SDLK_0:
						keyboard::key_0.key_up();
						break;
					case SDLK_F1:
						keyboard::key_f1.key_up();
						break;
					case SDLK_F2:
						keyboard::key_f2.key_up();
						break;
					case SDLK_F3:
						keyboard::key_f3.key_up();
						break;
					case SDLK_F4:
						keyboard::key_f4.key_up();
						break;
					case SDLK_F5:
						keyboard::key_f5.key_up();
						break;
					case SDLK_F6:
						keyboard::key_f6.key_up();
						break;
					case SDLK_F7:
						keyboard::key_f7.key_up();
						break;
					case SDLK_F8:
						keyboard::key_f8.key_up();
						break;
					case SDLK_F9:
						keyboard::key_f9.key_up();
						break;
					case SDLK_F10:
						keyboard::key_f10.key_up();
						break;
					case SDLK_F11:
						keyboard::key_f11.key_up();
						break;
					case SDLK_F12:
						keyboard::key_f12.key_up();
						break;
					case SDLK_RETURN:
						keyboard::key_enter.key_up();
						break;
					case SDLK_ESCAPE:
						keyboard::key_escape.key_up();
						break;
					case SDLK_BACKSPACE:
						keyboard::key_backspace.key_up();
						break;
					case SDLK_TAB:
						keyboard::key_tab.key_up();
						break;
					case SDLK_SPACE:
						keyboard::key_space.key_up();
						break;
					case SDLK_RIGHT:
						keyboard::key_right.key_up();
						break;
					case SDLK_LEFT:
						keyboard::key_left.key_up();
						break;
					case SDLK_DOWN:
						keyboard::key_down.key_up();
						break;
					case SDLK_UP:
						keyboard::key_up.key_up();
						break;
					case SDLK_LCTRL:
						keyboard::key_lctrl.key_up();
						break;
					case SDLK_RCTRL:
						keyboard::key_rctrl.key_up();
						break;
					case SDLK_LSHIFT:
						keyboard::key_lshift.key_up();
						break;
					case SDLK_RSHIFT:
						keyboard::key_rshift.key_up();
						break;
					case SDLK_LALT:
						keyboard::key_lalt.key_up();
						break;
					case SDLK_RALT:
						keyboard::key_ralt.key_up();
						break;
					}
				}
			}

			friend bool event();
		};
	}
}


#endif