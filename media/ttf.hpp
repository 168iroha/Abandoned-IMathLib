#ifndef IMATHLIB_H_MEDIA_TTF_HPP
#define IMATHLIB_H_MEDIA_TTF_HPP

#include "IMathLib/media/image/color.hpp"
#include "IMathLib/media/log.hpp"
#include "IMathLib/media/graphic.hpp"
#include "IMathLib/media/common/unicode.hpp"
#include <list>

#include <SDL_ttf.h>


namespace iml {
	namespace ml {

		// TrueTypeフォントクラス
		class ttf {
			friend class unit_string_texture;
			friend class string_texture;

			std::string		font_path_m;			// フォントパス
			TTF_Font*		font_ptr_m;
			size_t			point_size_m;			// フォントのポイントサイズ
			size_t			font_height_m;			// フォントの高さ
		public:
			// UTF-8
			ttf(const char* str, size_t size) : font_path_m(str), font_ptr_m(TTF_OpenFont(str, size)), point_size_m(size), font_height_m(0) {
				// インスタンスの存在確認をしてフォントハンドルが存在するかを確認する
				if (!this->font_ptr_m) {
					IMATHLIB_LOG(iml::ml::logger::error, str) << TTF_GetError();
					return;
				}
				this->font_height_m = TTF_FontHeight(this->font_ptr_m);
			}
			// UTF-16
			ttf(const char16_t* str, size_t size) : font_ptr_m(nullptr), point_size_m(size), font_height_m(0) {
				if (!convert_unicode(this->font_path_m, str)) {
					IMATHLIB_LOG(iml::ml::logger::error) << "Could not convert from UTF-16 to UTF-8.";
					return;
				}
				this->font_ptr_m = TTF_OpenFont(this->font_path_m.c_str(), size);
				// インスタンスの存在確認をしてフォントハンドルが存在するかを確認する
				if (!this->font_ptr_m) {
					IMATHLIB_LOG(iml::ml::logger::error, this->font_path_m.c_str()) << TTF_GetError();
					return;
				}
				this->font_height_m = TTF_FontHeight(this->font_ptr_m);
			}
			ttf(const wchar_t* str, size_t size) : font_ptr_m(nullptr), point_size_m(size), font_height_m(0) {
				if (!convert_unicode(this->font_path_m, str)) {
					IMATHLIB_LOG(iml::ml::logger::error) << "Could not convert from UTF-16 to UTF-8.";
					return;
				}
				this->font_ptr_m = TTF_OpenFont(this->font_path_m.c_str(), size);
				// インスタンスの存在確認をしてフォントハンドルが存在するかを確認する
				if (!this->font_ptr_m) {
					IMATHLIB_LOG(iml::ml::logger::error, this->font_path_m.c_str()) << TTF_GetError();
					return;
				}
				this->font_height_m = TTF_FontHeight(this->font_ptr_m);
			}
			template <class CharT, class Traits, class Allocator>
			ttf(const std::basic_string<CharT, Traits, Allocator>& str, size_t size) : ttf(str.c_str(), size) {}
			~ttf() {
				TTF_CloseFont(this->font_ptr_m);
			}

			// フォントパスの取得
			const std::string& font_path() const { return this->font_path_m; }

			// フォントのポイントサイズ
			size_t point_size() const { return this->point_size_m; }
			// フォント高さ
			size_t font_height() const { return this->font_height_m; }


			// 文字列の横幅を取得
			// UTF-16
			template <class Traits, class Allocator>
			friend size_t strlen(const ttf& font, const std::basic_string<char16_t, Traits, Allocator>& str) {
				int temp;
				TTF_SizeUNICODE(font.font_ptr_m, reinterpret_cast<const Uint16*>(str.c_str()), &temp, nullptr);
				return temp;
			}
			friend size_t strlen(const ttf& font, const char16_t* str) {
				int temp;
				TTF_SizeUNICODE(font.font_ptr_m, reinterpret_cast<const Uint16*>(str), &temp, nullptr);
				return temp;
			}
			template <class Traits, class Allocator>
			friend size_t strlen(const ttf& font, const std::basic_string<wchar_t, Traits, Allocator>& str) {
				int temp;
				TTF_SizeUNICODE(font.font_ptr_m, reinterpret_cast<const Uint16*>(str.c_str()), &temp, nullptr);
				return temp;
			}
			friend size_t strlen(const ttf& font, const wchar_t* str) {
				int temp;
				TTF_SizeUNICODE(font.font_ptr_m, reinterpret_cast<const Uint16*>(str), &temp, nullptr);
				return temp;
			}
			// UTF-8
			template <class Traits, class Allocator>
			friend size_t strlen(const ttf& font, const std::basic_string<char, Traits, Allocator>& str) {
				int temp;
				TTF_SizeUTF8(font.font_ptr_m, str.c_str(), &temp, nullptr);
				return temp;
			}
			friend size_t strlen(const ttf& font, const char* str) {
				int temp;
				TTF_SizeUTF8(font.font_ptr_m, str, &temp, nullptr);
				return temp;
			}
		};

		//一つあたりの文字列テクスチャ
		class unit_string_texture {
			texture			tex_m;
			std::string		str_m;				// テクスチャに記述されている文字列(UTF-8)
			int				style_m;			// テクスチャ構築時のスタイル

			//UTF-16
			SDL_Surface* string_texture_impl(const ttf& font, const char16_t* str, const color_rgb& color) {
				if (!convert_unicode(this->str_m, str)) {
					IMATHLIB_LOG(iml::ml::logger::error) << "Could not convert from UTF-16 to UTF-8.";
					return nullptr;
				}
				SDL_Surface* img = TTF_RenderUNICODE_Blended(font.font_ptr_m, reinterpret_cast<const Uint16*>(str), { color.r,color.g,color.b });
				if (!img) {
					IMATHLIB_LOG(iml::ml::logger::error, this->str_m.c_str()) << TTF_GetError();
					return nullptr;
				}
				return img;
			}
			SDL_Surface* string_texture_impl(const ttf& font, const char16_t* str, const color_rgb& color, const color_rgb& bg_color) {
				if (!convert_unicode(this->str_m, str)) {
					IMATHLIB_LOG(iml::ml::logger::error) << "Could not convert from UTF-16 to UTF-8.";
					return nullptr;
				}
				SDL_Surface* img = TTF_RenderUNICODE_Shaded(font.font_ptr_m, reinterpret_cast<const Uint16*>(str), { color.r,color.g,color.b }, { bg_color.r,bg_color.g,bg_color.b });
				if (!img) {
					IMATHLIB_LOG(iml::ml::logger::error, this->str_m.c_str()) << TTF_GetError();
					return nullptr;
				}
				return img;
			}
			SDL_Surface* string_texture_impl(const ttf& font, const wchar_t* str, const color_rgb& color) {
				if (!convert_unicode(this->str_m, str)) {
					IMATHLIB_LOG(iml::ml::logger::error) << "Could not convert from UTF-16 to UTF-8.";
					return nullptr;
				}
				SDL_Surface* img = TTF_RenderUNICODE_Blended(font.font_ptr_m, reinterpret_cast<const Uint16*>(str), { color.r,color.g,color.b });
				if (!img) {
					IMATHLIB_LOG(iml::ml::logger::error, this->str_m.c_str()) << TTF_GetError();
					return nullptr;
				}
				return img;
			}
			SDL_Surface* string_texture_impl(const ttf& font, const wchar_t* str, const color_rgb& color, const color_rgb& bg_color) {
				if (!convert_unicode(this->str_m, str)) {
					IMATHLIB_LOG(iml::ml::logger::error) << "Could not convert from UTF-16 to UTF-8.";
					return nullptr;
				}
				SDL_Surface* img = TTF_RenderUNICODE_Shaded(font.font_ptr_m, reinterpret_cast<const Uint16*>(str), { color.r,color.g,color.b }, { bg_color.r,bg_color.g,bg_color.b });
				if (!img) {
					IMATHLIB_LOG(iml::ml::logger::error, this->str_m.c_str()) << TTF_GetError();
					return nullptr;
				}
				return img;
			}
			//UTF-8
			SDL_Surface* string_texture_impl(const ttf& font, const char* str, const color_rgb&color) {
				SDL_Surface	*img;
				img = TTF_RenderUTF8_Blended(font.font_ptr_m, str, { color.r,color.g,color.b });
				if (!img) {
					IMATHLIB_LOG(iml::ml::logger::error, str) << TTF_GetError();
					return nullptr;
				}
				this->str_m = str;
				return img;
			}
			SDL_Surface* string_texture_impl(const ttf& font, const char* str, const color_rgb& color, const color_rgb& bg_color) {
				SDL_Surface* img;
				img = TTF_RenderUTF8_Shaded(font.font_ptr_m, str, { color.r,color.g,color.b }, { bg_color.r,bg_color.g,bg_color.b });
				if (!img) {
					IMATHLIB_LOG(iml::ml::logger::error, str) << TTF_GetError();
					return nullptr;
				}
				this->str_m = str;
				return img;
			}
		public:
			unit_string_texture() : style_m(TTF_STYLE_NORMAL) {}
			template <class CharT, class Traits, class Allocator>
			unit_string_texture(const ttf& font, int style, const std::basic_string<CharT, Traits, Allocator>& str, const color_rgb& color) : style_m(TTF_STYLE_NORMAL) {
				this->remake(font, style, str, color);
			}
			template <class CharT>
			unit_string_texture(const ttf& font, int style, const CharT* str, const color_rgb& color) : style_m(TTF_STYLE_NORMAL) {
				this->remake(font, style, str, color);
			}
			template <class CharT, class Traits, class Allocator>
			unit_string_texture(const ttf& font, int style, const std::basic_string<CharT, Traits, Allocator>& str, const color_rgb& color, const color_rgb& bg_color) : style_m(TTF_STYLE_NORMAL) {
				this->remake(font, style, str, color, bg_color);
			}
			template <class CharT>
			unit_string_texture(const ttf& font, int style, const CharT* str, const color_rgb& color, const color_rgb& bg_color) : style_m(TTF_STYLE_NORMAL) {
				this->remake(font, style, str, color, bg_color);
			}

			template <class CharT, class Traits, class Allocator>
			void remake(const ttf& font, int style, const std::basic_string<CharT, Traits, Allocator>& str, const color_rgb& color) {
				this->remake(font, style, str.c_str(), color);
			}
			template <class CharT>
			void remake(const ttf& font, int style, const CharT* str, const color_rgb& color) {
				// フォントのスタイルチェック
				if (TTF_GetFontStyle(font.font_ptr_m) != style) TTF_SetFontStyle(font.font_ptr_m, style);
				SDL_Surface* temp = this->string_texture_impl(font, str, color);
				if (temp == nullptr) return;
				this->tex_m.remake(temp);
				SDL_FreeSurface(temp);

				this->style_m = style;
			}
			template <class CharT, class Traits, class Allocator>
			void remake(const ttf& font, int style, const std::basic_string<CharT, Traits, Allocator>& str, const color_rgb& color, const color_rgb& bg_color) {
				this->remake(font, style, str.c_str(), color, bg_color);
			}
			template <class CharT>
			void remake(const ttf& font, int style, const CharT* str, const color_rgb& color, const color_rgb& bg_color) {
				// フォントのスタイルチェック
				if (TTF_GetFontStyle(font.font_ptr_m) != style) TTF_SetFontStyle(font.font_ptr_m, style);
				SDL_Surface* temp = this->string_texture_impl(font, str, color, bg_color);
				if (temp == nullptr) return;
				this->tex_m.remake(temp);
				SDL_FreeSurface(temp);

				this->style_m = style;
			}

			texture& tex_handle() { return this->tex_m; }
			const texture& tex_handle() const { return this->tex_m; }
		};


		// 1行を示す文字列用テクスチャ
		class line_string_texture {
			friend class string_texture;

			std::list<unit_string_texture>	str_list_m;
			size_t							width_m;
			size_t							height_m;
			int								style_m;			// 現在の描画時の文字列のスタイル
		public:
			line_string_texture() : width_m(0), height_m(0), style_m(TTF_STYLE_NORMAL) {}
			template <class CharT, class Traits, class Allocator>
			line_string_texture(const ttf& font, const std::basic_string<CharT, Traits, Allocator>& str, const color_rgb& color) : width_m(0), height_m(0), style_m(TTF_STYLE_NORMAL) {
				this->add(font, str, color);
			}
			template <class CharT>
			line_string_texture(const ttf& font, const CharT* str, const color_rgb& color) : width_m(0), height_m(0), style_m(TTF_STYLE_NORMAL) {
				this->add(font, str, color);
			}
			template <class CharT, class Traits, class Allocator>
			line_string_texture(const ttf& font, const std::basic_string<CharT, Traits, Allocator>& str, const color_rgb& color, const color_rgb& bg_color) : width_m(0), height_m(0), style_m(TTF_STYLE_NORMAL) {
				this->add(font, str, color, bg_color);
			}
			template <class CharT>
			line_string_texture(const ttf& font, const CharT* str, const color_rgb& color, const color_rgb& bg_color) : width_m(0), height_m(0), style_m(TTF_STYLE_NORMAL) {
				this->add(font, str, color, bg_color);
			}
			~line_string_texture() {}

			template <class CharT, class Traits, class Allocator>
			line_string_texture& add(const ttf& font, const std::basic_string<CharT, Traits, Allocator>& str, const color_rgb& color) {
				if (str.size() == 0) return *this;
				this->str_list_m.emplace_back(font, this->style_m, str, color);
				this->width_m += this->str_list_m.back().tex_handle().width();
				this->height_m = (max)(this->str_list_m.back().tex_handle().height(), this->height_m);
				return *this;
			}
			template <class CharT>
			line_string_texture& add(const ttf& font, const CharT* str, const color_rgb& color) {
				if (str == nullptr) return *this;
				this->str_list_m.emplace_back(font, this->style_m, str, color);
				this->width_m += this->str_list_m.back().tex_handle().width();
				this->height_m = (max)(this->str_list_m.back().tex_handle().height(), this->height_m);
				return *this;
			}
			template <class CharT, class Traits, class Allocator>
			line_string_texture& add(const ttf& font, const std::basic_string<CharT, Traits, Allocator>& str, const color_rgb& color, const color_rgb& bg_color) {
				if (str.size() == 0) return *this;
				this->str_list_m.emplace_back(font, this->style_m, str, color, bg_color);
				this->width_m += this->str_list_m.back().tex_handle().width();
				this->height_m = (max)(this->str_list_m.back().tex_handle().height(), this->height_m);
				return *this;
			}
			template <class CharT>
			line_string_texture& add(const ttf& font, const CharT* str, const color_rgb& color, const color_rgb& bg_color) {
				if (str == nullptr) return *this;
				this->str_list_m.emplace_back(font, this->style_m, str, color, bg_color);
				this->width_m += this->str_list_m.back().tex_handle().width();
				this->height_m = (max)(this->str_list_m.back().tex_handle().height(), this->height_m);
				return *this;
			}

			size_t width() const { return this->width_m; }
			size_t height() const { return this->height_m; }


			using iterator = typename std::list<unit_string_texture>::iterator;
			using const_iterator = typename std::list<unit_string_texture>::const_iterator;
			using reference = typename std::list<unit_string_texture>::reference;
			using const_reference = typename std::list<unit_string_texture>::const_reference;

			// 行を構成する文字列テクスチャのイテレータの取得
			iterator begin() { return this->str_list_m.begin(); }
			const_iterator begin() const { return this->str_list_m.begin(); }
			iterator end() { return this->str_list_m.end(); }
			const_iterator end() const { return this->str_list_m.end(); }

			// 要素アクセス
			reference front() { return this->str_list_m.front(); }
			const_reference front() const { return this->str_list_m.front(); }
			reference back() { return this->str_list_m.back(); }
			const_reference back() const { return this->str_list_m.back(); }


			// フォントスタイルの設定
			// ボールド
			line_string_texture& bold(bool f) {
				if (f) this->style_m |= TTF_STYLE_BOLD;
				else this->style_m &= ~TTF_STYLE_BOLD;
				return *this;
			}
			// イタリック
			line_string_texture& italic(bool f) {
				if (f) this->style_m |= TTF_STYLE_ITALIC;
				else this->style_m &= ~TTF_STYLE_ITALIC;
				return *this;
			}
			// アンダーライン
			line_string_texture& underline(bool f) {
				if (f) this->style_m |= TTF_STYLE_UNDERLINE;
				else this->style_m &= ~TTF_STYLE_UNDERLINE;
				return *this;
			}
		};


		// 文字列用テクスチャ
		class string_texture {
			std::list<line_string_texture>	str_list_m;
		public:
			// それぞれ1行目の構築準備をする
			string_texture() : str_list_m(1) {}
			template <class CharT>
			string_texture(const ttf& font, const CharT* str, const color_rgb& color) : str_list_m(1) {
				this->add(font, str, color);
			}
			template <class CharT>
			string_texture(const ttf& font, const CharT* str, const color_rgb& color, size_t width) : str_list_m(1) {
				this->add(font, str, color, width);
			}
			~string_texture() {}

			// 改行の挿入
			string_texture& ln() {
				if (this->str_list_m.size() != 0) {
					auto& temp = this->str_list_m.back();
					this->str_list_m.emplace_back();
					// フォントスタイルの伝搬
					this->str_list_m.back().style_m = temp.style_m;
				}
				// 1行目の構築
				else this->str_list_m.resize(1);
				return *this;
			}

			// 通常の文字列追加
			template <class CharT, class Traits, class Allocator>
			string_texture& add(const ttf& font, const std::basic_string<CharT, Traits, Allocator>& str, const color_rgb& color) {
				if (str.size() == 0) return *this;
				this->str_list_m.back().add(font, str.c_str(), color);
				return *this;
			}
			template <class CharT>
			string_texture& add(const ttf& font, const CharT* str, const color_rgb& color) {
				if (str == nullptr) return *this;
				this->str_list_m.back().add(font, str, color);
				return *this;
			}
			template <class CharT, class Traits, class Allocator>
			string_texture& add(const ttf& font, const std::basic_string<CharT, Traits, Allocator>& str, const color_rgb& color, const color_rgb& bg_color) {
				if (str.size() == 0) return *this;
				this->str_list_m.back().add(font, str.c_str(), color, bg_color);
				return *this;
			}
			template <class CharT>
			string_texture& add(const ttf& font, const CharT* str, const color_rgb& color, const color_rgb& bg_color) {
				if (str == nullptr) return *this;
				this->str_list_m.back().add(font, str, color, bg_color);
				return *this;
			}
			// 幅制限つき文字列データの追加
			template <class CharT, class Traits, class Allocator>
			string_texture& add(const ttf& font, const std::basic_string<CharT, Traits, Allocator>& str, const color_rgb& color, size_t width) {
				if (str.size() == 0) return *this;

				CharT* p = unset_const(str.c_str());
				for (CharT* q = p; (*q != 0) && (*p != 0); q = next_char(q)) {
					CharT temp = *q;
					*q = 0;
					// 改行の必要性の判定
					if (this->str_list_m.back().width() + strlen(font, p) > width) {
						CharT* temp2 = nullptr;
						// 1行に最低1字は入力するようにする
						if ((this->str_list_m.back().width() == 0) && (p == q)) temp2 = next_char(p);
						// widthに収まるように可能ならqを1つ前に戻して入力(現在の行に対して1文字以上入力するとき)
						else if ((p != q) && (p != prev_char(q))) temp2 = prev_char(q);
						// 入力の必要がないとき
						else {
							this->ln();
							// 状態を復元
							*q = temp;
							q = p;
							continue;
						}

						CharT temp3 = *temp2;
						*temp2 = 0;
						this->str_list_m.back().add(font, p, color);
						this->ln();
						// 状態を復元
						*temp2 = temp3;
						*q = temp;
						p = q = temp2;
					}
					// 状態を復元
					else *q = temp;
				}
				// 折り返しの一番最後の追加
				if (*p != 0) this->str_list_m.back().add(font, p, color);

				return *this;
			}
			template <class CharT>
			string_texture& add(const ttf& font, const CharT* c, const color_rgb& color, size_t width) {
				if (c == nullptr) return *this;
				return this->add(font, std::basic_string<CharT>(c), color, width);
			}
			template <class CharT, class Traits, class Allocator>
			string_texture& add(const ttf& font, const std::basic_string<CharT, Traits, Allocator>& str, const color_rgb& color, const color_rgb& bg_color, size_t width) {
				if (str.size() == 0) return *this;

				CharT* p = unset_const(str.c_str());
				for (CharT* q = p; *q != 0; q = next_char(q)) {
					CharT temp = *q;
					*q = 0;
					// 改行の必要性の判定
					if (this->str_list_m.back().width() + strlen(font, p) > width) {
						CharT* temp2 = nullptr;
						// 1行に最低1字は入力するようにする
						if ((this->str_list_m.back().width() == 0) && (p == q)) temp2 = next_char(p);
						// widthに収まるように可能ならqを1つ前に戻して入力(現在の行に対して1文字以上入力するとき)
						else if ((p != q) && (p != prev_char(q))) temp2 = prev_char(q);
						// 入力の必要がないとき
						else {
							this->ln();
							// 状態を復元
							*q = temp;
							q = p;
							continue;
						}

						CharT temp3 = *temp2;
						*temp2 = 0;
						this->str_list_m.back().add(font, p, color, bg_color);
						this->ln();
						// 状態を復元
						*temp2 = temp3;
						*q = temp;
						p = q = temp2;
					}
					// 状態を復元
					else *q = temp;
				}
				// 折り返しの一番最後の追加
				if (*p != 0) this->str_list_m.back().add(font, p, color, bg_color);

				return *this;
			}
			template <class CharT>
			string_texture& add(const ttf& font, const CharT* c, const color_rgb& color, const color_rgb& bg_color, size_t width) {
				if (c == nullptr) return *this;
				return this->add(font, std::basic_string<CharT>(c), color, bg_color, width);
			}
			// 文字列追加後改行を挿入
			template <class CharT, class Traits, class Allocator>
			string_texture& addln(const ttf& font, const std::basic_string<CharT, Traits, Allocator>& str, const color_rgb& color) {
				return this->add(font, str, color).ln();
			}
			template <class CharT>
			string_texture& addln(const ttf& font, const CharT* str, const color_rgb& color) {
				return this->add(font, str, color).ln();
			}
			template <class CharT, class Traits, class Allocator>
			string_texture& addln(const ttf& font, const std::basic_string<CharT, Traits, Allocator>& str, const color_rgb& color, const color_rgb& bg_color) {
				return this->add(font, str, color, bg_color).ln();
			}
			template <class CharT>
			string_texture& addln(const ttf& font, const CharT* str, const color_rgb& color, const color_rgb& bg_color) {
				return this->add(font, str, color, bg_color).ln();
			}
			template <class CharT, class Traits, class Allocator>
			string_texture& addln(const ttf& font, const std::basic_string<CharT, Traits, Allocator>& str, const color_rgb& color, size_t width) {
				return this->add(font, str, color, width).ln();
			}
			template <class CharT>
			string_texture& addln(const ttf& font, const CharT* c, const color_rgb& color, size_t width) {
				return this->add(font, str, color, width).ln();
			}
			template <class CharT, class Traits, class Allocator>
			string_texture& addln(const ttf& font, const std::basic_string<CharT, Traits, Allocator>& str, const color_rgb& color, const color_rgb& bg_color, size_t width) {
				return this->add(font, str, color, bg_color, width).ln();
			}
			template <class CharT>
			string_texture& addln(const ttf& font, const CharT* c, const color_rgb& color, const color_rgb& bg_color, size_t width) {
				return this->add(font, str, color, bg_color, width).ln();
			}

			using iterator = typename std::list<line_string_texture>::iterator;
			using const_iterator = typename std::list<line_string_texture>::const_iterator;
			using reference = typename std::list<line_string_texture>::reference;
			using const_reference = typename std::list<line_string_texture>::const_reference;

			// 行のイテレータの取得
			iterator begin() { return this->str_list_m.begin(); }
			const_iterator begin() const { return this->str_list_m.begin(); }
			iterator end() { return this->str_list_m.end(); }
			const_iterator end() const { return this->str_list_m.end(); }

			// 要素アクセス
			reference front() { return this->str_list_m.front(); }
			const_reference front() const { return this->str_list_m.front(); }
			reference back() { return this->str_list_m.back(); }
			const_reference back() const { return this->str_list_m.back(); }


			// フォントスタイルの設定
			// ボールド
			string_texture& bold(bool f) {
				if (this->str_list_m.size() != 0) this->str_list_m.back().bold(f);
				return *this;
			}
			// イタリック
			string_texture& italic(bool f) {
				if (this->str_list_m.size() != 0) this->str_list_m.back().italic(f);
				return *this;
			}
			// アンダーライン
			string_texture& underline(bool f) {
				if (this->str_list_m.size() != 0) this->str_list_m.back().underline(f);
				return *this;
			}
		};


		//文字列テクスチャの描画
		inline void draw_string_texture2(double x, double y, const string_texture& tex) {
			// 入力開始時の高さ
			double h = y;
			for (const auto& line : tex) {
				// 入力開始時の幅
				double w = x;
				h += line.height();
				for (const auto& unit_tex : line) {
					// 高さは下に合わせて描画
					draw_texture2(vertex2<double>(w, h - unit_tex.tex_handle().height(), w + unit_tex.tex_handle().width(), h), &unit_tex.tex_handle());
					w += unit_tex.tex_handle().width();
				}
			}
		}
		template <class CharT>
		inline void draw_string_texture2(double x, double y, const ttf& font, const CharT* str, const color_rgb& color, size_t width = -1) {
			string_texture tex(font, str, color, width);
			draw_string_texture2(x, y, tex);
		}
		template <class CharT>
		inline void draw_string_texture2(double x, double y, const ttf& font, CharT*&& str, const color_rgb& color, size_t width = -1) {
			string_texture tex(font, str, color, width);
			draw_string_texture2(x, y, tex);
		}
	}

}


#endif