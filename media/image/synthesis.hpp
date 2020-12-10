#ifndef IMATHLIB_H_MEDIA_IMAGE_SYNTHESIS_HPP
#define IMATHLIB_H_MEDIA_IMAGE_SYNTHESIS_HPP

#include "IMathLib/media/image/image.hpp"

//画像の合成処理


namespace iml {
	namespace img {
		namespace mix {

			//1色で決まる色の合成演算
			struct color_mix_1 {
				//零除算防止用に数式は書かれている

				//この構造体内の関数型
				using type = unsigned char(*)(unsigned char, unsigned char);

				//通常
				static unsigned char __normal(unsigned char c1, unsigned char c2) {
					return c2;
				}
				//乗算
				static unsigned char __multiplication(unsigned char c1, unsigned char c2) {
					return (c1*c2) / 255;
				}
				//除算
				static unsigned char __division(unsigned char c1, unsigned char c2) {
					return (c1 * 256) / (c2 + 1);
				}
				//加算(覆い焼きリニア)
				static unsigned char _addition(unsigned char c1, unsigned char c2) {
					return (iml::min<int>)(c1 + c2, 255);
				}
				//減算
				static unsigned char __subtraction(unsigned char c1, unsigned char c2) {
					return (c1 < c2) ? 0 : c1 - c2;
				}
				//スクリーン
				static unsigned char __screen(unsigned char c1, unsigned char c2) {
					return 255 - ((255 - c1)*(255 - c2)) / 255;
				}
				//オーバーレイ
				static unsigned char __overlay(unsigned char c1, unsigned char c2) {
					return c1 / 255. * (c1 + (2 * c2) / 255. * (255 - c1));
				}
				//覆い焼きカラー
				static unsigned char __dodge_color(unsigned char c1, unsigned char c2) {
					return (c1 * 256) / (255 - c2 + 1);
				}
				//焼き込みカラー
				static unsigned char __burn_color(unsigned char c1, unsigned char c2) {
					return 255 - (256 * (255 - c1)) / (c2 + 1);
				}
				//焼き込みリニア
				static unsigned char __burn_linear(unsigned char c1, unsigned char c2) {
					return (c1 + c2 < 255) ? 0 : c1 + c2 - 255;
				}
				//ハードライト
				static unsigned char __hard_light(unsigned char c1, unsigned char c2) {
					return (c2 > 128) ? 255 - ((255 - 2 * (c2 - 128))*(255 - c1)) / 256 : (2 * c1*c2) / 256;
				}
				//ソフトライト
				static unsigned char __soft_light(unsigned char c1, unsigned char c2) {
					return ((255 - c1)*c2 + __screen(c1, c2)) / 255. * c1;
				}
				//ビビッドライト
				static unsigned char __vivid_light(unsigned char c1, unsigned char c2) {
					return (c2 < 128) ? 255 - (255 - c1) / (2 * c2 + 1) : 255. * c1 / (255 - 2 * (c2 - 128));
				}
				//リニアライト
				static unsigned char __linear_right(unsigned char c1, unsigned char c2) {
					return (c2 < 128) ? c1 - 2 * c2 - 255 : c1 + 2 * (c2 - 128);
				}
				//ピンライト
				static unsigned char __pin_right(unsigned char c1, unsigned char c2) {
					return (c2 < 128) ? (iml::min<int>)(c1, 2 * c2) : (iml::max<int>)(c1, 2 * (c2 - 128));
				}
				//ハードミックス
				static unsigned char __hard_mix(unsigned char c1, unsigned char c2) {
					return (static_cast<int>(c1) + c2 >= 255) ? 255 : 0;
				}
				//微粒取り出し
				static unsigned char __fine_particle_removal(unsigned char c1, unsigned char c2) {
					int_t buf = c1 - c2 + 128;
					return (buf < 0) ? 0 : ((buf > 255) ? 255 : buf);
				}
				//微粒結合
				static unsigned char __fine_particle_bonding(unsigned char c1, unsigned char c2) {
					int_t buf = c1 + c2 - 127;
					return (buf < 0) ? 0 : ((buf > 255) ? 255 : buf);
				}
				//比較(明)
				static unsigned char __comparison_lighten(unsigned char c1, unsigned char c2) {
					return (iml::max)(c1, c2);
				}
				//比較(暗)
				static unsigned char __comparison_darken(unsigned char c1, unsigned char c2) {
					return (iml::min)(c1, c2);
				}
				//差の絶対値
				static unsigned char __difference(unsigned char c1, unsigned char c2) {
					return abs(c1 - c2);
				}
				//除外
				static unsigned char __exclusion(unsigned char c1, unsigned char c2) {
					return c1 + c2 - 2 * (c1*c2) / 255;
				}

			private:
				//アルファなしの場合の合成
				static void __none_alpha(image* base, const image* img, size_t w, size_t h, type f) {
					for (size_t i = w, k1 = (iml::min)(w + img->w, base->w); i < k1; ++i)
						for (size_t j = h, k2 = (iml::min)(h + img->h, base->h); j < k2; ++j)
							for (size_t k = 0; k < 3; ++k)
								(*base)(i, j)[k] = f((*base)(i, j)[k], (*img)(i - w, j - h)[k]);
				}
				//baseにのみアルファがある場合の合成
				static void __base_alpha(image* base, const image* img, size_t w, size_t h, type f) {
					for (size_t i = w, k1 = (iml::min)(w + img->w, base->w); i < k1; ++i)
						for (size_t j = h, k2 = (iml::min)(h + img->h, base->h); j < k2; ++j) {
							//アルファを考慮して合成
							for (size_t k = 0; k < 3; ++k)
								(*base)(i, j)[k] = (f((*base)(i, j)[k], (*img)(i - w, j - h)[k])*(*base)(i, j)[3]
									+ (255 - (*base)(i, j)[3]) * (*img)(i - w, j - h)[k]) / 255;
							//アルファの算出(計算都合上後から計算する)
							(*base)(i, j)[3] = 255;
						}
				}
				//imgにのみアルファがある場合の合成
				static void __img_alpha(image* base, const image* img, size_t w, size_t h, type f) {
					for (size_t i = w, k1 = (iml::min)(w + img->w, base->w); i < k1; ++i)
						for (size_t j = h, k2 = (iml::min)(h + img->h, base->h); j < k2; ++j) {
							//アルファの算出
							(*base)(i, j)[3] = 255;
							//アルファを考慮して合成
							for (size_t k = 0; k < 3; ++k)
								(*base)(i, j)[k] = (f((*base)(i, j)[k], (*img)(i - w, j - h)[k])*(*img)(i - w, j - h)[3]
									+ (255 - (*img)(i - w, j - h)[3]) * (*base)(i, j)[k]) / 255;
						}
				}
				//imgとbaseのどちらにもアルファがある場合の合成
				static void __img_base_alpha(image* base, const image* img, size_t w, size_t h, type f) {
					int_t alpha[3];			//アルファ計算用の一時計算バッファ

					for (size_t i = w, k1 = (iml::min)(w + img->w, base->w); i < k1; ++i)
						for (size_t j = h, k2 = (iml::min)(h + img->h, base->h); j < k2; ++j) {
							alpha[0] = (*base)(i, j)[3] * (*img)(i - w, j - h)[3] / 255;
							alpha[1] = (*img)(i - w, j - h)[3] * (255 - (*base)(i, j)[3]) / 255;
							alpha[2] = (*base)(i, j)[3] * (255 - (*img)(i - w, j - h)[3]) / 255;
							//アルファの算出
							(*base)(i, j)[3] = alpha[0] + alpha[1] + alpha[2];
							//アルファを考慮して合成
							for (size_t k = 0; k < 3; ++k)
								(*base)(i, j)[k] = (f((*base)(i, j)[k], (*img)(i - w, j - h)[k])*alpha[0]
									+ alpha[1] * (*img)(i - w, j - h)[k]
									+ alpha[2] * (*base)(i, j)[k]) / (*base)(i, j)[3];
						}
				}
			public:
				//画像の合成処理
				static void __image_synthesis(image* base, const image* img, size_t w, size_t h, type f) {
					if (base->color == 3) (img->color == 3) ? __none_alpha(base, img, w, h, f) : __img_alpha(base, img, w, h, f);
					else (img->color == 3) ? __base_alpha(base, img, w, h, f) : __img_base_alpha(base, img, w, h, f);
				}

			};
			//3色で決まる色の合成演算
			struct color_mix_3 {

				//この構造体内の関数型
				using type = void(*)(unsigned char*, const unsigned char*, const unsigned char*);

				//カラー比較(明)
				static void __color_comparison_lighten(unsigned char* result, const unsigned char *c1, const unsigned char *c2) {
					//上レイヤーの合成色を優先
					if (c1[0] + c1[1] + c1[2] < c2[0] + c2[1] + c2[2]) {
						result[0] = c1[0];
						result[1] = c1[1];
						result[2] = c1[2];
					}
					else {
						result[0] = c2[0];
						result[1] = c2[1];
						result[2] = c2[2];
					}
				}
				//カラー比較(暗)
				static void __color_comparison_darken(unsigned char* result, const unsigned char *c1, const unsigned char *c2) {
					//上レイヤーの合成色を優先
					if (c1[0] + c1[1] + c1[2] > c2[0] + c2[1] + c2[2]) {
						result[0] = c1[0];
						result[1] = c1[1];
						result[2] = c1[2];
					}
					else {
						result[0] = c2[0];
						result[1] = c2[1];
						result[2] = c2[2];
					}
				}

			private:
				//アルファなしの場合の合成
				static void __none_alpha(image* base, const image* img, size_t w, size_t h, type f) {
					for (size_t i = w, k1 = (iml::min)(w + img->w, base->w); i < k1; ++i)
						for (size_t j = h, k2 = (iml::min)(h + img->h, base->h); j < k2; ++j)
							for (size_t k = 0; k < 3; ++k)
								f((*base)(i, j), (*base)(i, j), (*img)(i - w, j - h));
				}
				//baseにのみアルファがある場合の合成
				static void __base_alpha(image* base, const image* img, size_t w, size_t h, type f) {
					unsigned char color_buf[3];				//色の計算用バッファ

					for (size_t i = w, k1 = (iml::min)(w + img->w, base->w); i < k1; ++i)
						for (size_t j = h, k2 = (iml::min)(h + img->h, base->h); j < k2; ++j) {
							//単純合成色を予め計算
							f(color_buf, (*base)(i, j), (*img)(i - w, j - h));
							//アルファを考慮して合成
							for (size_t k = 0; k < 3; ++k)
								(*base)(i, j)[k] = (color_buf[k] * (*base)(i, j)[3]
									+ (255 - (*base)(i, j)[3]) * (*img)(i - w, j - h)[k]) / 255;
							//アルファの算出(計算都合上後から計算する)
							(*base)(i, j)[3] = 255;
						}
				}
				//imgにのみアルファがある場合の合成
				static void __img_alpha(image* base, const image* img, size_t w, size_t h, type f) {
					unsigned char color_buf[3];				//色の計算用バッファ

					for (size_t i = w, k1 = (iml::min)(w + img->w, base->w); i < k1; ++i)
						for (size_t j = h, k2 = (iml::min)(h + img->h, base->h); j < k2; ++j) {
							//単純合成色を予め計算
							f(color_buf, (*base)(i, j), (*img)(i - w, j - h));
							//アルファの算出
							(*base)(i, j)[3] = 255;
							//アルファを考慮して合成
							for (size_t k = 0; k < 3; ++k)
								(*base)(i, j)[k] = (color_buf[k] * (*img)(i - w, j - h)[3]
									+ (255 - (*img)(i - w, j - h)[3]) * (*base)(i, j)[k]) / 255;
						}
				}
				//imgとbaseのどちらにもアルファがある場合の合成
				static void __img_base_alpha(image* base, const image* img, size_t w, size_t h, type f) {
					int_t alpha[3];							//アルファ計算用の一時計算バッファ
					unsigned char color_buf[3];				//色の計算用バッファ

					for (size_t i = w, k1 = (iml::min)(w + img->w, base->w); i < k1; ++i)
						for (size_t j = h, k2 = (iml::min)(h + img->h, base->h); j < k2; ++j) {
							//単純合成色を予め計算
							f(color_buf, (*base)(i, j), (*img)(i - w, j - h));

							alpha[0] = (*base)(i, j)[3] * (*img)(i - w, j - h)[3] / 255;
							alpha[1] = (*img)(i - w, j - h)[3] * (255 - (*base)(i, j)[3]) / 255;
							alpha[2] = (*base)(i, j)[3] * (255 - (*img)(i - w, j - h)[3]) / 255;
							//アルファの算出
							(*base)(i, j)[3] = alpha[0] + alpha[1] + alpha[2];
							//アルファを考慮して合成
							for (size_t k = 0; k < 3; ++k)
								(*base)(i, j)[k] = (color_buf[k] * alpha[0]
									+ alpha[1] * (*img)(i - w, j - h)[k]
									+ alpha[2] * (*base)(i, j)[k]) / (*base)(i, j)[3];
						}
				}
			public:
				//画像の合成処理
				static void __image_synthesis(image* base, const image* img, size_t w, size_t h, type f) {
					if (base->color == 3) (img->color == 3) ? __none_alpha(base, img, w, h, f) : __img_alpha(base, img, w, h, f);
					else (img->color == 3) ? __base_alpha(base, img, w, h, f) : __img_base_alpha(base, img, w, h, f);
				}

			};



			//画像の合成(レイヤー合成モード)
			inline void image_synthesis(image* base, const image* img, size_t w, size_t h, color_mix_1::type f = color_mix_1::__normal) {
				color_mix_1::__image_synthesis(base, img, w, h, f);
			}
			inline void image_synthesis(image* base, const image* img, size_t w, size_t h, color_mix_3::type f) {
				color_mix_3::__image_synthesis(base, img, w, h, f);
			}

		}
	}
}


#endif