#ifndef IMATHLIB_H_MEDIA_INTERFACE_HPP
#define IMATHLIB_H_MEDIA_INTERFACE_HPP


// オーディオ関連の設定
#ifdef IMATHLIB_F_MEDIA_AUDIO
// SEの種類の数
#ifndef IMATHLIB_F_MEDIA_SE_NAM
#define IMATHLIB_F_MEDIA_SE_NAM	2
#endif
// SEのチャネルの数(SEの種類に対してチャネルは等分される)
#ifndef IMATHLIB_F_MEDIA_CHANNEL_NUM
#define IMATHLIB_F_MEDIA_CHANNEL_NUM			16
#endif
#endif


// Zバッファの深度
#ifndef IMATHLIB_F_MEDIA_Z_BUFFER_BITS
#define IMATHLIB_F_MEDIA_Z_BUFFER_BITS			24
#endif


#include "IMathLib/media/common.hpp"
#include "IMathLib/media/common/singleton.hpp"
#include "IMathLib/math/liner_algebra.hpp"
#include "IMathLib/media/graphic.hpp"
#ifdef IMATHLIB_F_MEDIA_INPUT
#include "IMathLib/media/input.hpp"
#endif
#ifdef IMATHLIB_F_MEDIA_TTF
#include <unordered_map>
#include <memory>
#pragma comment(lib, "SDL2_ttf.lib")
#include <SDL_ttf.h>
namespace iml {
	namespace ml {
		class ttf;
	}
}
#endif

#include <chrono>
#include <thread>
#include <map>
#include <functional>


namespace iml {
	namespace ml {

		template <class T>
		class timer {
			std::chrono::system_clock::time_point start_m;
			std::chrono::system_clock::time_point pause_m;
		public:
			constexpr timer() : start_m(), pause_m() {}
			// 時間計測開始
			void start() { this->start_m = std::chrono::system_clock::now(); }
			// 現在の計測開始からの時間の取得
			long long now_time() const {
				std::chrono::system_clock::time_point temp(std::chrono::system_clock::now());
				return std::chrono::duration_cast<T>(temp - this->start_m).count();
			}
			// 一時停止
			void pause() { this->pause_m = std::chrono::system_clock::now(); }
			// 一時停止解除(戻り値はポーズ時間)
			long long resume() {
				std::chrono::system_clock::time_point temp(std::chrono::system_clock::now());
				this->start_m += temp - this->pause_m;
				return std::chrono::duration_cast<T>(temp - this->pause_m).count();
			}
		};

		// FPS管理
		template <class Float>
		class fps_control {
			using timer_t = timer<std::chrono::milliseconds>;

			size_t		frame_cnt_m;			// フレームカウンタ
			Float		fps_m;					// 現在のset_fps_m
			size_t		sample_m;				// サンプル数
			Float		set_fps_m;				// 設定したset_fps_m
			timer_t		timer_m;				// 時間計測のためのオブジェクト
			long long	start_time_m;			// 計測開始時間から前回のフレームまでの時間
		public:
			fps_control() :frame_cnt_m(0), fps_m(0), set_fps_m(60), sample_m(60), start_time_m(0) {
				this->timer_m.start();
			}
			fps_control(const Float& fps, size_t sample) : frame_cnt_m(0), fps_m(0), set_fps_m(fps), sample_m(sample), start_time_m(0) {
				this->timer_m.start();
			}

			// フレーム記憶
			fps_control& update() {
				// 平均fpsの算出
				if (this->frame_cnt_m == this->sample_m) {
					long long t = (this->timer_m.now_time() - this->start_time_m);
					this->fps_m = 1000 * static_cast<Float>(this->sample_m) / t;
					this->frame_cnt_m = 0;
					this->start_time_m = this->timer_m.now_time();
				}
				++frame_cnt_m;
				return *this;
			}
			// ウェイト
			fps_control& wait() {
				// 1フレームあたりの待機時間
				long long wait_time = static_cast<long long>(frame_cnt_m * 1000 / set_fps_m) - (this->timer_m.now_time() - this->start_time_m);
				// 待機
				if (wait_time > 0) std::this_thread::sleep_for(std::chrono::milliseconds(wait_time));

				return *this;
			}
			// 目標FPSとサンプル数の変更
			void fps(const Float& fps, size_t sample) {
				this->set_fps_m = fps; this->sample_m = sample;
			}
			// 現在のfps取得
			const Float& fps() const { return this->fps_m; }
			// 計測開始時間からの現在の時間(ms)の取得
			long long time() const { return this->timer_m.now_time(); }
		};


		// ウィンドウ(controlerで保持する1つのみ生成可能)
		class mlwindow {
			friend class controler;

			SDL_Window* hwnd_m;			// ウィンドウハンドル
			Uint32			wnd_id_m;			// ウィンドウID
			//SDL_SysWMinfo	wnd_info;			// ウィンドウ情報


			std::function<void(void)>			close_event;		// ウィンドウを閉じるときの処理

			// ルートウィンドウ用
			mlwindow(const char* title, size_t w, size_t h, Uint32 flag = SDL_WINDOW_HIDDEN) : close_event([&]() { this->hide(); }), hwnd_m(nullptr) {
				// 描画をOpenGLにするウィンドウの作成
				this->hwnd_m = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL | flag);
				if (this->hwnd_m == nullptr) IMATHLIB_LOG(iml::ml::logger::fatal, u8"SDL_CreateWindow") << SDL_GetError();
				// ウィンドウIDの取得
				if ((this->wnd_id_m = SDL_GetWindowID(this->hwnd_m)) == 0) IMATHLIB_LOG(iml::ml::logger::fatal, u8"SDL_GetWindowID") << SDL_GetError();
			}
			mlwindow(const mlwindow&) = delete;
			mlwindow(mlwindow&&) = delete;
			mlwindow& operator=(const mlwindow&) = delete;
			mlwindow& operator=(mlwindow&&) = delete;
		public:
			~mlwindow() { SDL_DestroyWindow(this->hwnd_m); }

			// ウィンドウの表示
			void show() { SDL_ShowWindow(this->hwnd_m); }
			// ウィンドウの非表示
			void hide() { SDL_HideWindow(this->hwnd_m); }

			// ウィンドウハンドルの取得
			SDL_Window* handle() { return this->hwnd_m; }
			const SDL_Window* handle() const { return this->hwnd_m; }
			// ウィンドウIDの取得
			Uint32 id() const { return this->wnd_id_m; }

			// ウィンドウ再構築
			mlwindow& remake(size_t w, size_t h, Uint32 flag = SDL_WINDOW_HIDDEN) {
				return this->remake("IMathLib mlwindow", w, h, flag);
			}
			mlwindow& remake(const char* title, size_t w, size_t h, Uint32 flag = SDL_WINDOW_HIDDEN) {
				if (this->hwnd_m != nullptr) SDL_DestroyWindow(this->hwnd_m);

				SDL_Window* temp = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL | flag);
				if (temp == nullptr) {
					IMATHLIB_LOG(iml::ml::logger::fatal, u8"SDL_CreateWindow") << SDL_GetError();
					return *this;
				}
				this->hwnd_m = temp;
				if ((this->wnd_id_m = SDL_GetWindowID(this->hwnd_m)) == 0) IMATHLIB_LOG(iml::ml::logger::fatal, u8"SDL_GetWindowID") << SDL_GetError();
				return *this;
			}

			// ウィンドウフレームの更新
			void update() { SDL_GL_SwapWindow(this->hwnd_m); }
			// カラーバッファとZバッファのクリア
			void clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

			// ウィンドウの高さと幅の取得
			int width() {
				int temp;
				SDL_GetWindowSize(this->hwnd_m, &temp, nullptr);
				return temp;
			}
			int height() {
				int temp;
				SDL_GetWindowSize(this->hwnd_m, nullptr, &temp);
				return temp;
			}

			// コールバック関数の設定
			template <class F>
			void close_ev(F f) { this->close_event = f; }

			friend void window_event(const SDL_WindowEvent&);
		};


#ifdef IMATHLIB_F_MEDIA_UI
		namespace ui {
			class gui_base;
		}
#endif

		// 各種制御系(インターフェース)のためのコントローラー(メインスレッド用)
		class controler : public singleton<controler> {
			friend class singleton<controler>;

			SDL_GLContext		context_m;				// OpenGLのコンテキスト
			mlwindow* hwnd_m;			// ライブラリで扱うウィンドウ
			fps_control<float>	fps_m;					// fpsの処理
#ifdef IMATHLIB_F_MEDIA_TTF
			// これらはGUIで用いる
		public:
			using font_tabel_t = std::unordered_map<int, std::shared_ptr<ttf>>;
		private:
			std::string			font_path_m;		// ライブラリで用いるフォントのパス
			font_tabel_t		font_table_m;		// フォントサイズをキーにもつフォントテーブル
#endif
#ifdef IMATHLIB_F_MEDIA_UI
			friend class ui::gui_base;
			std::list<ui::gui_base*>		gui_list_m;			// 存在しているGUIウィンドウのハンドルリスト(若い番号がカレントとなる)
#endif

			controler() : controler("IMediaLib + controler window", 640, 480, SDL_WINDOW_HIDDEN) {}
			controler(const char* title, size_t w, size_t h, Uint32 flag) : context_m(nullptr), hwnd_m(nullptr) {
				// ログの初期化
#ifdef _DEBUG
				logger::init(logger::debug);
#else
				logger::init(logger::error);
#endif

				// スクリーン領域の設定
				screen_rect::width_m = w;
				screen_rect::height_m = h;

				// SDL2の初期化
				if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
					IMATHLIB_LOG(iml::ml::logger::fatal, u8"SDL_Init") << SDL_GetError();
				SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);									//  ダブルバッファリングの有効
				SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, IMATHLIB_F_MEDIA_Z_BUFFER_BITS);			//  デプスバッファのビット精度
				// カラーバッファの読み書きの設定
				glDrawBuffer(GL_BACK);
				glReadBuffer(GL_NONE);
				// 各種インターフェースの初期化
#ifdef IMATHLIB_F_MEDIA_AUDIO
				// サウンドミキシングの初期化
				if (Mix_Init(MIX_INIT_OGG | MIX_INIT_MOD | MIX_INIT_MP3) == -1)
					IMATHLIB_LOG(iml::ml::logger::fatal, u8"Mix_Init") << Mix_GetError();
				// 44.1KHz, 16bit符号あり, システムのバイト順, ステレオ, 4096byteチャンクでサウンドミキサーを開く
				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
					IMATHLIB_LOG(iml::ml::logger::fatal, u8"Mix_OpenAudio") << Mix_GetError();
#endif
				// TrueTypeフォント
#ifdef IMATHLIB_F_MEDIA_TTF
				if (TTF_Init() == -1)
					IMATHLIB_LOG(iml::ml::logger::fatal, u8"TTF_Init") << TTF_GetError();
#endif

#ifdef IMATHLIB_F_MEDIA_AUDIO
				{
					// チャネル数の補正(チャネル数がSEの数で等分できる必要がある)
					size_t channel_sum = IMATHLIB_F_MEDIA_CHANNEL_NUM;
					if (channel_sum % IMATHLIB_F_MEDIA_SE_NAM != 0)
						channel_sum += IMATHLIB_F_MEDIA_SE_NAM - channel_sum % IMATHLIB_F_MEDIA_SE_NAM;
					// 一つあたりのチャネルの個数
					size_t channel_1 = channel_sum / IMATHLIB_F_MEDIA_SE_NAM;
					// ミキシングチャネルを確保する
					Mix_AllocateChannels(channel_sum);
					// 各チャンネルを登録
					for (size_t i = 0; i < IMATHLIB_F_MEDIA_SE_NAM; ++i)
						Mix_GroupChannels(i * channel_1, (i + 1) * channel_1 - 1, i);
				}
#endif
				// デフォルトでは文字入力を受け付けない
				SDL_StopTextInput();

				// ウィンドウの構築
				this->hwnd_m = new mlwindow(title, w, h, flag);
				// OpenGLのコンテキストの生成
				this->context_m = SDL_GL_CreateContext(this->hwnd_m->handle());
				if (this->context_m == nullptr) IMATHLIB_LOG(iml::ml::logger::fatal, u8"SDL_GL_CreateContext") << SDL_GetError();
				glClearColor(1, 1, 1, 1);				// バッファクリア時の背景色
				glFrontFace(GL_CCW);					// 左回りがポリゴンの前面として扱う(ちなみにこれはOpenGLのデフォルト)

#ifdef IMATHLIB_F_GLEW
				{
					// GLEWの初期化(ウィンドウを作ってからでないとエラーになる)
					size_t err = glewInit();
					if (err != GLEW_OK) IMATHLIB_LOG(iml::ml::logger::fatal, u8"glewInit") << glewGetErrorString(err);
				}
				// テクスチャユニットの管理を初期化
				texture_unit_control::init();
				// FBOの管理の初期化
				init_frame_buffer_object();
#endif
			}
		public:
			~controler() {
				// ログが蓄積されていれば出力する
				logger::wrrite();

#ifdef IMATHLIB_F_GLEW
				quit_frame_buffer_object<float>();
				texture_unit_control::quit();
#endif

				delete this->hwnd_m;
				SDL_GL_DeleteContext(this->context_m);

				// 各種終了処理
#ifdef IMATHLIB_F_MEDIA_AUDIO
				Mix_CloseAudio();
				Mix_Quit();
#endif
#ifdef IMATHLIB_F_MEDIA_TTF
				// TTF_Quit前にフォントを解放する
				this->font_table_m.clear();
				TTF_Quit();
#endif
			}

			// コンテキストの破棄(というよりは初期化)
			void context_destroy() {
				SDL_GL_DeleteContext(this->context_m);
				this->context_m = SDL_GL_CreateContext(this->hwnd_m->handle());
				if (this->context_m == nullptr) IMATHLIB_LOG(iml::ml::logger::fatal, u8"SDL_GL_CreateContext") << SDL_GetError();
			}

			// ウィンドウのハンドルの取得
			mlwindow& window() { return *(this->hwnd_m); }
			const mlwindow& window() const { return *(this->hwnd_m); }

			// fps制御ハンドルの取得
			fps_control<float>& fps() { return this->fps_m; }
			const fps_control<float>& fps() const { return this->fps_m; }

#ifdef IMATHLIB_F_MEDIA_TTF
			// ライブラリで用いるためのフォントパスの設定取得
			// UTF-16
			controler& font_path(const char16_t* path);
			controler& font_path(const wchar_t* path);
			// UTF-8
			controler& font_path(const char* path);
			// 上記のいづれかを適用
			template <class CharT, class Traits, class Allocator>
			controler& font_path(const std::basic_string<CharT, Traits, Allocator>& path);
			const std::string& font_path() const;
			// 現在のライブラリで保持しているフォントからフォントサイズを指定してハンドルの取得
			std::shared_ptr<ttf> font(size_t n);
			// フォントハンドルを保持している連想配列への参照を取得
			font_tabel_t& font();
			const font_tabel_t& font() const;
			// フォントハンドルの整理
			controler& font_organizing();
#endif


			friend void window_event(const SDL_WindowEvent&);
			friend bool event();
			friend void glViewport(GLint, GLint, GLsizei, GLsizei);
		};

		// controlerのインスタンスの呼び出し
		inline controler& medialib() { return *controler::inst(); }

#ifdef IMATHLIB_F_MEDIA_TTF

	}
}
// TTFでログ出力を利用するためにここでインクルード
#include "IMathLib/media/ttf.hpp"

namespace iml {
	namespace ml {

		// ライブラリで用いるためのフォントパスの設定取得
		// UTF-16
		inline controler& controler::font_path(const char16_t* path) {
			this->font_path_m.clear();
			convert_unicode(this->font_path_m, path);
			return *this;
		}
		inline controler& controler::font_path(const wchar_t* path) {
			this->font_path_m.clear();
			convert_unicode(this->font_path_m, path);
			return *this;
		}
		// UTF-8
		inline controler& controler::font_path(const char* path) {
			this->font_path_m = path;
			return *this;
		}
		// 上記のいづれかを適用
		template <class CharT, class Traits, class Allocator>
		inline controler& controler::font_path(const std::basic_string<CharT, Traits, Allocator>& path) {
			return this->font_path(path.c_str());
		}
		const std::string& controler::font_path() const { return this->font_path_m; }

		// 現在のライブラリで保持しているフォントからフォントサイズを指定してハンドルの取得
		inline std::shared_ptr<ttf> controler::font(size_t n) {
			auto itr = this->font_table_m.find(n);
			// そのサイズのフォントハンドルが存在しなければ新しく構築
			if (itr == this->font_table_m.end()) {
				return this->font_table_m.try_emplace(n, new ttf(this->font_path_m, n)).first->second;
			}
			// 作成時のフォントパスが異なるならば新しくフォントハンドルを生成し上書き
			if (itr->second->font_path() != this->font_path_m) {
				itr->second.reset(new ttf(this->font_path_m, n));
			}
			return itr->second;
		}
		// フォントハンドルを保持している連想配列への参照を取得
		inline typename controler::font_tabel_t& controler::font() { return this->font_table_m; }
		inline const typename controler::font_tabel_t& controler::font() const { return this->font_table_m; }
		// フォントハンドルの整理
		inline controler& controler::font_organizing() {
			// フォントハンドルの所有権が唯一であるリソースを除去
			for (auto itr = this->font_table_m.begin(); itr != this->font_table_m.end();) {
				if (itr->second.use_count() == 1) itr = this->font_table_m.erase(itr);
				else ++itr;
			}
			return *this;
		}
#endif
	}
}

#ifdef IMATHLIB_F_MEDIA_UI
#include "IMathLib/media/ui/interface.hpp"
#endif

namespace iml {
	namespace ml {

		// ウィンドウイベントの処理
		inline void window_event(const SDL_WindowEvent& ev) {
			mlwindow& wnd = medialib().window();

			if (ev.windowID != wnd.id()) return;

			switch (ev.event) {
				// リサイズ
			case SDL_WINDOWEVENT_RESIZED:
				screen_rect::base_width_m = ev.data1;
				screen_rect::base_height_m = ev.data2;
				if (screen_rect::frame_buffer_object_id() == 0) {
					screen_rect::width_m = screen_rect::base_width_m;
					screen_rect::height_m = screen_rect::base_height_m;
				}
				break;
#ifdef IMATHLIB_F_MEDIA_UI
				// フォーカスを失った
			case SDL_WINDOWEVENT_FOCUS_LOST:
				// GUIのアクティブを解除する
				if (medialib().gui_list_m.size() > 0) if (medialib().gui_list_m.front()->active_m) medialib().gui_list_m.front()->inactive_event();
				break;
#endif
				// ウィンドウを閉じるイベント
			case SDL_WINDOWEVENT_CLOSE:
				wnd.close_event();
				break;
			}
		}

		// イベントループを処理する関数
		inline bool event() {
			SDL_Event ev;

			//FPS安定化処理
			medialib().fps_m.wait().update();
			// 入力の更新
			mouse::update();
			keyboard::update();

			// キューに積まれたイベントを消化する
			while (SDL_PollEvent(&ev)) {
				switch (ev.type) {
					// ウィンドウイベント
				case SDL_WINDOWEVENT:
					// ウィンドウに対して定義されたイベントへ飛ぶ
					window_event(ev.window);
					break;
					// マウスボタンダウンイベント
				case SDL_MOUSEBUTTONDOWN:
					switch (ev.button.button) {
					case SDL_BUTTON_LEFT:
						mouse::l.key_down();
						break;
					case SDL_BUTTON_RIGHT:
						mouse::r.key_down();
						break;
					case SDL_BUTTON_MIDDLE:
						mouse::m.key_down();
						break;
					}
					break;
					// マウスボタンアップイベント
				case SDL_MOUSEBUTTONUP:
					switch (ev.button.button) {
					case SDL_BUTTON_LEFT:
						mouse::l.key_up();
						break;
					case SDL_BUTTON_RIGHT:
						mouse::r.key_up();
						break;
					case SDL_BUTTON_MIDDLE:
						mouse::m.key_up();
						break;
					}
					break;
					// マウスホイールイベント
				case SDL_MOUSEWHEEL:
					mouse::wheel_h_m = ev.wheel.x;
					mouse::wheel_v_m = ev.wheel.y;
					break;
					// マウスが動いたときのイベント
				case SDL_MOUSEMOTION:
					// タッチデバイスは無視
					if (ev.motion.which == SDL_TOUCH_MOUSEID) break;
					mouse::pos_m[0] = ev.motion.x;
					mouse::pos_m[1] = ev.motion.y;
					mouse::mov_m[0] = ev.motion.xrel;
					mouse::mov_m[1] = ev.motion.yrel;
					break;
					// キーボードダウンイベント
				case SDL_KEYDOWN:
					keyboard::key_down_ev(ev.key);
					break;
					// キーボードアップイベント
				case SDL_KEYUP:
					keyboard::key_up_ev(ev.key);
					break;
					// テキスト入力イベント
				case SDL_TEXTINPUT:
					keyboard::input_text_m += ev.text.text;
					break;
					// テキスト編集イベント
				case SDL_TEXTEDITING:
					keyboard::editting_text_m = ev.edit.text;
					keyboard::editting_cursor_m = ev.edit.start;
					break;
					// 終了イベント
				case SDL_QUIT:
					return false;
				}
			}


#ifdef IMATHLIB_F_MEDIA_UI
			// 若い順からGUIに対するアクティブの切り替え
			if (mouse::l.trigger()) {
				if (medialib().gui_list_m.size() > 0) {
					// 現在アクティブなGUIがアクティブが解除されるかの判定
					if (!medialib().gui_list_m.front()->in_rect(mouse::pos()) && medialib().gui_list_m.front()->active_m) {
						medialib().gui_list_m.front()->inactive_event();
					}
					// GUIがアクティブになったかを処理
					for (auto itr = medialib().gui_list_m.begin(); itr != medialib().gui_list_m.end(); ++itr) {
						if ((*itr)->in_rect(mouse::pos())) {
							// アクティブでないならアクティブにして一番若い番号にする
							if (!(*itr)->active_m) {
								(*itr)->active_event(mouse::pos());
								medialib().gui_list_m.push_front(*itr);
								medialib().gui_list_m.erase(itr);
							}
							break;
						}
					}
				}
			}
			// 現在アクティブなGUIに対してイベントを発行
			if (medialib().gui_list_m.size() > 0) {
				if (medialib().gui_list_m.front()->active_m) medialib().gui_list_m.front()->event();
			}
#endif


			return true;
		}
	}
}




#endif