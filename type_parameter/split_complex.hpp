#ifndef IMATHLIB_H_MATH_HYPERCOMPLEX_SPLIT_COMPLEX_HPP
#define IMATHLIB_H_MATH_HYPERCOMPLEX_SPLIT_COMPLEX_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/type_parameter.hpp"


namespace iml {

	template <class>
	class split_complex;


	//分解型複素数型のパラメータ
	template <class T, class Re, class Im>
	using split_complex_parameter = type_parameter<split_complex<T>, type_tuple<Re, Im>>;
	template <class T, class ReP, class ImP>
	struct type_parameter<split_complex<T>, type_tuple<type_parameter<T, ReP>, type_parameter<T, ImP>>> {
		using type = split_complex<T>;
		static constexpr type value = type(type_parameter<T, ReP>::value, type_parameter<T, ImP>::value);

		template <class = std::enable_if_t<is_exist_additive_inverse_v<T>>>
		auto operator-() const { return split_complex_parameter<T, decltype(-type_parameter<T, ReP>()), decltype(-type_parameter<T, ImP>())>(); }
		auto operator+() const { return *this; }

		template <class T, T Val, class = std::enable_if_t<(Val >= 0) && (Val < 2)>>
			auto operator[](int_parameter<T, Val>) const { return at_type_tuple_t<Val, type_tuple<type_parameter<T, ReP>, type_parameter<T, ImP>>>(); }
	};


	// 二重数におけるスカラー演算と標準演算の十分条件の定義
	// 加算
	template <class T1, class T2>
	struct is_lscalar_operation_impl2<is_addable, T1, split_complex<T2>> : std::bool_constant<is_high_rank_math_type_v<T1, add_result_t<T1, T2>> || is_high_rank_math_type_v<add_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_addable, split_complex<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<T2, add_result_t<T1, T2>> || is_high_rank_math_type_v<add_result_t<T1, T2>, T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_addable, split_complex<T1>, split_complex<T2>> : std::bool_constant<is_addable_v<T1, T2>> {};
	// 減算
	template <class T1, class T2>
	struct is_lscalar_operation_impl2<is_subtractable, T1, split_complex<T2>> : std::bool_constant<is_high_rank_math_type_v<T1, sub_result_t<T1, T2>> || is_high_rank_math_type_v<sub_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_subtractable, split_complex<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<T2, sub_result_t<T1, T2>> || is_high_rank_math_type_v<sub_result_t<T1, T2>, T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_subtractable, split_complex<T1>, split_complex<T2>> : std::bool_constant<is_subtractable_v<T1, T2>> {};
	// 乗算
	template <class T1, class T2>
	struct is_lscalar_operation_impl2<is_multipliable, T1, split_complex<T2>> : std::bool_constant<is_multipliable_v<T1, T2>> {};
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_multipliable, split_complex<T1>, T2> : std::bool_constant<is_multipliable_v<T1, T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_multipliable, split_complex<T1>, split_complex<T2>> : std::bool_constant<is_addable_v<mul_result_t<T1, T2>, mul_result_t<T1, T2>>> {};
	// 除算
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_divisible, split_complex<T1>, T2> : std::bool_constant<is_divisible_v<T1, T2>> {};
	// 等価比較
	template <class T1, class T2>
	struct is_lscalar_operation_impl2<is_comparable, T1, split_complex<T2>> : std::bool_constant<is_comparable_v<T1, T2> && is_exist_additive_identity_v<T1>> {};
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_comparable, split_complex<T1>, T2> : std::bool_constant<is_comparable_v<T1, T2> && is_exist_additive_identity_v<T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_comparable, split_complex<T1>, split_complex<T2>> : std::bool_constant<is_comparable_v<T1, T2>> {};
	// 代入
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_assignable, split_complex<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<T2, T1> && is_exist_additive_identity_v<T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_assignable, split_complex<T1>, split_complex<T2>> : std::bool_constant<is_high_rank_math_type_v<T2, T1>> {};
	// 加算代入
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_add_assignable, split_complex<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<add_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_add_assignable, split_complex<T1>, split_complex<T2>> : std::bool_constant<is_high_rank_math_type_v<add_result_t<T1, T2>, T1>> {};
	// 減算代入
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_sub_assignable, split_complex<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<sub_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_sub_assignable, split_complex<T1>, split_complex<T2>> : std::bool_constant<is_high_rank_math_type_v<sub_result_t<T1, T2>, T1>> {};
	// 乗算代入
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_mul_assignable, split_complex<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<mul_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_mul_assignable, split_complex<T1>, split_complex<T2>> : std::bool_constant<is_high_rank_math_type_v<add_result_t<mul_result_t<T1, T2>, mul_result_t<T1, T2>>, T1> && is_addable_v<mul_result_t<T1, T2>, mul_result_t<T1, T2>>> {};
	// 除算代入
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_div_assignable, split_complex<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<div_result_t<T1, T2>, T1>> {};



	//分解型複素数型
	template <class T>
	class split_complex {
		template <class> friend class split_complex;
		T x_m[2];
	public:
		constexpr split_complex() : x_m{} {}
		template <class U, class = std::enable_if_t<is_rscalar_operation_v<is_assignable, split_complex, U>>>
		constexpr split_complex(const U& re) : x_m{ re } {}
		template <class U1, class U2, class = std::enable_if_t<is_standard_operation_v<is_assignable, split_complex, split_complex<common_math_type_t<U1, U2>>>>>
		constexpr split_complex(const U1& re, const U2& im) : x_m{ T(re),T(im) } {}
		constexpr split_complex(const split_complex& c) : x_m{ c.x_m[0],c.x_m[1] } {}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_assignable, split_complex, split_complex<U>>>>
		constexpr split_complex(const split_complex<U>& c) : x_m{ c.x_m[0],c.x_m[1] } {}

		using basis_type = T;
		using iterator = linear_iterator<T>;
		using const_iterator = linear_iterator<const T>;

		template <class Other>
		struct rebind {
			using other = split_complex<Other>;
		};

		constexpr iterator begin() noexcept { return iterator(x_m); }
		constexpr const_iterator begin() const noexcept { return const_iterator(x_m); }
		constexpr iterator end() noexcept { return iterator(x_m + 2); }
		constexpr const_iterator end() const noexcept { return const_iterator(x_m + 2); }

		//単項演算
		template <class = std::enable_if_t<is_exist_additive_inverse_v<T>>>
		constexpr split_complex operator-() const { return split_complex(-this->x_m[0], -this->x_m[1]); }
		constexpr split_complex operator+() const { return split_complex(*this); }
		//代入演算
		split_complex& operator=(const split_complex& c) {
			if (this != std::addressof(c)) { this->x_m[0] = c.x_m[0]; this->x_m[1] = c.x_m[1]; }
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_assignable, split_complex, split_complex<U>>>>
		split_complex& operator=(const split_complex<U>& c) {
			this->x_m[0] = c.x_m[0]; this->x_m[1] = c.x_m[1];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_rscalar_operation_v<is_assignable, split_complex, U>>>
		split_complex& operator=(const U& n) {
			this->x_m[0] = n; this->x_m[1] = addition_traits<T>::identity_element();
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_add_assignable, split_complex, split_complex<U>>>>
		split_complex& operator+=(const split_complex<U>& c) {
			this->x_m[0] += c.x_m[0]; this->x_m[1] += c.x_m[1];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_rscalar_operation_v<is_add_assignable, split_complex, U>>>
		split_complex& operator+=(const U& n) {
			this->x_m[0] += n;
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_sub_assignable, split_complex, split_complex<U>>>>
		split_complex& operator-=(const split_complex<U>& c) {
			this->x_m[0] -= c.x_m[0]; this->x_m[1] -= c.x_m[1];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_rscalar_operation_v<is_sub_assignable, split_complex, U>>>
		split_complex& operator-=(const U& n) {
			this->x_m[0] -= n;
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_mul_assignable, split_complex, split_complex<U>>>>
		split_complex& operator*=(const split_complex<U>& c) {
			T temp[2] = { this->x_m[0] * c.x_m[0] + this->x_m[1] * c.x_m[1], this->x_m[0] * c.x_m[1] + this->x_m[1] * c.x_m[0] };
			this->x_m[0] = temp[0]; this->x_m[1] = temp[1];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_rscalar_operation_v<is_mul_assignable, split_complex, U>>>
		split_complex& operator*=(const U& k) {
			this->x_m[0] *= k; this->x_m[1] *= k;
			return *this;
		}
		template <class U, class = std::enable_if_t<is_rscalar_operation_v<is_div_assignable, split_complex, U>>>
		split_complex& operator/=(const U& k) {
			this->x_m[0] /= k; this->x_m[1] /= k;
			return *this;
		}

		//添え字演算
		const constexpr T& operator[](size_t index) const { return this->x_m[index]; }
		constexpr T& operator[](size_t index) { return this->x_m[index]; }

		//ストリーム出力
		friend std::ostream& operator<<(std::ostream& os, const split_complex& n) {
			os << '(' << n.x_m[0] << ',' << n.x_m[1] << ')';
			return os;
		}
		friend std::wostream& operator<<(std::wostream& os, const split_complex& n) {
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
	inline constexpr split_complex<common_math_type_t<U1, U2>> make_split_complex(U1&& re, U2&& im) {
		return split_complex<common_math_type_t<U1, U2>>(std::forward<U1>(re), std::forward<U2>(im));
	}


	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_addable, split_complex<T1>, split_complex<T2>>>>
	inline constexpr auto operator+(const split_complex<T1>& lhs, const split_complex<T2>& rhs) {
		return split_complex<add_result_t<T1, T2>>(lhs[0] + rhs[0], lhs[1] + rhs[1]);
	}
	template <class T1, class T2, class Re1, class Im1, class Re2, class Im2, class = std::enable_if_t<is_standard_operation_v<is_addable, split_complex<T1>, split_complex<T2>>>>
	inline auto operator+(split_complex_parameter<T1, Re1, Im1>, split_complex_parameter<T2, Re2, Im2>) {
		return split_complex_parameter<add_result_t<T1, T2>, decltype(Re1() + Re2()), decltype(Im1() + Im2())>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_rscalar_operation_v<is_addable, split_complex<T1>, T2>>>
	inline constexpr auto operator+(const split_complex<T1>& lhs, const T2& rhs) {
		return split_complex<select_high_rank_math_type_t<add_result_t<T1, T2>, T1>>(lhs[0] + rhs, lhs[1]);
	}
	template <class T1, class T2, class Re, class Im, class Param, class = std::enable_if_t<is_rscalar_operation_v<is_addable, split_complex<T1>, T2>>>
	inline auto operator+(split_complex_parameter<T1, Re, Im>, type_parameter<T2, Param> rhs) {
		return split_complex_parameter<select_high_rank_math_type_t<add_result_t<T1, T2>, T1>, decltype(Re() + rhs), Im>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_lscalar_operation_v<is_addable, T1, split_complex<T2>>>>
	inline constexpr auto operator+(const T1& lhs, const split_complex<T2>& rhs) {
		return split_complex<select_high_rank_math_type_t<add_result_t<T1, T2>, T2>>(lhs + rhs[0], rhs[1]);
	}
	template <class T1, class T2, class Param, class Re, class Im, class = std::enable_if_t<is_lscalar_operation_v<is_addable, T1, split_complex<T2>>>>
	inline auto operator+(type_parameter<T1, Param> lhs, split_complex_parameter<T2, Re, Im>) {
		return split_complex_parameter<select_high_rank_math_type_t<add_result_t<T1, T2>, T2>, decltype(lhs + Re()), Im>();
	}

	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_subtractable, split_complex<T1>, split_complex<T2>>>>
	inline constexpr auto operator-(const split_complex<T1>& lhs, const split_complex<T2>& rhs) {
		return split_complex<sub_result_t<T1, T2>>(lhs[0] - rhs[0], lhs[1] - rhs[1]);
	}
	template <class T1, class T2, class Re1, class Im1, class Re2, class Im2, class = std::enable_if_t<is_standard_operation_v<is_subtractable, split_complex<T1>, split_complex<T2>>>>
	inline auto operator-(split_complex_parameter<T1, Re1, Im1>, split_complex_parameter<T2, Re2, Im2>) {
		return split_complex_parameter<sub_result_t<T1, T2>, decltype(Re1() - Re2()), decltype(Im1() - Im2())>();
	}
	template <class T1, class T2, std::enable_if_t<is_rscalar_operation_v<is_subtractable, split_complex<T1>, T2>>>
	inline constexpr auto operator-(const split_complex<T1>& lhs, const T2& rhs) {
		return split_complex<select_high_rank_math_type_t<sub_result_t<T1, T2>, T1>>(lhs[0] - rhs, lhs[1]);
	}
	template <class T1, class T2, class Re, class Im, class Param, class = std::enable_if_t<is_rscalar_operation_v<is_subtractable, split_complex<T1>, T2>>>
	inline auto operator-(split_complex_parameter<T1, Re, Im>, type_parameter<T2, Param> rhs) {
		return split_complex_parameter<select_high_rank_math_type_t<sub_result_t<T1, T2>, T1>, decltype(Re() - rhs), Im>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_lscalar_operation_v<is_subtractable, T1, split_complex<T2>>>>
	inline constexpr auto operator-(const T1& lhs, const split_complex<T2>& rhs) {
		return split_complex<select_high_rank_math_type_t<sub_result_t<T1, T2>, T2>>(lhs - rhs[0], -rhs[1]);
	}
	template <class T1, class T2, class Param, class Re, class Im, class = std::enable_if_t<is_lscalar_operation_v<is_subtractable, T1, split_complex<T2>>>>
	inline auto operator-(type_parameter<T1, Param> lhs, split_complex_parameter<T2, Re, Im>) {
		return split_complex_parameter<select_high_rank_math_type_t<sub_result_t<T1, T2>, T2>, decltype(lhs - Re()), decltype(-Im())>();
	}

	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_multipliable, split_complex<T1>, split_complex<T2>>>>
	inline constexpr auto operator*(const split_complex<T1>& lhs, const split_complex<T2>& rhs) {
		return split_complex<mul_result_t<T1, T2>>(lhs[0] * rhs[0] + lhs[1] * rhs[1], lhs[0] * rhs[1] + lhs[1] * rhs[0]);
	}
	template <class T1, class T2, class Re1, class Im1, class Re2, class Im2, class = std::enable_if_t<is_standard_operation_v<is_multipliable, split_complex<T1>, split_complex<T2>>>>
	inline auto operator*(split_complex_parameter<T1, Re1, Im1>, split_complex_parameter<T2, Re2, Im2>) {
		return split_complex_parameter<mul_result_t<T1, T2>, decltype(Re1() * Re2() + Im1() * Im2()), decltype(Re1() * Im2() + Im1() * Re2())>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_rscalar_operation_v<is_multipliable, split_complex<T1>, T2>>>
	inline constexpr auto operator*(const split_complex<T1>& lhs, const T2& rhs) {
		return split_complex<mul_result_t<T1, T2>>(lhs[0] * rhs, lhs[1] * rhs);
	}
	template <class T1, class T2, class Re, class Im, class Param, class = std::enable_if_t<is_rscalar_operation_v<is_multipliable, split_complex<T1>, T2>>>
	inline auto operator*(split_complex_parameter<T1, Re, Im>, type_parameter<T2, Param> rhs) {
		return split_complex_parameter<mul_result_t<T1, T2>, decltype(Re() * rhs), decltype(Im() * rhs)>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_lscalar_operation_v<is_multipliable, T1, split_complex<T2>>>>
	inline constexpr auto operator*(const T1& lhs, const split_complex<T2>& rhs) {
		return split_complex<mul_result_t<T1, T2>>(lhs * rhs[0], lhs * rhs[1]);
	}
	template <class T1, class T2, class Param, class Re, class Im, class = std::enable_if_t<is_lscalar_operation_v<is_multipliable, T1, split_complex<T2>>>>
	inline auto operator*(type_parameter<T1, Param> lhs, split_complex_parameter<T2, Re, Im>) {
		return split_complex_parameter<mul_result_t<T1, T2>, decltype(lhs * Re()), decltype(lhs * Im())>();
	}

	template <class T1, class T2, class = std::enable_if_t<is_rscalar_operation_v<is_divisible, split_complex<T1>, T2>>>
	inline constexpr auto operator/(const split_complex<T1>& lhs, const T2& rhs) {
		return split_complex<div_result_t<T1, T2>>(lhs[0] / rhs, lhs[1] / rhs);
	}
	template <class T1, class T2, class Re, class Im, class Param, class = std::enable_if_t<is_rscalar_operation_v<is_divisible, split_complex<T1>, T2>>>
	inline auto operator/(split_complex_parameter<T1, Re, Im>, type_parameter<T2, Param> rhs) {
		return split_complex_parameter<div_result_t<T1, T2>, decltype(Re() / rhs), decltype(Im() / rhs)>();
	}


	//比較演算
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_comparable, split_complex<T1>, split_complex<T2>>>>
	inline constexpr bool operator==(const split_complex<T1>& lhs, const split_complex<T2>& rhs) {
		return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1]);
	}
	template <class T1, class T2, class Re1, class Im1, class Re2, class Im2, class = std::enable_if_t<is_standard_operation_v<is_comparable, split_complex<T1>, split_complex<T2>>>>
	inline auto operator==(split_complex_parameter<T1, Re1, Im1>, split_complex_parameter<T2, Re2, Im2>) {
		return (Re1() == Re2()) && (Im1() == Im2());
	}
	template <class T1, class T2, class = std::enable_if_t<is_rscalar_operation_v<is_comparable, split_complex<T1>, T2>>>
	inline constexpr bool operator==(const split_complex<T1>& lhs, T2& rhs) {
		return (lhs[0] == rhs) && is_absorbing_element(lhs[1]);
	}
	template <class T1, class T2, class Re, class Im, class Param, class = std::enable_if_t<is_rscalar_operation_v<is_comparable, split_complex<T1>, T2>>>
	inline auto operator==(split_complex_parameter<T1, Re, Im>, type_parameter<T2, Param> rhs) {
		return (Re() == rhs) && int_parameter<bool, is_absorbing_element(Im::value)>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_lscalar_operation_v<is_comparable, T1, split_complex<T2>>>>
	inline constexpr bool operator==(const T1& lhs, const split_complex<T2>& rhs) {
		return (lhs == rhs[0]) && is_absorbing_element(rhs[1]);
	}
	template <class T1, class T2, class Param, class Re, class Im, class = std::enable_if_t<is_lscalar_operation_v<is_comparable, T1, split_complex<T2>>>>
	inline auto operator==(type_parameter<T1, Param> lhs, split_complex_parameter<T2, Re, Im>) {
		return (lhs == Re()) && int_parameter<bool, is_absorbing_element(Im::value)>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_comparable, split_complex<T1>, split_complex<T2>>>>
	inline constexpr bool operator!=(const split_complex<T1>& lhs, const split_complex<T2>& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class Re1, class Im1, class Re2, class Im2, class = std::enable_if_t<is_standard_operation_v<is_comparable, split_complex<T1>, split_complex<T2>>>>
	inline auto operator!=(split_complex_parameter<T1, Re1, Im1> lhs, split_complex_parameter<T2, Re2, Im2> rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class = std::enable_if_t<is_rscalar_operation_v<is_comparable, split_complex<T1>, T2>>>
	inline constexpr bool operator!=(const split_complex<T1>& lhs, T2& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class Re, class Im, class Param, class = std::enable_if_t<is_rscalar_operation_v<is_comparable, split_complex<T1>, T2>>>
	inline auto operator!=(split_complex_parameter<T1, Re, Im> lhs, type_parameter<T2, Param> rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class = std::enable_if_t<is_lscalar_operation_v<is_comparable, T1, split_complex<T2>>>>
	inline constexpr bool operator!=(const T1& lhs, const split_complex<T2>& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class Param, class Re, class Im, class = std::enable_if_t<is_lscalar_operation_v<is_comparable, T1, split_complex<T2>>>>
	inline auto operator!=(type_parameter<T1, Param> lhs, split_complex_parameter<T2, Re, Im> rhs) { return !(lhs == rhs); }


	//分解型複素数型の生成
	template <class T1, class T2>
	inline constexpr split_complex<unwrap_reference_t<T1>> make_split_complex(T1&& re, T2&& im) {
		return split_complex<unwrap_reference_t<T1>>(std::forward<T1>(re), std::forward<T2>(im));
	}


	//分解型複素数の判定
	template <class T>
	struct is_split_complex_impl : std::false_type {};
	template <class T>
	struct is_split_complex_impl<split_complex<T>> : std::true_type {};
	template <class T>
	struct is_split_complex : is_split_complex_impl<std::remove_cv_t<T>> {};
	template <class T>
	inline constexpr bool is_split_complex_v = is_split_complex<T>::value;


	//分解型複素数の除去
	template <class T>
	struct remove_split_complex {
		using type = T;
	};
	template <class T>
	struct remove_split_complex<split_complex<T>> {
		using type = T;
	};
	template <class T>
	using remove_split_complex_t = typename remove_split_complex<T>::type;


	template <class From, class To>
	struct is_high_rank_math_type_split_complex : is_high_rank_math_type<From, typename To::basis_type> {};
	template <class From, class To>
	struct is_high_rank_math_type_split_complex<split_complex<From>, To> : std::bool_constant<
		//split_complex<From>がToの基底となる場合も含めて判定
		is_high_rank_math_type_v<split_complex<From>, typename To::basis_type> || is_high_rank_math_type_v<From, typename To::basis_type>
	> {};
	//Fromの型によりis_high_rank_math_type_split_complexで分岐
	template <class From, class To>
	struct is_high_rank_math_type<From, split_complex<To>> : is_high_rank_math_type_split_complex<From, split_complex<To>> {};


	//加法パラメータ取得
	template <class T>
	struct addition_traits<split_complex<T>> {
		//単位元
		template <class = std::enable_if_t<is_exist_additive_identity_v<T>>>
		static constexpr T identity_element() { return T(); }
		//結合律
		static constexpr bool associative_value = addition_traits<T>::associative_value;
		//消約律
		static constexpr bool cancellative_value = addition_traits<T>::cancellative_value;
		//可換律
		static constexpr bool commutative_value = addition_traits<T>::commutative_value;
	};
	//乗法パラメータ取得
	template <class T>
	struct multiplication_traits<split_complex<T>> {
		//単位元
		template <class = std::enable_if_t<is_exist_multiplicative_identity_v<T>>>
		static constexpr auto identity_element() { return addition_traits<T>::identity_element(); }
		//吸収元
		template <class = std::enable_if_t<is_exist_absorbing_element_v<T>>>
		static constexpr T absorbing_element() { return T(); }
		//結合律
		static constexpr bool associative_value = multiplication_traits<T>::associative_value;
		//消約律
		static constexpr bool cancellative_value = multiplication_traits<T>::cancellative_value;
		//可換律
		static constexpr bool commutative_value = multiplication_traits<T>::commutative_value;
		//分配律
		static constexpr bool distributive_value = multiplication_traits<T>::distributive_value;
	};


	//逆元が存在するならば逆元の取得(存在しない場合は例外を出す)
	template <class T>
	struct Inverse_element<split_complex<T>> {
		static constexpr split_complex<T> _additive_inverse_(const split_complex<T>& c) {
			return split_complex<T>(additive_inverse(c[0]), additive_inverse(c[1]));
		}

		//Tに乗法逆元が存在する場合の乗法逆元
		static constexpr split_complex<T> _multiplicative_inverse_impl_(const split_complex<T>& c, std::true_type) {
			//共役を絶対値の2乗で割る
			T temp = c[0] * c[0] - c[1] * c[1];
			return split_complex<T>(c[0] / temp, -c[1] / temp);
		}
		//Tの乗法逆元が存在しない場合の乗法逆元
		static constexpr split_complex<T> _multiplicative_inverse_impl_(const split_complex<T>& c, std::false_type) {
			T temp = multiplicative_inverse(c[0] * c[0] - c[1] * c[1]);
			return split_complex<T>(c[0] * temp, -c[1] * temp);
		}
		static constexpr split_complex<T> _multiplicative_inverse_(const split_complex<T>& c) {
			return _multiplicative_inverse_impl_(c, std::bool_constant<is_exist_multiplicative_inverse_v<T>>());
		}
	};


	//誤差評価
	template <class T>
	struct Error_evaluation<split_complex<T>> {
		static constexpr split_complex<T> epsilon() { return split_complex<T>(Error_evaluation<T>::epsilon(), Error_evaluation<T>::epsilon()); }
		static constexpr bool _error_evaluation_(const split_complex<T>& x1, const split_complex<T>& x2) {
			return (error_evaluation(x1[0], x2[0])) && (error_evaluation(x1[1], x2[1]));
		}
	};
}


#endif