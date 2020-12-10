#ifndef IMATHLIB_H_MEDIA_COMMON_HPP
#define IMATHLIB_H_MEDIA_COMMON_HPP


#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#pragma comment(lib, "SDL2_mixer.lib")

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_syswm.h>

#include "IMathLib/math_traits.hpp"
#include "IMathLib/media/log.hpp"

namespace iml {
	namespace ml {

		// 現在のスクリーン領域
		class screen_rect {
			friend class controler;
			friend struct FBO_OBJECT;

			// 現在のスクリーン領域のサイズ(フレームバッファなど)
			static inline size_t width_m = 0;
			static inline size_t height_m = 0;
			// 基底状態のサイズ(ウィンドウサイズ)
			static inline size_t base_width_m = 0;
			static inline size_t base_height_m = 0;
			// フレームバッファのID
			static inline GLuint id_m = 0;
		public:
			static size_t width() { return screen_rect::width_m; }
			static size_t height() { return screen_rect::height_m; }
			static size_t base_width() { return screen_rect::base_width_m; }
			static size_t base_height() { return screen_rect::base_height_m; }
			static GLuint frame_buffer_object_id() { return screen_rect::id_m; }

			friend void window_event(const SDL_WindowEvent&);
		};

		// 矩形領域
		template <class T>
		struct rect {
			T left, right, bottom, top;
			constexpr rect() :left{}, right{}, bottom{}, top{} {}
			constexpr rect(const T& l, const T& r, const T& b, const T& t) : left(l), right(r), bottom(b), top(t) {}

			template <class S>
			rect& operator=(const rect<S>& r) {
				left = r.left;
				right = r.right;
				bottom = r.bottom;
				top = r.top;
				return *this;
			}
		};

		// quitイベントを送信する
		inline void send_quit_event() {
			SDL_Event ev;
			ev.type = SDL_QUIT;
			SDL_PushEvent(&ev);
		}


		// glViewportのラップ(要修正：描画先のスクリーンがウィンドウ固定となっているため)
		inline void glViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
			// 右上が0になるようにする
			::glViewport(x, screen_rect::height() - height - y, width, height);
		}
		inline void glViewport(const rect<int_t>& r) {
			::glViewport(r.left, r.top, r.right - r.left, r.bottom - r.top);
		}
	}
}


#endif