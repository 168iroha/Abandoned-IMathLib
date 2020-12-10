#ifndef IMATHLIB_H_MEDIA_IMAGE_FILTER_HPP
#define IMATHLIB_H_MEDIA_IMAGE_FILTER_HPP

#include "IMathLib/math/extra_math.hpp"
#include "IMathLib/math/statistics.hpp"
#include "IMathLib/math/liner_algebra.hpp"
#include "IMathLib/media/image/image.hpp"

//画像に対するフィルタ処理


//基本処理
namespace iml {
	namespace img {

		//トリミング(rectが正しくないとき何も処理をしない)
		inline image* trimming(const image* img, const image::rect& rect) {
			image* result;
			if (!(rect.top < rect.bottom) || !(rect.left < rect.right)) return nullptr;
			result = new image(rect.left - rect.right, rect.bottom - rect.top, img->color);
			if (!result->reserve()) { delete result; return nullptr; }
			
			//全ての成分のコピー
			for(size_t i=rect.left;i<rect.right;++i)
				for (size_t j = rect.top; j < rect.bottom; ++j)
					for (size_t k = 0; k < img->color; ++k)
						(*result)(i - rect.left, j - rect.top)[k] = (*img)(i, j)[k];
			return result;
		}
		//サイズの変更(双線形補間か最近傍補間の選択)
		inline image* resize(const image* img, size_t width, size_t height, size_t interpolation = image::nearest) {
			image* result;
			double rate_w = static_cast<double>(img->w) / static_cast<double>(width)
				, rate_h = static_cast<double>(img->h) / static_cast<double>(height);		//拡大率を出しておく
			result = new image(width, height, img->color);
			if (!result->reserve()) { delete result; return nullptr; }

			//逆変換により実装
			for (size_t i = 0; i < width; ++i) {
				double vicinity_w = rate_w*i;
				for (size_t j = 0; j < height; ++j) {
					//近傍点の算出
					double vicinity_h = rate_h*j;
					if (interpolation == image::nearest) {
						//最近傍点は四捨五入で算出
						size_t round_w = static_cast<size_t>(round(vicinity_w)), round_h = static_cast<size_t>(round(vicinity_h));
						for (size_t k = 0; k < img->color; ++k)
							(*result)(i, j)[k] = (*img)(round_w, round_h)[k];
					}
					else if (interpolation == image::linear) {
						//整数への丸めの取得
						size_t w = static_cast<size_t>(vicinity_w), h = static_cast<size_t>(vicinity_h);
						//小数部のみの取得
						double dw = vicinity_w - w, dh = vicinity_h - h;
						//4方で重みを付けて足す
						for (size_t k = 0; k < img->color; ++k)
							(*result)(i, j)[k] = static_cast<int_t>((1 - dw)*(1 - dh)*(*img)(w, h)[k] + dw*(1 - dh)*(*img)(w + 1, h)[k]
								+ (1 - dw)*dh*(*img)(w, h + 1)[k] + dw*dw*(*img)(w + 1, h + 1)[k]);
					}
				}
			}
			return result;
		}
		//上下反転
		inline image* flip_vertical(const image* img) {
			image* result;
			result = new image(img->w, img->h, img->color);
			if (!result->reserve()) { delete result; return nullptr; }

			//全ての成分のコピー
			for (size_t i = 0; i < img->w; ++i)
				for (size_t j = 0; j < img->h; ++j)
					for (size_t k = 0; k < img->color; ++k)
						(*result)(i, j)[k] = (*img)(i, img->h - j - 1)[k];
			return result;
		}
		//左右反転
		inline image* flip_horizontal(const image* img) {
			image* result;
			result = new image(img->w, img->h, img->color);
			if (!result->reserve()) { delete result; return nullptr; }

			//全ての成分のコピー
			for (size_t i = 0; i < img->w; ++i)
				for (size_t j = 0; j < img->h; ++j)
					for (size_t k = 0; k < img->color; ++k)
						(*result)(i, j)[k] = (*img)(img->w - i - 1, j)[k];
			return result;
		}
		//回転(ラジアン,回転の中心,補間法)
		inline image* rotation(const image* img, double radian, int_t center_w, int_t center_h, size_t interpolation = image::nearest) {
			image* result;
			result = new image(img->w, img->h, img->color);
			if (!result->reserve()) { delete result; return nullptr; }

			//出力画像からの逆回転による実装
			for (size_t i = 0; i < img->w; ++i)
				for (size_t j = 0; j < img->h; ++j) {
					//近傍点の算出(heightの上下反転を考慮)
					double vicinity_w = cos(radian)*(static_cast<double>(i) - center_w) - sin(radian)*(static_cast<double>(j) - center_h) + center_w
						, vicinity_h = -(-sin(radian)*(static_cast<double>(i) - center_w) - cos(radian)*(static_cast<double>(j) - center_h) - center_h);
					//範囲外ならば255で埋める
					if ((vicinity_w > img->w) || (vicinity_w < 0) || (vicinity_h > img->h) || (vicinity_h < 0))
						for (size_t k = 0; k < img->color; ++k)
							(*result)(i, j)[k] = 255;
					//補間を取る
					else {
						if (interpolation == image::nearest) {
							//最近傍点は四捨五入で算出
							size_t round_w = static_cast<size_t>(round(vicinity_w)), round_h = static_cast<size_t>(round(vicinity_h));
							for (size_t k = 0; k < img->color; ++k)
								(*result)(i, j)[k] = (*img)(round_w, round_h)[k];
						}
						else if (interpolation == image::linear) {
							//整数への丸めの取得
							size_t w = static_cast<size_t>(vicinity_w), h = static_cast<size_t>(vicinity_h);
							//小数部のみの取得
							double dw = vicinity_w - w, dh = vicinity_h - h;
							//4方で重みを付けて足す
							for (size_t k = 0; k < img->color; ++k)
								(*result)(i, j)[k] = static_cast<int_t>((1 - dw)*(1 - dh)*(*img)(w, h)[k] + dw*(1 - dh)*(*img)(w + 1, h)[k]
								+ (1 - dw)*dh*(*img)(w, h + 1)[k] + dw*dw*(*img)(w + 1, h + 1)[k]);
						}
					}
				}
			return result;
		}
	}
}

//フィルタ処理(1つの画像でできる処理のことを指すとする)
namespace iml {
	namespace img {
		namespace filter {

			//単純平均によるグレースケール変換
			inline void grayscale(image *img) {
				unsigned char color;
				unsigned char* p = img->pixels;
				for (size_t i = 0, k = img->h*img->w; i < k; ++i) {
					color = (p[0] + p[1] + p[2]) / 3;
					p[0] = p[1] = p[2] = color;
					p += img->color;
				}
			}
			//NTSC加重平均法によるグレースケール変換
			inline void NTSC_grayscale(image *img) {
				unsigned char color;
				unsigned char* p = img->pixels;
				for (size_t i = 0, k = img->h*img->w; i < k; ++i) {
					color = (77 * p[0] + 150 * p[1] + 29 * p[2]) / 256;
					p[0] = p[1] = p[2] = color;
					p += img->color;
				}
			}
			//ネガティブフィルタ
			inline void negative(image *img) {
				for (size_t i = 0; i < img->w; ++i)
					for (size_t j = 0; j < img->h; ++j)
						for (size_t k = 0; k < 3; ++k)
							(*img)(i, j)[k] = 255 - (*img)(i, j)[k];
			}
			//ガンマ補正
			inline void gamma(image *img, double g) {
				double inv = (g <= 0) ? (std::numeric_limits<double>::max)() : 1 / g;
				for (size_t i = 0; i < img->w; ++i)
					for (size_t j = 0; j < img->h; ++j)
						for (size_t k = 0; k < 3; ++k)
							(*img)(i, j)[0] = 255 * pow((*img)(i, j)[k] / 255., inv);
			}
			//モザイク処理(全ての色の成分について)
			inline void mosaic(image *img, size_t size) {
				size_t sum;

				for (size_t i = 0; i < img->w; i += size) {
					for (size_t j = 0; j < img->h; j += size) {
						for (size_t k = 0; k < img->color; ++k) {
							sum = 0;
							//和を算出
							for (size_t l = 0, _l = (iml::min)(img->w - i, size); l < _l; ++l)
								for (size_t m = 0, _m = (iml::min)(img->h - j, size); m < _m; ++m)
									sum += (*img)(i + l, j + m)[k];
							//平均
							sum /= (iml::min)(img->w - i, size)*(iml::min)(img->h - j, size);
							//代入
							for (size_t l = 0, _l = (iml::min)(img->w - i, size); l < _l; ++l)
								for (size_t m = 0, _m = (iml::min)(img->h - j, size); m < _m; ++m)
									(*img)(i + l, j + m)[k] = sum;
						}
					}
				}
			}
			//RGBの増減(R:[0,255],G:[0,255],B:[0,255])(それぞれの色のスペクトルを増減させる操作)
			inline void rgb(image *img, int_t r, int_t g, int_t b) {
				int_t buf;
				for (size_t i = 0; i < img->w; ++i)
					for (size_t j = 0; j < img->h; ++j) {
						//値を加えて数値補正して代入
						buf = (*img)(i, j)[0] + r; (*img)(i, j)[0] = (buf < 0) ? 0 : ((buf > 255) ? 255 : buf);
						buf = (*img)(i, j)[1] + g; (*img)(i, j)[1] = (buf < 0) ? 0 : ((buf > 255) ? 255 : buf);
						buf = (*img)(i, j)[2] + b; (*img)(i, j)[2] = (buf < 0) ? 0 : ((buf > 255) ? 255 : buf);
					}
			}
			//RGBをHSVに変換
			inline void rgb_to_hsv(int_t *hsv, const unsigned char *rgb) {
				unsigned char rgb_max = *max_element(rgb, &rgb[3]), rgb_min = *min_element(rgb, &rgb[3]);
				hsv[2] = rgb_max;
				hsv[1] = static_cast<int_t>(255. * (rgb_max - rgb_min) / hsv[2]);
				if (rgb_max == rgb[0]) hsv[0] = static_cast<int_t>(60. * (rgb[2] - rgb[1]) / (rgb_max - rgb_min));
				else if (rgb_max == rgb[1]) hsv[0] = static_cast<int_t>(60. * (2 + static_cast<double>(rgb[0] - rgb[2]) / (rgb_max - rgb_min)));
				else if (rgb_max == rgb[2]) hsv[0] = static_cast<int_t>(60. * (4 + static_cast<double>(rgb[1] - rgb[0]) / (rgb_max - rgb_min)));
				if (hsv[0] < 0) hsv[0] += 360;
			}
			//HSVをRGBに変換
			inline void hsv_to_rgb(unsigned char *rgb, const int_t *hsv) {
				int_t hi = hsv[0] / 60;
				double f = hsv[0] / 60. - hi;
				unsigned char p, q, t;

				if (hsv[1] == 0) {
					rgb[0] = rgb[1] = rgb[2] = hsv[2];
					return;
				}

				p = hsv[2] * (1 - hsv[1] / 255.);
				q = hsv[2] * (1 - hsv[1] / 255.*f);
				t = hsv[2] * (1 - hsv[1] / 255.*(1 - f));
				switch (hi) {
				case 0: rgb[0] = hsv[2]; rgb[1] = t; rgb[2] = p; break;
				case 1: rgb[0] = q; rgb[1] = hsv[2]; rgb[2] = p; break;
				case 2: rgb[0] = p; rgb[1] = hsv[2]; rgb[2] = t; break;
				case 3: rgb[0] = p; rgb[1] = q; rgb[2] = hsv[2]; break;
				case 4: rgb[0] = t; rgb[1] = p; rgb[2] = hsv[2]; break;
				case 5: rgb[0] = hsv[2]; rgb[1] = p; rgb[2] = q; break;
				}
			}
			//HSVの増減(H:[0,359],S:[0,255],V:[0,255])
			inline void hsv(image *img, int_t h, int_t s, int_t v) {
				int_t buf[3];
				for (size_t i = 0; i < img->w; ++i)
					for (size_t j = 0; j < img->h; ++j) {
						//元データのrgbをhsvに変換
						rgb_to_hsv(buf, (*img)(i, j));
						//値を加えて数値補正
						buf[0] += h; buf[0] %= 360; if (buf[0] < 0) buf[0] += 360;
						buf[1] += s; buf[1] = (buf[1] < 0) ? 0 : ((buf[1] > 255) ? 255 : buf[1]);
						buf[2] += v; buf[2] = (buf[2] < 0) ? 0 : ((buf[2] > 255) ? 255 : buf[2]);
						//hsvをrgbに変換する
						hsv_to_rgb((*img)(i, j), buf);
					}
			}
			//レベル補正(ガンマ,入力,出力)
			inline void level(image *img, double g, unsigned char in_min, unsigned char in_max, unsigned char out_min, unsigned char out_max) {

			}
			//ディザリング(アルファの2値化)
			inline void dissolve(image* img) {

			}

			//横方向平均平滑化フィルタ(n:平滑化係数(奇数),nk:中央の重み)
			inline void smooth_horizon_average(image *img, size_t n, int_t nk = 1) {
				image* buf = new image(img->w, img->h, img->color);
				size_t half_n = (n >> 1);
				size_t sum;
				if ((n & 1) == 0) ++n;			//奇数にする
				if (!buf->reserve()) { delete buf; return; }

				for (size_t i = 0; i < img->w; ++i)
					for (size_t j = 0; j < img->h; ++j)
						for (size_t k = 0; k < img->color; ++k) {
							sum = 0;
							//和の算出
							for (int_t l = -static_cast<int_t>((iml::min)(i, half_n)), _l = (iml::min)(img->w - i, half_n + 1); l < _l; ++l)
								sum += (*img)(i + l, j)[k];
							//重みの代入
							sum += (*img)(i, j)[k] * (nk - 1);
							//平均と代入
							(*buf)(i, j)[k] = sum / (nk - 1 + (iml::min)(i, half_n) + (iml::min)(img->w - i, half_n + 1));
						}
				iml::swap(img->pixels, buf->pixels);
				delete buf;
			}
			//縦方向平均平滑化フィルタ(n:平滑化係数(奇数),nk:中央の重み)
			inline void smooth_vertical_average(image *img, size_t n, int_t nk = 1) {
				image* buf = new image(img->w, img->h, img->color);
				size_t half_n = (n >> 1);
				size_t sum;
				if ((n & 1) == 0) ++n;			//奇数にする
				if (!buf->reserve()) { delete buf; return; }

				for (size_t i = 0; i < img->w; ++i)
					for (size_t j = 0; j < img->h; ++j)
						for (size_t k = 0; k < img->color; ++k) {
							sum = 0;
							//和の算出
							for (int_t l = -static_cast<int_t>((iml::min)(j, half_n)), _l = (iml::min)(img->h - j, half_n + 1); l < _l; ++l)
								sum += (*img)(i, j + l)[k];
							//重みの代入
							sum += (*img)(i, j)[k] * (nk - 1);
							//平均と代入
							(*buf)(i, j)[k] = sum / (nk - 1 + (iml::min)(j, half_n) + (iml::min)(img->h - j, half_n + 1));
						}
				iml::swap(img->pixels, buf->pixels);
				delete buf;
			}
			//平均平滑化フィルタ(n:平滑化係数(奇数),nk:中央の重み)
			inline void smooth_average(image *img, size_t n, int_t nk = 1) {
				image* buf = new image(img->w, img->h, img->color);
				size_t half_n = (n >> 1);
				size_t sum;
				if ((n & 1) == 0) ++n;			//奇数にする
				if (!buf->reserve()) { delete buf; return; }

				for (size_t i = 0; i < img->w; ++i)
					for (size_t j = 0; j < img->h; ++j)
						for (size_t k = 0; k < img->color; ++k) {
							sum = 0;
							//和の算出
							for (int_t l = -static_cast<int_t>((iml::min)(i, half_n)), _l = (iml::min)(img->w - i, half_n + 1); l < _l; ++l)
								for (int_t m = -static_cast<int_t>((iml::min)(j, half_n)), _m = (iml::min)(img->h - j, half_n + 1); m < _m; ++m)
									sum += (*img)(i + l, j + m)[k];
							//重みの代入
							sum += (*img)(i, j)[k] * (nk - 1);
							//平均と代入
							(*buf)(i, j)[k] = sum / ((nk - 1) + ((iml::min)(i, half_n) + (iml::min)(img->w - i, half_n + 1))*((iml::min)(j, half_n) + (iml::min)(img->h - j, half_n + 1)));
						}
				iml::swap(img->pixels, buf->pixels);
				delete buf;
			}

			//中央値フィルタ(nは奇数)
			inline void median(image *img, size_t n) {
				image* buf = new image(img->w, img->h, img->color);
				size_t half_n = (n >> 1);
				size_t array_n;				//データ配列用の配列番号
				size_t* data;					//データ配列
				if ((n & 1) == 0) ++n;			//奇数にする
				data = new size_t[n*n];
				if (!buf->reserve()) { delete buf; delete[] data; return; }

				for (size_t i = 0; i < img->w; ++i)
					for (size_t j = 0; j < img->h; ++j)
						for (size_t k = 0; k < img->color; ++k) {
							array_n = 0;
							//データ配列の作成
							for (int_t l = -static_cast<int_t>((iml::min)(i, half_n)), _l = (iml::min)(img->w - i, half_n + 1); l < _l; ++l)
								for (int_t m = -static_cast<int_t>((iml::min)(j, half_n)), _m = (iml::min)(img->h - j, half_n + 1); m < _m; ++m) {
									data[array_n] = (*img)(i + l, j + m)[k];
									++array_n;
								}
							//中央値の代入
							(*buf)(i, j)[k] = stats::median(data, &data[((iml::min)(i, half_n) + (iml::min)(img->w - i, half_n + 1))*((iml::min)(j, half_n) + (iml::min)(img->h - j, half_n + 1))]);
						}
				iml::swap(img->pixels, buf->pixels);
				delete buf;
				delete[] data;
			}

			//膨張フィルタ(nは奇数)
			inline void dilation(image *img, size_t n) {
				image* buf = new image(img->w, img->h, img->color);
				int_t hsv[3];
				size_t half_n = (n >> 1);
				if ((n & 1) == 0) ++n;			//奇数にする
				if (!buf->reserve()) { delete buf; return; }

				for (size_t i = 0; i < img->w; ++i)
					for (size_t j = 0; j < img->h; ++j) {
						//明度の最大値を取得と同時にポインタの位置の記録
						int_t v_max = 0;
						unsigned char *rgb_p
							= (*img)(i - static_cast<int_t>((iml::min)(i, half_n)), j - static_cast<int_t>((iml::min)(j, half_n)));
						for (int_t l = -static_cast<int_t>((iml::min)(i, half_n)), _l = (iml::min)(img->w - i, half_n + 1); l < _l; ++l)
							for (int_t m = -static_cast<int_t>((iml::min)(j, half_n)), _m = (iml::min)(img->h - j, half_n + 1); m < _m; ++m) {
								rgb_to_hsv(hsv, (*img)(i + l, j + m));
								if (v_max < hsv[2]) {
									v_max = hsv[2];
									rgb_p = (*img)(i + l, j + m);
								}
							}
						//代入
						for (size_t k = 0; k < img->color; ++k)
							(*buf)(i, j)[k] = rgb_p[k];
					}
				iml::swap(img->pixels, buf->pixels);
				delete buf;
			}
			//収縮フィルタ(nは奇数)
			inline void erosion(image *img, size_t n) {
				image* buf = new image(img->w, img->h, img->color);
				int_t hsv[3];
				size_t half_n = (n >> 1);
				if ((n & 1) == 0) ++n;			//奇数にする
				if (!buf->reserve()) { delete buf; return; }

				for (size_t i = 0; i < img->w; ++i)
					for (size_t j = 0; j < img->h; ++j) {
						//明度の最小値を取得と同時にポインタの位置の記録
						int_t v_min = 255;
						unsigned char *rgb_p
							= (*img)(i - static_cast<int_t>((iml::min)(i, half_n)), j - static_cast<int_t>((iml::min)(j, half_n)));
						for (int_t l = -static_cast<int_t>((iml::min)(i, half_n)), _l = (iml::min)(img->w - i, half_n + 1); l < _l; ++l)
							for (int_t m = -static_cast<int_t>((iml::min)(j, half_n)), _m = (iml::min)(img->h - j, half_n + 1); m < _m; ++m) {
								rgb_to_hsv(hsv, (*img)(i + l, j + m));
								if (v_min > hsv[2]) {
									v_min = hsv[2];
									rgb_p = (*img)(i + l, j + m);
								}
							}
						//代入
						for (size_t k = 0; k < img->color; ++k)
							(*buf)(i, j)[k] = rgb_p[k];
					}
				iml::swap(img->pixels, buf->pixels);
				delete buf;
			}

			//横方向一次微分フィルタ
			inline void horizon_first_derivative(image *img) {
				image* buf = new image(img->w, img->h, img->color);
				if (!buf->reserve()) { delete buf; return; }

				//端以外の処理
				for (size_t i = 1; i < img->w - 1; ++i)
					for (size_t j = 0; j < img->h; ++j)
						for (size_t k = 0; k < 3; ++k)
							(*buf)(i, j)[k] = abs((*img)(i + 1, j)[k] - (*img)(i - 1, j)[k]);
				//端の処理
				for (size_t i = 0; i < img->h; ++i)
					for (size_t k = 0; k < 3; ++k) {
						(*buf)(0, i)[k] = abs((*img)(1, i)[k] - (*img)(0, i)[k]);
						(*buf)(img->w - 1, i)[k] = abs((*img)(img->w - 1, i)[k] - (*img)(img->w - 2, i)[k]);
					}
				iml::swap(img->pixels, buf->pixels);
				delete buf;
			}
			//縦方向一次微分フィルタ
			inline void vertical_first_derivative(image *img) {
				image* buf = new image(img->w, img->h, img->color);
				if (!buf->reserve()) { delete buf; return; }

				//端以外の処理
				for (size_t i = 0; i < img->w; ++i)
					for (size_t j = 1; j < img->h - 1; ++j)
						for (size_t k = 0; k < 3; ++k)
							(*buf)(i, j)[k] = abs((*img)(i, j + 1)[k] - (*img)(i, j - 1)[k]);
				//端の処理
				for (size_t i = 0; i < img->w; ++i)
					for (size_t k = 0; k < 3; ++k) {
						(*buf)(i, 0)[k] = abs((*img)(i, 1)[k] - (*img)(i, 0)[k]);
						(*buf)(i, img->h - 1)[k] = abs((*img)(i, img->h - 2)[k] - (*img)(i, img->h - 1)[k]);
					}
				iml::swap(img->pixels, buf->pixels);
				delete buf;
			}
			//一次微分フィルタ(左右両方の平均を取る)
			inline void first_derivative(image *img) {
				image* buf = new image(img->w, img->h, img->color);
				size_t calc_buf[2];
				if (!buf->reserve()) { delete buf; return; }

				//端以外の処理
				for (size_t i = 1; i < img->w - 1; ++i)
					for (size_t j = 1; j < img->h - 1; ++j)
						for (size_t k = 0; k < 3; ++k) {
							calc_buf[0] = ((*img)(i, j + 1)[k] - (*img)(i, j - 1)[k]);
							calc_buf[1] = ((*img)(i + 1, j)[k] - (*img)(i - 1, j)[k]);
							(*buf)(i, j)[k] = isqrt(calc_buf[0] * calc_buf[0] + calc_buf[1] * calc_buf[1]);
						}
				//端の処理(横の方を優先する)
				for (size_t i = 0; i < img->h; ++i)
					for (size_t k = 0; k < 3; ++k) {
						(*buf)(0, i)[k] = abs((*img)(1, i)[k] - (*img)(0, i)[k]);
						(*buf)(img->w - 1, i)[k] = abs((*img)(img->w - 1, i)[k] - (*img)(img->w - 2, i)[k]);
					}
				for (size_t i = 1; i < img->w - 1; ++i)
					for (size_t k = 0; k < 3; ++k) {
						(*buf)(i, 0)[k] = abs((*img)(i, 1)[k] - (*img)(i, 0)[k]);
						(*buf)(i, img->h - 1)[k] = abs((*img)(i, img->h - 2)[k] - (*img)(i, img->h - 1)[k]);
					}
				iml::swap(img->pixels, buf->pixels);
				delete buf;
			}

			//横方向prewittフィルタ
			inline void horizon_prewitt(image *img, size_t n) {
				image* buf = new image(img->w, img->h, img->color);
				int_t sum;
				size_t half_n = (n >> 1);
				if ((n & 1) == 0) ++n;			//奇数にする
				if (!buf->reserve()) { delete buf; return; }

				//端以外の処理
				for (size_t i = 1; i < img->w - 1; ++i)
					for (size_t j = 0; j < img->h; ++j)
						for (size_t k = 0; k < 3; ++k) {
							sum = 0;
							//和の算出
							for (int_t m = -static_cast<int_t>((iml::min)(j, half_n)), _m = (iml::min)(img->h - j, half_n + 1); m < _m; ++m)
								sum += (*img)(i + 1, j + m)[k] - (*img)(i - 1, j + m)[k];
							//代入
							(*buf)(i, j)[k] = abs(sum);
						}
				//端の処理
				for (size_t i = 0; i < img->h; ++i)
					for (size_t k = 0; k < 3; ++k) {
						sum = 0;
						//和の算出
						for (int_t m = -static_cast<int_t>((iml::min)(i, half_n)), _m = (iml::min)(img->h - i, half_n + 1); m < _m; ++m)
							sum += (*img)(1, i + m)[k] - (*img)(0, i + m)[k];
						//代入
						(*buf)(0, i)[k] = abs(sum);

						sum = 0;
						//和の算出
						for (int_t m = -static_cast<int_t>((iml::min)(i, half_n)), _m = (iml::min)(img->h - i, half_n + 1); m < _m; ++m)
							sum += (*img)(img->w - 1, i + m)[k] - (*img)(img->w - 2, i + m)[k];
						//代入
						(*buf)(img->w - 1, i)[k] = abs(sum);
					}
				iml::swap(img->pixels, buf->pixels);
				delete buf;
			}
			//縦方向prewittフィルタ
			inline void vertical_prewitt(image *img, size_t n) {
				image* buf = new image(img->w, img->h, img->color);
				int_t sum;
				size_t half_n = (n >> 1);
				if ((n & 1) == 0) ++n;			//奇数にする
				if (!buf->reserve()) { delete buf; return; }

				//端以外の処理
				for (size_t i = 0; i < img->w; ++i)
					for (size_t j = 1; j < img->h - 1; ++j)
						for (size_t k = 0; k < 3; ++k) {
							sum = 0;
							//和の算出
							for (int_t l = -static_cast<int_t>((iml::min)(i, half_n)), _l = (iml::min)(img->w - i, half_n + 1); l < _l; ++l)
								sum += (*img)(i + l, j + 1)[k] - (*img)(i + l, j - 1)[k];
							//代入
							(*buf)(i, j)[k] = abs(sum);
						}
				//端の処理
				for (size_t i = 0; i < img->w; ++i)
					for (size_t k = 0; k < 3; ++k) {
						sum = 0;
						//和の算出
						for (int_t m = -static_cast<int_t>((iml::min)(i, half_n)), _m = (iml::min)(img->w - i, half_n + 1); m < _m; ++m)
							sum += (*img)(i + m, 1)[k] - (*img)(i + m, 0)[k];
						//代入
						(*buf)(i, 0)[k] = abs(sum);

						sum = 0;
						//和の算出
						for (int_t m = -static_cast<int_t>((iml::min)(i, half_n)), _m = (iml::min)(img->w - i, half_n + 1); m < _m; ++m)
							sum += (*img)(i + m, img->w - 1)[k] - (*img)(i + m, img->h - 2)[k];
						//代入
						(*buf)(i, img->w - 1)[k] = abs(sum);
					}
				iml::swap(img->pixels, buf->pixels);
				delete buf;
			}
			//prewittフィルタ(左右両方の平均を取る)
			inline void prewitt(image *img, size_t n) {
				image* buf = new image(img->w, img->h, img->color);
				int_t sum[2];
				size_t half_n = (n >> 1);
				if ((n & 1) == 0) ++n;			//奇数にする
				if (!buf->reserve()) { delete buf; return; }

				//端以外の処理
				for (size_t i = 1; i < img->w - 1; ++i)
					for (size_t j = 1; j < img->h - 1; ++j)
						for (size_t k = 0; k < 3; ++k) {
							sum[0] = sum[1] = 0;
							//和の算出
							for (int_t m = -static_cast<int_t>((iml::min)(j, half_n)), _m = (iml::min)(img->h - j, half_n + 1); m < _m; ++m)
								sum[0] += (*img)(i + 1, j + m)[k] - (*img)(i - 1, j + m)[k];
							for (int_t l = -static_cast<int_t>((iml::min)(i, half_n)), _l = (iml::min)(img->w - i, half_n + 1); l < _l; ++l)
								sum[1] += (*img)(i + l, j + 1)[k] - (*img)(i + l, j - 1)[k];
							//代入
							(*buf)(i, j)[k] = isqrt(sum[0] * sum[0] + sum[1] * sum[1]);
						}
				//端の処理(横の方を優先する)
				for (size_t i = 0; i < img->h; ++i)
					for (size_t k = 0; k < 3; ++k) {
						sum[0] = 0;
						//和の算出
						for (int_t m = -static_cast<int_t>((iml::min)(i, half_n)), _m = (iml::min)(img->h - i, half_n + 1); m < _m; ++m)
							sum[0] += (*img)(1, i + m)[k] - (*img)(0, i + m)[k];
						//代入
						(*buf)(0, i)[k] = abs(sum[0]);

						sum[0] = 0;
						//和の算出
						for (int_t m = -static_cast<int_t>((iml::min)(i, half_n)), _m = (iml::min)(img->h - i, half_n + 1); m < _m; ++m)
							sum[0] += (*img)(img->w - 1, i + m)[k] - (*img)(img->w - 2, i + m)[k];
						//代入
						(*buf)(img->w - 1, i)[k] = abs(sum[0]);
					}
				for (size_t i = 1; i < img->w - 1; ++i)
					for (size_t k = 0; k < 3; ++k) {
						sum[0] = 0;
						//和の算出
						for (int_t m = -static_cast<int_t>((iml::min)(i, half_n)), _m = (iml::min)(img->w - i, half_n + 1); m < _m; ++m)
							sum[0] += (*img)(i + m, 1)[k] - (*img)(i + m, 0)[k];
						//代入
						(*buf)(i, 0)[k] = abs(sum[0]);

						sum[0] = 0;
						//和の算出
						for (int_t m = -static_cast<int_t>((iml::min)(i, half_n)), _m = (iml::min)(img->w - i, half_n + 1); m < _m; ++m)
							sum[0] += (*img)(i + m, img->w - 1)[k] - (*img)(i + m, img->h - 2)[k];
						//代入
						(*buf)(i, img->w - 1)[k] = abs(sum[0]);
					}
				iml::swap(img->pixels, buf->pixels);
				delete buf;
			}

			//横方向sobelフィルタ(s:加重係数)
			inline void horizon_sobel(image *img, size_t n, size_t s = 2) {
				image* buf = new image(img->w, img->h, img->color);
				int_t sum;
				size_t half_n = (n >> 1);
				if ((n & 1) == 0) ++n;			//奇数にする
				if (!buf->reserve()) { delete buf; return; }

				//端以外の処理
				for (size_t i = 1; i < img->w - 1; ++i)
					for (size_t j = 0; j < img->h; ++j)
						for (size_t k = 0; k < 3; ++k) {
							sum = 0;
							//和の算出
							for (int_t m = -static_cast<int_t>((iml::min)(j, half_n)); m < 0; ++m)
								sum += (*img)(i + 1, j + m)[k] - (*img)(i - 1, j + m)[k];
							sum += ((*img)(i + 1, j)[k] - (*img)(i - 1, j)[k])*s;					//重みを付ける
							for (int_t m = 1, _m = (iml::min)(img->h - j, half_n + 1); m < _m; ++m)
								sum += (*img)(i + 1, j + m)[k] - (*img)(i - 1, j + m)[k];
							//代入
							(*buf)(i, j)[k] = static_cast<unsigned char>(abs(sum));
						}
				//端の処理
				for (size_t i = 0; i < img->h; ++i)
					for (size_t k = 0; k < 3; ++k) {
						sum = 0;
						//和の算出
						for (int_t m = -static_cast<int_t>((iml::min)(i, half_n)); m < 0; ++m)
							sum += (*img)(1, i + m)[k] - (*img)(0, i + m)[k];
						sum += ((*img)(1, i)[k] - (*img)(0, i)[k])*s;
						for (int_t m = 1, _m = (iml::min)(img->h - i, half_n + 1); m < _m; ++m)
							sum += (*img)(1, i + m)[k] - (*img)(0, i + m)[k];
						//代入
						(*buf)(0, i)[k] = static_cast<unsigned char>(abs(sum));

						sum = 0;
						//和の算出
						for (int_t m = -static_cast<int_t>((iml::min)(i, half_n)); m < 0; ++m)
							sum += (*img)(img->w - 1, i + m)[k] - (*img)(img->w - 2, i + m)[k];
						sum += ((*img)(img->w - 1, i)[k] - (*img)(img->w - 2, i)[k])*s;
						for (int_t m = 1, _m = (iml::min)(img->h - i, half_n + 1); m < _m; ++m)
							sum += (*img)(img->w - 1, i + m)[k] - (*img)(img->w - 2, i + m)[k];
						//代入
						(*buf)(img->w - 1, i)[k] = static_cast<unsigned char>(abs(sum));
					}
				iml::swap(img->pixels, buf->pixels);
				delete buf;
			}
			//縦方向sobelフィルタ
			inline void vertical_sobel(image *img, size_t n, size_t s = 2) {
				image* buf = new image(img->w, img->h, img->color);
				int_t sum;
				size_t half_n = (n >> 1);
				if ((n & 1) == 0) ++n;			//奇数にする
				if (!buf->reserve()) { delete buf; return; }

				//端以外の処理
				for (size_t i = 0; i < img->w; ++i)
					for (size_t j = 1; j < img->h - 1; ++j)
						for (size_t k = 0; k < 3; ++k) {
							sum = 0;
							//和の算出
							for (int_t l = -static_cast<int_t>((iml::min)(i, half_n)); l < 0; ++l)
								sum += (*img)(i + l, j + 1)[k] - (*img)(i + l, j - 1)[k];
							sum += ((*img)(i, j + 1)[k] - (*img)(i, j - 1)[k])*s;
							for (int_t l = 1, _l = (iml::min)(img->w - i, half_n + 1); l < _l; ++l)
								sum += (*img)(i + l, j + 1)[k] - (*img)(i + l, j - 1)[k];
							//代入
							(*buf)(i, j)[k] = static_cast<unsigned char>(abs(sum));
						}
				//端の処理
				for (size_t i = 0; i < img->w; ++i)
					for (size_t k = 0; k < 3; ++k) {
						sum = 0;
						//和の算出
						for (int_t m = -static_cast<int_t>((iml::min)(i, half_n)); m < 0; ++m)
							sum += (*img)(i + m, 1)[k] - (*img)(i + m, 0)[k];
						sum += ((*img)(i, 1)[k] - (*img)(i, 0)[k])*s;
						for (int_t m = 1, _m = (iml::min)(img->w - i, half_n + 1); m < _m; ++m)
							sum += (*img)(i + m, 1)[k] - (*img)(i + m, 0)[k];
						//代入
						(*buf)(i, 0)[k] = abs(sum);

						sum = 0;
						//和の算出
						for (int_t m = -static_cast<int_t>((iml::min)(i, half_n)); m < 0; ++m)
							sum += (*img)(i + m, img->w - 1)[k] - (*img)(i + m, img->h - 2)[k];
						sum += (*img)(i, img->w - 1)[k] - (*img)(i, img->h - 2)[k];
						for (int_t m = 1, _m = (iml::min)(img->w - i, half_n + 1); m < _m; ++m)
							sum += (*img)(i + m, img->w - 1)[k] - (*img)(i + m, img->h - 2)[k];
						//代入
						(*buf)(i, img->w - 1)[k] = abs(sum);
					}
				iml::swap(img->pixels, buf->pixels);
				delete buf;
			}
			//sobelフィルタ(左右両方の平均を取る)
			inline void sobel(image *img, size_t n, size_t s = 2) {
				image* buf = new image(img->w, img->h, img->color);
				int_t sum[2];
				size_t half_n = (n >> 1);
				if ((n & 1) == 0) ++n;			//奇数にする
				if (!buf->reserve()) { delete buf; return; }

				//端以外の処理
				for (size_t i = 1; i < img->w - 1; ++i)
					for (size_t j = 1; j < img->h - 1; ++j)
						for (size_t k = 0; k < 3; ++k) {
							sum[0] = sum[1] = 0;
							//和の算出
							for (int_t m = -static_cast<int_t>((iml::min)(j, half_n)); m < 0; ++m)
								sum[0] += (*img)(i + 1, j + m)[k] - (*img)(i - 1, j + m)[k];
							sum[0] += ((*img)(i + 1, j)[k] - (*img)(i - 1, j)[k])*s;
							for (int_t m = 1, _m = (iml::min)(img->h - j, half_n + 1); m < _m; ++m)
								sum[0] += (*img)(i + 1, j + m)[k] - (*img)(i - 1, j + m)[k];
							for (int_t l = -static_cast<int_t>((iml::min)(i, half_n)); l < 0; ++l)
								sum[1] += (*img)(i + l, j + 1)[k] - (*img)(i + l, j - 1)[k];
							sum[1] += ((*img)(i, j + 1)[k] - (*img)(i, j - 1)[k])*s;
							for (int_t l = 1, _l = (iml::min)(img->w - i, half_n + 1); l < _l; ++l)
								sum[1] += (*img)(i + l, j + 1)[k] - (*img)(i + l, j - 1)[k];
							//代入
							(*buf)(i, j)[k] = isqrt(sum[0] * sum[0] + sum[1] * sum[1]);
						}
				//端の処理(横の方を優先する)
				for (size_t i = 0; i < img->h; ++i)
					for (size_t k = 0; k < 3; ++k) {
						sum[0] = 0;
						//和の算出
						for (int_t m = -static_cast<int_t>((iml::min)(i, half_n)); m < 0; ++m)
							sum[0] += (*img)(1, i + m)[k] - (*img)(0, i + m)[k];
						sum[0] += ((*img)(1, i)[k] - (*img)(0, i)[k])*s;
						for (int_t m = 1, _m = (iml::min)(img->h - i, half_n + 1); m < _m; ++m)
							sum[0] += (*img)(1, i + m)[k] - (*img)(0, i + m)[k];
						//代入
						(*buf)(0, i)[k] = abs(sum[0]);

						sum[0] = 0;
						//和の算出
						for (int_t m = -static_cast<int_t>((iml::min)(i, half_n)); m < 0; ++m)
							sum[0] += (*img)(img->w - 1, i + m)[k] - (*img)(img->w - 2, i + m)[k];
						sum[0] += ((*img)(img->w - 1, i)[k] - (*img)(img->w - 2, i)[k])*s;
						for (int_t m = 1, _m = (iml::min)(img->h - i, half_n + 1); m < _m; ++m)
							sum[0] += (*img)(img->w - 1, i + m)[k] - (*img)(img->w - 2, i + m)[k];
						//代入
						(*buf)(img->w - 1, i)[k] = abs(sum[0]);
					}
				for (size_t i = 1; i < img->w - 1; ++i)
					for (size_t k = 0; k < 3; ++k) {
						sum[0] = 0;
						//和の算出
						for (int_t m = -static_cast<int_t>((iml::min)(i, half_n)); m < 0; ++m)
							sum[0] += (*img)(i + m, 1)[k] - (*img)(i + m, 0)[k];
						sum[0] += ((*img)(i, 1)[k] - (*img)(i, 0)[k])*s;
						for (int_t m = 1, _m = (iml::min)(img->w - i, half_n + 1); m < _m; ++m)
							sum[0] += (*img)(i + m, 1)[k] - (*img)(i + m, 0)[k];
						//代入
						(*buf)(i, 0)[k] = abs(sum[0]);

						sum[0] = 0;
						//和の算出
						for (int_t m = -static_cast<int_t>((iml::min)(i, half_n)); m < 0; ++m)
							sum[0] += (*img)(i + m, img->w - 1)[k] - (*img)(i + m, img->h - 2)[k];
						sum[0] += (*img)(i, img->w - 1)[k] - (*img)(i, img->h - 2)[k];
						for (int_t m = 1, _m = (iml::min)(img->w - i, half_n + 1); m < _m; ++m)
							sum[0] += (*img)(i + m, img->w - 1)[k] - (*img)(i + m, img->h - 2)[k];
						//代入
						(*buf)(i, img->w - 1)[k] = abs(sum[0]);
					}
				iml::swap(img->pixels, buf->pixels);
				delete buf;
			}

			//4方向ラプラシアンフィルタ
			inline void laplacian4(image *img, size_t n) {
				image* buf = new image(img->w, img->h, img->color);
				int_t sum;
				if (!buf->reserve()) { delete buf; return; }

				for (size_t i = 0; i < img->w; ++i)
					for (size_t j = 0; j < img->h; ++j)
						for (size_t k = 0; k < 3; ++k) {
							sum = 0;
							//4方向の情報(上、下、左、右)
							int_t d[4] = { -static_cast<int_t>((iml::min)(j, size_t(1))), (iml::min)(img->h - j, size_t(1))
								,-static_cast<int_t>((iml::min)(i, size_t(1))),(iml::min)(img->w - i, size_t(1)) };
							//上下
							sum -= ((*img)(i, j + d[0])[k] + (*img)(i, j + d[1])[k]);
							//左右
							sum -= ((*img)(i + d[2], j)[k] + (*img)(i + d[3], j)[k]);
							//中央補正をして代入
							(*buf)(i, j)[k] = abs(sum + (*img)(i, j)[k] * 4);
						}
				iml::swap(img->pixels, buf->pixels);
				delete buf;
			}
			//8方向ラプラシアンフィルタ
			inline void laplacian8(image *img) {
				image* buf = new image(img->w, img->h, img->color);
				int_t sum;
				if (!buf->reserve()) { delete buf; return; }

				for (size_t i = 0; i < img->w; ++i)
					for (size_t j = 0; j < img->h; ++j)
						for (size_t k = 0; k < 3; ++k) {
							sum = 0;
							//4方向の情報(上、下、左、右)
							int_t d[4] = { -static_cast<int_t>((iml::min)(j, size_t(1))), (iml::min)(img->h - j, size_t(1))
								,-static_cast<int_t>((iml::min)(i, size_t(1))),(iml::min)(img->w - i, size_t(1)) };
							//上下
							sum -= ((*img)(i, j + d[0])[k] + (*img)(i, j + d[1])[k]);
							//左右
							sum -= ((*img)(i + d[2], j)[k] + (*img)(i + d[3], j)[k]);
							//斜め
							sum -= ((*img)(i + d[2], j + d[0])[k] + (*img)(i + d[3], j + d[0])[k]
								+ (*img)(i + d[2], j + d[1])[k] + (*img)(i + d[3], j + d[1])[k]);
							//中央補正をして代入
							(*buf)(i, j)[k] = abs(sum + (*img)(i, j)[k] * 8);
						}
				iml::swap(img->pixels, buf->pixels);
				delete buf;
			}
		}
	}
}


#endif