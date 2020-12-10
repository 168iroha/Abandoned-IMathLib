#ifndef IMATHLIB_H_MEDIA_IMAGE_IMAGE_HPP
#define IMATHLIB_H_MEDIA_IMAGE_IMAGE_HPP

#include "IMathLib/media/image/color.hpp"
#include "IMathLib/encryption/zLib.hpp"
#include <list>

// 画像入出力


namespace iml {


	// 0～255のargbカラー(32bitカラー)
	/*union color_32bit {
	private:
		struct _Rgba_color {
			unsigned char b;
			unsigned char g;
			unsigned char r;
			unsigned char a;
			constexpr _Rgba_color() :a(255), b(255), g(255), r(255) {}
			constexpr _Rgba_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : a(a), b(b), g(g), r(r) {}
		};
	public:
		uint32_t code;			// カラーコード
		color_rgb c;
	private:
		// アルファを考慮した色の合成(f:合成関数)
		template <class F>
		static void _Color_mix(color_32bit& c1, const color_32bit& c2, F f) {
			unsigned char temp[3];

			temp[0] = c1.c.a*c2.c.a / 255;
			temp[1] = c2.c.a*(255 - c1.c.a) / 255;
			temp[2] = c1.c.a*(255 - c2.c.a) / 255;

			c1.c.a = temp[0] + temp[1] + temp[2];
			c1.c.r = (f(c1.c.r, c2.c.r)*temp[0] + temp[1] * c2.c.r + temp[2] * c1.c.r) / c1.c.a;
			c1.c.g = (f(c1.c.g, c2.c.g)*temp[0] + temp[1] * c2.c.g + temp[2] * c1.c.g) / c1.c.a;
			c1.c.b = (f(c1.c.b, c2.c.b)*temp[0] + temp[1] * c2.c.b + temp[2] * c1.c.b) / c1.c.a;
		}
		// 乗算
		static unsigned char __multiplication(unsigned char c1, unsigned char c2) {
			return (c1*c2) / 255;
		}
		// 除算
		static unsigned char __division(unsigned char c1, unsigned char c2) {
			return (c1 * 256) / (c2 + 1);
		}
		// 加算(覆い焼きリニア)
		static unsigned char _addition(unsigned char c1, unsigned char c2) {
			return (iml::min<int>)(c1 + c2, 255);
		}
		// 減算
		static unsigned char __subtraction(unsigned char c1, unsigned char c2) {
			return (c1 < c2) ? 0 : c1 - c2;
		}
	public:
		constexpr color_32bit() :c() {}
		explicit constexpr color_32bit(__int32 v) :code(v) {}
		constexpr color_32bit(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) :c(r, g, b, a) {}

		color_32bit& operator+=(const color_32bit& c_v) {
			_Color_mix(*this, c_v, _addition);
			return *this;
		}
		color_32bit& operator-=(const color_32bit& c_v) {
			_Color_mix(*this, c_v, __subtraction);
			return *this;
		}
		color_32bit& operator*=(const color_32bit& c_v) {
			_Color_mix(*this, c_v, __multiplication);
			return *this;
		}
		template <class T>
		color_32bit& operator*=(const T& k) {
			this->c.r *= k;
			this->c.g *= k;
			this->c.b *= k;
			return *this;
		}
		color_32bit& operator/=(const color_32bit& c_v) {
			_Color_mix(*this, c_v, __division);
			return *this;
		}
		template <class T>
		color_32bit& operator/=(const T& k) {
			this->c.r /= k;
			this->c.g /= k;
			this->c.b /= k;
			return *this;
		}

		color_32bit& operator=(const color_32bit& c_v) {
			this->code = c_v.code;
			return *this;
		}
	};
	// 二項演算子
	inline color_32bit operator+(const color_32bit& c1, const color_32bit& c2) {
		color_32bit temp(c1);
		return temp += c2;
	}
	inline color_32bit operator-(const color_32bit& c1, const color_32bit& c2) {
		color_32bit temp(c1);
		return temp -= c2;
	}
	inline color_32bit operator*(const color_32bit& c1, const color_32bit& c2) {
		color_32bit temp(c1);
		return temp *= c2;
	}
	template <class T>
	inline color_32bit operator*(const T& k, const color_32bit& c) {
		color_32bit temp(c);
		return temp *= k;
	}
	template <class T>
	inline color_32bit operator*(const color_32bit& c, const T& k) {
		color_32bit temp(c);
		return temp *= k;
	}
	inline color_32bit operator/(const color_32bit& c1, const color_32bit& c2) {
		color_32bit temp(c1);
		return temp /= c2;
	}
	template <class T>
	inline color_32bit operator/(const color_32bit& c, const T& k) {
		color_32bit temp(c);
		return temp /= k;
	}*/


	namespace img {

		// 画像の構造体(3バイト以上のカラー構造体でビット深度は8のみをサポート)
		struct image {
		private:
			// 4の倍数に切り上げるの関数
			static size_t ceil4(size_t n) {
				return (n & 3) ? (((n >> 2) + 1) << 2) : (n);
			}
		public:
			const size_t w, h;				// 幅、高さ
			size_t real_w;					// 実際の幅のビット長(4の倍数)
			size_t color;						// 色数(3～で最初の3色はRGBで基本的に3か4のみ)
			unsigned char* pixels;				// データ配列(左上から右下へ[R,G,B,...])

			image(size_t width, size_t height, size_t n)
				: w(width), real_w(ceil4(width*n)), h(height), color(n), pixels(nullptr) {}
			~image() { if (pixels != nullptr) delete[] pixels; }

			// メモリ確保(成功でtrue)
			bool reserve() {
				pixels = new unsigned char[real_w*h]{};
				return (pixels != nullptr);
			}

			// 任意のピクセルに対するアクセサ
			const unsigned char* operator()(size_t width, size_t height) const { return pixels + height*(real_w) + width*color; }
			unsigned char* operator()(size_t width, size_t height) { return pixels + height*(real_w) + width*color; }

			// 矩形領域の構造体
			struct rect {
				size_t  left, right, bottom, top;
			};

			// 補間法を示すの定数
			static const size_t nearest = 0;				// 最近傍補間
			static const size_t linear = 1;				// 双線形補間
		};

		// BMPの読み込み(カラーパレット未対応)
		inline image* load_bmp(const unsigned char* p) {
			const unsigned char* img_p = &p[54];			// 画像データ行開始地点のポインタ
			image* img;

			// BMPシグネチャのチェック
			if (!((p[0] == 'B') && (p[1] == 'M'))) return nullptr;
			// 初期化(ビットマップは3色設定とする)
			img = new image(*reinterpret_cast<const unsigned int*>(&p[18])
				, *reinterpret_cast<const unsigned int*>(&p[22]), 3);
			if (!img->reserve()) { delete img; return nullptr; }
			// 色ビット数で分岐
			switch (*reinterpret_cast<const unsigned int*>(&p[28])) {
			case 1:			// 2色
			case 4:			// 16色
			case 8:			// 256色
				// これらはまだ対応しない(カラーパレット周辺が面倒)
				delete img;
				return nullptr;
			case 24:
				// 全てのデータでRBを交換しながらコピーする
				for (size_t i = 0; i < img->h; ++i) {
					for (size_t j = 0; j < img->w; ++j) {
						(*img)(j, img->h - i - 1)[0] = img_p[j * 3 + 2];			// R
						(*img)(j, img->h - i - 1)[1] = img_p[j * 3 + 1];			// G
						(*img)(j, img->h - i - 1)[2] = img_p[j * 3];				// B
					}
					// 行の更新
					img_p += img->real_w;
				}
				break;
			case 32:
				// 予約領域を無視しながら読み込む
				for (size_t i = 0; i < img->h; ++i) {
					for (size_t j = 0; j < img->w; ++j) {
						(*img)(j, img->h - i - 1)[0] = img_p[j * 4 + 2];			// R
						(*img)(j, img->h - i - 1)[1] = img_p[j * 4 + 1];			// G
						(*img)(j, img->h - i - 1)[2] = img_p[j * 4];				// B
					}
					// 行の更新
					img_p += img->real_w;
				}
				break;
			default:
				delete img;
				return nullptr;
			}

			return img;
		}

		// PNGの読み込み(p:ビット配列)
		inline image* load_png(const unsigned char* p) {
			const unsigned char* img_p;
			unsigned int chunk_size;
			size_t comp_size = 0;			// 圧縮データのサイズ
			unsigned char *comp_img;		// 圧縮画像データ配列
			unsigned char interlace;		// インターレース手法
			unsigned char filter;			// フィルター手法
			image* img;
			std::list<pair<unsigned int, const unsigned char*>> img_index;	// 画像データのチャンクが複数ある場合を想定したインデックス

			// reinterpret_castではメモリ配置が逆になるときのuintへのキャスト
			static const auto unsigned_int_cast = [](const unsigned char* p) -> unsigned int {
				return (p[0] << 24) + (p[1] << 16) + (p[2] << 8) + (p[3]);
			};

			// 以下フィルタ解除の関数(そのうち整理する)

			// paeth predictorによる色の選択
			static const auto paeth_predictor = [](unsigned char a, unsigned char b, unsigned char c) -> unsigned char {
				int_t p = a + b - c;
				int_t pa = abs(p - a);
				int_t pb = abs(p - b);
				int_t pc = abs(p - c);

				//  横向きのほうがなだらかな値の変化 → 左
				if ((pa <= pb) && (pa <= pc)) return a;
				//  縦向きのほうがなだらかな値の変化 → 上
				if (pb <= pc) return b;
				//  縦横それぞれ正反対に値が変化するため中間色を選択 → 左上        
				return c;
			};
			// 差分の復元
			static const auto restore_difference = [](unsigned char a, unsigned char b) -> unsigned char {
				return (a + b) & 0xFF;
			};
			// 上の色の取得
			static const auto get_up = [](image* img, size_t width, size_t height, size_t k) -> unsigned char {
				return (height == 0) ? 0 : (*img)(width, height - 1)[k];
			};
			// 左の色の取得
			static const auto get_left = [](image* img, size_t width, size_t height, size_t k) -> unsigned char {
				return (width == 0) ? 0 : (*img)(width - 1, height)[k];
			};
			// 左上の色の取得
			static const auto get_up_left = [](image* img, size_t width, size_t height, size_t k) -> unsigned char {
				return ((width == 0) || (height == 0)) ? 0 : (*img)(width - 1, height - 1)[k];
			};

			// PNGシグネチャのチェック
			if (!((p[0] == 0x89) && (p[1] == 'P') && (p[2] == 'N') && (p[3] == 'G')
				&& (p[4] == '\r') && (p[5] == '\n') && (p[6] == 0x1A) && (p[7] == '\n'))) return nullptr;
			img_p = &p[8];
			// イメージヘッダ
			img = new image(unsigned_int_cast(&img_p[8])
				, unsigned_int_cast(&img_p[12]), 3 + ((img_p[17] & 4) == 4));
			filter = img_p[19];
			interlace = img_p[20];
			img_p += 25;

			// チャンク読み込み
			while (1) {
				chunk_size = unsigned_int_cast(&img_p[0]);
				if ((img_p[4] == 'I') && (img_p[5] == 'E') && (img_p[6] == 'N') && (img_p[7] == 'D')) break;
				// 画像データ(複数ある可能性がある(このとき連続で存在))
				else if ((img_p[4] == 'I') && (img_p[5] == 'D') && (img_p[6] == 'A') && (img_p[7] == 'T')) {
					img_index.push_back(pair<unsigned int, const unsigned char*>(chunk_size, &img_p[8]));
					comp_size += chunk_size;
				}

				// 次のチャンクを読み込むために進める
				img_p += chunk_size + 12;
			}
			// 圧縮画像データを結合
			comp_img = new unsigned char[comp_size]; comp_size = 0;
			for (auto itr = img_index.begin(); itr != img_index.end(); ++itr) {
				// copy_order(&comp_img[comp_size], itr->second, itr->first);
				for (size_t i = 0; i < itr->first; ++i)
					comp_img[comp_size + i] = itr->second[i];
				comp_size += itr->first;
			}

			auto decode = zlib::decode(comp_img, comp_size);
			
			// データのフィルタリングを解いて代入
			img_p = &decode[0];
			if (!img->reserve()) { delete img; return nullptr; }
			for (size_t i = 0; i < img->h; ++i) {
				// フィルタータイプの取得と逆フィルタリングをして代入
				switch ((img_p++)[0]) {
					// None Filer
				case 0:
					// そのまま代入
					for (size_t j = 0; j < img->w; ++j)
						for (size_t k = 0; k < img->color; ++k)
							(*img)(j, i)[k] = img_p[j * img->color + k];
					break;
					// Sub Filter
				case 1:
					// 左との差のため左を足す
					for (size_t j = 0; j < img->w; ++j)
						for (size_t k = 0; k < img->color; ++k)
							(*img)(j, i)[k] = restore_difference(get_left(img, j, i, k), img_p[j * img->color + k]);
					break;
					// Up Filter
				case 2:
					// 上との差のため上を足す
					for (size_t j = 0; j < img->w; ++j)
						for (size_t k = 0; k < img->color; ++k)
							(*img)(j, i)[k] = restore_difference(get_up(img, j, i, k), img_p[j * img->color + k]);
					break;
					// Average Filter
				case 3:
					// 左と上の平均の差のため左と上の平均を足す
					for (size_t j = 0; j < img->w; ++j)
						for (size_t k = 0; k < img->color; ++k)
							(*img)(j, i)[k] = restore_difference((get_left(img, j, i, k) + get_up(img, j, i, k)) / 2
								, img_p[j * img->color + k]);
					break;
					// Paeth Filter
				case 4:
					// paeth_predictorによる値が引かれているためそれを足す
					for (size_t j = 0; j < img->w; ++j)
						for (size_t k = 0; k < img->color; ++k)
							(*img)(j, i)[k] = restore_difference(paeth_predictor(get_left(img, j, i, k), get_up(img, j, i, k), get_up_left(img, j, i, k))
								, img_p[j * img->color + k]);
					break;
				default:
					throw std::runtime_error("フィルタがおかしい");
				}
				// 行の更新
				img_p += img->real_w;
			}

			return img;
		}

		// 画像データのロード(シグネチャにより分岐)
		inline image* load_image(const unsigned char* p) {

			if ((p[0] == 'B') && (p[1] == 'M')) return load_bmp(p);
			else if ((p[0] == 0x89) && (p[1] == 'P') && (p[2] == 'N') && (p[3] == 'G')
				&& (p[4] == '\r') && (p[5] == '\n') && (p[6] == 0x1A) && (p[7] == '\n')) return load_png(p);

			return nullptr;
		}
	}
}


#endif