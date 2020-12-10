#ifndef IMATHLIB_H_MEDIA_GRAPHIC_HPP
#define IMATHLIB_H_MEDIA_GRAPHIC_HPP


#include "IMathLib/media/log.hpp"
#include "IMathLib/media/camera.hpp"
#include "IMathLib/image/color.hpp"
#include "IMathLib/image/image.hpp"
#include "IMathLib/utility/tuple.hpp"
#include "IMathLib/utility/enabler_object.hpp"
#include <vector>
#include <unordered_map>
#include <list>



// 画像処理(ビューポート領域の左上を(0,0)とする)
// 2次元限定

namespace iml {
	namespace ml {

		inline void glVertexPointer(GLint dim, GLsizei stride, const GLshort* vtx) { ::glVertexPointer(dim, GL_SHORT, stride, vtx); }
		inline void glVertexPointer(GLint dim, GLsizei stride, const GLint* vtx) { ::glVertexPointer(dim, GL_INT, stride, vtx); }
		inline void glVertexPointer(GLint dim, GLsizei stride, const GLfloat* vtx) { ::glVertexPointer(dim, GL_FLOAT, stride, vtx); }
		inline void glVertexPointer(GLint dim, GLsizei stride, const GLdouble* vtx) { ::glVertexPointer(dim, GL_DOUBLE, stride, vtx); }
		inline void glTexCoordPointer(GLint dim, GLsizei stride, const GLshort* vtx) { ::glTexCoordPointer(dim, GL_SHORT, stride, vtx); }
		inline void glTexCoordPointer(GLint dim, GLsizei stride, const GLint* vtx) { ::glTexCoordPointer(dim, GL_INT, stride, vtx); }
		inline void glTexCoordPointer(GLint dim, GLsizei stride, const GLfloat* vtx) { ::glTexCoordPointer(dim, GL_FLOAT, stride, vtx); }
		inline void glTexCoordPointer(GLint dim, GLsizei stride, const GLdouble* vtx) { ::glTexCoordPointer(dim, GL_DOUBLE, stride, vtx); }


		// シェーダークラス
		class shader {
			size_t shader_id_m;			// シェーダーの管理id
		public:
			static constexpr size_t vertex = 0;			// バーテクスシェーダー
			static constexpr size_t fragment = 1;		// フラグメントシェーダー
			static constexpr size_t geometry = 2;		// ジオメトリシェーダー

			shader(const char* str, size_t ident) {
				int result = GL_FALSE;					// コンパイル結果

				shader_id_m = (ident == vertex) ? glCreateShader(GL_VERTEX_SHADER) :
					((ident == fragment) ? glCreateShader(GL_FRAGMENT_SHADER) :
					((ident == geometry) ? glCreateShader(GL_GEOMETRY_SHADER) : 0));

				glShaderSource(shader_id_m, 1, &str, nullptr);		// シェーダオブジェクトに変換
				glCompileShader(shader_id_m);							// シェーダのコンパイル
				// エラーチェック
				glGetShaderiv(shader_id_m, GL_COMPILE_STATUS, &result);
				if (result == GL_FALSE) {
					char* error_mes;
					int log_length;
					glGetShaderiv(shader_id_m, GL_INFO_LOG_LENGTH, &log_length);
					error_mes = new char[log_length + 1];
					glGetShaderInfoLog(shader_id_m, log_length, nullptr, error_mes);
					IMATHLIB_LOG(iml::ml::logger::error, str) << error_mes;
					delete[] error_mes;
				}
			}
			~shader() { glDeleteShader(shader_id_m); }

			size_t id() const { return shader_id_m; }
		};


		// シェアポインタとして管理するためのプログラムオブジェクトクラス
		class program_object {
			size_t po_id;			// プログラムオブジェクトのid

			// 変数の位置の取得
			int get_variable(const char* str) {
				return glGetUniformLocation(po_id, str);
			}
		public:
			template <class... Args>
			program_object(const Args&... shaders) {
				po_id = glCreateProgram();
				attach(shaders...);
			}
			~program_object() { glDeleteProgram(po_id); }

			// アタッチ
			void attach() {}
			void attach(const shader& first) { glAttachShader(po_id, first.id()); }
			template <class... Args>
			void attach(const shader& first, const Args&... shaders) { glAttachShader(po_id, first.id()); attach(shaders...); }
			void attach(size_t first) { glAttachShader(po_id, first); }
			template <class... Args>
			void attach(size_t first, const Args&... shaders) { glAttachShader(po_id, first); attach(shaders...); }
			// デタッチ
			void dettach() {}
			void dettach(const shader& first) { glDetachShader(po_id, first.id()); }
			template <class... Args>
			void dettach(const shader& first, const Args&... shaders) { glDetachShader(po_id, first.id()); dettach(shaders...); }
			void dettach(size_t first) { glDetachShader(po_id, first); }
			template <class... Args>
			void dettach(size_t first, const Args&... shaders) { glDetachShader(po_id, first); dettach(shaders...); }

			// シェーダのリンク
			void link() {
				int result = GL_FALSE;
				glLinkProgram(po_id);
				//  プログラムをチェックする
				glGetProgramiv(po_id, GL_LINK_STATUS, &result);
				if (result == GL_FALSE) {
					char* error_mes;
					int log_length;
					glGetProgramiv(po_id, GL_INFO_LOG_LENGTH, &log_length);
					error_mes = new char[log_length + 1];
					glGetProgramInfoLog(po_id, log_length, nullptr, error_mes);
					IMATHLIB_LOG(iml::ml::logger::error, u8"glLinkProgram") << error_mes;
					delete[] error_mes;
				}
			}

			// 変数値の設定
			void set_variable(const char* str, int v) { glUniform1i(get_variable(str), v); }
			void set_variable(const char* str, int v1, int v2) { glUniform2i(get_variable(str), v1, v2); }
			void set_variable(const char* str, int v1, int v2, int v3) { glUniform3i(get_variable(str), v1, v2, v3); }
			void set_variable(const char* str, float value) { glUniform1f(get_variable(str), value); }
			void set_variable(const char* str, float v1, float v2) { glUniform2f(get_variable(str), v1, v2); }
			void set_variable(const char* str, float v1, float v2, float v3) { glUniform3f(get_variable(str), v1, v2, v3); }
			void set_variable(const char* str, size_t size, int* v) { glUniform1iv(get_variable(str), size, v); }
			void set_variable(const char* str, size_t size, float* v) { glUniform1fv(get_variable(str), size, v); }
			void set_variable(const char* str, const matrix2<float>& ma) { glUniformMatrix2fv(get_variable(str), 1, GL_TRUE, ma[0]); }
			void set_variable(const char* str, const matrix3<float>& ma) { glUniformMatrix3fv(get_variable(str), 1, GL_TRUE, ma[0]); }
			void set_variable(const char* str, const matrix4<float>& ma) { glUniformMatrix4fv(get_variable(str), 1, GL_TRUE, ma[0]); }

			// プログラムオブジェクトの使用
			void shader_start() { glUseProgram(po_id); }
			// プログラムオブジェクトの使用の解除
			void shader_end() { glUseProgram(0); }

		};

		// テクスチャのための定数
		namespace txc {
			// テクスチャの種類
			inline constexpr size_t rgb_texture = GL_RGB;					// RGBカラーテクスチャ
			inline constexpr size_t rgba_texture = GL_RGBA;					// RGBAカラーテクスチャ
			inline constexpr size_t depth_texture = GL_DEPTH_COMPONENT;		// デプステクスチャ(z値のみのテクスチャ)
			inline constexpr size_t stencil_texture = GL_STENCIL_INDEX;		// ステンシルテクスチャ(ステンシル値のみのテクスチャ)
			// テクスチャ拡大縮小時の補間
			inline constexpr size_t nearest = GL_NEAREST;					// 最近傍補間
			inline constexpr size_t linear = GL_LINEAR;						// 双線形補間
			// クランプの設定
			inline constexpr size_t clamp_default = GL_CLAMP;				// 最外周色をそのまま拡張(補間済み)
			inline constexpr size_t clamp_texture = GL_CLAMP_TO_EDGE;		// テクスチャの最外周色で拡張
			inline constexpr size_t clamp_border = GL_CLAMP_TO_BORDER;		// ボーダー色で拡張
		}


		// 1つのテクスチャのための頂点配列の構造体
		template <class T>
		struct vertex2 {
			T vtx[8];
			vertex2() :vtx{} {}
			vertex2(const T& left_up_x, const T& left_up_y, const T& right_down_x, const T& right_down_y) {
				vtx[0] = left_up_x; vtx[1] = left_up_y;			// 左上
				vtx[2] = left_up_x; vtx[3] = right_down_y;		// 左下
				vtx[4] = right_down_x; vtx[5] = right_down_y;	// 右下
				vtx[6] = right_down_x; vtx[7] = left_up_y;		// 右上
			}
			vertex2(const T& _1, const T& _2, const T& _3, const T& _4, const T& _5, const T& _6, const T& _7, const T& _8) {
				vtx[0] = _1; vtx[1] = _2;		// 左上
				vtx[2] = _3; vtx[3] = _4;		// 左下
				vtx[4] = _5; vtx[5] = _6;		// 右下
				vtx[6] = _7; vtx[7] = _8;		// 右上
				/*glTexCoord2f(0, 0);
				glTexCoord2f(0, 1);
				glTexCoord2f(1, 1);
				glTexCoord2f(1, 0);*/
			}
			vertex2(const T* p) {
				for (size_t i = 0; i < 8; ++i)
					vtx[i] = p[i];
			}
			template <class S>
			vertex2(const vertex2<S>& vtx) {
				for (size_t i = 0; i < 8; ++i)
					this->vtx[i] = static_cast<T>(vtx.vtx[i]);
			}
		};


		// テクスチャとそれを
		class texture;
		using shared_texture = std::shared_ptr<texture>;

		class texture_unit_impl;
		using texture_unit = std::shared_ptr<texture_unit_impl>;


		// OpenGLのテクスチャユニットの管理
		class texture_unit_control {
			friend class texture;
			friend class texture_unit_impl;

			// 各種管理の違い
			// data1_m:テクスチャユニットの番号に対するテクスチャユニットハンドルの保持
			// data2_m:テクスチャIDとテクスチャユニットの番号の関連付け(全てのテクスチャIDを保持する)
			// data3_m:古い順にリバインドされるためのデータリスト(data1_mとは0番目の要素を除いて一対一対応)
			static inline std::vector<texture_unit>				data1_m;		// 添え字はテクスチャユニット番号
			static inline std::unordered_map<GLuint, int_t>		data2_m;		// Key:テクスチャID, Value:テクスチャユニット番号
			static inline std::list<int_t>						data3_m;		// テクスチャユニット番号のリスト

			static inline GLint						color_buffer_num_m;			// 利用可能なカラーバッファの数
			static inline std::vector<GLenum>		color_buffer_list_m;		// 描画するカラーバッファのためのリスト

			static inline program_object	*default_program_m;			// フレームバッファのためのシェーダ

			// textureをtexture_unit_controlに登録
			static void set_texture(const texture&);
			// textureをtexture_unit_controlから破棄
			static void unset_texture(const texture&);
		public:
			// 初期化
			static void init();
			// 終了処理
			static void quit();
			// 適当なtexture_unitの取得
			static texture_unit get_texture_unit(const texture&);
			static texture_unit get_texture_unit(const shared_texture& tex) {
				if (tex.get() == nullptr) return texture_unit();
				return texture_unit_control::get_texture_unit(*tex.get());
			}
			// 利用可能なカラーバッファの数の取得
			static GLint color_buffer_num() { return texture_unit_control::color_buffer_num_m; }
			// カラーバッファリストのデータポインタの取得
			static GLenum* color_buffer_list() { return texture_unit_control::color_buffer_list_m.data(); }
		};


		// テクスチャ
		class texture {
			GLuint	id_m;
			size_t	width_m, height_m;
			GLint	format_m;


			//4の倍数に切り上げるの関数
			static constexpr size_t ceil4(size_t n) {
				return (n & 3) ? (((n >> 2) + 1) << 2) : (n);
			}
			void remake_impl(const SDL_Surface* img, GLint interpolation, GLint clamp) {
				GLint		internal_format;
				GLint		pixel_format;
				GLubyte		*pixels;
				bool		use_palette = false;		// 処理時にパレットを用いたか

				if (img == nullptr) return;
				this->width_m = img->w; this->height_m = img->h;

				// OpenGL形式に変換
				switch (img->format->BitsPerPixel) {
				case 32:
					internal_format = GL_RGBA;
					pixel_format = (img->format->Bshift > img->format->Rshift) ? GL_RGBA : GL_BGRA;
					pixels = static_cast<GLubyte*>(img->pixels);
					this->format_m = txc::rgba_texture;
					break;
				case 24:
					internal_format = GL_RGB;
					pixel_format = (img->format->Bshift > img->format->Rshift) ? GL_RGB : GL_BGR;
					pixels = static_cast<GLubyte*>(img->pixels);
					this->format_m = txc::rgb_texture;
					break;
				case 8:
					internal_format = GL_RGB;
					pixel_format = GL_RGB;
					this->format_m = txc::rgb_texture;
					{
						SDL_LockSurface(unset_const(img));
						// 幅を4の倍数に切り上げる
						size_t real_w = texture::ceil4(this->width_m * 3);
						pixels = new GLubyte[real_w * this->height_m];
						// カラーパレットを展開してテクスチャを構築する
						for (size_t y = 0; y < this->height_m; ++y) {
							for (size_t x = 0; x < this->width_m; ++x) {
								auto& temp = img->format->palette->colors[static_cast<const Uint8*>(img->pixels)[y * img->pitch + x]];
								pixels[(y * real_w + x * 3)] = temp.r;
								pixels[(y * real_w + x * 3) + 1] = temp.g;
								pixels[(y * real_w + x * 3) + 2] = temp.b;
							}
						}
						SDL_UnlockSurface(unset_const(img));
					}
					use_palette = true;
					break;
				default:
					// ビットレートが8か24か32でない
					IMATHLIB_LOG(iml::ml::logger::error, u8"bit rate error.") << u8"The bit rate is not 8, 24 or 32.";
					return;
				}

				// OpenGLテクスチャの作成
				glGenTextures(1, &this->id_m);
				glBindTexture(GL_TEXTURE_2D, this->id_m);
				// テクスチャにPNGファイルから読み込んだピクセルを書き込む
				//gluBuild2DMipmaps(GL_TEXTURE_2D, internal_format, img->w, img->h, pixel_format, GL_UNSIGNED_BYTE, img->pixels);
				glTexImage2D(GL_TEXTURE_2D, 0, internal_format, img->w, img->h, 0, pixel_format, GL_UNSIGNED_BYTE, pixels);
				// クランプ(uv座標が[0,1]を超えたときに範囲を超えて描画するときの設定)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp);
				// 縮小時の補間
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolation);
				// 拡大時の補間
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolation);
				glBindTexture(GL_TEXTURE_2D, 0);

				if (use_palette) delete[] pixels;

				texture_unit_control::set_texture(*this);
			}
			void remake_impl(const img::image* img, GLint interpolation, GLint clamp) {
				GLint			internal_format;
				GLint			pixel_format;

				if (img == nullptr) return;
				this->width_m = img->w; this->height_m = img->h;

				// OpenGL形式に変換
				switch (img->color) {
				case 3:
					internal_format = GL_RGB;
					pixel_format = GL_RGB;
					this->format_m = txc::rgb_texture;
					break;
				case 4:
					internal_format = GL_RGBA;
					pixel_format = GL_RGBA;
					this->format_m = txc::rgba_texture;
					break;
				default:
					// ビットレートが24か32でない
					IMATHLIB_LOG(iml::ml::logger::error, u8"bit rate error.") << u8"The bit rate is not 24 or 32.";
					return;
				}

				// OpenGLテクスチャの作成
				glGenTextures(1, &this->id_m);
				glBindTexture(GL_TEXTURE_2D, this->id_m);
				// テクスチャにPNGファイルから読み込んだピクセルを書き込む
				//gluBuild2DMipmaps(GL_TEXTURE_2D, internal_format, img->w, img->h, pixel_format, GL_UNSIGNED_BYTE, img->pixels);
				glTexImage2D(GL_TEXTURE_2D, 0, internal_format, img->w, img->h, 0, pixel_format, GL_UNSIGNED_BYTE, img->pixels);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolation);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolation);
				glBindTexture(GL_TEXTURE_2D, 0);

				texture_unit_control::set_texture(*this);
			}
			void remake_impl(size_t w, size_t h, GLenum color, GLint interpolation, GLint clamp) {
				this->width_m = w; this->height_m = h;
				this->format_m = color;
				
				// OpenGLテクスチャの作成
				glGenTextures(1, &this->id_m);
				glBindTexture(GL_TEXTURE_2D, this->id_m);
				// 空のときはミップマップによる生成はしない
				glTexImage2D(GL_TEXTURE_2D, 0, color, w, h, 0, color, GL_UNSIGNED_BYTE, nullptr);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolation);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolation);
				glBindTexture(GL_TEXTURE_2D, 0);

				texture_unit_control::set_texture(*this);
			}
			texture(const texture&) = delete;
			texture& operator=(const texture&) = delete;
		public:
			constexpr texture() : width_m(0), height_m(0), id_m(0), format_m(0) {}
			texture(texture&& tex) noexcept : width_m(tex.width_m), height_m(tex.height_m), id_m(tex.id_m), format_m(tex.format_m) { tex.id_m = 0; }
			texture(const SDL_Surface *img, GLint interpolation = txc::nearest, GLint clamp = txc::clamp_texture) : width_m(0), height_m(0), id_m(0), format_m(0) {
				this->remake_impl(img, interpolation, clamp);
			}
			texture(const img::image *img, GLint interpolation = txc::nearest, GLint clamp = txc::clamp_texture) : width_m(0), height_m(0), id_m(0), format_m(0) {
				this->remake_impl(img, interpolation, clamp);
			}
			// 空のテクスチャの構成
			texture(size_t w, size_t h, GLenum color = txc::rgb_texture, GLint interpolation = txc::nearest, GLint clamp = txc::clamp_texture) : width_m(0), height_m(0), id_m(0), format_m(0) {
				this->remake_impl(w, h, color, interpolation, clamp);
			}
			~texture() {
				if (this->id_m != 0) {
					// リソースおよび所有権の破棄
					texture_unit_control::unset_texture(*this);
					glDeleteTextures(1, &this->id_m);
				}
			}

			// テクスチャの再構築
			// SDL画像からの構築
			texture& remake(const SDL_Surface* img, GLint interpolation = txc::nearest, GLint clamp = txc::clamp_texture) {
				// リソースおよび所有権の破棄
				texture_unit_control::unset_texture(*this);
				glDeleteTextures(1, &this->id_m);
				// テクスチャの構築
				this->remake_impl(img, interpolation, clamp);
				return *this;
			}
			// 画像データからの構築
			texture& remake(const img::image* img, GLint interpolation = txc::nearest, GLint clamp = txc::clamp_texture) {
				// リソースおよび所有権の破棄
				texture_unit_control::unset_texture(*this);
				glDeleteTextures(1, &this->id_m);
				// テクスチャの構築
				this->remake_impl(img, interpolation, clamp);
				return *this;
			}
			// 空のテクスチャの構成
			texture& remake(size_t w, size_t h, GLenum color = txc::rgb_texture, GLint interpolation = txc::nearest, GLint clamp = txc::clamp_texture) {
				// リソースおよび所有権の破棄
				texture_unit_control::unset_texture(*this);
				glDeleteTextures(1, &this->id_m);
				// テクスチャの構築
				this->remake_impl(w, h, color, interpolation, clamp);
				return *this;
			}

			// 各種パラメータの取得
			GLuint id() const { return this->id_m; }
			size_t width() const { return this->width_m; }
			size_t height() const { return this->height_m; }
			GLint format() const { return this->format_m; }

			// ボーダーカラーの設定
			void border_color(const normal_color<float>& color) {
				if (this->id_m != 0) {
					glBindTexture(GL_TEXTURE_2D, this->id_m);
					glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color.v);
					glBindTexture(GL_TEXTURE_2D, 0);
				}
			}

			texture& operator=(texture&& tex) noexcept {
				this->width_m = tex.width_m;
				this->height_m = tex.height_m;
				this->id_m = tex.id_m;
				tex.id_m = 0;
			}
		};

		// テクスチャユニット
		class texture_unit_impl {
			friend class texture_unit_control;

			const texture*	tex_m;				// テクスチャユニットへと参照するテクスチャ
			int_t			tex_unit_m;			// テクスチャユニット番号
		public:
			texture_unit_impl() : tex_m(nullptr), tex_unit_m(-1) {}
			texture_unit_impl(const texture& tex, int_t tex_unit) : tex_m(std::addressof(tex)), tex_unit_m(tex_unit) {
				// テクスチャユニットにテクスチャをバインドする
				glActiveTexture(GL_TEXTURE0 + tex_unit);
				glBindTexture(GL_TEXTURE_2D, tex.id());
				glActiveTexture(GL_TEXTURE0);
			}

			// 参照しているテクスチャ
			const texture& reference() const { return *(this->tex_m); }
			// インスタンスの有無
			bool is_exist_inst() const { return tex_m != nullptr; }
			// テクスチャユニット番号
			int_t tex_unit() const { return this->tex_unit_m; }
			// tex_mと同じテクスチャidを保持するテクスチャの判定
			bool is_same_texture(const texture& tex) const {
				return this->is_exist_inst() ? (this->tex_m->id() == tex.id()) : false;
			}
			// アクティブにする
			void active() { glActiveTexture(GL_TEXTURE0 + this->tex_unit_m); }

			// テクスチャ座標の設定
			void set_texcoord() const {
				// デフォルト
				if (tex_unit_m != -1) {
					// 画像は上下反転して読み込まれるため。そうする
					//static const vertex2<double> tex_coord(0, 1, 1, 0);
					static const vertex2<float> tex_coord(0, 0, 1, 1);			// 左上座標, 右下座標
					glMultiTexCoordPointerEXT(GL_TEXTURE0 + tex_unit_m, 2, GL_FLOAT, 0, tex_coord.vtx);
				}
			}
			void set_texcoord(const float* coord, size_t byte_offset = 0) const {
				if (tex_unit_m != -1) glMultiTexCoordPointerEXT(GL_TEXTURE0 + tex_unit_m, 2, GL_FLOAT, byte_offset, coord);
			}
			void set_texcoord(const double* coord, size_t byte_offset = 0) const {
				if (tex_unit_m != -1) glMultiTexCoordPointerEXT(GL_TEXTURE0 + tex_unit_m, 2, GL_DOUBLE, byte_offset, coord);
			}
		};


		// texture_unit_controlの初期化
		inline void texture_unit_control::init() {
			GLint temp[4];
			// OpenGLで利用可能なテクスチャユニット
			glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &temp[0]);
			// バーテクスシェーダで利用可能なテクスチャユニット
			glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &temp[1]);
			// フラグメントシェーダで利用可能なテクスチャユニット
			glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &temp[2]);
			temp[3] = (min)(temp[0], temp[1], temp[2]);
			// 最小値の分だけ管理領域を確保
			texture_unit_control::data1_m.reserve(temp[3]);
			// 現在GL_TEXTURE0は管轄外であるが将来のために予約
			texture_unit_control::data1_m.push_back(texture_unit(new texture_unit_impl()));
			for (GLint i = 1; i < temp[3]; ++i) {
				texture_unit_control::data1_m.push_back(texture_unit(new texture_unit_impl()));
				texture_unit_control::data3_m.push_back(i);
			}

			// 利用可能なカラーバッファの数
			glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &texture_unit_control::color_buffer_num_m);
			// カラーバッファリストの構築
			texture_unit_control::color_buffer_list_m.reserve(texture_unit_control::color_buffer_num_m);
			for (size_t i = 0; i < texture_unit_control::color_buffer_num_m; ++i)
				texture_unit_control::color_buffer_list_m.push_back(GL_COLOR_ATTACHMENT0_EXT + i);

			// シェーダの構築
			texture_unit_control::default_program_m = new program_object();
			texture_unit_control::default_program_m->attach(
				shader(
					"#version 120\n"
					"varying vec2 pos;"
					"void main(void) {"
						"pos = gl_Vertex.xy;"
						"gl_Position = ftransform();"
					"}"
					, shader::vertex)
				, shader(
					"#version 120\n"
					"uniform sampler2D image;"
					"uniform int img_width, img_height;"
					"varying vec2 pos;"
					"void main(void) {"
						"vec2 texCoord = vec2(pos.x / float(img_width), pos.y / float(img_height));"
						"gl_FragColor = texture2D(image, texCoord);"
					"}"
					, shader::fragment));
			texture_unit_control::default_program_m->link();
		}
		// texture_unit_controlの終了処理
		inline void texture_unit_control::quit() {
			delete texture_unit_control::default_program_m;

			texture_unit_control::data1_m.clear();
			texture_unit_control::data2_m.clear();
			texture_unit_control::data3_m.clear();
		}
		// 適当なtexture_unitの取得
		inline texture_unit texture_unit_control::get_texture_unit(const texture& tex) {
			// テクスチャハンドル0は無効
			if (tex.id() == 0) return texture_unit();

			auto itr = texture_unit_control::data2_m.find(tex.id());
			// 全てのテクスチャをdata2_mに登録しているためここには来ないはず
			if (itr == texture_unit_control::data2_m.end()) return texture_unit();

			// テクスチャユニットを利用中(バインド済み)ならばそのまま利用
			if (itr->second != -1) {
				// テクスチャユニット番号(itr->second)を最新としてdata3_mに再登録
				auto itr2 = std::find_if(texture_unit_control::data3_m.begin(), texture_unit_control::data3_m.end(), [&itr](int_t x) { return x == itr->second; });
				texture_unit_control::data3_m.erase(itr2); texture_unit_control::data3_m.push_back(itr->second);
				return texture_unit_control::data1_m[itr->second];
			}
			// 新規にテクスチャユニットを取得するとき
			else {
				auto itr2 = texture_unit_control::data3_m.begin();
				// 全てのテクスチャユニットがビジーなとき
				if (texture_unit_control::data1_m[*itr2].use_count() != 1) return texture_unit();
				// itr2に対応するdata2_mのテクスチャユニット番号を-1にする
				if (texture_unit_control::data1_m[*itr2]->is_exist_inst()) texture_unit_control::data2_m[texture_unit_control::data1_m[*itr2]->reference().id()] = -1;
				// テクスチャユニットの再生成
				texture_unit_control::data1_m[*itr2].reset(new texture_unit_impl(tex, *itr2));
				// data2_mにテクスチャユニット番号を登録
				itr->second = *itr2;
				// テクスチャユニット番号(itr->second(=*itr2))を最新としてdata3_mに再登録
				texture_unit_control::data3_m.pop_front();
				texture_unit_control::data3_m.push_back(itr->second);
				return texture_unit_control::data1_m[itr->second];
			}
		}
		// textureをtexture_unit_controlに登録
		inline void texture_unit_control::set_texture(const texture& tex) {
			texture_unit_control::data2_m[tex.id()] = -1;
		}
		// textureをtexture_unit_controlから破棄
		inline void texture_unit_control::unset_texture(const texture& tex) {
			// テクスチャハンドル0は無効
			if (tex.id() == 0) return;

			// リストからテクスチャユニットと管理しているテクスチャが同一であるものを探索
			for (auto itr = texture_unit_control::data3_m.begin(); itr != texture_unit_control::data3_m.end(); ++itr) {
				if (texture_unit_control::data1_m[*itr]->is_same_texture(tex)) {
					// 強制的にバインドを解除してdata1_m[*itr]の情報を初期化する
					glActiveTexture(GL_TEXTURE0 + *itr);
					glBindTexture(GL_TEXTURE_2D, 0);
					glActiveTexture(GL_TEXTURE0);
					texture_unit_control::data1_m[*itr].reset(new texture_unit_impl());
					// テクスチャユニット番号(*itr)を最古としてdata3_mに再登録
					texture_unit_control::data3_m.push_front(*itr);
					texture_unit_control::data3_m.erase(itr);
					break;
				}
			}
			// data2_mからtexの情報を破棄
			texture_unit_control::data2_m.erase(tex.id());
		}

		// frame_buffer_objectのenabler object
		class frame_buffer_object;
		struct FBO_OBJECT : ENABLER_OBJECT_BASE<pair<GLuint, const frame_buffer_object*>, FBO_OBJECT> {
			// FBOオブジェクトのIDを保持
			using base_pair = pair<GLuint, const frame_buffer_object*>;
			using base_class = ENABLER_OBJECT_BASE<base_pair, FBO_OBJECT>;
			friend base_class;

		private:
			// 初期状態のためのコンストラクタ
			FBO_OBJECT(enabler_object_init_tag) : base_class() { base_class::init_state_construct(); }
			FBO_OBJECT(enabler_object_init_tag, const pair<GLuint, const frame_buffer_object*>& fbo) : base_class(fbo) {
				base_class::init_state_construct();
			}
		public:
			// 通常のコンストラクタ
			FBO_OBJECT() : base_class() { base_class::construct(); }
			FBO_OBJECT(const FBO_OBJECT& fbo) : base_class(base_pair(fbo.id(), fbo.obj_m->second)) {
				base_class::construct();
			}
			FBO_OBJECT(FBO_OBJECT&& fbo) : base_class(std::move(fbo.obj_m)) {}
			FBO_OBJECT(const frame_buffer_object&);
			~FBO_OBJECT() {
				// 状態を持つならば処理する
				if (this->obj_m.get() != nullptr) base_class::destroy();
			}

			// ENABLER_OBJECT_CONTROLのエイリアス
			using base_class::enabler_object_control_t;

			// 状態を更新するメソッド
			void push_state();
			// 状態を外すメソッド
			void pop_state() {
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			}
			// 状態を設定するメソッド
			void set_state();

			// IDの取得
			GLuint id() const { return this->obj_m->first; }
		};
		// 現在のFBOの状態をfboと共通化
		inline void set_frame_buffer_object(const FBO_OBJECT& fbo) {
			using enabler_object_control_t = typename FBO_OBJECT::enabler_object_control_t;
			enabler_object_control_t::inst()->top()->set(fbo);
		}
		// 現在のFBOのIDの取得
		inline GLuint frame_buffer_object_id() {
			using enabler_object_control_t = typename FBO_OBJECT::enabler_object_control_t;
			return static_cast<FBO_OBJECT*>(enabler_object_control_t::inst()->top())->id();
		}
		// FBOの状態の初期化
		inline void init_frame_buffer_object() {
			FBO_OBJECT::init(pair<GLuint, const frame_buffer_object*>(GLuint(0), nullptr));
		}
		// FBOの終了
		inline void quit_frame_buffer_object() {
			FBO_OBJECT::quit();
		}


		// レンダーバッファのためのオブジェクト
		class render_buffer {
			friend class frame_buffer_object;

			GLuint	id_m;
			size_t	width_m, height_m;
			GLint	format_m;
			GLenum	render_type_m;
			GLuint	object_id_m;			// フレームバッファのオブジェクトid

			render_buffer(const render_buffer&) = delete;
			render_buffer& operator=(const render_buffer&) = delete;

			render_buffer() : id_m(0), width_m(0), height_m(0), render_type_m(0), format_m(0), object_id_m(0) {}
			render_buffer(GLuint obj_id, GLenum type, size_t w, size_t h, size_t color) : id_m(0), width_m(0), height_m(0), render_type_m(type), format_m(color), object_id_m(obj_id) {
				glGenRenderbuffersEXT(1, &this->id_m);
				glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->id_m);
				glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, color, w, h);
				glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
			}
		public:
			render_buffer(render_buffer&& tex) noexcept : id_m(tex.id_m), width_m(tex.width_m), height_m(tex.height_m), render_type_m(tex.render_type_m), format_m(tex.format_m), object_id_m(tex.object_id_m) {
				tex.id_m = 0;
				tex.render_type_m = 0;
				tex.object_id_m = 0;
			}
			~render_buffer() {
				if (this->id_m != 0) glDeleteRenderbuffersEXT(1, &this->id_m);
			}

			GLuint id() const { return this->id_m; }
			size_t width() const { return this->width_m; }
			size_t height() const { return this->height_m; }
			GLint format() const { return this->format_m; }
			GLenum type() const { return this->render_type_m; }

			render_buffer& operator=(render_buffer&& tex) noexcept {
				this->width_m = tex.width_m;
				this->height_m = tex.height_m;
				this->id_m = tex.id_m;
				this->render_type_m = tex.render_type_m;
				this->object_id_m = tex.object_id_m;
				tex.id_m = 0;
				tex.render_type_m = 0;
				tex.object_id_m = 0;
			}

			// カラーバッファとしてクリア
			void clear_color(const normal_color<float>& c = normal_color<float>()) {
				if ((this->render_type_m == GL_DEPTH_ATTACHMENT_EXT) || (this->render_type_m == GL_STENCIL_ATTACHMENT_EXT)) return;
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->object_id_m);
				glClearBufferfv(GL_COLOR, this->render_type_m - GL_COLOR_ATTACHMENT0_EXT, c.v);
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object_id());
			}
			// デプスバッファとしてクリア
			void clear_depth(GLfloat d = 1) {
				if (this->render_type_m != GL_DEPTH_ATTACHMENT_EXT) return;
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->object_id_m);
				glClearBufferfi(GL_DEPTH, 0, d, 0);
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object_id());
			}
			// ステンシルバッファとしてクリア
			void clear_stencil(GLint s = 0) {
				if (this->render_type_m != GL_STENCIL_ATTACHMENT_EXT) return;
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->object_id_m);
				glClearBufferfi(GL_STENCIL, 0, 0, s);
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object_id());
			}
		};
		using shared_render_buffer = std::shared_ptr<render_buffer>;
		// テクスチャバッファのためのオブジェクト
		class texture_buffer {
			friend class frame_buffer_object;

			shared_texture		tex_m;
			GLenum				texture_type_m;
			GLuint				object_id_m;			// フレームバッファのオブジェクトid

			texture_buffer(const texture_buffer&) = delete;
			texture_buffer& operator=(const texture_buffer&) = delete;

			texture_buffer() : object_id_m(0), texture_type_m(0) {}
			texture_buffer(GLuint obj_id, GLenum type, const shared_texture& tex) : tex_m(tex), texture_type_m(type), object_id_m(obj_id) {}
		public:
			texture_buffer(texture_buffer&& tex) noexcept : tex_m(tex.tex_m), texture_type_m(tex.texture_type_m), object_id_m(tex.object_id_m) {
				tex.texture_type_m = 0;
				tex.object_id_m = 0;
			}
			~texture_buffer() {}

			shared_texture tex_handle() const { return this->tex_m; }
			GLenum type() const { return this->texture_type_m; }

			texture_buffer& operator=(texture_buffer&& tex) noexcept {
				this->tex_m = tex.tex_m;
				this->texture_type_m = tex.texture_type_m;
				this->object_id_m = tex.object_id_m;
				tex.texture_type_m = 0;
				tex.object_id_m = 0;
			}

			// カラーバッファとしてクリア
			void clear_color(const normal_color<float>& c = normal_color<float>()) {
				if ((this->texture_type_m == GL_DEPTH_ATTACHMENT_EXT) || (this->texture_type_m == GL_STENCIL_ATTACHMENT_EXT)) return;
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->object_id_m);
				glClearBufferfv(GL_COLOR, this->texture_type_m - GL_COLOR_ATTACHMENT0_EXT, c.v);
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object_id());
			}
			// デプスバッファとしてクリア
			void clear_depth(GLfloat d = 1) {
				if (this->texture_type_m != GL_DEPTH_ATTACHMENT_EXT) return;
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->object_id_m);
				glClearBufferfi(GL_DEPTH, 0, d, 0);
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object_id());
			}
			// ステンシルバッファとしてクリア
			void clear_stencil(GLint s = 0) {
				if (this->texture_type_m != GL_STENCIL_ATTACHMENT_EXT) return;
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->object_id_m);
				glClearBufferfi(GL_STENCIL, 0, 0, s);
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object_id());
			}
		};
		using shared_texture_buffer = std::shared_ptr<texture_buffer>;

		// フレームバッファオブジェクト(全てのテクスチャバッファは同一のサイズであるべき)
		class frame_buffer_object {
			using texture_buffer_t = std::unordered_map<size_t, shared_texture_buffer>;
			using render_buffer_t = std::unordered_map<size_t, shared_render_buffer>;

			texture_buffer_t	texture_buffer_m;		// テクスチャバッファ
			render_buffer_t		render_buffer_m;		// レンダーバッファ
			GLuint				object_id_m;			// フレームバッファのオブジェクトid
			size_t				color_buffer_num_m;		// 利用中のカラーバッファの数

			size_t				width_m, height_m;		// フレームバッファにより用いられる最大サイズ

			// フレームバッファの設定のための定数
			static constexpr size_t color_buffer = GL_COLOR_ATTACHMENT0_EXT;			// カラーバッファ
			static constexpr size_t depth_buffer = GL_DEPTH_ATTACHMENT_EXT;				// デプスバッファ
			static constexpr size_t stencil_buffer = GL_STENCIL_ATTACHMENT_EXT;			// ステンシルバッファ

			frame_buffer_object(const frame_buffer_object&) = delete;
			frame_buffer_object& operator=(const frame_buffer_object&) = delete;

			// カラーバッファのセット
			void set_color_buffer() {
				// 描画するカラーバッファリストが存在しなくなるときはカラーバッファの書き込み禁止
				if (this->color_buffer_num_m == 0) glDrawBuffer(GL_NONE);
				else glDrawBuffers(this->color_buffer_num_m, texture_unit_control::color_buffer_list());
			}
		public:
			frame_buffer_object() : color_buffer_num_m(0) {
				glGenFramebuffersEXT(1, &this->object_id_m);
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->object_id_m);
				// デフォルトではカラーバッファの読み書き禁止
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object_id());
			}
			~frame_buffer_object() {
				glDeleteFramebuffersEXT(1, &this->object_id_m);
			}

			// FBOのID
			GLuint id() const { return this->object_id_m; }
			// このFBOがカレントであるかの判定
			bool is_current() const {
				return (this->object_id_m != 0) && (this->object_id_m == frame_buffer_object_id());
			}

			// サイズの取得
			size_t width() const { return this->width_m; }
			size_t height() const { return this->height_m; }

			// 全てのバッファを走査してFBOの有効領域の再設定
			void shrink_to_fit() {
				size_t temp_w = 0, temp_h = 0;
				for (auto& x : this->texture_buffer_m) {
					temp_w = (max)(temp_w, x.second->tex_handle()->width());
					temp_h = (max)(temp_h, x.second->tex_handle()->height());
				}
				for (auto& x : this->render_buffer_m) {
					temp_w = (max)(temp_w, x.second->width());
					temp_h = (max)(temp_h, x.second->height());
				}
				this->width_m = temp_w;
				this->height_m = temp_h;
			}

			// テクスチャバッファのアタッチ
			[[nodiscard]] pair<bool, shared_texture_buffer> attach(const shared_texture& tex) {
				if (tex.get() == nullptr) return pair<bool, shared_texture_buffer>(false, shared_texture_buffer());
				if (tex->id() == 0) return pair<bool, shared_texture_buffer>(false, shared_texture_buffer());

				// テクスチャのカラーフォーマットにより生成するテクスチャバッファの切り替えを行う
				switch (tex->format()) {
				case txc::rgba_texture:
				case txc::rgb_texture:
				{
					// これ以上カラーバッファを生成することができないとき
					if (this->color_buffer_num_m >= texture_unit_control::color_buffer_num()) return pair<bool, shared_texture_buffer>(false, shared_texture_buffer());
					size_t n = frame_buffer_object::color_buffer + this->color_buffer_num_m;
					++this->color_buffer_num_m;
					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->object_id_m);
					glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, n, GL_TEXTURE_2D, tex->id(), 0);
					this->set_color_buffer();
					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object_id());

					this->width_m = (max)(this->width_m, tex->width()); this->height_m = (max)(this->height_m, tex->height());
					return pair<bool, shared_texture_buffer>(true, this->texture_buffer_m[n] = shared_texture_buffer(new texture_buffer(this->object_id_m, n, tex)));
				}
				case txc::depth_texture:
					// すでにバッファがアタッチされているとき
					if ((this->texture_buffer_m.find(frame_buffer_object::depth_buffer) != this->texture_buffer_m.end())
						|| (this->render_buffer_m.find(frame_buffer_object::depth_buffer) != this->render_buffer_m.end())) {
						return pair<bool, shared_texture_buffer>(false, shared_texture_buffer());
					}
					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->object_id_m);
					glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object::depth_buffer, GL_TEXTURE_2D, tex->id(), 0);
					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object_id());
					
					this->width_m = (max)(this->width_m, tex->width()); this->height_m = (max)(this->height_m, tex->height());
					return pair<bool, shared_texture_buffer>(true, this->texture_buffer_m[frame_buffer_object::depth_buffer] = shared_texture_buffer(new texture_buffer(this->object_id_m, frame_buffer_object::depth_buffer, tex)));
				case txc::stencil_texture:
					// すでにバッファがアタッチされているとき
					if ((this->texture_buffer_m.find(frame_buffer_object::stencil_buffer) != this->texture_buffer_m.end())
						|| (this->render_buffer_m.find(frame_buffer_object::stencil_buffer) != this->render_buffer_m.end())) {
						return pair<bool, shared_texture_buffer>(false, shared_texture_buffer());
					}
					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->object_id_m);
					glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object::stencil_buffer, GL_TEXTURE_2D, tex->id(), 0);
					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object_id());
					
					this->width_m = (max)(this->width_m, tex->width()); this->height_m = (max)(this->height_m, tex->height());
					return pair<bool, shared_texture_buffer>(true, this->texture_buffer_m[frame_buffer_object::stencil_buffer] = shared_texture_buffer(new texture_buffer(this->object_id_m, frame_buffer_object::stencil_buffer, tex)));
				default:
					return pair<bool, shared_texture_buffer>(false, shared_texture_buffer());
				}
			}
			[[nodiscard]] pair<bool, shared_texture_buffer> attach(texture&& tex) {
				if (tex.id() == 0) return pair<bool, shared_texture_buffer>(false, shared_texture_buffer()); 

				// テクスチャのカラーフォーマットにより生成するテクスチャバッファの切り替えを行う
				switch (tex.format()) {
				case txc::rgba_texture:
				case txc::rgb_texture:
				{
					// これ以上カラーバッファを生成することができないとき
					if (this->color_buffer_num_m >= texture_unit_control::color_buffer_num()) return pair<bool, shared_texture_buffer>(false, shared_texture_buffer());
					size_t n = frame_buffer_object::color_buffer + this->color_buffer_num_m;
					++this->color_buffer_num_m;
					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->object_id_m);
					glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, n, GL_TEXTURE_2D, tex.id(), 0);
					this->set_color_buffer();
					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object_id());
					
					this->width_m = (max)(this->width_m, tex.width()); this->height_m = (max)(this->height_m, tex.height());
					return pair<bool, shared_texture_buffer>(true, this->texture_buffer_m[n] = shared_texture_buffer(new texture_buffer(this->object_id_m, n, shared_texture(new texture(std::move(tex))))));
				}
				case txc::depth_texture:
					// すでにバッファがアタッチされているとき
					if ((this->texture_buffer_m.find(frame_buffer_object::depth_buffer) != this->texture_buffer_m.end())
						|| (this->render_buffer_m.find(frame_buffer_object::depth_buffer) != this->render_buffer_m.end())) {
						return pair<bool, shared_texture_buffer>(false, shared_texture_buffer());
					}
					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->object_id_m);
					glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object::depth_buffer, GL_TEXTURE_2D, tex.id(), 0);
					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object_id());
					
					this->width_m = (max)(this->width_m, tex.width()); this->height_m = (max)(this->height_m, tex.height());
					return pair<bool, shared_texture_buffer>(true, this->texture_buffer_m[frame_buffer_object::depth_buffer] = shared_texture_buffer(new texture_buffer(this->object_id_m, frame_buffer_object::depth_buffer, shared_texture(new texture(std::move(tex))))));
				case txc::stencil_texture:
					// すでにバッファがアタッチされているとき
					if ((this->texture_buffer_m.find(frame_buffer_object::stencil_buffer) != this->texture_buffer_m.end())
						|| (this->render_buffer_m.find(frame_buffer_object::stencil_buffer) != this->render_buffer_m.end())) {
						return pair<bool, shared_texture_buffer>(false, shared_texture_buffer());
					}
					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->object_id_m);
					glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object::stencil_buffer, GL_TEXTURE_2D, tex.id(), 0);
					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object_id());
					
					this->width_m = (max)(this->width_m, tex.width()); this->height_m = (max)(this->height_m, tex.height());
					return pair<bool, shared_texture_buffer>(true, this->texture_buffer_m[frame_buffer_object::stencil_buffer] = shared_texture_buffer(new texture_buffer(this->object_id_m, frame_buffer_object::stencil_buffer, shared_texture(new texture(std::move(tex))))));
				default:
					return pair<bool, shared_texture_buffer>(false, shared_texture_buffer());
				}
			}

			// レンダーバッファのアタッチ
			[[nodiscard]] pair<bool, shared_render_buffer> attach_color(size_t w, size_t h, bool alpha = false) {
				// これ以上カラーバッファを生成することができないとき
				if (this->color_buffer_num_m >= texture_unit_control::color_buffer_num()) return pair<bool, shared_render_buffer>(false, shared_render_buffer());
				size_t n = frame_buffer_object::color_buffer + this->color_buffer_num_m;
				++this->color_buffer_num_m;
				shared_render_buffer temp(new render_buffer(this->object_id_m, n, w, h, alpha ? txc::rgba_texture : txc::rgb_texture));
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->object_id_m);
				glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, n, GL_RENDERBUFFER_EXT, temp->id());
				this->set_color_buffer();
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object_id());
				this->render_buffer_m[n] = temp;

				this->width_m = (max)(this->width_m, w); this->height_m = (max)(this->height_m, h);
				return pair<bool, shared_render_buffer>(true, temp);
			}
			[[nodiscard]] pair<bool, shared_render_buffer> attach_depth(size_t w, size_t h) {
				// すでにバッファがアタッチされているとき
				if ((this->texture_buffer_m.find(frame_buffer_object::depth_buffer) == this->texture_buffer_m.end())
					|| (this->render_buffer_m.find(frame_buffer_object::depth_buffer) == this->render_buffer_m.end())) {
					return pair<bool, shared_render_buffer>(false, shared_render_buffer());
				}
				shared_render_buffer temp(new render_buffer(this->object_id_m, frame_buffer_object::depth_buffer, w, h, txc::depth_texture));
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->object_id_m);
				glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object::depth_buffer, GL_RENDERBUFFER_EXT, temp->id());
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object_id());
				this->render_buffer_m[frame_buffer_object::depth_buffer] = temp;

				this->width_m = (max)(this->width_m, w); this->height_m = (max)(this->height_m, h);
				return pair<bool, shared_render_buffer>(true, temp);
			}
			[[nodiscard]] pair<bool, shared_render_buffer> attach_stencil(size_t w, size_t h) {
				// すでにバッファがアタッチされているとき
				if ((this->texture_buffer_m.find(frame_buffer_object::stencil_buffer) == this->texture_buffer_m.end())
					|| (this->render_buffer_m.find(frame_buffer_object::stencil_buffer) == this->render_buffer_m.end())) {
					return pair<bool, shared_render_buffer>(false, shared_render_buffer());
				}
				shared_render_buffer temp(new render_buffer(this->object_id_m, frame_buffer_object::stencil_buffer, w, h, txc::stencil_texture));
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->object_id_m);
				glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object::stencil_buffer, GL_RENDERBUFFER_EXT, temp->id());
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object_id());
				this->render_buffer_m[frame_buffer_object::stencil_buffer] = temp;
				
				this->width_m = (max)(this->width_m, w); this->height_m = (max)(this->height_m, h);
				return pair<bool, shared_render_buffer>(true, temp);
			}

			// バッファのデタッチ
			bool dettach(const shared_texture_buffer& tex) {
				if (tex.get() == nullptr) return false;
				if (tex->tex_handle().get() == nullptr) return false;
				if (tex->tex_handle()->id() == 0) return false;
				if (tex->object_id_m != this->object_id_m) return false;

				// バッファの種類によりデタッチ処理の切り替えを行う
				switch (tex->type()) {
				case frame_buffer_object::depth_buffer:
				case frame_buffer_object::stencil_buffer:
					// インスタンスが見つからないか一致しないならば処理を中断
					if (auto itr = this->texture_buffer_m.find(tex->type()); itr == this->texture_buffer_m.end()) return false;
					else if (itr->second->tex_handle()->id() != tex->tex_handle()->id()) return false;
					else {
						// インスタンスの破棄
						glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->object_id_m);
						glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, tex->type(), GL_TEXTURE_2D, 0, 0);
						this->texture_buffer_m.erase(itr);
						glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object_id());
					}
					break;
				default:
					// カラーバッファが有効でない
					if ((tex->type() >= frame_buffer_object::color_buffer) && (tex->type() < frame_buffer_object::color_buffer + this->color_buffer_num_m)) break;
					// インスタンスが見つからないか一致しないならば処理を中断
					if (auto itr = this->texture_buffer_m.find(tex->type()); itr == this->texture_buffer_m.end()) return false;
					else if (itr->second->tex_handle()->id() != tex->tex_handle()->id()) return false;
					else {
						--this->color_buffer_num_m;
						GLenum temp = tex->type();
						// インスタンスの破棄
						glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->object_id_m);
						glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, tex->type(), GL_TEXTURE_2D, 0, 0);
						this->texture_buffer_m.erase(itr);
						this->set_color_buffer();
						glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object_id());
						// tempよりも大きいtypeを1小さくする
						for (auto& x : this->texture_buffer_m) {
							if (x.second->texture_type_m > temp) --x.second->texture_type_m;
						}
						for (auto& x : this->render_buffer_m) {
							if (x.second->render_type_m > temp) --x.second->render_type_m;
						}
					}
					break;
				}
				return true;
			}
			bool dettach(const shared_render_buffer& tex) {
				if (tex.get() == nullptr) return false;
				if (tex->id() == 0) return false;
				if (tex->object_id_m != this->object_id_m) return false;

				// バッファの種類によりデタッチ処理の切り替えを行う
				switch (tex->type()) {
				case frame_buffer_object::depth_buffer:
				case frame_buffer_object::stencil_buffer:
					// インスタンスが見つからないか一致しないならば処理を中断
					if (auto itr = this->render_buffer_m.find(tex->type()); itr == this->render_buffer_m.end()) return false;
					else if (itr->second->id() != tex->id()) return false;
					else {
						// インスタンスの破棄
						glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->object_id_m);
						glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, tex->type(), GL_TEXTURE_2D, 0);
						this->render_buffer_m.erase(itr);
						glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object_id());
					}
					break;
				default:
					// カラーバッファが有効でない
					if ((tex->type() >= frame_buffer_object::color_buffer) && (tex->type() < frame_buffer_object::color_buffer + this->color_buffer_num_m)) break;
					// インスタンスが見つからないか一致しないならば処理を中断
					if (auto itr = this->render_buffer_m.find(tex->type()); itr == this->render_buffer_m.end()) return false;
					else if (itr->second->id() != tex->id()) return false;
					else {
						--this->color_buffer_num_m;
						GLenum temp = tex->type();
						// インスタンスの破棄
						glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->object_id_m);
						glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, tex->type(), GL_TEXTURE_2D, 0);
						this->render_buffer_m.erase(itr);
						this->set_color_buffer();
						glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object_id());
						// tempよりも大きいtypeを1小さくする
						for (auto& x : this->texture_buffer_m) {
							if (x.second->texture_type_m > temp) --x.second->texture_type_m;
						}
						for (auto& x : this->render_buffer_m) {
							if (x.second->render_type_m > temp) --x.second->render_type_m;
						}
					}
					break;
				}
				return true;
			}

			// テクスチャバッファが存在するならば取得
			[[nodiscard]] const shared_texture_buffer color(size_t n) const {
				if (n >= texture_unit_control::color_buffer_num()) return shared_texture_buffer();
				if (auto itr = this->texture_buffer_m.find(frame_buffer_object::color_buffer + n); itr != this->texture_buffer_m.end()) return itr->second;
				return shared_texture_buffer();
			}
			[[nodiscard]] shared_texture_buffer color(size_t n) {
				if (n >= texture_unit_control::color_buffer_num()) return shared_texture_buffer();
				if (auto itr = this->texture_buffer_m.find(frame_buffer_object::color_buffer + n); itr != this->texture_buffer_m.end()) return itr->second;
				return shared_texture_buffer();
			}
			[[nodiscard]] const shared_texture_buffer depth() const {
				if (auto itr = this->texture_buffer_m.find(frame_buffer_object::depth_buffer); itr != this->texture_buffer_m.end()) return itr->second;
				return shared_texture_buffer();
			}
			[[nodiscard]] shared_texture_buffer depth() {
				if (auto itr = this->texture_buffer_m.find(frame_buffer_object::depth_buffer); itr != this->texture_buffer_m.end()) return itr->second;
				return shared_texture_buffer();
			}
			[[nodiscard]] const shared_texture_buffer stencil() const {
				if (auto itr = this->texture_buffer_m.find(frame_buffer_object::stencil_buffer); itr != this->texture_buffer_m.end()) return itr->second;
				return shared_texture_buffer();
			}
			[[nodiscard]] shared_texture_buffer stencil() {
				if (auto itr = this->texture_buffer_m.find(frame_buffer_object::stencil_buffer); itr != this->texture_buffer_m.end()) return itr->second;
				return shared_texture_buffer();
			}

			// それぞれのバッファのクリア
			void clear() {
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->object_id_m);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object_id());
				return;
			}
		};
		inline FBO_OBJECT::FBO_OBJECT(const frame_buffer_object& fbo) : base_class(base_pair(fbo.id(), std::addressof(fbo))) {
			base_class::construct();
		}
		inline void FBO_OBJECT::push_state() {
			// フレームバッファが有効ならばスクリーン領域の変更
			if (this->id() != 0) {
				screen_rect::width_m = this->obj_m->second->width();
				screen_rect::height_m = this->obj_m->second->height();
			}
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->id());
		}
		inline void FBO_OBJECT::set_state() {
			if (this->id() != 0) {
				screen_rect::width_m = this->obj_m->second->width();
				screen_rect::height_m = this->obj_m->second->height();
			}
			else {
				screen_rect::width_m = screen_rect::base_width_m;
				screen_rect::height_m = screen_rect::base_height_m;
			}
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->id());
			// 便宜的に与えておく(FBO_OBJECTはVIEWPORT_OBJECTをもたせる)
			using enabler_object_control_t = typename VIEWPORT_OBJECT<float>::enabler_object_control_t;
			if (enabler_object_control_t::inst()->size() > 0) {
				auto& temp = viewport_rect<float>();
				glViewport(temp.left, temp.top, temp.right - temp.left, temp.bottom - temp.top);
			}
		}
		inline FBO_OBJECT CREATE_FBO_OBJECT(const frame_buffer_object& fbo) {
			return FBO_OBJECT(fbo);
		}
		// 現在のFBOの状態をobjにより変更
		inline void set_frame_buffer_object(const frame_buffer_object& fbo) {
			using enabler_object_control_t = typename FBO_OBJECT::enabler_object_control_t;
			enabler_object_control_t::inst()->top()->set(pair<GLuint, const frame_buffer_object*>(fbo.id(), std::addressof(fbo)));
		}
#define IMATHLIB_SCREEN_FBO(NAME, FBO)	IMATHLIB_ENABLER_OBJECT(auto NAME = ::iml::ml::CREATE_FBO_OBJECT(FBO))


		// 描画先のスクリーン(フレームバッファの指定)(nullptrで裏画面)
		inline void set_draw_framebuffer_object(const frame_buffer_object* fbo = nullptr) {
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, (fbo == nullptr) ? 0 : fbo->id());
		}

		// 色の設定
		inline void set_color(const normal_color<GLfloat>& color) { glColor4f(color.r, color.g, color.b, color.a); }
		inline void set_color(const normal_color<GLdouble>& color) { glColor4d(color.r, color.g, color.b, color.a); }
		inline void set_color(GLfloat r, GLfloat g, GLfloat b, GLfloat a) { glColor4f(r, g, b, a); }
		inline void set_color(GLfloat r, GLfloat g, GLfloat b) { glColor3f(r, g, b); }
		inline void set_color(GLdouble r, GLdouble g, GLdouble b, GLdouble a) { glColor4d(r, g, b, a); }
		inline void set_color(GLdouble r, GLdouble g, GLdouble b) { glColor3d(r, g, b); }


		// 各テクスチャのテクスチャ座標配列の登録(何故かシェーダーではこの情報が無効になる)
		// 同じ頂点上にテクスチャ(マルチテクスチャ)を描画(複数テクスチャの場合はシェーダーを用いるべき)
		/*inline void draw_texture2(const vertex2<double>& vtx, const frame_buffer_object_base* tex) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glVertexPointer(2, 0, vtx.vtx);			// 頂点の登録
			// フレームバッファオブジェクトのときは上下反転させる
			static const vertex2<double> tex_coord(0, 1, 1, 0);
			//static const vertex2<double> tex_coord(0, 0, 1, 1);
			glTexCoordPointer(2, 0, tex_coord.vtx);

			// テクスチャの有効
			glEnable(GL_TEXTURE_2D);
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			// 各テクスチャをバインドして描画
			glBindTexture(GL_TEXTURE_2D, tex->handle());
			glDrawArrays(GL_QUADS, 0, 4 * 1);

			// 0に戻してテクスチャの終了
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisable(GL_TEXTURE_2D);

			glDisable(GL_BLEND);
		}*/
		inline void draw_texture2(const vertex2<double>& vtx, const texture* tex) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glVertexPointer(2, 0, vtx.vtx);			// 頂点の登録
			static const vertex2<double> tex_coord(0, 0, 1, 1);
			glTexCoordPointer(2, 0, tex_coord.vtx);

			// テクスチャの有効
			glEnable(GL_TEXTURE_2D);
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			// 各テクスチャをバインドして描画
			glBindTexture(GL_TEXTURE_2D, tex->id());
			glDrawArrays(GL_QUADS, 0, 4 * 1);
			glBindTexture(GL_TEXTURE_2D, 0);

			// 0に戻してテクスチャの終了
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisable(GL_TEXTURE_2D);

			glDisable(GL_BLEND);
		}

		// 汎用ステートの有効
		template <size_t FirstIndex>
		inline void enable_state() {
			glEnable(FirstIndex);
		}
		template <size_t FirstIndex, size_t SecondIndex, size_t... Index>
		inline void enable_state() {
			glEnable(FirstIndex);
			enable_state<SecondIndex, Index...>();
		}
		// 汎用ステートの無効
		template <size_t FirstIndex>
		inline void disable_state() {
			glDisable(FirstIndex);
		}
		template <size_t FirstIndex, size_t SecondIndex, size_t... Index>
		inline void disable_state() {
			glDisable(FirstIndex);
			disable_state<SecondIndex, Index...>();
		}

		// アルファブレンド有効
		inline void enable_alpha_state() {
			enable_state<GL_BLEND>();
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		// アルファブレンド無効
		inline void disable_alpha_state() {
			disable_state<GL_BLEND>();
		}

		// テクスチャのバインド(tex_unitは描画に用いるテクスチャ番号)
		inline void bind_texture(const texture* tex) {
			glBindTexture(GL_TEXTURE_2D, tex->id());
		}
		/*inline void bind_texture(const frame_buffer_object_base* tex) {
			glBindTexture(GL_TEXTURE_2D, tex->handle());
		}*/
		// 頂点座標配列のセット(dimは次元)
		template <size_t Byte = 0>
		inline void set_vertex_array(size_t dim, const float* vtx) {
			::glVertexPointer(dim, GL_FLOAT, Byte, vtx);
		}
		template <size_t Byte = 0>
		inline void set_vertex_array(size_t dim, const double* vtx) {
			::glVertexPointer(dim, GL_DOUBLE, Byte, vtx);
		}

		// 配列のステート有効
		template <size_t FirstVtxIndex>
		inline void enable_array_state() {
			glEnableClientState(FirstVtxIndex);
		}
		template <size_t FirstVtxIndex, size_t SecondVtxIndex, size_t... VtxIndex>
		inline void enable_array_state() {
			glEnableClientState(FirstVtxIndex);
			enable_array_state<SecondVtxIndex, VtxIndex...>();
		}
		template <size_t... VtxIndex>
		inline void _Enable_array_state(index_tuple<size_t, VtxIndex...>) { enable_array_state<VtxIndex...>(); }
		// 配列のステート無効
		template <size_t FirstVtxIndex>
		inline void disable_array_state() {
			glDisableClientState(FirstVtxIndex);
		}
		template <size_t FirstVtxIndex, size_t SecondVtxIndex, size_t... VtxIndex>
		inline void disable_array_state() {
			glDisableClientState(FirstVtxIndex);
			disable_array_state<SecondVtxIndex, VtxIndex...>();
		}
		template <size_t... VtxIndex>
		inline void _Disable_array_state(index_tuple<size_t, VtxIndex...>) { disable_array_state<VtxIndex...>(); }

		// テクスチャの描画(VtxIndexTupleは有効にするステート)
		template <size_t Mode, class VtxIndexTuple>
		inline void draw_texture(size_t first, size_t cnt) {
			_Enable_array_state(VtxIndexTuple());

			enable_state<GL_TEXTURE_2D>();
			glDrawArrays(Mode, first, cnt);
			disable_state<GL_TEXTURE_2D>();

			_Disable_array_state(VtxIndexTuple());
		}
		template <size_t Mode, class VtxIndexTuple, class Texture>
		inline void draw_texture(Texture tex, size_t first, size_t cnt) {
			glBindTexture(GL_TEXTURE_2D, tex->id());
			draw_texture<Mode, VtxIndexTuple>(first, cnt);
		}
		template <size_t Mode, class VtxIndexTuple, size_t VtxByte, size_t CoordByte, class Texture, class Float>
		inline void draw_texture(size_t dim, Texture tex, const Float* vtx, const Float* coord, size_t first, size_t cnt) {
			glVertexPointer(dim, VtxByte, vtx);
			glTexCoordPointer(2, CoordByte, coord);

			glBindTexture(GL_TEXTURE_2D, tex->id());

			_Enable_array_state(VtxIndexTuple());

			enable_state<GL_TEXTURE_2D>();
			glDrawArrays(Mode, first, cnt);
			disable_state<GL_TEXTURE_2D>();

			glBindTexture(GL_TEXTURE_2D, 0);

			_Disable_array_state(VtxIndexTuple());
		}

		// インデックス指定verのテクスチャ描画
		/*template <size_t Mode, class VtxIndexTuple>
		inline void draw_texture(const unsigned int* index, size_t cnt) {
			_Enable_array_state(VtxIndexTuple());

			enable_state<GL_TEXTURE_2D>();
			glDrawElements(Mode, cnt, GL_UNSIGNED_INT, index);
			disable_state<GL_TEXTURE_2D>();

			_Disable_array_state(VtxIndexTuple());
		}
		template <size_t Mode, class VtxIndexTuple>
		inline void draw_texture(const unsigned short* index, size_t cnt) {
			_Enable_array_state(VtxIndexTuple());

			enable_state<GL_TEXTURE_2D>();
			glDrawElements(Mode, cnt, GL_UNSIGNED_SHORT, index);
			disable_state<GL_TEXTURE_2D>();

			_Disable_array_state(VtxIndexTuple());
		}
		template <size_t Mode, class VtxIndexTuple>
		inline void draw_texture(const unsigned char* index, size_t cnt) {
			_Enable_array_state(VtxIndexTuple());

			enable_state<GL_TEXTURE_2D>();
			glDrawElements(Mode, cnt, GL_UNSIGNED_BYTE, index);
			disable_state<GL_TEXTURE_2D>();

			_Disable_array_state(VtxIndexTuple());
		}
		template <size_t Mode, class VtxIndexTuple, class Texture, class IndexType>
		inline void draw_texture(Texture tex, const IndexType* index, size_t cnt) {
			bind_texture(tex);
			draw_texture<Mode, VtxIndexTuple>(index, cnt);
		}*/

		// 図形描画
		template <size_t Mode, class VtxIndexTuple>
		inline void draw_figure(size_t first, size_t cnt) {
			_Enable_array_state(VtxIndexTuple());

			glDrawArrays(Mode, first, cnt);

			_Disable_array_state(VtxIndexTuple());
		}
		template <size_t Mode, class VtxIndexTuple, size_t VtxByte, class Float>
		inline void draw_figure(size_t dim, const Float* vtx, size_t first, size_t cnt) {
			set_vertex_array<VtxByte>(dim, vtx);
			draw_figure<Mode, VtxIndexTuple>(first, cnt);
		}
		// インデックス指定verの図形描画
		/*template <size_t Mode, class VtxIndexTuple>
		inline void draw_figure(const unsigned int* index, size_t cnt) {
			_Enable_array_state(VtxIndexTuple());

			glDrawElements(Mode, cnt, GL_UNSIGNED_INT, index);

			_Disable_array_state(VtxIndexTuple());
		}
		template <size_t Mode, class VtxIndexTuple>
		inline void draw_figure(const unsigned short* index, size_t cnt) {
			_Enable_array_state(VtxIndexTuple());

			glDrawElements(Mode, cnt, GL_UNSIGNED_SHORT, index);

			_Disable_array_state(VtxIndexTuple());
		}
		template <size_t Mode, class VtxIndexTuple>
		inline void draw_figure(const unsigned char* index, size_t cnt) {
			_Enable_array_state(VtxIndexTuple());

			glDrawElements(Mode, cnt, GL_UNSIGNED_BYTE, index);

			_Disable_array_state(VtxIndexTuple());
		}*/


		// 以後基本図形

		// 点の描画
		template <class T, size_t N>
		inline void draw_point(const vector<T, N>& p, GLfloat size = 1) {
			glPointSize(size);
			glEnable(GL_POINT_SMOOTH);
			glHint(GL_POINT_SMOOTH_HINT, GL_DONT_CARE);
			draw_figure<GL_POINTS, index_tuple<size_t, GL_VERTEX_ARRAY>, 0>(N, &p[0], 0, 1);
			glDisable(GL_POINT_SMOOTH);
		}
		// 線の描画
		template <class T, size_t N>
		inline void draw_line(const vector<T, N>& start, const vector<T, N>& end, GLfloat size = 1) {
			vector<T, N> temp[2] = { start,end };
			glLineWidth(size);
			glEnable(GL_LINE_SMOOTH);		//アンチエイリアス(デプスが無効になる)
			glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
			draw_figure<GL_LINES, index_tuple<size_t, GL_VERTEX_ARRAY>, sizeof(vector<T, N>)>(N, &temp[0][0], 0, 2);
			glDisable(GL_LINE_SMOOTH);
		}
		// 三角形ポリゴンの描画
		template <class T, size_t N>
		inline void draw_triangle(const vector<T, N>& v1, const vector<T, N>& v2, const vector<T, N>& v3) {
			vector<T, N> temp[3] = { v1,v2,v3 };
			glEnable(GL_POLYGON_SMOOTH);
			glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);
			draw_figure<GL_POLYGON, index_tuple<size_t, GL_VERTEX_ARRAY>, sizeof(vector<T, N>)>(N, &temp[0][0], 0, 3);
			glDisable(GL_POLYGON_SMOOTH);
		}
		// 円ポリゴンの描画
		template <class T, size_t N>
		inline void draw_circle(const vector<T, N>& v, const T& r, size_t n = 30) {
			std::vector<T> vertex;
			vertex.reserve(n * 2);
			// xy平面上の円を計算
			for (size_t i = 0; i < n; ++i) {
				//x = r * cos(2 * pi<T> * ((double)i / n));
				//y = r * sin(2 * pi<T> * ((double)i / n));
				vertex.push_back(v[0] + r * cos(2 * pi<T> * ((double)i / n)));
				vertex.push_back(v[1] + r * sin(2 * pi<T> * ((double)i / n)));
			}
			glEnable(GL_POLYGON_SMOOTH);
			glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);
			draw_figure<GL_POLYGON, index_tuple<size_t, GL_VERTEX_ARRAY>, 0>(2, vertex.data(), 0, n);
			glDisable(GL_POLYGON_SMOOTH);
		}


		// 2次元図形の描画のためのオブジェクト
		template <class Float>
		struct drawer_for_figure2 {
			std::vector<Float>	vertex_m;			// 頂点座標
			GLuint				mode_m;				// 描画方法(GL_POLYGON等)


			drawer_for_figure2() : mode_m(0) {}
			drawer_for_figure2(GLuint mode, const std::vector<Float>& vtx) : vertex_m(vtx), mode_m(mode) {}
			drawer_for_figure2(GLuint mode, std::vector<Float>&& vtx) : vertex_m(vtx), mode_m(mode) {}

			// 平行移動
			drawer_for_figure2& shift(const Float& x, const Float& y) {
				for (size_t i = 0; i < this->vertex_m.size(); i += 2) {
					this->vertex_m[i] += x;
					this->vertex_m[i + 1] += y;
				}
				return *this;
			}
			drawer_for_figure2& shift(const vector2<Float>& v) {
				return this->shift(v[0], v[1]);
			}
			drawer_for_figure2& shift_x(const Float& x) {
				for (size_t i = 0; i < this->vertex_m.size(); i += 2) this->vertex_m[i] += x;
				return *this;
			}
			drawer_for_figure2& shift_y(const Float& y) {
				for (size_t i = 0; i < this->vertex_m.size(); i += 2) this->vertex_m[i + 1] += y;
				return *this;
			}

			// 回転
			// centerを中心に回転(y方向が下であることに留意)
			drawer_for_figure2& rotate(const Float& radian, const Float& center_x, const Float& center_y) {
				auto s = sin(radian);
				auto c = cos(radian);
				for (size_t i = 0; i < this->vertex_m.size(); i += 2) {
					auto temp_x = c * (this->vertex_m[i] - center_x) - s * (this->vertex_m[i + 1] - center_y);
					auto temp_y = s * (this->vertex_m[i] - center_x) + c * (this->vertex_m[i + 1] - center_y);
					this->vertex_m[i] = temp_x + center_x;
					this->vertex_m[i + 1] = temp_y + center_y;
				}
				return *this;
			}
			drawer_for_figure2& rotate(const Float& radian, const vector2<Float>& center) {
				return this->rotate(radian, center[0], center[1]);
			}

			// fにより任意に動かす
			template <class F>
			drawer_for_figure2& move(F f) {
				for (size_t i = 0; i < this->vertex_m.size(); i += 2) f(this->vertex_m[i], this->vertex_m[i + 1]);
				return *this;
			}
			template <class F>
			drawer_for_figure2& move_x(F f) {
				for (size_t i = 0; i < this->vertex_m.size(); i += 2) f(this->vertex_m[i]);
				return *this;
			}
			template <class F>
			drawer_for_figure2& move_y(F f) {
				for (size_t i = 0; i < this->vertex_m.size(); i += 2) f(this->vertex_m[i + 1]);
				return *this;
			}


			// 描画直前に指定するべきメソッド

			// 色
			drawer_for_figure2& color(const Float& r, const Float& g, const Float& b, const Float& a) { set_color(r, g, b, a); return *this; }
			drawer_for_figure2& color(const color_rgba& c) { set_color(Float(c.r) / 255, Float(c.g) / 255, Float(c.b) / 255, Float(c.a) / 255); return *this; }
			drawer_for_figure2& color(const Float& r, const Float& g, const Float& b) { set_color(r, g, b); return *this; }
			drawer_for_figure2& color(const color_rgb& c) { set_color(Float(c.r) / 255, Float(c.g) / 255, Float(c.b) / 255); return *this; }
			drawer_for_figure2& color(const normal_color<Float>& c) { return this->color(c.r, c.g, c.b, c.a); }
			// 大きさ
			drawer_for_figure2& size(const Float& s) {
				switch (this->mode_m) {
				case GL_POINTS:
					glPointSize(s);
					break;
				case GL_LINES:
				case GL_LINE_STRIP:
				case GL_LINE_LOOP:
					glLineWidth(s);
					break;
				}
				return *this;
			}

			// 描画
			void draw() {
				glVertexPointer(2, 0, this->vertex_m.data());
				glEnableClientState(GL_VERTEX_ARRAY);
				glDrawArrays(this->mode_m, 0, this->vertex_m.size() / 2);
				glDisableClientState(GL_VERTEX_ARRAY);
			}

			// モードの切り替え
			drawer_for_figure2& border() {
				this->mode_m = GL_LINE_STRIP;
				return *this;
			}
			drawer_for_figure2& polygon() {
				this->mode_m = GL_POLYGON;
				return *this;
			}
		};

		// 線分の描画のためのオブジェクトの生成
		template <class Float>
		inline drawer_for_figure2<Float> line(const vector2<Float>& v1, const vector2<Float>& v2) {
			std::vector<Float> temp = { v1[0], v1[1], v2[0], v2[1] };
			return drawer_for_figure2<Float>(GL_LINES, std::move(temp));
		}
		// 三角形の描画のためのオブジェクトの生成
		template <class Float>
		inline drawer_for_figure2<Float> triangle(const vector2<Float>& v1, const vector2<Float>& v2, const vector2<Float>& v3) {
			std::vector<Float> temp = { v1[0], v1[1], v2[0], v2[1], v3[0], v3[1] };
			return drawer_for_figure2<Float>(GL_POLYGON, std::move(temp));
		}
		// 長方形の描画のためのオブジェクトの生成(左上,右下)
		template <class Float>
		inline drawer_for_figure2<Float> rectangle(const vector2<Float>& v1, const vector2<Float>& v2) {
			std::vector<Float> temp;
			temp.reserve(8);
			temp.push_back(v1[0]); temp.push_back(v1[1]);
			temp.push_back(v1[0]); temp.push_back(v2[1]);
			temp.push_back(v2[0]); temp.push_back(v2[1]);
			temp.push_back(v2[0]); temp.push_back(v1[1]);
			return drawer_for_figure2<Float>(GL_POLYGON, std::move(temp));
		}
		// 角丸長方形の描画のためのオブジェクトの生成(左上,右下,半径,1角あたりの分割数)
		template <class Float>
		inline drawer_for_figure2<Float> rounded_rectangle(const vector2<Float>& v1, const vector2<Float>& v2, const Float r, size_t n = 10) {
			std::vector<Float> temp;
			temp.reserve(n * 8 + 8);
			size_t sum_n = n * 4;
			size_t i = 0;
			// y方向が下であることに留意
			// 右下の角
			for (; i <= n; ++i) {
				temp.push_back(v2[0] - r + r * cos(2 * pi<Float> * (Float(i) / sum_n)));
				temp.push_back(v2[1] - r + r * sin(2 * pi<Float> * (Float(i) / sum_n)));
			}
			// 左下の角
			for (; i <= 2 * n; ++i) {
				temp.push_back(v1[0] + r + r * cos(2 * pi<Float> * (Float(i) / sum_n)));
				temp.push_back(v2[1] - r + r * sin(2 * pi<Float> * (Float(i) / sum_n)));
			}
			// 左上の角
			for (; i <= 3 * n; ++i) {
				temp.push_back(v1[0] + r + r * cos(2 * pi<Float> * (Float(i) / sum_n)));
				temp.push_back(v1[1] + r + r * sin(2 * pi<Float> * (Float(i) / sum_n)));
			}
			// 右上の角
			for (; i <= sum_n; ++i) {
				temp.push_back(v2[0] - r + r * cos(2 * pi<Float> * Float(i) / sum_n));
				temp.push_back(v1[1] + r + r * sin(2 * pi<Float> * Float(i) / sum_n));
			}
			return drawer_for_figure2<Float>(GL_POLYGON, std::move(temp));
		}
		// 上角丸長方形の描画のためのオブジェクトの生成(左上,右下,半径,1角あたりの分割数)
		template <class Float>
		inline drawer_for_figure2<Float> upper_rounded_rectangle(const vector2<Float>& v1, const vector2<Float>& v2, const Float r, size_t n = 10) {
			std::vector<Float> temp;
			temp.reserve(n * 4 + 4 + 4);
			size_t sum_n = n * 4;
			size_t i = 2 * n;
			// y方向が下であることに留意
			// 右下の角
			temp.push_back(v2[0]); temp.push_back(v2[1]);
			// 左下の角
			temp.push_back(v1[0]); temp.push_back(v2[1]);
			// 左上の角
			for (; i <= 3 * n; ++i) {
				temp.push_back(v1[0] + r + r * cos(2 * pi<Float> * (Float(i) / sum_n)));
				temp.push_back(v1[1] + r + r * sin(2 * pi<Float> * (Float(i) / sum_n)));
			}
			// 右上の角
			for (; i <= sum_n; ++i) {
				temp.push_back(v2[0] - r + r * cos(2 * pi<Float> * Float(i) / sum_n));
				temp.push_back(v1[1] + r + r * sin(2 * pi<Float> * Float(i) / sum_n));
			}
			return drawer_for_figure2<Float>(GL_POLYGON, std::move(temp));
		}
		// 角丸長方形の描画のためのオブジェクトの生成(左上,右下,半径,1角あたりの分割数)
		template <class Float>
		inline drawer_for_figure2<Float> lower_rounded_rectangle(const vector2<Float>& v1, const vector2<Float>& v2, const Float r, size_t n = 10) {
			std::vector<Float> temp;
			temp.reserve(n * 4 + 4 + 4);
			size_t sum_n = n * 4;
			size_t i = 0;
			// y方向が下であることに留意
			// 右下の角
			for (; i <= n; ++i) {
				temp.push_back(v2[0] - r + r * cos(2 * pi<Float> * (Float(i) / sum_n)));
				temp.push_back(v2[1] - r + r * sin(2 * pi<Float> * (Float(i) / sum_n)));
			}
			// 左下の角
			for (; i <= 2 * n; ++i) {
				temp.push_back(v1[0] + r + r * cos(2 * pi<Float> * (Float(i) / sum_n)));
				temp.push_back(v2[1] - r + r * sin(2 * pi<Float> * (Float(i) / sum_n)));
			}
			// 左上の角
			temp.push_back(v1[0]); temp.push_back(v1[1]);
			// 右上の角
			temp.push_back(v2[0]); temp.push_back(v1[1]);
			return drawer_for_figure2<Float>(GL_POLYGON, std::move(temp));
		}
		// 円の描画のためのオブジェクトの生成
		template <class Float>
		inline drawer_for_figure2<Float> circle(const vector2<Float>& v, const Float& r, size_t n = 30) {
			std::vector<Float> temp;
			temp.reserve(n * 2 + 2);
			for (size_t i = 0; i <= n; ++i) {
				//x = r * cos(2 * pi<T> * ((double)i / n));
				//y = r * sin(2 * pi<T> * ((double)i / n));
				temp.push_back(v[0] + r * cos(2 * pi<Float> * (Float(i) / n)));
				temp.push_back(v[1] + r * sin(2 * pi<Float> * (Float(i) / n)));
			}
			return drawer_for_figure2<Float>(GL_POLYGON, std::move(temp));
		}
	}
}

#endif