#ifndef IMATHLIB_H_MEDIA_IMAGE_COLOR_HPP
#define IMATHLIB_H_MEDIA_IMAGE_COLOR_HPP

#include "IMathLib/math/math.hpp"

// 色の関数

namespace iml {

	// [0-255]のRGBカラーを示す構造体
	struct color_rgb {
		unsigned char b;
		unsigned char g;
		unsigned char r;

		// 色の合成(f:合成関数)
		template <class F>
		static void _Color_mix(color_rgb& c1, const color_rgb& c2, F f) {
			c1.r = f(c1.r, c2.r);
			c1.g = f(c1.g, c2.g);
			c1.b = f(c1.b, c2.b);
		}
		// 乗算
		static unsigned char __multiplication(unsigned char c1, unsigned char c2) {
			return (c1 * c2) / 255;
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

		color_rgb& operator+=(const color_rgb& c_v) {
			_Color_mix(*this, c_v, _addition);
			return *this;
		}
		color_rgb& operator-=(const color_rgb& c_v) {
			_Color_mix(*this, c_v, __subtraction);
			return *this;
		}
		color_rgb& operator*=(const color_rgb& c_v) {
			_Color_mix(*this, c_v, __multiplication);
			return *this;
		}
		template <class T>
		color_rgb& operator*=(const T& k) {
			this->r *= k;
			this->g *= k;
			this->b *= k;
			return *this;
		}
		color_rgb& operator/=(const color_rgb& c_v) {
			_Color_mix(*this, c_v, __division);
			return *this;
		}
		template <class T>
		color_rgb& operator/=(const T& k) {
			this->r /= k;
			this->g /= k;
			this->b /= k;
			return *this;
		}
	};
	inline color_rgb operator+(const color_rgb& c1, const color_rgb& c2) {
		color_rgb temp(c1);
		return temp += c2;
	}
	inline color_rgb operator-(const color_rgb& c1, const color_rgb& c2) {
		color_rgb temp(c1);
		return temp -= c2;
	}
	inline color_rgb operator*(const color_rgb& c1, const color_rgb& c2) {
		color_rgb temp(c1);
		return temp *= c2;
	}
	template <class T>
	inline color_rgb operator*(const T& k, const color_rgb& c) {
		color_rgb temp(c);
		return temp *= k;
	}
	template <class T>
	inline color_rgb operator*(const color_rgb& c, const T& k) {
		color_rgb temp(c);
		return temp *= k;
	}
	inline color_rgb operator/(const color_rgb& c1, const color_rgb& c2) {
		color_rgb temp(c1);
		return temp /= c2;
	}
	template <class T>
	inline color_rgb operator/(const color_rgb& c, const T& k) {
		color_rgb temp(c);
		return temp /= k;
	}
	// [0-255]のRGBAカラーを示す構造体
	struct color_rgba {
		unsigned char b;
		unsigned char g;
		unsigned char r;
		unsigned char a;

		// アルファを考慮した色の合成(f:合成関数)
		template <class F>
		static void _Color_mix(color_rgba& c1, const color_rgba& c2, F f) {
			unsigned char temp[3];

			temp[0] = c1.a * c2.a / 255;
			temp[1] = c2.a * (255 - c1.a) / 255;
			temp[2] = c1.a * (255 - c2.a) / 255;

			c1.a = temp[0] + temp[1] + temp[2];
			c1.r = (f(c1.r, c2.r) * temp[0] + temp[1] * c2.r + temp[2] * c1.r) / c1.a;
			c1.g = (f(c1.g, c2.g) * temp[0] + temp[1] * c2.g + temp[2] * c1.g) / c1.a;
			c1.b = (f(c1.b, c2.b) * temp[0] + temp[1] * c2.b + temp[2] * c1.b) / c1.a;
		}
		//乗算
		static unsigned char __multiplication(unsigned char c1, unsigned char c2) {
			return (c1 * c2) / 255;
		}
		//除算
		static unsigned char __division(unsigned char c1, unsigned char c2) {
			return unsigned char((c1 * 256) / (c2 + 1));
		}
		//加算(覆い焼きリニア)
		static unsigned char _addition(unsigned char c1, unsigned char c2) {
			return unsigned char((iml::min<int>)(int(c1 + c2), 255));
		}
		//減算
		static unsigned char __subtraction(unsigned char c1, unsigned char c2) {
			return (c1 < c2) ? 0 : c1 - c2;
		}

		color_rgba& operator+=(const color_rgba& c_v) {
			_Color_mix(*this, c_v, _addition);
			return *this;
		}
		color_rgba& operator-=(const color_rgba& c_v) {
			_Color_mix(*this, c_v, __subtraction);
			return *this;
		}
		color_rgba& operator*=(const color_rgba& c_v) {
			_Color_mix(*this, c_v, __multiplication);
			return *this;
		}
		template <class T>
		color_rgba& operator*=(const T& k) {
			this->r *= k;
			this->g *= k;
			this->b *= k;
			return *this;
		}
		color_rgba& operator/=(const color_rgba& c_v) {
			_Color_mix(*this, c_v, __division);
			return *this;
		}
		template <class T>
		color_rgba& operator/=(const T& k) {
			this->r /= k;
			this->g /= k;
			this->b /= k;
			return *this;
		}
	};
	inline color_rgba operator+(const color_rgba& c1, const color_rgba& c2) {
		color_rgba temp(c1);
		return temp += c2;
	}
	inline color_rgba operator-(const color_rgba& c1, const color_rgba& c2) {
		color_rgba temp(c1);
		return temp -= c2;
	}
	inline color_rgba operator*(const color_rgba& c1, const color_rgba& c2) {
		color_rgba temp(c1);
		return temp *= c2;
	}
	template <class T>
	inline color_rgba operator*(const T& k, const color_rgba& c) {
		color_rgba temp(c);
		return temp *= k;
	}
	template <class T>
	inline color_rgba operator*(const color_rgba& c, const T& k) {
		color_rgba temp(c);
		return temp *= k;
	}
	inline color_rgba operator/(const color_rgba& c1, const color_rgba& c2) {
		color_rgba temp(c1);
		return temp /= c2;
	}
	template <class T>
	inline color_rgba operator/(const color_rgba& c, const T& k) {
		color_rgba temp(c);
		return temp /= k;
	}


	// 0～1で正規化された色
	template <class T>
	struct normal_color {
		T	v[4];						// 配列化
		T	&r = v[0], &g = v[1], &b = v[2], &a = v[3];
		constexpr normal_color() :v{ 1,1,1,1 } {}
		constexpr normal_color(const normal_color& c) : v{ c.v[0],c.v[1],c.v[2],c.v[3] } {}
		constexpr normal_color(T r, T g, T b, T a = 1) : v{ r,g,b,a } {}

		normal_color& operator=(const normal_color& c) {
			for (size_t i = 0; i < 4; ++i) this->v[i] = c.v[i];
			return *this;
		}
		template <class S>
		normal_color& operator=(const normal_color<S>& c) {
			for (size_t i = 0; i < 4; ++i) this->v[i] = c.v[i];
			return *this;
		}
	};

	// 色の相互変換
	template <class T>
	inline constexpr normal_color<T> to_normal_color(const color_rgba& c) {
		return normal_color<T>(static_cast<T>(c.r) / 255, static_cast<T>(c.g) / 255, static_cast<T>(c.b) / 255, static_cast<T>(c.a) / 255);
	}


	// HSVからRGBへの変換(0<=h<=360, 0<=s<=1, 0<=v<=1)
	template <class T>
	normal_color<T> HSV_TO_RGB(T& h, T& s, T& v) {
		int hi;
		T f, p, q, t;
		T r, g, b;

		hi = static_cast<int>(h / 60);
		hi = hi == 6 ? 5 : hi %= 6;
		f = h / 60 - static_cast<double>(hi);
		p = v * (1 - s);
		q = v * (1 - f * s);
		t = v * (1 - (1 - f) * s);
		switch (hi) {
		case 0: r = v; g = t; b = p; break;
		case 1: r = q; g = v; b = p; break;
		case 2: r = p; g = v; b = t; break;
		case 3: r = p; g = q; b = v; break;
		case 4: r = t; g = p; b = v; break;
		case 5: r = v; g = p; b = q; break;
		}
		return normal_color<T>(r, g, b);
	}
	// CMYからRGBへの変換
	template <class T>
	normal_color<T> CMY_TO_RGB(T& c, T& m, T& y) {
		return normal_color<T>(1 - c, 1 - m, 1 - y);
	}
	// RGBコードに変換(24bitカラー)
	template <class T>
	uint32_t RGB_TO_CODE(const normal_color<T>& c) {
		unsigned char r = static_cast<unsigned char>(c.r * 255), g = static_cast<unsigned char>(c.g * 255)
			, b = static_cast<unsigned char>(c.b * 255);
		return (r | (g << 8) | (b << 16));
	}
	// RGBAコードに変換(32bitカラー)
	template <class T>
	uint32_t RGBA_TO_CODE(const normal_color<T>& c) {
		unsigned char r = static_cast<unsigned char>(c.r * 255), g = static_cast<unsigned char>(c.g * 255)
			, b = static_cast<unsigned char>(c.b * 255), a = static_cast<unsigned char>(c.a * 255);
		return (r | (g << 8) | (b << 16) | (a << 24));
	}

	// レベル補正
	template <class T>
	inline normal_color<T> level_correction(const normal_color<T>& c, const T& in_min, const T& in_max, const T& out_min, const T& out_max, const T& g) {
		T g_inv = (g < 0.00001) ? 100000 : 1 / g;
		T in_min_cor, in_max_cor, out_min_cor, out_max_cor;
		T in_length, out_length;
		// 色領域の補正
		in_min_cor = (in_min > 1) ? 1 : ((in_min < 0) ? 0 : in_min);
		in_max_cor = (in_max > 1) ? 1 : ((in_max < 0) ? 0 : in_max);
		in_min_cor = (in_min_cor > in_max_cor) ? in_max_cor : in_min_cor;
		out_min_cor = (out_min > 1) ? 1 : ((out_min < 0) ? 0 : out_min);
		out_max_cor = (out_max > 1) ? 1 : ((out_max < 0) ? 0 : out_max);
		out_min_cor = (out_min_cor > out_max_cor) ? out_max_cor : out_min_cor;
		// 入出力領域
		in_length = in_max_cor - in_min_cor;
		out_length = out_max_cor - out_min_cor;

		// 線形補間で入出力領域を決めてガンマ補正
		T r = (in_length <= 0) ? 1 : (c.r - in_min_cor) / in_length;
		r = (r > 1) ? 1 : r;
		r = pow(r, g_inv);
		r = out_min + out_length * r;
		T g = (in_length <= 0) ? 1 : (c.g - in_min_cor) / in_length;
		g = (g > 1) ? 1 : g;
		g = pow(g, g_inv);
		g = out_min + out_length * g;
		T b = (in_length <= 0) ? 1 : (c.b - in_min_cor) / in_length;
		b = (b > 1) ? 1 : b;
		b = pow(b, g_inv);
		b = out_min + out_length * b;

		return normal_color<T>(r, g, b, c.a);
	}
}

#endif