#ifndef IMATHLIB_H_MATH_MATH_NUMERIC_TRAITS_HPP
#define IMATHLIB_H_MATH_MATH_NUMERIC_TRAITS_HPP

#include "IMathLib/IMathLib_config.hpp"
#include "IMathLib/math/math/ldexp.hpp"
#include "IMathLib/math/math/abs.hpp"


//浮動小数点の定数
#define IMATHLIB_HUGE_NUM				1E+300			//巨大数
#define IMATHLIB_INFINITYF				(float)(IMATHLIB_HUGE_NUM*IMATHLIB_HUGE_NUM)			//無限大
#define IMATHLIB_INFINITYD				(double)(IMATHLIB_HUGE_NUM*IMATHLIB_HUGE_NUM)
#define IMATHLIB_NANF					(float)(IMATHLIB_INFINITYF*0)						//非数
#define IMATHLIB_NAND					(double)(IMATHLIB_INFINITYD*0)


namespace iml {

	//数値型に対する極限や不定形のための数値特性
	template <class T>
	struct numeric_traits;
	template <>
	struct numeric_traits<int8_t> {
		using type = int8_t;

		static constexpr int_t digits = 7;			//表現ビット数
		static constexpr int_t digits10 = 2;			//10進数での表現桁数
		static constexpr type(min)() noexcept { return -127 - 1; }		//最小値
		static constexpr type(max)() noexcept { return 127; }			//最大値
	};
	template <>
	struct numeric_traits<uint8_t> {
		using type = uint8_t;

		static constexpr int_t digits = 8;
		static constexpr int_t digits10 = 2;
		static constexpr type(min)() noexcept { return 0; }
		static constexpr type(max)() noexcept { return 0XFF; }
	};
	template <>
	struct numeric_traits<int16_t> {
		using type = int16_t;

		static constexpr int_t digits = 15;
		static constexpr int_t digits10 = 4;
		static constexpr type(min)() noexcept { return -32767 - 1; }
		static constexpr type(max)() noexcept { return 32767; }
	};
	template <>
	struct numeric_traits<uint16_t> {
		using type = uint16_t;

		static constexpr int_t digits = 16;
		static constexpr int_t digits10 = 4;
		static constexpr type(min)() noexcept { return 0; }
		static constexpr type(max)() noexcept { return 65535; }
	};
	template <>
	struct numeric_traits<float32_t> {
		using type = float32_t;
		using int_type = int32_t;				//浮動小数点型と同じもしくはそれ以上のビット数をもつ整数型

		static constexpr int_type digits = 24;
		static constexpr int_type digits10 = 6;
		static constexpr int_type fraction_digits = 23;				//符号ビットを除いた仮数部のビット数
		static constexpr int_type exponent_digits = 8;				//指数部のビット数
		static constexpr int_type sign_mask = int_type(1) << (fraction_digits + exponent_digits);		//符号部マスク
		static constexpr int_type fraction_mask = (int_type(1) << fraction_digits) - 1;					//仮数部マスク
		static constexpr int_type exponent_mask = (int_type(1) << exponent_digits) - 1;					//指数部マスク
		static constexpr int_type exponent_bias = (1 << (exponent_digits - 1)) - 1;						//指数部のバイアス

		//演算誤差をどうにかしたいところ・・・
		static constexpr type(min)() noexcept { return /*2*ldexp2(-1.f, exponent_bias)*/-3.402823466E+38f; }
		static constexpr type(max)() noexcept { return /*2*ldexp2(1.f, exponent_bias)*/3.402823466E+38f; }
		static constexpr type norm() { return ldexp2(1.f, 1 - exponent_bias); }								//最小の正規化数
		static constexpr type denorm() { return ldexp2(1.f, 1 - exponent_bias - fraction_digits); }			//最小の非正規化数
		static constexpr type positive_infinity() noexcept { return IMATHLIB_INFINITYF; }
		static constexpr type negative_infinity() noexcept { return -IMATHLIB_INFINITYF; }
		static constexpr type nan() noexcept { return IMATHLIB_NANF; }
		static constexpr type epsilon() noexcept { return ldexp2(1.f, 1 - (fraction_digits + 1)); }

		static constexpr bool is_positive_infinity(type x) { return x == positive_infinity(); }
		static constexpr bool is_negative_infinity(type x) { return x == negative_infinity(); }
		static constexpr bool is_nan(type x) { return x != x; }
	};
	template <>
	struct numeric_traits<float64_t> {
		using type = float64_t;
		using int_type = int64_t;

		static constexpr int_type digits = 53;
		static constexpr int_type digits10 = 15;
		static constexpr int_type fraction_digits = 52;
		static constexpr int_type exponent_digits = 11;
		static constexpr int_type sign_mask = int_type(1) << (fraction_digits + exponent_digits);
		static constexpr int_type fraction_mask = (int_type(1) << fraction_digits) - 1;
		static constexpr int_type exponent_mask = (int_type(1) << exponent_digits) - 1;
		static constexpr int_type exponent_bias = (1 << (exponent_digits - 1)) - 1;

		static constexpr type(min)() noexcept { return /*2*ldexp2(-1., exponent_bias)*/-1.7976931348623158E+308; }
		static constexpr type(max)() noexcept { return /*2*ldexp2(1., exponent_bias)*/1.7976931348623158E+308; }
		static constexpr type norm() { return ldexp2(1., 1 - exponent_bias); }
		static constexpr type denorm() { return ldexp2(1., 1 - exponent_bias - fraction_digits); }
		static constexpr type positive_infinity() noexcept { return IMATHLIB_INFINITYD; }
		static constexpr type negative_infinity() noexcept { return -IMATHLIB_INFINITYD; }
		static constexpr type nan() noexcept { return IMATHLIB_NAND; }
		static constexpr type epsilon() noexcept { return ldexp2(1., 1 - (fraction_digits + 1)); }

		static constexpr bool is_positive_infinity(type x) { return x == positive_infinity(); }
		static constexpr bool is_negative_infinity(type x) { return x == negative_infinity(); }
		static constexpr bool is_nan(type x) { return x != x; }
	};
	template <>
	struct numeric_traits<int32_t> {
		using type = int32_t;
		using float_type = float;				//整数型と同じもしくはそれ以下のビット数をもつ浮動小数点型
		using float_trait = numeric_traits<float_type>;

		//quiet NaN(符号部0かつ指数部全1かつ仮数部が0以外)
		static constexpr type quiet_nan = (float_trait::exponent_mask << float_trait::fraction_digits) | float_trait::fraction_mask;
		//signaling NaN(符号部1かつ指数部全1かつ仮数部が0以外)
		static constexpr type signaling_nan = (1 << 31) | (float_trait::exponent_mask << float_trait::fraction_digits) | float_trait::fraction_mask;
		//正の無限大(符号部0かつ指数部全1かつ仮数部0)
		static constexpr type positive_infinity = float_trait::exponent_mask << float_trait::fraction_digits;
		//負の無限大(符号部1かつ指数部全1かつ仮数部0)
		static constexpr type negative_infinity = (1 << 31) | (float_trait::exponent_mask << float_trait::fraction_digits);

		static constexpr bool is_positive_infinity(type x) { return x == positive_infinity; }
		static constexpr bool is_negative_infinity(type x) { return x == negative_infinity; }
		static constexpr bool is_quiet_nan(type x) { return ((x ^ quiet_nan) <= float_trait::fraction_mask) && ((x ^ quiet_nan) > 0); }
		static constexpr bool is_signaling_nan(type x) { return ((x ^ signaling_nan) <= float_trait::fraction_mask) && ((x ^ signaling_nan) > 0); }
		static constexpr bool is_nan(type x) { return is_quiet_nan(x) || is_signaling_nan(x); }

		static constexpr int_t digits = 31;
		static constexpr int_t digits10 = 9;
		static constexpr type(min)() noexcept { return -2147483647L - 1; }
		static constexpr type(max)() noexcept { return 2147483647L; }
	};
	template <>
	struct numeric_traits<uint32_t> {
		using type = uint32_t;
		using float_type = float;
		using float_trait = numeric_traits<float_type>;

		static constexpr type quiet_nan = (float_trait::exponent_mask << float_trait::fraction_digits) | float_trait::fraction_mask;
		static constexpr type signaling_nan = (type(1) << 31) | (float_trait::exponent_mask << float_trait::fraction_digits) | float_trait::fraction_mask;
		static constexpr type positive_infinity = float_trait::exponent_mask << float_trait::fraction_digits;
		static constexpr type negative_infinity = (type(1) << 31) | (float_trait::exponent_mask << float_trait::fraction_digits);

		static constexpr bool is_positive_infinity(type x) { return x == positive_infinity; }
		static constexpr bool is_negative_infinity(type x) { return x == negative_infinity; }
		static constexpr bool is_quiet_nan(type x) { return ((x ^ quiet_nan) <= float_trait::fraction_mask) && ((x ^ quiet_nan) > 0); }
		static constexpr bool is_signaling_nan(type x) { return ((x ^ signaling_nan) <= float_trait::fraction_mask) && ((x ^ signaling_nan) > 0); }
		static constexpr bool is_nan(type x) { return is_quiet_nan(x) || is_signaling_nan(x); }

		static constexpr int_t digits = 32;
		static constexpr int_t digits10 = 9;
		static constexpr type(min)() noexcept { return 0L; }
		static constexpr type(max)() noexcept { return 4294967295UL; }
	};
	template <>
	struct numeric_traits<int64_t> {
		using type = int64_t;
		using float_type = double;
		using float_trait = numeric_traits<float_type>;

		static constexpr type quiet_nan = (float_trait::exponent_mask << float_trait::fraction_digits) | float_trait::fraction_mask;
		static constexpr type signaling_nan = (type(1) << 63) | (float_trait::exponent_mask << float_trait::fraction_digits) | float_trait::fraction_mask;
		static constexpr type positive_infinity = float_trait::exponent_mask << float_trait::fraction_digits;
		static constexpr type negative_infinity = (type(1) << 63) | (float_trait::exponent_mask << float_trait::fraction_digits);

		static constexpr bool is_positive_infinity(type x) { return x == positive_infinity; }
		static constexpr bool is_negative_infinity(type x) { return x == negative_infinity; }
		static constexpr bool is_quiet_nan(type x) { return ((x ^ quiet_nan) <= float_trait::fraction_mask) && ((x ^ quiet_nan) > 0); }
		static constexpr bool is_signaling_nan(type x) { return ((x ^ signaling_nan) <= float_trait::fraction_mask) && ((x ^ signaling_nan) > 0); }
		static constexpr bool is_nan(type x) { return is_quiet_nan(x) || is_signaling_nan(x); }

		static constexpr int_t digits = 63;
		static constexpr int_t digits10 = 18;
		static constexpr type(min)() noexcept { return -9223372036854775807LL - 1; }
		static constexpr type(max)() noexcept { return 9223372036854775807LL; }
	};
	template <>
	struct numeric_traits<uint64_t> {
		using type = uint64_t;
		using float_type = double;
		using float_trait = numeric_traits<float_type>;

		static constexpr type quiet_nan = (float_trait::exponent_mask << float_trait::fraction_digits) | float_trait::fraction_mask;
		static constexpr type signaling_nan = (type(1) << 63) | (float_trait::exponent_mask << float_trait::fraction_digits) | float_trait::fraction_mask;
		static constexpr type positive_infinity = float_trait::exponent_mask << float_trait::fraction_digits;
		static constexpr type negative_infinity = (type(1) << 63) | (float_trait::exponent_mask << float_trait::fraction_digits);

		static constexpr bool is_positive_infinity(type x) { return x == positive_infinity; }
		static constexpr bool is_negative_infinity(type x) { return x == negative_infinity; }
		static constexpr bool is_quiet_nan(type x) { return ((x ^ quiet_nan) <= float_trait::fraction_mask) && ((x ^ quiet_nan) > 0); }
		static constexpr bool is_signaling_nan(type x) { return ((x ^ signaling_nan) <= float_trait::fraction_mask) && ((x ^ signaling_nan) > 0); }
		static constexpr bool is_nan(type x) { return is_quiet_nan(x) || is_signaling_nan(x); }

		static constexpr int_t digits = 64;
		static constexpr int_t digits10 = 19;
		static constexpr type(min)() noexcept { return 0ULL; }
		static constexpr type(max)() noexcept { return 18446744073709551615ULL; }
	};

	//正の無限大の判定
	template <class T>
	inline constexpr bool is_positive_infinity(const T& x) {
		return numeric_traits<T>::is_positive_infinity(x);
	}
	//負の無限大の判定
	template <class T>
	inline constexpr bool is_negative_infinity(const T& x) {
		return numeric_traits<T>::is_negative_infinity(x);
	}
	//無限大の判定
	template <class T>
	inline constexpr bool is_infinity(const T& x) {
		return is_positive_infinity(x) || is_negative_infinity(x);
	}
	//非数の判定
	template <class T>
	inline constexpr bool is_nan(const T& x) {
		return numeric_traits<T>::is_nan(x);
	}

}


#endif