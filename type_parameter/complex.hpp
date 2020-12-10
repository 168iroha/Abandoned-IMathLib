#ifndef IMATHLIB_H_MATH_HYPERCOMPLEX_COMPLEX_HPP
#define IMATHLIB_H_MATH_HYPERCOMPLEX_COMPLEX_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/type_parameter.hpp"


namespace iml {

	template <class>
	class complex;

	// 複素数型のパラメータ
	template <class T, class Re, class Im>
	using complex_parameter = type_parameter<complex<T>, type_tuple<Re, Im>>;
	template <class T, class ReP, class ImP>
	struct type_parameter<complex<T>, type_tuple<type_parameter<T, ReP>, type_parameter<T, ImP>>> {
		using type = complex<T>;
		static constexpr type value = type(type_parameter<T, ReP>::value, type_parameter<T, ImP>::value);

		template <class = std::enable_if_t<is_exist_additive_inverse_v<T>>>
		auto operator-() const { return complex_parameter<T, decltype(-type_parameter<T, ReP>()), decltype(-type_parameter<T, ImP>())>(); }
		auto operator+() const { return *this; }

		template <class T, T Val, class = std::enable_if_t<(Val >= 0) && (Val < 2)>>
		auto operator[](int_parameter<T, Val>) const { return at_type_tuple_t<Val, type_tuple<type_parameter<T, ReP>, type_parameter<T, ImP>>>(); }
	};


	// 複素数におけるスカラー演算と標準演算の十分条件の定義
	// 加算
	template <class T1, class T2>
	struct is_lscalar_operation_impl2<is_addable, T1, complex<T2>> : std::bool_constant<is_high_rank_math_type_v<T1, add_result_t<T1, T2>> || is_high_rank_math_type_v<add_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_addable, complex<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<T2, add_result_t<T1, T2>> || is_high_rank_math_type_v<add_result_t<T1, T2>, T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_addable, complex<T1>, complex<T2>> : std::bool_constant<is_addable_v<T1, T2>> {};
	// 減算
	template <class T1, class T2>
	struct is_lscalar_operation_impl2<is_subtractable, T1, complex<T2>> : std::bool_constant<is_high_rank_math_type_v<T1, sub_result_t<T1, T2>> || is_high_rank_math_type_v<sub_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_subtractable, complex<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<T2, sub_result_t<T1, T2>> || is_high_rank_math_type_v<sub_result_t<T1, T2>, T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_subtractable, complex<T1>, complex<T2>> : std::bool_constant<is_subtractable_v<T1, T2>> {};
	// 乗算
	template <class T1, class T2>
	struct is_lscalar_operation_impl2<is_multipliable, T1, complex<T2>> : std::bool_constant<is_multipliable_v<T1, T2>> {};
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_multipliable, complex<T1>, T2> : std::bool_constant<is_multipliable_v<T1, T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_multipliable, complex<T1>, complex<T2>> : std::bool_constant<is_exist_additive_inverse_v<mul_result_t<T1, T2>>> {};
	// 除算
	template <class T1, class T2>
	struct is_lscalar_operation_impl2<is_divisible, T1, complex<T2>> : std::bool_constant<is_multipliable_v<T1, T2> && is_skew_field_v<T2>> {};
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_divisible, complex<T1>, T2> : std::bool_constant<is_divisible_v<T1, T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_divisible, complex<T1>, complex<T2>> : std::bool_constant<is_exist_additive_inverse_v<mul_result_t<T1, T2>> && is_skew_field_v<T2>> {};
	// 等価比較
	template <class T1, class T2>
	struct is_lscalar_operation_impl2<is_comparable, T1, complex<T2>> : std::bool_constant<is_comparable_v<T1, T2> && is_exist_additive_identity_v<T1>> {};
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_comparable, complex<T1>, T2> : std::bool_constant<is_comparable_v<T1, T2> && is_exist_additive_identity_v<T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_comparable, complex<T1>, complex<T2>> : std::bool_constant<is_comparable_v<T1, T2>> {};
	// 代入
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_assignable, complex<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<T2, T1> && is_exist_additive_identity_v<T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_assignable, complex<T1>, complex<T2>> : std::bool_constant<is_high_rank_math_type_v<T2, T1>> {};
	// 加算代入
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_add_assignable, complex<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<add_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_add_assignable, complex<T1>, complex<T2>> : std::bool_constant<is_high_rank_math_type_v<add_result_t<T1, T2>, T1>> {};
	// 減算代入
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_sub_assignable, complex<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<sub_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_sub_assignable, complex<T1>, complex<T2>> : std::bool_constant<is_high_rank_math_type_v<sub_result_t<T1, T2>, T1>> {};
	// 乗算代入
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_mul_assignable, complex<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<mul_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_mul_assignable, complex<T1>, complex<T2>> : std::bool_constant<is_high_rank_math_type_v<mul_result_t<T1, T2>, T1> && is_exist_additive_inverse_v<mul_result_t<T1, T2>>> {};
	// 除算代入
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_div_assignable, complex<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<div_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_div_assignable, complex<T1>, complex<T2>> : std::bool_constant<is_high_rank_math_type_v<mul_result_t<T1, T2>, T1> && is_exist_additive_inverse_v<mul_result_t<T1, T2>> && is_skew_field_v<T2>> {};


	// 複素数型
	template <class T>
	class complex {
		template <class> friend class complex;
		T x_m[2];
	public:
		constexpr complex() : x_m{} {}
		template <class U, class = std::enable_if_t<is_rscalar_operation_v<is_assignable, complex, U>>>
		constexpr complex(const U& re) : x_m{ re } {}
		template <class U1, class U2, class = std::enable_if_t<is_standard_operation_v<is_assignable, complex, complex<common_math_type_t<U1, U2>>>>>
		constexpr complex(const U1& re, const U2& im) : x_m{ T(re),T(im) } {}
		constexpr complex(const complex& c) : x_m{ c.x_m[0],c.x_m[1] } {}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_assignable, complex, complex<U>>>>
		constexpr complex(const complex<U>& c) : x_m{ c.x_m[0],c.x_m[1] } {}

		using basis_type = T;
		using iterator = linear_iterator<T>;
		using const_iterator = linear_iterator<const T>;

		template <class Other>
		struct rebind {
			using other = complex<Other>;
		};

		constexpr iterator begin() noexcept { return iterator(x_m); }
		constexpr const_iterator begin() const noexcept { return const_iterator(x_m); }
		constexpr iterator end() noexcept { return iterator(x_m + 2); }
		constexpr const_iterator end() const noexcept { return const_iterator(x_m + 2); }

		// 単項演算
		template <class = std::enable_if_t<is_exist_additive_inverse_v<T>>>
		constexpr complex operator-() const { return complex(-this->x_m[0], -this->x_m[1]); }
		constexpr complex operator+() const { return complex(*this); }
		// 代入演算
		complex& operator=(const complex& c) {
			if (this != std::addressof(c)) { this->x_m[0] = c.x_m[0]; this->x_m[1] = c.x_m[1]; }
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_assignable, complex, complex<U>>>>
		complex& operator=(const complex<U>& c) {
			this->x_m[0] = c.x_m[0]; this->x_m[1] = c.x_m[1];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_rscalar_operation_v<is_assignable, complex, U>>>
		complex& operator=(const U& n) {
			this->x_m[0] = n; this->x_m[1] = addition_traits<T>::identity_element();
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_add_assignable, complex, complex<U>>>>
		complex& operator+=(const complex<U>& c) {
			this->x_m[0] += c.x_m[0]; this->x_m[1] += c.x_m[1];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_rscalar_operation_v<is_add_assignable, complex, U>>>
		complex& operator+=(const U& n) {
			this->x_m[0] += n;
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_sub_assignable, complex, complex<U>>>>
		complex& operator-=(const complex<U>& c) {
			this->x_m[0] -= c.x_m[0]; this->x_m[1] -= c.x_m[1];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_rscalar_operation_v<is_sub_assignable, complex, U>>>
		complex& operator-=(const U& n) {
			this->x_m[0] -= n;
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_mul_assignable, complex, complex<U>>>>
		complex& operator*=(const complex<U>& c) {
			T temp[2] = { this->x_m[0] * c.x_m[0] - this->x_m[1] * c.x_m[1], this->x_m[0] * c.x_m[1] + this->x_m[1] * c.x_m[0] };
			this->x_m[0] = temp[0]; this->x_m[1] = temp[1];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_rscalar_operation_v<is_mul_assignable, complex, U>>>
		complex& operator*=(const U& k) {
			this->x_m[0] *= k; this->x_m[1] *= k;
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_div_assignable, complex, complex<U>>>>
		complex& operator/=(const complex<U>& c) {
			T temp[] = { this->x_m[0] * conj(c.x_m[0]) + this->x_m[1] * conj(c.x_m[1]), this->x_m[1] * conj(c.x_m[0]) - this->x_m[0] * conj(c.x_m[1]) };
			U temp2 = abs2(c.x_m[0]) + abs2(c.x_m[1]);
			this->x_m[0] = temp[0] / temp2; this->x_m[1] = temp[1] / temp2;
			return *this;
		}
		template <class U, class = std::enable_if_t<is_rscalar_operation_v<is_div_assignable, complex, U>>>
		complex& operator/=(const U& k) {
			this->x_m[0] /= k; this->x_m[1] /= k;
			return *this;
		}

		// 添え字演算
		const constexpr T& operator[](size_t index) const { return this->x_m[index]; }
		constexpr T& operator[](size_t index) { return this->x_m[index]; }

		// ストリーム出力
		friend std::ostream& operator<<(std::ostream& os, const complex& n) {
			os << '(' << n.x_m[0] << ',' << n.x_m[1] << ')';
			return os;
		}
		friend std::wostream& operator<<(std::wostream& os, const complex& n) {
			os << L'(' << n.x_m[0] << L',' << n.x_m[1] << L')';
			return os;
		}

		template <class T>
		constexpr linear_input<iterator> operator<<(const T& value) {
			iterator itr = this->begin();
			*itr = value; ++itr;
			return linear_input<iterator>(itr, this->end());
		}
	};


	template <class U1, class U2>
	inline constexpr complex<common_math_type_t<U1, U2>> make_complex(U1&& re, U2&& im) {
		return complex<common_math_type_t<U1, U2>>(std::forward<U1>(re), std::forward<U2>(im));
	}


	// 複素数の2項演算
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_addable, complex<T1>, complex<T2>>>>
	inline constexpr auto operator+(const complex<T1>& lhs, const complex<T2>& rhs) {
		return complex<add_result_t<T1, T2>>(lhs[0] + rhs[0], lhs[1] + rhs[1]);
	}
	template <class T1, class T2, class Re1, class Im1, class Re2, class Im2, class = std::enable_if_t<is_standard_operation_v<is_addable, complex<T1>, complex<T2>>>>
	inline auto operator+(complex_parameter<T1, Re1, Im1>, complex_parameter<T2, Re2, Im2>) {
		return complex_parameter<add_result_t<T1, T2>, decltype(Re1() + Re2()), decltype(Im1() + Im2())>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_rscalar_operation_v<is_addable, complex<T1>, T2>>>
	inline constexpr auto operator+(const complex<T1>& lhs, const T2& rhs) {
		return complex<select_high_rank_math_type_t<add_result_t<T1, T2>, T1>>(lhs[0] + rhs, lhs[1]);
	}
	template <class T1, class T2, class Re, class Im, class Param, class = std::enable_if_t<is_rscalar_operation_v<is_addable, complex<T1>, T2>>>
	inline auto operator+(complex_parameter<T1, Re, Im>, type_parameter<T2, Param> rhs) {
		return complex_parameter<select_high_rank_math_type_t<add_result_t<T1, T2>, T1>, decltype(Re() + rhs), Im>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_lscalar_operation_v<is_addable, T1, complex<T2>>>>
	inline constexpr auto operator+(const T1& lhs, const complex<T2>& rhs) {
		return complex<select_high_rank_math_type_t<add_result_t<T1, T2>, T2>>(lhs + rhs[0], rhs[1]);
	}
	template <class T1, class T2, class Param, class Re, class Im, class = std::enable_if_t<is_lscalar_operation_v<is_addable, T1, complex<T2>>>>
	inline auto operator+(type_parameter<T1, Param> lhs, complex_parameter<T2, Re, Im>) {
		return complex_parameter<select_high_rank_math_type_t<add_result_t<T1, T2>, T2>, decltype(lhs + Re()), Im>();
	}

	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_subtractable, complex<T1>, complex<T2>>>>
	inline constexpr auto operator-(const complex<T1>& lhs, const complex<T2>& rhs) {
		return complex<sub_result_t<T1, T2>>(lhs[0] - rhs[0], lhs[1] - rhs[1]);
	}
	template <class T1, class T2, class Re1, class Im1, class Re2, class Im2, class = std::enable_if_t<is_standard_operation_v<is_subtractable, complex<T1>, complex<T2>>>>
	inline auto operator-(complex_parameter<T1, Re1, Im1>, complex_parameter<T2, Re2, Im2>) {
		return complex_parameter<sub_result_t<T1, T2>, decltype(Re1() - Re2()), decltype(Im1() - Im2())>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_rscalar_operation_v<is_subtractable, complex<T1>, T2>>>
	inline constexpr auto operator-(const complex<T1>& lhs, const T2& rhs) {
		return complex<select_high_rank_math_type_t<sub_result_t<T1, T2>, T1>>(lhs[0] - rhs, lhs[1]);
	}
	template <class T1, class T2, class Re, class Im, class Param, class = std::enable_if_t<is_rscalar_operation_v<is_subtractable, complex<T1>, T2>>>
	inline auto operator-(complex_parameter<T1, Re, Im>, type_parameter<T2, Param> rhs) {
		return complex_parameter<select_high_rank_math_type_t<sub_result_t<T1, T2>, T1>, decltype(Re() - rhs), Im>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_lscalar_operation_v<is_subtractable, T1, complex<T2>>>>
	inline constexpr auto operator-(const T1& lhs, const complex<T2>& rhs) {
		return complex<select_high_rank_math_type_t<sub_result_t<T1, T2>, T2>>(lhs - rhs[0], -rhs[1]);
	}
	template <class T1, class T2, class Param, class Re, class Im, class = std::enable_if_t<is_lscalar_operation_v<is_subtractable, T1, complex<T2>>>>
	inline auto operator-(type_parameter<T1, Param> lhs, complex_parameter<T2, Re, Im>) {
		return complex_parameter<select_high_rank_math_type_t<sub_result_t<T1, T2>, T2>, decltype(lhs - Re()), decltype(-Im())>();
	}

	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_multipliable, complex<T1>, complex<T2>>>>
	inline constexpr auto operator*(const complex<T1>& lhs, const complex<T2>& rhs) {
		return complex<mul_result_t<T1, T2>>(lhs[0] * rhs[0] - lhs[1] * rhs[1], lhs[0] * rhs[1] + lhs[1] * rhs[0]);
	}
	template <class T1, class T2, class Re1, class Im1, class Re2, class Im2, class = std::enable_if_t<is_standard_operation_v<is_multipliable, complex<T1>, complex<T2>>>>
	inline auto operator*(complex_parameter<T1, Re1, Im1>, complex_parameter<T2, Re2, Im2>) {
		return complex_parameter<mul_result_t<T1, T2>, decltype(Re1() * Re2() - Im1() * Im2()), decltype(Re1() * Im2() + Im1() * Re2())>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_rscalar_operation_v<is_multipliable, complex<T1>, T2>>>
	inline constexpr auto operator*(const complex<T1>& lhs, const T2& rhs) {
		return complex<mul_result_t<T1, T2>>(lhs[0] * rhs, lhs[1] * rhs);
	}
	template <class T1, class T2, class Re, class Im, class Param, class = std::enable_if_t<is_rscalar_operation_v<is_multipliable, complex<T1>, T2>>>
	inline auto operator*(complex_parameter<T1, Re, Im>, type_parameter<T2, Param> rhs) {
		return complex_parameter<mul_result_t<T1, T2>, decltype(Re() * rhs), decltype(Im() * rhs)>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_lscalar_operation_v<is_multipliable, T1, complex<T2>>>>
	inline constexpr auto operator*(const T1& lhs, const complex<T2>& rhs) {
		return complex<mul_result_t<T1, T2>>(lhs * rhs[0], lhs * rhs[1]);
	}
	template <class T1, class T2, class Param, class Re, class Im, class = std::enable_if_t<is_lscalar_operation_v<is_multipliable, T1, complex<T2>>>>
	inline auto operator*(type_parameter<T1, Param> lhs, complex_parameter<T2, Re, Im>) {
		return complex_parameter<mul_result_t<T1, T2>, decltype(lhs * Re()), decltype(lhs * Im())>();
	}

	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_divisible, complex<T1>, complex<T2>>>>
	inline constexpr auto operator/(const complex<T1>& lhs, const complex<T2>& rhs) {
		auto temp = rhs[0] * rhs[0] + rhs[1] * rhs[1];
		return complex<mul_result_t<T1, T2>>((lhs[0] * rhs[0] + lhs[1] * rhs[1]) / temp
			, (lhs[1] * rhs[0] - lhs[0] * rhs[1]) / temp);
	}
	template <class T1, class T2, class Re1, class Im1, class Re2, class Im2, class = std::enable_if_t<is_standard_operation_v<is_divisible, complex<T1>, complex<T2>>>>
	inline auto operator/(complex_parameter<T1, Re1, Im1>, complex_parameter<T2, Re2, Im2>) {
		using type = decltype(Re2() * Re2() + Im2() * Im2());
		return complex_parameter<mul_result_t<T1, T2>, decltype((Re1() * Re2() + Im1() * Im2()) / type())
			, decltype((Im1() * Re2() - Re1() * Im2()) / type())>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_rscalar_operation_v<is_divisible, complex<T1>, T2>>>
	inline constexpr auto operator/(const complex<T1>& lhs, const T2& rhs) {
		return complex<div_result_t<T1, T2>>(lhs[0] / rhs, lhs[1] / rhs);
	}
	template <class T1, class T2, class Re, class Im, class Param, class = std::enable_if_t<is_rscalar_operation_v<is_divisible, complex<T1>, T2>>>
	inline auto operator/(complex_parameter<T1, Re, Im>, type_parameter<T2, Param> rhs) {
		return complex_parameter<div_result_t<T1, T2>, decltype(Re() / rhs), decltype(Im() / rhs)>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_lscalar_operation_v<is_divisible, T1, complex<T2>>>>
	inline constexpr auto operator/(const T1& lhs, const complex<T2>& rhs) {
		auto temp = rhs[0] * rhs[0] + rhs[1] * rhs[1];
		return complex<mul_result_t<T1, T2>>(lhs * rhs[0] / temp, -lhs * rhs[1] / temp);
	}
	template <class T1, class T2, class Param, class Re, class Im, class = std::enable_if_t<is_lscalar_operation_v<is_divisible, T1, complex<T2>>>>
	inline auto operator/(type_parameter<T1, Param> lhs, complex_parameter<T2, Re, Im>) {
		using type = decltype(Re() * Re() + Im() * Im());
		return complex<mul_result_t<T1, T2>, decltype(lhs * Re() / type()), decltype(-lhs * Im() / type())>();
	}


	// 比較演算
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_comparable, complex<T1>, complex<T2>>>>
	inline constexpr bool operator==(const complex<T1>& lhs, const complex<T2>& rhs) {
		return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1]);
	}
	template <class T1, class T2, class Re1, class Im1, class Re2, class Im2, class = std::enable_if_t<is_standard_operation_v<is_comparable, complex<T1>, complex<T2>>>>
	inline auto operator==(complex_parameter<T1, Re1, Im1>, complex_parameter<T2, Re2, Im2>) {
		return (Re1() == Re2()) && (Im1() == Im2());
	}
	template <class T1, class T2, class = std::enable_if_t<is_rscalar_operation_v<is_comparable, complex<T1>, T2>>>
	inline constexpr bool operator==(const complex<T1>& lhs, T2& rhs) {
		return (lhs[0] == rhs) && is_absorbing_element(lhs[1]);
	}
	template <class T1, class T2, class Re, class Im, class Param, class = std::enable_if_t<is_rscalar_operation_v<is_comparable, complex<T1>, T2>>>
	inline auto operator==(complex_parameter<T1, Re, Im>, type_parameter<T2, Param> rhs) {
		return (Re() == rhs) && int_parameter<bool, is_absorbing_element(Im::value)>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_lscalar_operation_v<is_comparable, T1, complex<T2>>>>
	inline constexpr bool operator==(const T1& lhs, const complex<T2>& rhs) {
		return (lhs == rhs[0]) && is_absorbing_element(rhs[1]);
	}
	template <class T1, class T2, class Param, class Re, class Im, class = std::enable_if_t<is_lscalar_operation_v<is_comparable, T1, complex<T2>>>>
	inline auto operator==(type_parameter<T1, Param> lhs, complex_parameter<T2, Re, Im>) {
		return (lhs == Re()) && int_parameter<bool, is_absorbing_element(Im::value)>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_comparable, complex<T1>, complex<T2>>>>
	inline constexpr bool operator!=(const complex<T1>& lhs, const complex<T2>& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class Re1, class Im1, class Re2, class Im2, class = std::enable_if_t<is_standard_operation_v<is_comparable, complex<T1>, complex<T2>>>>
	inline auto operator!=(complex_parameter<T1, Re1, Im1> lhs, complex_parameter<T2, Re2, Im2> rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class = std::enable_if_t<is_rscalar_operation_v<is_comparable, complex<T1>, T2>>>
	inline constexpr bool operator!=(const complex<T1>& lhs, T2& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class Re, class Im, class Param, class = std::enable_if_t<is_rscalar_operation_v<is_comparable, complex<T1>, T2>>>
	inline auto operator!=(complex_parameter<T1, Re, Im> lhs, type_parameter<T2, Param> rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class = std::enable_if_t<is_lscalar_operation_v<is_comparable, T1, complex<T2>>>>
	inline constexpr bool operator!=(const T1& lhs, const complex<T2>& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class Param, class Re, class Im, class = std::enable_if_t<is_lscalar_operation_v<is_comparable, T1, complex<T2>>>>
	inline auto operator!=(type_parameter<T1, Param> lhs, complex_parameter<T2, Re, Im> rhs) { return !(lhs == rhs); }


	// 複素数型の生成
	template <class T1, class T2>
	inline constexpr complex<unwrap_reference_t<T1>> make_complex(T1&& re, T2&& im) {
		return complex<unwrap_reference_t<T1>>(std::forward<T1>(re), std::forward<T2>(im));
	}


	// 複素数の判定
	template <class T>
	struct is_complex_impl : std::false_type {};
	template <class T>
	struct is_complex_impl<complex<T>> : std::true_type {};
	template <class T>
	struct is_complex : is_complex_impl<std::remove_cv_t<T>> {};
	template <class T>
	inline constexpr bool is_complex_v = is_complex<T>::value;


	// 複素数の除去
	template <class T>
	struct remove_complex {
		using type = T;
	};
	template <class T>
	struct remove_complex<complex<T>> {
		using type = T;
	};
	template <class T>
	using remove_complex_t = typename remove_complex<T>::type;


	template <class From, class To>
	struct is_high_rank_math_type_complex : is_high_rank_math_type<From, typename To::basis_type> {};
	template <class From, class To>
	struct is_high_rank_math_type_complex<complex<From>, To> : std::bool_constant<
		// complex<From>がToの基底となる場合も含めて判定
		is_high_rank_math_type_v<complex<From>, typename To::basis_type> || is_high_rank_math_type_v<From, typename To::basis_type>
	> {};
	// Fromの型によりis_high_rank_math_type_complexで分岐
	template <class From, class To>
	struct is_high_rank_math_type<From, complex<To>> : is_high_rank_math_type_complex<From, complex<To>> {};


	// 加法の特性
	template <class T>
	struct addition_traits<complex<T>> {
		// 単位元
		template <class = std::enable_if_t<is_exist_additive_identity_v<T>>>
		static constexpr T identity_element() { return T(); }
		// 結合律
		static constexpr bool associative_value = addition_traits<T>::associative_value;
		// 消約律
		static constexpr bool cancellative_value = addition_traits<T>::cancellative_value;
		// 可換律
		static constexpr bool commutative_value = addition_traits<T>::commutative_value;
	};
	// 乗法の特性
	template <class T>
	struct multiplication_traits<complex<T>> {
		// 単位元
		template <class = std::enable_if_t<is_exist_multiplicative_identity_v<T>>>
		static constexpr auto identity_element() { return addition_traits<T>::identity_element(); }
		// 吸収元
		template <class = std::enable_if_t<is_exist_absorbing_element_v<T>>>
		static constexpr T absorbing_element() { return T(); }
		// 結合律
		static constexpr bool associative_value = multiplication_traits<T>::associative_value;
		// 消約律
		static constexpr bool cancellative_value = multiplication_traits<T>::cancellative_value;
		// 可換律
		static constexpr bool commutative_value = multiplication_traits<T>::commutative_value;
		// 分配律
		static constexpr bool distributive_value = multiplication_traits<T>::distributive_value;
	};


	// 逆元が存在するならば逆元の取得(存在しない場合は例外を出す)
	template <class T>
	struct Inverse_element<complex<T>> {
		static constexpr complex<T> _additive_inverse_(const complex<T>& c) {
			return complex<T>(additive_inverse(c[0]), additive_inverse(c[1]));
		}

		// Tに乗法逆元が存在する場合の乗法逆元
		static constexpr complex<T> _multiplicative_inverse_impl_(const complex<T>& c, std::true_type) {
			// 共役を絶対値の2乗で割る
			T temp = c[0] * c[0] + c[1] * c[1];
			return complex<T>(c[0] / temp, -c[1] / temp);
		}
		// Tの乗法逆元が存在しない場合の乗法逆元
		static constexpr complex<T> _multiplicative_inverse_impl_(const complex<T>& c, std::false_type) {
			T temp = multiplicative_inverse(c[0] * c[0] + c[1] * c[1]);
			return complex<T>(c[0] * temp, -c[1] * temp);
		}
		static constexpr complex<T> _multiplicative_inverse_(const complex<T>& c) {
			return _multiplicative_inverse_impl_(c, std::bool_constant<is_exist_multiplicative_inverse_v<T>>());
		}
	};


	// 誤差評価
	template <class T>
	struct Error_evaluation<complex<T>> {
		static constexpr complex<T> epsilon() { return complex<T>(Error_evaluation<T>::epsilon(), Error_evaluation<T>::epsilon()); }
		static constexpr bool _error_evaluation_(const complex<T>& x1, const complex<T>& x2) {
			return (error_evaluation(x1[0], x2[0])) && (error_evaluation(x1[1], x2[1]));
		}
	};
}

#endif