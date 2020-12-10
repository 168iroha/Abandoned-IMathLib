#ifndef IMATHLIB_INTERFACE_UI_INTERFACE_HPP
#define IMATHLIB_INTERFACE_UI_INTERFACE_HPP
#define IMATHLIB_F_MEDIA_UI
#ifdef IMATHLIB_F_MEDIA_UI

#include "IMathLib/media/common.hpp"
#include "IMathLib/media/graphic.hpp"
#include "IMathLib/media/ttf.hpp"
#include "IMathLib/media/camera.hpp"
#include "IMathLib/math/liner_algebra.hpp"


// あくまでも最低限のGUIの機能を提供し，リソースの管理等はユーザのデザインに任せる仕様をとる
namespace iml {
	namespace ml {
		namespace ui {

			namespace col {

			}


			// GUIの基底
			class gui_base {
				friend class line_gui;

				gui_base* parent_m;			// 親GUI(ここでの親子関係とはGUI内に描画されるものであり，GUIから生成される関係ではない)
			protected:
				bool			auto_resize_m;		// サイズをオートで決定するかのフラグ
				vector2<int_t>	pos_m;				// 現在の(相対)位置
				size_t			width_m;			// 現在の幅(padding分は含まない)
				size_t			height_m;			// 現在の高さ(padding分は含まない)
				bool			active_m;			// 現在アクティブかのフラグ
				rect<int_t>		padding_m;			// padding(負の値ととるときの動作は未定義)
				rect<int_t>		margin_m;			// margin(負の値ととるときの動作は未定義)
			public:
				gui_base(gui_base* parent = nullptr) : auto_resize_m(false), width_m(0), height_m(0), active_m(false), parent_m(parent) {
					// 親を持たないならばGUIのインデックスに追加
					if (this->parent_m == nullptr) medialib().gui_list_m.push_back(this);
				}
				virtual ~gui_base() {
					// GUIのインデックスから除去
					if (this->parent_m == nullptr) medialib().gui_list_m.remove(this);
				}

				// 親GUIのハンドルのポインタの取得
				gui_base* parent_gui() { return this->parent_m; }
				const gui_base* parent_gui() const { return this->parent_m; }

				// posがGUIの領域に入っているかの判定(pos:親GUIからの相対位置)
				virtual bool in_rect(const vector2<int_t>& pos) = 0;
				// アクティブ切り替え時のイベント(pos:イベントが発生した座標で親GUIからの相対位置)
				virtual void inactive_event() = 0;
				virtual void active_event(const vector2<int_t>& pos) = 0;

				// イベントの発火(p:親GUIの絶対座標)
				virtual void event(const vector2<int_t>& p = vector2<int_t>()) = 0;

				// リサイズイベント(サイズの再計算)
				virtual void resize(size_t width, size_t height) = 0;
				// 内部領域のリサイズイベント
				virtual void inner_resize() = 0;

				// 親GUIの描画開始位置の絶対座標pで描画(p + offset + this->pos_mがこのGUIの絶対座標となる)
				// offset:主にpaddingやスクロールバーによる補正を行う
				// (p, (width, height))の矩形領域が有効描画領域
				virtual void draw(const vector2<int_t>& p = vector2<int_t>(), const vector2<int_t>& offset = vector2<int_t>(), size_t width = 16384, size_t height = 16384) = 0;

				// アクティブであるかの判定
				bool is_active() const { return this->active_m; }
				// オートでリサイズするかの判定
				bool is_resizable() const { return this->auto_resize_m; }

				// 幅と高さの取得
				size_t width() const { return this->width_m + this->padding_m.left + this->padding_m.right; }
				size_t height() const { return this->height_m + this->padding_m.top + this->padding_m.bottom; }

				// 親からの相対位置の取得
				const vector2<int_t>& pos() const { return this->pos_m; }

				// paddingとmargin
				rect<int_t>& padding() { return this->padding_m; }
				const rect<int_t>& padding() const { return this->padding_m; }
				rect<int_t>& margin() { return this->margin_m; }
				const rect<int_t>& margin() const { return this->margin_m; }

				friend bool iml::ml::event();
				friend void iml::ml::window_event(const SDL_WindowEvent&);
			};

			// 一列分のGUIリスト
			class line_gui {
				std::list<gui_base*>	gui_list_m;
				size_t					width_m;
				size_t					height_m;
			public:
				line_gui() : width_m(0), height_m(0) {}

				// GUIの追加
				template <class Gui, class = std::enable_if_t<std::is_base_of_v<gui_base, Gui>>>
				line_gui& add(Gui& gui, int_t offset_w, int_t offset_h) {
					// 行の左上からの相対位置の設定
					gui.pos_m[0] = offset_w + this->width_m + gui.margin().left;
					gui.pos_m[1] = offset_h + gui.margin().top;
					// 高さと幅を更新
					this->width_m += gui.margin().left + gui.width() + gui.margin().right;
					this->height_m = (max)(this->height_m, gui.margin().top + gui.height() + gui.margin().bottom);
					this->gui_list_m.push_back(std::addressof(gui));
					return *this;
				}

				size_t width() const { return this->width_m; }
				size_t height() const { return this->height_m; }

				using iterator = typename std::list<gui_base*>::iterator;
				using const_iterator = typename std::list<gui_base*>::const_iterator;

				// 一行分のGUIリストのイテレータの取得
				iterator begin() { return this->gui_list_m.begin(); }
				const_iterator begin() const { return this->gui_list_m.begin(); }
				iterator end() { return this->gui_list_m.end(); }
				const_iterator end() const { return this->gui_list_m.end(); }
			};

			// GUIウィンドウ
			class gui_window : public gui_base {
				bool			moveable_m;			// 移動可能かのフラグ
				bool			system_menu_m;		// システムメニューを表示するかのフラグ
				std::string		title_m;			// システムメニューに表示されるタイトル
				bool			exit_button_m;		// システムメニューにexitボタンを表示するかのフラグ
				bool			visible_m;			// 表示かのフラグ

				bool			move_m;				// ウィンドウの移動フラグ(moveable_mが有効のときのみ動作)
				vector2<int_t>	mouse_pos_m;		// ウィンドウ移動開始時のウィンドウ左上からの相対位置

				std::list<line_gui>		child_m;	// 子GUI
				gui_base* active_child_m;			// 現在アクティブな子GUI

				static constexpr size_t system_menu_height = 25;			// システムメニューの高さ

				// 子GUIのためのアクティブイベントの実行(pos:イベントが発生した座標で親GUIからの相対位置)
				void active_event_impl(const vector2<int_t>& pos) {
					// 相対位置を計算するためのオフセット(現在のGUIのイベントの発生した座標の計算)
					vector2<int_t> offset = (gui_base::parent_gui() != nullptr) ?
						vector2<int_t>(pos[0] - gui_base::parent_gui()->padding().left - gui_base::pos_m[0], pos[1] - gui_base::parent_gui()->padding().top - gui_base::pos_m[1])
						: vector2<int_t>(pos[0] - gui_base::pos_m[0], pos[1] - gui_base::pos_m[1]);
					// システムメニュー分のオフセットを与える
					if (this->system_menu_m) offset[1] += this->system_menu_height;

					// 子GUIでactive_eventが生じたかのフラグ
					gui_base* child_active_ev = nullptr;
					// 子GUIがアクティブになったかを処理
					for (auto& row : this->child_m) {
						for (auto& element : row) {
							if (element->in_rect(offset + element->pos())) {
								child_active_ev = element;
								// アクティブイベントが発生したならアクティブイベントのフラグを立ててアクティブイベントの実行
								if (!element->is_active()) element->active_event(offset);
								break;
							}
						}
					}
					// 現在アクティブな子GUIがアクティブが解除されるかの判定
					if ((this->active_child_m != nullptr) && (this->active_child_m != child_active_ev)) this->active_child_m->inactive_event();
					this->active_child_m = child_active_ev;

					// 子GUIがアクティブでなければウィンドウの移動のための位置のキャプチャをする
					if (this->moveable_m && (child_active_ev == nullptr)) {
						this->move_m = true;
						this->mouse_pos_m = mouse::pos() - gui_base::pos_m;
					}
				}
			public:
				// 1行目の構築準備をする
				gui_window() : child_m(1), active_child_m(nullptr), moveable_m(true), system_menu_m(true), title_m(u8"gui window"), exit_button_m(true), visible_m(false), move_m(false) {
					// サイズはオートで決定
					gui_base::auto_resize_m = true;
					// システムメニュー分の領域はpaddingとして扱わない
				}
				~gui_window() {}

				// posがGUIの領域に入っているかの判定(pos:親GUIからの相対位置)
				bool in_rect(const vector2<int_t>& pos) {
					if (this->system_menu_m) return (gui_base::pos_m[0] <= pos[0]) && (gui_base::pos_m[0] + gui_base::width() >= pos[0])
						&& (gui_base::pos_m[1] <= pos[1]) && (gui_base::pos_m[1] + gui_base::height() + gui_window::system_menu_height >= pos[1]);
					else return (gui_base::pos_m[0] <= pos[0]) && (gui_base::pos_m[0] + gui_base::width() >= pos[0])
						&& (gui_base::pos_m[1] <= pos[1]) && (gui_base::pos_m[1] + gui_base::height() >= pos[1]);
				}
				// アクティブ切り替え時のイベント(pos:イベントが発生した座標で親GUIからの相対位置)
				void inactive_event() {
					gui_base::active_m = false;
				}
				void active_event(const vector2<int_t>& pos) {
					gui_base::active_m = true;
					// 子GUIのアクティブイベントの実行
					this->active_event_impl(pos);
				}
				// イベントの発火(p:親GUIの絶対座標)
				void event(const vector2<int_t>& p = vector2<int_t>()) {
					// 子GUIのアクティブイベントもしくは現在の位置のキャプチャ
					if (mouse::l.trigger()) this->active_event_impl(mouse::pos() - p);

					if (this->moveable_m) {
						// マウスプッシュかつ子GUIがアクティブでなければ移動
						if (mouse::l.push() && (this->active_child_m == nullptr)) gui_base::pos_m = mouse::pos() - this->mouse_pos_m;
						// マウスリリースされれば
						if (mouse::l.release()) this->move_m = false;

						if (this->pos_m[0] < 0) this->pos_m[0] = 0;
						if (this->pos_m[1] < 0) this->pos_m[1] = 0;
						if (this->pos_m[0] + this->width_m > medialib().window().width()) this->pos_m[0] = medialib().window().width() - this->width_m;
						if (this->pos_m[1] + this->height_m > medialib().window().height()) this->pos_m[1] = medialib().window().height() - this->height_m;
					}
					// 子GUIのイベントの実行
					if (this->active_child_m != nullptr) {
						this->active_child_m->event(vector2<int_t>(p[0] + gui_base::padding().left, p[1] + gui_base::padding().top));
					}
				}

				// リサイズイベント(サイズの再計算)
				void resize(size_t width, size_t height) {
					// サイズはオートでのみ変更可能であるため何もしない
				}
				// 内部領域のリサイズイベント
				void inner_resize() {
					gui_base::width_m = gui_base::height_m = 0;
					// GUIの内部領域の幅と高さの取得
					for (auto& row : this->child_m) {
						gui_base::width_m = (max)(gui_base::width_m, row.width());
						gui_base::height_m += row.height();
					}
					// 親は存在しないためresizeの伝搬はしない
				}

				// 改行の挿入
				gui_window& ln() {
					if (this->child_m.size() != 0) this->child_m.emplace_back();
					// 1行目の構築
					else this->child_m.resize(1);
					return *this;
				}
				// GUIの追加
				template <class Gui, class = std::enable_if_t<std::is_base_of_v<gui_base, Gui>>>
				gui_window& add(Gui& gui) {
					size_t temp = this->child_m.back().height();
					// 位置はこれまでの行の分の高さをオフセットで与える
					this->child_m.back().add(gui, 0, gui_base::height_m - this->child_m.back().height());
					// 幅と高さの再構築
					gui_base::width_m = (max)(gui_base::width_m, this->child_m.back().width());
					gui_base::height_m += this->child_m.back().height() - temp;
					return *this;
				}
				template <class Gui, class = std::enable_if_t<std::is_base_of_v<gui_base, Gui>>>
				gui_window& addln(Gui& gui) {
					return this->add(gui).ln();
				}


				// GUIの描画
				void draw(const vector2<int_t>& p = vector2<int_t>(), const vector2<int_t>& offset = vector2<int_t>(), size_t width = 16384, size_t height = 16384) {
					GLint viewport[4];
					glGetIntegerv(GL_VIEWPORT, viewport);
					int_t w = gui_base::width();
					int_t h = gui_base::height();
					if (this->system_menu_m) {
						vector2<int_t> new_pos = p + gui_base::pos_m + offset;
						// ビューポートをGUIの領域から制限(マスクでは流石に重そうであるためなし)
						glMatrixMode(GL_PROJECTION);
						glPushMatrix();
						glLoadTransposeMatrix(iml::ortho_projection_matrix<float>(0, w, h + gui_window::system_menu_height, 0, -1, 1));
						glViewport(new_pos[0], new_pos[1], w, h + gui_window::system_menu_height);

						// system_menu_height px分のシステムメニューの描画
						auto rr = rectangle(iml::vector2<float>(), iml::vector2<float>(w, gui_window::system_menu_height));
						// アクティブ時と非アクティブ時で色を分ける
						if (this->active_m) rr.color(color_rgb{ 50, 185, 85 });
						else rr.color(color_rgb{ 25, 75, 40 });
						rr.draw();
						// ボーダーカラーは明るめ
						rr.border().color(color_rgb{ 180, 235, 200 }).draw();
						// システムメニューの部分を除く
						new_pos[1] += gui_window::system_menu_height;
						glLoadTransposeMatrix(iml::ortho_projection_matrix<float>(0, w, h, 0, -1, 1));
						glViewport(new_pos[0], new_pos[1], w, h);
						rr = rectangle(iml::vector2<float>(), iml::vector2<float>(w, h));
						// 色は非アクティブ時のシステムメニューに同じ
						rr.color(color_rgb{ 25, 75, 40 }).draw();
						rr.border().color(color_rgb{ 180, 235, 200 }).draw();

						// 子GUIの描画(描画領域判定はする必要なし)
						vector2<int_t> new_offset = vector2<int_t>(gui_base::padding().left, gui_base::padding().top);
						for (auto& row : this->child_m) {
							for (auto& element : row) element->draw(new_pos, new_offset, w, h);
						}
					}
					else {
						vector2<int_t> new_pos = p + gui_base::pos_m + offset;
						// ビューポートをGUIの領域から制限
						glMatrixMode(GL_PROJECTION);
						glPushMatrix();
						glLoadTransposeMatrix(iml::ortho_projection_matrix<float>(0, w, h, 0, -1, 1));
						glViewport(new_pos[0], new_pos[1], w, h);

						auto rr = rectangle(iml::vector2<float>(), iml::vector2<float>(w, h));
						rr.color(color_rgb{ 25, 75, 40 }).draw();
						rr.border().color(color_rgb{ 180, 235, 200 }).draw();

						// 子GUIの描画(描画領域判定はする必要なし)
						vector2<int_t> new_offset = vector2<int_t>(gui_base::padding().left, gui_base::padding().top);
						for (auto& row : this->child_m) {
							for (auto& element : row) element->draw(new_pos, new_offset, w, h);
						}
					}
					glPopMatrix();
					::glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
				}
			};


			// 矩形領域(固定領域GUIの生成に利用)
			class gui_rectangle : public gui_base {
				size_t			inner_width_m;		// 現在の内部幅(gui_rectangle自体のpadding分は除く)
				size_t			inner_height_m;		// 現在の内部高さ(gui_rectangle自体のpadding分は除く)
				size_t			scroll_w_m;			// 現在の水平スクロール量
				size_t			scroll_h_m;			// 現在の垂直スクロール量

				std::list<line_gui>		child_m;	// 子GUI
				gui_base* active_child_m;			// 現在アクティブな子GUI

				static constexpr size_t scrollbar_width = 16;				// スクロールバーの幅
				static constexpr size_t scrollbar_height = 16;				// スクロールバーの高さ


				// 子GUIのためのアクティブイベントの実行(pos:イベントが発生した座標で親GUIからの相対位置)
				void active_event_impl(const vector2<int_t>& pos) {
					// 相対位置を計算するためのオフセット
					vector2<int_t> offset = (gui_base::parent_gui() != nullptr) ?
						vector2<int_t>(pos[0] - gui_base::parent_gui()->padding().left - gui_base::pos_m[0] + int_t(this->scroll_w_m), pos[1] - gui_base::parent_gui()->padding().top - gui_base::pos_m[1] + int_t(this->scroll_h_m))
						: vector2<int_t>(pos[0] - gui_base::pos_m[0] + int_t(this->scroll_w_m), pos[1] - gui_base::pos_m[1] + int_t(this->scroll_h_m));

					// 子GUIでactive_eventが生じたかのフラグ
					gui_base* child_active_ev = nullptr;
					// 子GUIがアクティブになったかを処理
					for (auto& row : this->child_m) {
						for (auto& element : row) {
							if (element->in_rect(offset + element->pos())) {
								child_active_ev = element;
								// アクティブイベントが発生したならアクティブイベントのフラグを立ててアクティブイベントの実行
								if (!element->is_active()) element->active_event(offset);
								break;
							}
						}
					}
					// 現在アクティブな子GUIがアクティブが解除されるかの判定
					if ((this->active_child_m != nullptr) && (this->active_child_m != child_active_ev)) this->active_child_m->inactive_event();
					this->active_child_m = child_active_ev;
				}
			public:
				gui_rectangle(gui_base* parent = nullptr) : gui_base(parent), child_m(1), active_child_m(nullptr), inner_width_m(0), inner_height_m(0), scroll_w_m(0), scroll_h_m(0) {
					// サイズはデフォルトで固定
					gui_base::auto_resize_m = false;
					// paddingおよびmarginは0
				}
				~gui_rectangle() {}

				// posがGUIの領域に入っているかの判定(pos:親GUIからの相対位置)
				bool in_rect(const vector2<int_t>& pos) {
					return (gui_base::pos_m[0] <= pos[0]) && (gui_base::pos_m[0] + gui_base::width() >= pos[0])
						&& (gui_base::pos_m[1] <= pos[1]) && (gui_base::pos_m[1] + gui_base::height() >= pos[1]);
				}
				// アクティブ切り替え時のイベント(pos:イベントが発生した座標で親GUIからの相対位置)
				void inactive_event() {
					gui_base::active_m = false;
				}
				void active_event(const vector2<int_t>& pos) {
					gui_base::active_m = true;
					// 子GUIのアクティブイベントの実行
					this->active_event_impl(pos);
				}
				// イベントの発火(p:親GUIの絶対座標)
				void event(const vector2<int_t>& p = vector2<int_t>()) {
					// 子GUIのアクティブイベントの実行
					if (mouse::l.trigger()) this->active_event_impl(mouse::pos() - p);
					// 子GUIのイベントの実行
					if (this->active_child_m != nullptr) {
						this->active_child_m->event(vector2<int_t>(p[0] + gui_base::padding().left - int_t(this->scroll_w_m), p[1] + gui_base::padding().top - int_t(this->scroll_h_m)));
					}
				}

				// スクロールバーが存在する
				bool is_exist_scroll_w() { return gui_base::width_m < this->inner_width_m; }
				bool is_exist_scroll_h() { return gui_base::height_m < this->inner_height_m; }

				// サイズを可変にする
				gui_rectangle& resizable() {
					gui_base::auto_resize_m = true;
					return *this;
				}
				// サイズを固定にする
				gui_rectangle& unresizable() {
					gui_base::auto_resize_m = false;
					return *this;
				}

				// サイズ変更イベント(サイズの再計算)
				void resize(size_t width, size_t height) {
					// サイズが可変でないとき有効
					if (!gui_base::auto_resize_m) {
						// paddingの分を引く
						if (gui_base::padding().left + gui_base::padding().right <= width)
							gui_base::width_m = width - (gui_base::padding().left + gui_base::padding().right);
						else gui_base::width_m = gui_base::padding().left + gui_base::padding().right;
						if (gui_base::padding().top + gui_base::padding().bottom <= height)
							gui_base::height_m = height - (gui_base::padding().top + gui_base::padding().bottom);
						else gui_base::height_m = gui_base::padding().top + gui_base::padding().bottom;
						// スクロールの位置の補正
						if (this->is_exist_scroll_w())
							this->scroll_w_m = (min)(this->inner_width_m - (gui_base::width_m + gui_rectangle::scrollbar_width), this->scroll_w_m);
						if (this->is_exist_scroll_h())
							this->scroll_h_m = (min)(this->inner_height_m - (gui_base::height_m + gui_rectangle::scrollbar_height), this->scroll_h_m);
						// 親へとresizeを伝搬
						if (gui_base::parent_gui() != nullptr) gui_base::parent_gui()->inner_resize();
					}
				}
				// 内部領域のリサイズイベント
				void inner_resize() {
					// サイズが可変であるとき
					if (gui_base::auto_resize_m) {
						// gui_windowの場合と同じ
						gui_base::width_m = gui_base::height_m = 0;
						// GUIの内部領域の幅と高さの取得
						for (auto& row : this->child_m) {
							gui_base::width_m = (max)(gui_base::width_m, row.width());
							gui_base::height_m += row.height();
						}
						// 親にresizeの伝搬
						if (gui_base::parent_gui() != nullptr) gui_base::parent_gui()->inner_resize();
					}
					else {
						// 幅と高さは固定であるためinner_width_mとinner_height_mを再計算する
						this->inner_width_m = this->inner_height_m = 0;
						for (auto& row : this->child_m) {
							this->inner_width_m = (max)(this->inner_width_m, row.width());
							this->inner_height_m += row.height();
						}
						// スクロールの位置の補正
						if (this->is_exist_scroll_w())
							this->scroll_w_m = (min)(this->inner_width_m - (gui_base::width_m + gui_rectangle::scrollbar_width), this->scroll_w_m);
						if (this->is_exist_scroll_h())
							this->scroll_h_m = (min)(this->inner_height_m - (gui_base::height_m + gui_rectangle::scrollbar_height), this->scroll_h_m);
					}
				}


				// 改行の挿入
				gui_rectangle& ln() {
					if (this->child_m.size() != 0) this->child_m.emplace_back();
					// 1行目の構築
					else this->child_m.resize(1);
					return *this;
				}
				// GUIの追加
				template <class Gui, class = std::enable_if_t<std::is_base_of_v<gui_base, Gui>>>
				gui_rectangle& add(Gui& gui) {
					size_t temp = this->child_m.back().height();
					// 位置はこれまでの行の分の高さをオフセットで与える
					this->child_m.back().add(gui, 0, gui_base::height_m - this->child_m.back().height());
					// 幅と高さの再構築
					gui_base::width_m = (max)(gui_base::width_m, this->child_m.back().width());
					gui_base::height_m += this->child_m.back().height() - temp;
					return *this;
				}
				template <class Gui, class = std::enable_if_t<std::is_base_of_v<gui_base, Gui>>>
				gui_rectangle& addln(Gui& gui) {
					return this->add(gui).ln();
				}


				// GUIの描画
				void draw(const vector2<int_t>& p = vector2<int_t>(), const vector2<int_t>& offset = vector2<int_t>(), size_t width = 16384, size_t height = 16384) {
					GLint viewport[4];
					glGetIntegerv(GL_VIEWPORT, viewport);
					int_t w = gui_base::width();
					int_t h = gui_base::height();

					// ビューポートをGUIの領域から制限
					glMatrixMode(GL_PROJECTION);
					glPushMatrix();

					// サイズが可変ならそのまま描画する
					if (gui_base::auto_resize_m) {
						// 左上の座標をoffset分の補正を加えて描画(ビューポート)領域の制限
						if (gui_base::parent_gui() != nullptr) {
							// (p, (width, height))の矩形領域に収まるようにする
							vector2<int_t> new_pos((max)(p[0], p[0] + gui_base::pos_m[0] + offset[0]), (max)(p[1], p[1] + gui_base::pos_m[1] + offset[1]));
							vector2<int_t> new_pos2((min)(p[0] + width, p[0] + gui_base::pos_m[0] + offset[0] + w), (min)(p[1] + height, p[1] + gui_base::pos_m[1] + offset[1] + h));
							w = new_pos[0] - new_pos2[0];
							h = new_pos[1] - new_pos2[1];
							glLoadTransposeMatrix(iml::ortho_projection_matrix<float>(0, w, h, 0, -1, 1));
							glViewport(new_pos[0], new_pos[1], w, h);

							// 子GUIの描画
							// new_posとpが一致するときは実際の描画開始位置はpよりも左上となるためoffsetを伝搬する
							vector2<int_t> new_offset = (new_pos == p) ? vector2<int_t>(gui_base::padding().left + offset[0], gui_base::padding().top + offset[1]) :  vector2<int_t>(gui_base::padding().left, gui_base::padding().top);
							for (auto& row : this->child_m) {
								for (auto& element : row) {
									// 親領域(p, (width, height))と子領域(new_pos + new_offset + element->pos(), (element->width(), element->height()))が重なるか判定
									// それぞれの矩形領域の中心座標の差
									vector2<int_t> temp(abs((p[0] + int_t(width) / 2) - (new_offset[0] + element->pos()[0] + element->width() / 2))
										, abs((p[1] + int_t(height) / 2) - (new_offset[1] + element->pos()[1] + element->height() / 2)));
									if ((temp[0] < (width + element->width()) / 2) && (temp[1] < (height + element->height()) / 2))
										element->draw(new_pos, new_offset, w, h);
								}
							}
						}
						else {
							vector2<int_t> new_pos = p + gui_base::pos_m + offset;
							// 描画領域を制限せずにそのまま描画
							glLoadTransposeMatrix(iml::ortho_projection_matrix<float>(0, w, h, 0, -1, 1));
							glViewport(new_pos[0], new_pos[1], w, h);

							// 子GUIの描画
							vector2<int_t> new_offset = vector2<int_t>(gui_base::padding().left, gui_base::padding().top);
							for (auto& row : this->child_m) {
								for (auto& element : row) element->draw(new_pos, new_offset, w, h);
							}
						}
					}
					else {

						// スクロールバー分の描画領域の調整は後回しにする

						// 左上の座標をoffset分の補正を加えて描画(ビューポート)領域の制限
						if (gui_base::parent_gui() != nullptr) {
							// (p, (width, height))の矩形領域に収まるようにする
							vector2<int_t> new_pos((max)(p[0], p[0] + gui_base::pos_m[0] + offset[0]), (max)(p[1], p[1] + gui_base::pos_m[1] + offset[1]));
							vector2<int_t> new_pos2((min)(p[0] + width, p[0] + gui_base::pos_m[0] + offset[0] + w), (min)(p[1] + height, p[1] + gui_base::pos_m[1] + offset[1] + h));
							w = new_pos[0] - new_pos2[0];
							h = new_pos[1] - new_pos2[1];
							glLoadTransposeMatrix(iml::ortho_projection_matrix<float>(0, w, h, 0, -1, 1));
							glViewport(new_pos[0], new_pos[1], w, h);

							// 子GUIの描画
							// new_posとpが一致するときは実際の描画開始位置はpよりも左上となるためoffsetを伝搬する
							vector2<int_t> new_offset = (new_pos == p) ? vector2<int_t>(gui_base::padding().left + offset[0], gui_base::padding().top + offset[1]) : vector2<int_t>(gui_base::padding().left, gui_base::padding().top);
							for (auto& row : this->child_m) {
								for (auto& element : row) {
									// 親領域(p, (width, height))と子領域(new_pos + new_offset + element->pos(), (element->width(), element->height()))が重なるか判定
									// それぞれの矩形領域の中心座標の差
									vector2<int_t> temp(abs((p[0] + int_t(width) / 2) - (new_offset[0] + element->pos()[0] + element->width() / 2))
										, abs((p[1] + int_t(height) / 2) - (new_offset[1] + element->pos()[1] + element->height() / 2)));
									if ((temp[0] < (width + element->width()) / 2) && (temp[1] < (height + element->height()) / 2))
										element->draw(new_pos, new_offset, w, h);
								}
							}
						}
						else {
							vector2<int_t> new_pos = p + gui_base::pos_m + offset;
							// 描画領域を制限せずにそのまま描画
							glLoadTransposeMatrix(iml::ortho_projection_matrix<float>(0, w, h, 0, -1, 1));
							glViewport(new_pos[0], new_pos[1], w, h);

							vector2<int_t> new_offset = vector2<int_t>(gui_base::padding().left - int_t(this->scroll_w_m), gui_base::padding().top - int_t(this->scroll_h_m));
							// GUIの位置に応じて描画可能であるかを判定する
							for (auto& row : this->child_m) {
								for (auto& element : row) {
									// 親領域(p, (width, height))と子領域(new_pos + new_offset + element->pos(), (element->width(), element->height()))が重なるか判定
									// それぞれの矩形領域の中心座標の差
									vector2<int_t> temp(abs((p[0] + int_t(width) / 2) - (new_offset[0] + element->pos()[0] + element->width() / 2))
										, abs((p[1] + int_t(height) / 2) - (new_offset[1] + element->pos()[1] + element->height() / 2)));
									if ((temp[0] < (width + element->width()) / 2) && (temp[1] < (height + element->height()) / 2))
										element->draw(new_pos, new_offset, w, h);
								}
							}
						}

						w = gui_base::width();
						h = gui_base::height();
						// スクロールバーが必要であれば描画
						if (this->is_exist_scroll_h()) {
							// 両方あるときは(gui_rectangle::scrollbar_width)だけの水平スクロールバーが短くなる
							if (this->is_exist_scroll_w()) {
								// 垂直スクロールバー
								auto rr = rectangle(iml::vector2<float>(w - gui_rectangle::scrollbar_width, 0), iml::vector2<float>(w, h));
								// ウィンドウは背景よりは少し明るくする(ボタンのマウスオン時と同じ色)
								rr.color(color_rgb{ 25, 130, 40 }).draw();
								// マーカーサイズは割合から算出(正規化してhを乗ずる)
								size_t marker_h_size = (h * h) / this->inner_height_m;
								// 実際のスクロールバーの位置(正規化して高さ(h-marker_h_size-2)を乗ずる(上下ともに1pxの隙間を空ける))
								size_t pos_scroll_h = ((h - marker_h_size - 2) * this->scroll_h_m) / this->inner_height_m;
								rr = rectangle(iml::vector2<float>(w - gui_rectangle::scrollbar_width + 1, pos_scroll_h), iml::vector2<float>(w - 1, pos_scroll_h + marker_h_size));
								// マーカーはさらに明るくする(マウスプッシュ時と同じ色)
								rr.color(color_rgb{ 155, 215, 160 }).draw();

								// 水平スクロールバー
								rr = rectangle(iml::vector2<float>(0, h - gui_rectangle::scrollbar_height), iml::vector2<float>(w - gui_rectangle::scrollbar_width, h));
								// ウィンドウは背景よりは少し明るくする(ボタンのマウスオン時と同じ色)
								rr.color(color_rgb{ 25, 130, 40 }).draw();
								// マーカーサイズは割合から算出(正規化してhを乗ずる)
								size_t marker_w_size = ((w - gui_rectangle::scrollbar_width) * w) / this->inner_width_m;
								// 実際のスクロールバーの位置(正規化して高さ(w-marker_w_size-gui_rectangle::scrollbar_width-2)を乗ずる(上下ともに1pxの隙間を空ける))
								size_t pos_scroll_w = ((w - marker_w_size - gui_rectangle::scrollbar_width - 2) * this->scroll_w_m) / this->inner_width_m;
								rr = rectangle(iml::vector2<float>(pos_scroll_w, h - gui_rectangle::scrollbar_height + 1), iml::vector2<float>(pos_scroll_w + marker_w_size, h - 1));
								// マーカーはさらに明るくする(マウスプッシュ時と同じ色)
								rr.color(color_rgb{ 155, 215, 160 }).draw();
							}
							else {
								// 垂直スクロールバー
								auto rr = rectangle(iml::vector2<float>(w - gui_rectangle::scrollbar_width, 0), iml::vector2<float>(w, h));
								// ウィンドウは背景よりは少し明るくする(ボタンのマウスオン時と同じ色)
								rr.color(color_rgb{ 25, 130, 40 }).draw();
								// マーカーサイズは割合から算出(正規化してhを乗ずる)
								size_t marker_h_size = (h * h) / this->inner_height_m;
								// 実際のスクロールバーの位置(正規化して高さ(h-marker_h_size-1)を乗ずる(上下ともに1pxの隙間を空ける))
								size_t pos_scroll_h = ((h - marker_h_size) * this->scroll_h_m) / this->inner_height_m;
								rr = rectangle(iml::vector2<float>(w - gui_rectangle::scrollbar_width + 1, pos_scroll_h), iml::vector2<float>(w, pos_scroll_h + marker_h_size));
								// マーカーはさらに明るくする(マウスプッシュ時と同じ色)
								rr.color(color_rgb{ 155, 215, 160 }).draw();
							}
						}
						else if (this->is_exist_scroll_w()) {
							// 水平スクロールバー
							auto rr = rectangle(iml::vector2<float>(0, h - gui_rectangle::scrollbar_height), iml::vector2<float>(w, h));
							// ウィンドウは背景よりは少し明るくする(ボタンのマウスオン時と同じ色)
							rr.color(color_rgb{ 25, 130, 40 }).draw();
							// マーカーサイズは割合から算出(正規化してhを乗ずる)
							size_t marker_w_size = (w * w) / this->inner_width_m;
							// 実際のスクロールバーの位置(正規化して高さ(w-marker_w_size-2)を乗ずる(上下ともに1pxの隙間を空ける))
							size_t pos_scroll_w = ((w - marker_w_size - 2) * this->scroll_w_m) / this->inner_width_m;
							rr = rectangle(iml::vector2<float>(pos_scroll_w, h - gui_rectangle::scrollbar_height + 1), iml::vector2<float>(pos_scroll_w + marker_w_size, h - 1));
							// マーカーはさらに明るくする(マウスプッシュ時と同じ色)
							rr.color(color_rgb{ 155, 215, 160 }).draw();
						}
					}

					glPopMatrix();
					::glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
				}
			};


			// GUIテキスト(テキスト入力はまだ)
			class gui_text : public gui_base {
				std::shared_ptr<ttf>	font_m;				// フォント
				string_texture			str_tex_m;			// 文字列テクスチャ
			public:
				gui_text(const std::shared_ptr<ttf>& font, gui_base* parent = nullptr) : gui_base(parent), font_m(font) {
					// サイズは可変
					gui_base::auto_resize_m = true;
					// paddingおよびmarginは0

					// フォント高さ分を初期の高さに加える
					gui_base::height_m = font_m->font_height();
				}
				~gui_text() {}


				// posがGUIの領域に入っているかの判定(pos:親GUIからの相対位置)
				bool in_rect(const vector2<int_t>& pos) {
					return (gui_base::pos_m[0] <= pos[0]) && (gui_base::pos_m[0] + gui_base::width() >= pos[0])
						&& (gui_base::pos_m[1] <= pos[1]) && (gui_base::pos_m[1] + gui_base::height() >= pos[1]);
				}
				// アクティブ切り替え時のイベント(pos:イベントが発生した座標で親GUIからの相対位置)
				void inactive_event() {
					gui_base::active_m = false;
				}
				void active_event(const vector2<int_t>& pos) {
					gui_base::active_m = true;
				}
				// イベントの発火(p:親GUIの絶対座標)
				void event(const vector2<int_t>& p = vector2<int_t>()) {

				}

				// リサイズイベント(サイズの再計算)
				void resize(size_t width, size_t height) {
					// サイズはオートでのみ変更可能であるため何もしない

					// 親にリサイズの伝搬
					if (gui_base::parent_gui() != nullptr) gui_base::parent_gui()->inner_resize();
				}
				// 内部領域のリサイズイベント
				void inner_resize() {
					// 内部領域は存在しない
				}

				// 改行の挿入
				gui_text& ln() {
					this->str_tex_m.ln();
					gui_base::height_m += font_m->font_height();
					return *this;
				}
				// 文字列の追加
				template <class String>
				gui_text& add(const String& str) {
					this->str_tex_m.add(*(this->font_m), str, color_rgb{255, 255, 255});
					gui_base::width_m = (max)(gui_base::width_m, this->str_tex_m.back().width());
					return *this;
				}
				template <class String>
				gui_text& addln(const String& str) {
					return this->add(str).ln();
				}


				// GUIの描画
				void draw(const vector2<int_t>& p = vector2<int_t>(), const vector2<int_t>& offset = vector2<int_t>(), size_t width = 16384, size_t height = 16384) {
					glColor(1.f, 1.f, 1.f);
					// 入力開始時の高さ
					int_t h = gui_base::pos_m[1] + offset[1];
					for (const auto& line : this->str_tex_m) {
						// 入力開始時の幅
						int_t w = gui_base::pos_m[0] + offset[0];
						// 高さが描画範囲内に存在しないなら描画しない
						if ((h + line.height() >= 0) && (height >= h)) {
							for (const auto& unit_tex : line) {
								draw_texture2(vertex2<double>(w, h, w + unit_tex.tex_handle().width(), h + line.height()), &unit_tex.tex_handle());
								w += unit_tex.tex_handle().width();
							}
						}
						h += line.height();

					}
				}
			};

			// GUIボタン
			class gui_button : public gui_base {
				std::shared_ptr<ttf>	font_m;				// フォント
				string_texture			str_tex_m;			// 文字列テクスチャ
			public:
				gui_button(const std::shared_ptr<ttf>& font, gui_base* parent = nullptr) : gui_base(parent), font_m(font) {
					// サイズは可変
					gui_base::auto_resize_m = true;
					// paddingの設定
					gui_base::padding_m.left = gui_base::padding_m.right = 20;
					gui_base::padding_m.top = gui_base::padding_m.bottom = 5;
					// フォント高さ分を初期の高さに加える
					gui_base::height_m = font_m->font_height();
				}
				~gui_button() {}


				// posがGUIの領域に入っているかの判定(pos:親GUIからの相対位置)
				bool in_rect(const vector2<int_t>& pos) {
					return (gui_base::pos_m[0] <= pos[0]) && (gui_base::pos_m[0] + gui_base::width() >= pos[0])
						&& (gui_base::pos_m[1] <= pos[1]) && (gui_base::pos_m[1] + gui_base::height() >= pos[1]);
				}
				// アクティブ切り替え時のイベント(pos:イベントが発生した座標で親GUIからの相対位置)
				void inactive_event() {
					gui_base::active_m = false;
				}
				void active_event(const vector2<int_t>& pos) {
					gui_base::active_m = true;
				}
				// イベントの発火(p:親GUIの絶対座標)
				void event(const vector2<int_t>& p = vector2<int_t>()) {

				}

				// リサイズイベント(サイズの再計算)
				void resize(size_t width, size_t height) {
					// サイズはオートでのみ変更可能であるため何もしない

					// 親にリサイズの伝搬
					if (gui_base::parent_gui() != nullptr) gui_base::parent_gui()->inner_resize();
				}
				// 内部領域のリサイズイベント
				void inner_resize() {
					// 内部領域は存在しない
				}


				// GUIの描画
				void draw(const vector2<int_t>& p = vector2<int_t>(), const vector2<int_t>& offset = vector2<int_t>(), size_t width = 16384, size_t height = 16384) {
					glColor(1.f, 1.f, 1.f);
					// 入力開始時の高さ
					int_t h = gui_base::pos_m[1] + offset[1];
					for (const auto& line : this->str_tex_m) {
						// 入力開始時の幅
						int_t w = gui_base::pos_m[0] + offset[0];
						// 高さが描画範囲内に存在しないなら描画しない
						if ((h + line.height() >= 0) && (height >= h)) {
							for (const auto& unit_tex : line) {
								draw_texture2(vertex2<double>(w, h, w + unit_tex.tex_handle().width(), h + line.height()), &unit_tex.tex_handle());
								w += unit_tex.tex_handle().width();
							}
						}
						h += line.height();

					}
				}
			};
		}
	}
}


#endif
#endif