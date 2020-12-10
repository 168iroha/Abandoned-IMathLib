#ifndef IMATH_MATH_MATH_TYPE_PARAMETER_HPP
#define IMATH_MATH_MATH_TYPE_PARAMETER_HPP

#include "IMathLib/utility/tuple.hpp"


namespace iml {

	template <class>
	struct Inverse_element;


	// あらゆる非型テンプレートを渡すため補助テンプレート
	template <class, class>
	struct type_parameter;


	// type_parameterの判定
	template <class>
	struct is_type_parameter_impl : std::false_type {};
	template <class T, class Param>
	struct is_type_parameter_impl<type_parameter<T, Param>> : std::true_type {};
	template <class T>
	struct is_type_parameter : is_type_parameter_impl<std::remove_cv_t<T>> {};
	template <class T>
	inline constexpr bool is_type_parameter_v = is_type_parameter<T>::value;


	// 整数パラメータの場合
	template <class T, T Val>
	using int_parameter = type_parameter<T, index_tuple<T, Val>>;
	//  bool値
	template <bool Val>
	struct type_parameter<bool, index_tuple<bool, Val>> {
		using type = bool;
		static constexpr type value = Val;
		auto operator-() const { return int_parameter<bool, -value>(); }
		auto operator+() const { return *this; }
		auto operator!() const { return int_parameter<bool, !value>(); }
		auto operator~() const { return int_parameter<bool, ~value>(); }
	};
#define IMATH_TYPE_PARAMETER(NAME)\
	template <NAME Val>\
	struct type_parameter<NAME, index_tuple<NAME, Val>> {\
		using type = NAME;\
		static constexpr type value = Val;\
		auto operator-() const { return int_parameter<NAME, -value>(); }\
		auto operator+() const { return *this; }\
		auto operator!() const { return int_parameter<bool, !value>(); }\
		auto operator~() const { return int_parameter<NAME, ~value>(); }\
		auto operator++() const { return int_parameter<NAME, ++value>(); }\
		auto operator++(int) const { return int_parameter<NAME, value++>(); }\
		auto operator--() const { return int_parameter<NAME, --value>(); }\
		auto operator--(int) const { return int_parameter<NAME, value-->(); }\
	};
	IMATH_TYPE_PARAMETER(uint8_t);
	IMATH_TYPE_PARAMETER(uint16_t);
	IMATH_TYPE_PARAMETER(uint32_t);
	IMATH_TYPE_PARAMETER(uint64_t);
	IMATH_TYPE_PARAMETER(int8_t);
	IMATH_TYPE_PARAMETER(int16_t);
	IMATH_TYPE_PARAMETER(int32_t);
	IMATH_TYPE_PARAMETER(int64_t);
#undef IMATH_TYPE_PARAMETER


	// 浮動小数点型を同じかそれ以上のビット数の整数型へとビット情報を保持したまま変換
	template<class Float>
	constexpr auto float_to_int(Float val) -> decltype(typename numeric_traits<Float>::int_type()) {
		using float_traits = numeric_traits<Float>;
		using int_type = typename float_traits::int_type;
		using int_traits = numeric_traits<int_type>;

		// 非数の場合(全部qNaNとして扱う)
		if (float_traits::is_nan(val)) return int_traits::quiet_nan;
		// 正の無限大の場合
		if (float_traits::is_positive_infinity(val)) return int_traits::positive_infinity;
		// 負の無限大の場合
		if (float_traits::is_negative_infinity(val)) return int_traits::negative_infinity;

		// 符号部マスクの構築
		int_type sign = (val < 0) * float_traits::sign_mask;
		if (sign) val = -val;

		// 非正規化数の場合(0でいいと思う)
		if (val < float_traits::norm()) return 0;

		// 仮数部を仮数部ビット長整数として得る(指数部は予め仮数部ビット長整数分の桁数の補正)
		int_type exponent = float_traits::exponent_bias + float_traits::fraction_digits;
		while (val >= (1 << (numeric_traits<Float>::fraction_digits + 1))) { val *= 0.5; ++exponent; }
		while (val < (1 << (numeric_traits<Float>::fraction_digits))) { val *= 2; --exponent; }

		// 符号部と指数部と仮数部を設定して返す
		return (sign | (exponent << float_traits::fraction_digits) | (static_cast<int_type>(val) & float_traits::fraction_mask));
	}
	// 整数型を同じかそれ以下のビット数の浮動小数点型へとビット情報を保持したまま変換
	template<class Int>
	constexpr auto int_to_float(Int val) -> decltype(typename numeric_traits<Int>::float_type()) {
		using int_traits = numeric_traits<Int>;
		using float_type = typename int_traits::float_type;
		using float_traits = numeric_traits<float_type>;
		using int_type = typename float_traits::int_type;			// Intが符号無し整数の場合も考慮して定義

		// 非数の場合(全部qNaNとして扱う)
		if (int_traits::is_nan(val)) return float_traits::nan();
		// 正の無限大の場合
		if (int_traits::is_positive_infinity(val)) return float_traits::positive_infinity();
		// 負の無限大の場合
		if (int_traits::is_negative_infinity(val)) return float_traits::negative_infinity();

		// 符号部と指数部と仮数部の取得
		int_type sign = !!(val & float_traits::sign_mask);
		int_type exponent = ((val >> float_traits::fraction_digits) & float_traits::exponent_mask);
		int_type fraction = val & float_traits::fraction_mask;

		// 非正規化数の場合(0でいいと思う)
		if ((exponent == 0) && (fraction != 0)) return 0;

		// float_to_intと同様のバイアスの補正
		exponent -= float_traits::exponent_bias + float_traits::fraction_digits;
		// 1.fractionとなるように復元
		fraction |= int_type(1) << float_traits::fraction_digits;

		float_type temp = ldexp2<float_type>(fraction, exponent);
		return (sign) ? -temp : temp;
	}

	// 浮動小数点パラメータの場合
	template <class Float, typename numeric_traits<Float>::int_type Val>
	using float_parameter = type_parameter<Float, index_tuple<typename numeric_traits<Float>::int_type, Val>>;
#define IMATH_TYPE_PARAMETER(NAME)\
	template <typename numeric_traits<NAME>::int_type Val>\
	struct type_parameter<NAME, index_tuple<typename numeric_traits<NAME>::int_type, Val>> {\
		using type = NAME;\
		using int_type = typename numeric_traits<NAME>::int_type;\
		static constexpr type value = int_to_float(Val);\
		auto operator-() const { return float_parameter<NAME, float_to_int(-value)>(); }\
		auto operator+() const { return *this; }\
		auto operator!() const { return int_parameter<bool, !value>(); }\
		auto operator++() const { return float_parameter<NAME, float_to_int(++value)>(); }\
		auto operator++(int) const { return float_parameter<NAME, float_to_int(value++)>(); }\
		auto operator--() const { return float_parameter<NAME, float_to_int(--value)>(); }\
		auto operator--(int) const { return float_parameter<NAME, float_to_int(value--)>(); }\
	};
	IMATH_TYPE_PARAMETER(float32_t);
	IMATH_TYPE_PARAMETER(float64_t);
#undef IMATH_TYPE_PARAMETER


	// 整数型の2項演算の定義
#define IMATH_TYPE_PARAMETER_BINARY_OPERATION1(OP) \
	template <class Int1, Int1 Val1, class Int2, Int2 Val2>\
	inline auto operator##OP##(int_parameter<Int1, Val1>, int_parameter<Int2, Val2>) {\
		constexpr auto temp = Val1##OP##Val2;\
		using temp_type = std::decay_t<decltype(temp)>;\
		return int_parameter<temp_type, temp>();\
	}
	// 整数型と浮動小数点型の2項演算の定義
#define IMATH_TYPE_PARAMETER_BINARY_OPERATION2(OP) \
	IMATH_TYPE_PARAMETER_BINARY_OPERATION1(OP)\
	template <class Int, Int Val1, class Float, typename numeric_traits<Float>::int_type Val2>\
	inline auto operator##OP##(int_parameter<Int, Val1>, float_parameter<Float, Val2>) {\
		constexpr auto temp = Val1##OP##int_to_float(Val2);\
		using temp_type = std::decay_t<decltype(temp)>;\
		return float_parameter<temp_type, float_to_int(temp)>();\
	}\
	template <class Float, typename numeric_traits<Float>::int_type Val1, class Int, Int Val2>\
	inline auto operator##OP##(float_parameter<Float, Val1>, int_parameter<Int, Val2>) {\
		constexpr auto temp = int_to_float(Val1)##OP##Val2;\
		using temp_type = std::decay_t<decltype(temp)>;\
		return float_parameter<temp_type, float_to_int(temp)>();\
	}\
	template <class Float1, typename numeric_traits<Float1>::int_type Val1, class Float2, typename numeric_traits<Float2>::int_type Val2>\
	inline auto operator##OP##(float_parameter<Float1, Val1>, float_parameter<Float2, Val2>) {\
		constexpr auto temp = int_to_float(Val1)##OP##int_to_float(Val2);\
		using temp_type = std::decay_t<decltype(temp)>;\
		return float_parameter<temp_type, float_to_int(temp)>();\
	}
	IMATH_TYPE_PARAMETER_BINARY_OPERATION2(+);
	IMATH_TYPE_PARAMETER_BINARY_OPERATION2(-);
	IMATH_TYPE_PARAMETER_BINARY_OPERATION2(*);
	IMATH_TYPE_PARAMETER_BINARY_OPERATION2(/);
	IMATH_TYPE_PARAMETER_BINARY_OPERATION1(%);
	IMATH_TYPE_PARAMETER_BINARY_OPERATION1(<<);
	IMATH_TYPE_PARAMETER_BINARY_OPERATION1(>>);
	IMATH_TYPE_PARAMETER_BINARY_OPERATION1(&);
	IMATH_TYPE_PARAMETER_BINARY_OPERATION1(|);
	IMATH_TYPE_PARAMETER_BINARY_OPERATION1(^);
#undef IMATH_TYPE_PARAMETER_BINARY_OPERATION1
#undef IMATH_TYPE_PARAMETER_BINARY_OPERATION2

	// 比較演算と論理演算の定義
#define IMATH_TYPE_PARAMETER_COMPARISON_OPERATION(OP) \
	template <class Int1, Int1 Val1, class Int2, Int2 Val2>\
	inline auto operator##OP##(int_parameter<Int1, Val1>, int_parameter<Int2, Val2>) {\
		return int_parameter<bool, Va1##OP##Val2>();\
	}\
	template <class Int, Int Val1, class Float, typename numeric_traits<Float>::int_type Val2>\
	inline auto operator##OP##(int_parameter<Int, Val1>, float_parameter<Float, Val2>) {\
		return int_parameter<bool, Va1##OP##int_to_float(Val2)>();\
	}\
	template <class Float, typename numeric_traits<Float>::int_type Val1, class Int, Int Val2>\
	inline auto operator##OP##(float_parameter<Float, Val1>, int_parameter<Int, Val2>) {\
		return int_parameter<bool, int_to_float(Va1)##OP##Val2>();\
	}\
	template <class Float1, typename numeric_traits<Float1>::int_type Val1, class Float2, typename numeric_traits<Float2>::int_type Val2>\
	inline auto operator##OP##(float_parameter<Float1, Val1>, float_parameter<Float2, Val2>) {\
		return int_parameter<bool, int_to_float(Val1)##OP##int_to_float(Val2)>();\
	}
	IMATH_TYPE_PARAMETER_COMPARISON_OPERATION(<);
	IMATH_TYPE_PARAMETER_COMPARISON_OPERATION(<=);
	IMATH_TYPE_PARAMETER_COMPARISON_OPERATION(>);
	IMATH_TYPE_PARAMETER_COMPARISON_OPERATION(>=);
	IMATH_TYPE_PARAMETER_COMPARISON_OPERATION(==);
	IMATH_TYPE_PARAMETER_COMPARISON_OPERATION(!=);
	IMATH_TYPE_PARAMETER_COMPARISON_OPERATION(&&);
	IMATH_TYPE_PARAMETER_COMPARISON_OPERATION(||);
#undef IMATH_TYPE_PARAMETER_COMPARISON_OPERATION


	// type_parameterの逆元
#define IMATH_INVERSE_ELEMENT(TYPE)\
	template <TYPE Val> struct Inverse_element<int_parameter<TYPE, Val>> {};
	IMATH_INVERSE_ELEMENT(uint8_t);
	IMATH_INVERSE_ELEMENT(uint16_t);
	IMATH_INVERSE_ELEMENT(uint32_t);
	IMATH_INVERSE_ELEMENT(uint64_t);
#undef IMATH_INVERSE_ELEMENT
#define IMATH_INVERSE_ELEMENT(TYPE)\
	template <TYPE Val>\
	struct Inverse_element<int_parameter<TYPE, Val>> {\
		static auto _additive_inverse_(int_parameter<TYPE, Val>) { return int_parameter<TYPE, -Val>(); }\
	};
	IMATH_INVERSE_ELEMENT(int8_t);
	IMATH_INVERSE_ELEMENT(int16_t);
	IMATH_INVERSE_ELEMENT(int32_t);
	IMATH_INVERSE_ELEMENT(int64_t);
#undef IMATH_INVERSE_ELEMENT
#define IMATH_INVERSE_ELEMENT(TYPE)\
	template <typename numeric_traits<TYPE>::int_type Val>\
	struct Inverse_element<float_parameter<TYPE, Val>> {\
		static auto _additive_inverse_(float_parameter<TYPE, Val>) { return float_parameter<TYPE, float_to_int(-int_to_float(Val))>(); }\
		static auto _multiplicative_inverse_(float_parameter<TYPE, Val>) { return float_parameter<TYPE, float_to_int(1 / int_to_float(Val))>(); }\
	};
	IMATH_INVERSE_ELEMENT(float32_t);
	IMATH_INVERSE_ELEMENT(float64_t);
#undef IMATH_INVERSE_ELEMENT


	// 配列型パラメータ
	// double temp[M][N][L] = {...} ならarray_parameter<double, index_tuple<size_t, M, N, L>, ...>
	template <class, class, class>
	struct array_parameter_impl;
	// T : 配列の生成元となる型, Indices : 次元のリスト, TypeTuple : 配列のそれぞれの要素
	template <class T, size_t... Indices, class TypeTuple>
	struct array_parameter_impl<T, index_tuple<size_t, Indices...>, TypeTuple> {
		using type = type_parameter<multi_array<T, Indices...>, TypeTuple>;
	};
	template <class T, class IndexTuple, class... Types>
	using array_parameter = typename array_parameter_impl<T, IndexTuple, type_tuple<Types...>>::type;
	// array_parameterをN等分する(Cnt1 : 分割した1つ当たりの要素数のカウント, Cnt2 : 現在分割した数のカウント)
	// (Types : 配列の値リスト, Result : type_tupleによって保持される結果)
	template <class Array, size_t N, class Types, class Result = type_tuple<type_tuple<>>, size_t Cnt1 = 0, size_t Cnt2 = 0, bool = (Cnt1 == Array::value), bool = (Cnt2 == N)>
	struct split_array_parameter {};
	template <class T, size_t... Indices, size_t N, class... Types2, size_t Cnt1, size_t Cnt2>
	struct split_array_parameter<multi_array<T, Indices...>, N, type_tuple<>, type_tuple<type_tuple<>, Types2...>, Cnt1, Cnt2, false, true> {
		using type = reverse_type_tuple_t<type_tuple<Types2...>>;
	};
	template <class T, size_t... Indices, size_t N, class... Types1, class... Types2, class... Types3, size_t Cnt1, size_t Cnt2>
	struct split_array_parameter<multi_array<T, Indices...>, N, type_tuple<Types1...>, type_tuple<type_tuple<Types2...>, Types3...>, Cnt1, Cnt2, true, false>
		: split_array_parameter<multi_array<T, Indices...>, N, type_tuple<Types1...>, type_tuple<type_tuple<>, array_parameter<T, index_tuple<size_t, Indices...>, Types2...>, Types3...>, 0, Cnt2 + 1> {};
	template <class T, size_t... Indices, size_t N, class FirstT, class... Types1, class... Types2, class... Types3, size_t Cnt1, size_t Cnt2>
	struct split_array_parameter<multi_array<T, Indices...>, N, type_tuple<FirstT, Types1...>, type_tuple<type_tuple<Types2...>, Types3...>, Cnt1, Cnt2, false, false>
		: split_array_parameter<multi_array<T, Indices...>, N, type_tuple<Types1...>, type_tuple<type_tuple<Types2..., FirstT>, Types3...>, Cnt1 + 1, Cnt2> {};

	// 1次元配列
	template <class T, size_t First, class... Params>
	struct type_parameter<multi_array<T, First>, type_tuple<type_parameter<T, Params>...>> {
		static_assert(First == type_tuple<type_parameter<T, Params>...>::value, "The number of parameters and dimensions must be equal.");

		using type = multi_array_t<T, First>;
		static constexpr type value = { (type_parameter<T, Params>::value)... };

		template <class T, T Val, class = typename enable_if<(Val >= 0) && (Val < First)>::type>
		auto operator[](int_parameter<T, Val>) const { return at_type_t<Val, type_parameter<T, Params>...>(); }
	};
	// 2次元以上の配列
	template <class T, size_t First, size_t Second, size_t... Indices, class... Params>
	struct type_parameter<multi_array<T, First, Second, Indices...>, type_tuple<type_parameter<T, Params>...>> {
		static_assert(dimension_v<index_tuple<size_t, First, Second, Indices...>> == type_tuple<type_parameter<T, Params>...>::value
			, "The number of parameters and dimensions must be equal.");

		using type = multi_array_t<T, First, Second, Indices...>;
		// static constexpr type value = { (type_parameter<T, Params>::value)... };

		template <class T, T Val, class = typename enable_if<(Val >= 0) && (Val < First)>::type>
		auto operator[](int_parameter<T, Val>) const {
			using split_type = typename split_array_parameter<multi_array<T, Second, Indices...>, First, type_tuple<type_parameter<T, Params>...>>::type;
			return at_type_tuple_t<Val, split_type>();
		}
	};


	// 非標準的な演算は名前空間 tp に定義
	namespace tp {

		// 全てのtype_parameterを加算する
		template <class ResultT, class ResultP>
		auto sum_impl(type_parameter<ResultT, ResultP> result) { return result; }
		template <class ResultT, class ResultP, class FirstT, class FirstP, class... Types, class... Params>
		auto sum_impl(type_parameter<ResultT, ResultP> result, type_parameter<FirstT, FirstP> first, type_parameter<Types, Params>... args) {
			return sum_impl(result + first, args...);
		}
		template <class FirstT, class FirstP, class... Types, class... Params>
		auto sum(type_parameter<FirstT, FirstP> first, type_parameter<Types, Params>... args) { return sum_impl(first, args...); }

	}
}


#endif