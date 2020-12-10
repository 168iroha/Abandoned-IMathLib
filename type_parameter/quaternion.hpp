#ifndef IMATHLIB_H_MATH_HYPERCOMPLEX_QUATERNION_HPP
#define IMATHLIB_H_MATH_HYPERCOMPLEX_QUATERNION_HPP

#include "IMathLib/math/liner_algebra/vector.hpp"
#include "IMathLib/math/hypercomplex/complex.hpp"


namespace iml {

	template <class>
	class quaternion;


	//四元数型のパラメータ
	template <class T, class Re, class Im1, class Im2, class Im3>
	using quaternion_parameter = type_parameter<quaternion<T>, type_tuple<Re, Im1, Im2, Im3>>;
	template <class T, class ReP, class ImP1, class ImP2, class ImP3>
	struct type_parameter<quaternion<T>, type_tuple<type_parameter<T, ReP>, type_parameter<T, ImP1>, type_parameter<T, ImP2>, type_parameter<T, ImP3>>> {
		using type = quaternion<T>;
		static constexpr type value = type(type_parameter<T, ReP>::value, type_parameter<T, ImP1>::value, type_parameter<T, ImP2>::value, type_parameter<T, ImP3>::value);

		template <class = std::enable_if_t<is_exist_additive_inverse_v<T>>>
		auto operator-() const { return quaternion_parameter<T, decltype(-type_parameter<T, ReP>()), decltype(-type_parameter<T, ImP1>()), decltype(-type_parameter<T, ImP2>()), decltype(-type_parameter<T, ImP3>())>(); }
		auto operator+() const { return *this; }

		template <class T, T Val, class = std::enable_if_t<(Val >= 0) && (Val < 4)>>
			auto operator[](int_parameter<T, Val>) const { return at_type_tuple_t<Val, type_tuple<type_parameter<T, ReP>, type_parameter<T, ImP1>, type_parameter<T, ImP2>, type_parameter<T, ImP3>>>(); }
	};


	// 四元数におけるスカラー演算と標準演算の十分条件の定義
	// 加算
	template <class T1, class T2>
	struct is_lscalar_operation_impl2<is_addable, T1, quaternion<T2>> : std::bool_constant<is_high_rank_math_type_v<T1, add_result_t<T1, T2>> || is_high_rank_math_type_v<add_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_addable, quaternion<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<T2, add_result_t<T1, T2>> || is_high_rank_math_type_v<add_result_t<T1, T2>, T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_addable, quaternion<T1>, quaternion<T2>> : std::bool_constant<is_addable_v<T1, T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_addable, quaternion<T1>, complex<T2>> : std::bool_constant<is_addable_v<T1, T2> && (is_high_rank_math_type_v<T1, add_result_t<T1, T2>> || is_high_rank_math_type_v<add_result_t<T1, T2>, T1>)> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_addable, complex<T1>, quaternion<T2>> : std::bool_constant<is_addable_v<T1, T2> && (is_high_rank_math_type_v<T1, add_result_t<T1, T2>> || is_high_rank_math_type_v<add_result_t<T1, T2>, T1>)> {};
	// 減算
	template <class T1, class T2>
	struct is_lscalar_operation_impl2<is_subtractable, T1, quaternion<T2>> : std::bool_constant<is_high_rank_math_type_v<T1, sub_result_t<T1, T2>> || is_high_rank_math_type_v<sub_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_subtractable, quaternion<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<T2, sub_result_t<T1, T2>> || is_high_rank_math_type_v<sub_result_t<T1, T2>, T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_subtractable, quaternion<T1>, quaternion<T2>> : std::bool_constant<is_subtractable_v<T1, T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_subtractable, quaternion<T1>, complex<T2>> : std::bool_constant<is_subtractable_v<T1, T2> && (is_high_rank_math_type_v<T1, sub_result_t<T1, T2>> || is_high_rank_math_type_v<sub_result_t<T1, T2>, T1>)> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_subtractable, complex<T1>, quaternion<T2>> : std::bool_constant<is_subtractable_v<T1, T2> && (is_high_rank_math_type_v<T1, sub_result_t<T1, T2>> || is_high_rank_math_type_v<sub_result_t<T1, T2>, T1>)> {};
	// 乗算
	template <class T1, class T2>
	struct is_lscalar_operation_impl2<is_multipliable, T1, quaternion<T2>> : std::bool_constant<is_multipliable_v<T1, T2>> {};
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_multipliable, quaternion<T1>, T2> : std::bool_constant<is_multipliable_v<T1, T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_multipliable, quaternion<T1>, quaternion<T2>> : std::bool_constant<is_exist_additive_inverse_v<mul_result_t<T1, T2>>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_multipliable, quaternion<T1>, complex<T2>> : std::bool_constant<is_exist_additive_inverse_v<mul_result_t<T1, T2>>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_multipliable, complex<T1>, quaternion<T2>> : std::bool_constant<is_exist_additive_inverse_v<mul_result_t<T1, T2>>> {};
	// 除算
	template <class T1, class T2>
	struct is_lscalar_operation_impl2<is_divisible, T1, quaternion<T2>> : std::bool_constant<is_multipliable_v<T1, T2> && is_skew_field_v<T2>> {};
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_divisible, quaternion<T1>, T2> : std::bool_constant<is_divisible_v<T1, T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_divisible, quaternion<T1>, quaternion<T2>> : std::bool_constant<is_exist_additive_inverse_v<mul_result_t<T1, T2>> && is_skew_field_v<T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_divisible, quaternion<T1>, complex<T2>> : std::bool_constant<is_exist_additive_inverse_v<mul_result_t<T1, T2>> && is_skew_field_v<T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_divisible, complex<T1>, quaternion<T2>> : std::bool_constant<is_exist_additive_inverse_v<mul_result_t<T1, T2>> && is_skew_field_v<T2>> {};
	// 等価比較
	template <class T1, class T2>
	struct is_lscalar_operation_impl2<is_comparable, T1, quaternion<T2>> : std::bool_constant<is_comparable_v<T1, T2> && is_exist_additive_identity_v<T1>> {};
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_comparable, quaternion<T1>, T2> : std::bool_constant<is_comparable_v<T1, T2> && is_exist_additive_identity_v<T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_comparable, quaternion<T1>, quaternion<T2>> : std::bool_constant<is_comparable_v<T1, T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_comparable, quaternion<T1>, complex<T2>> : std::bool_constant<is_comparable_v<T1, T2> && is_exist_additive_identity_v<T1>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_comparable, complex<T1>, quaternion<T2>> : std::bool_constant<is_comparable_v<T1, T2> && is_exist_additive_identity_v<T2>> {};
	// 代入
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_assignable, quaternion<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<T2, T1> && is_exist_additive_identity_v<T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_assignable, quaternion<T1>, quaternion<T2>> : std::bool_constant<is_high_rank_math_type_v<T2, T1> && is_exist_additive_identity_v<T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_assignable, quaternion<T1>, complex<T2>> : std::bool_constant<is_high_rank_math_type_v<T2, T1> && is_exist_additive_identity_v<T2>> {};
	// 加算代入
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_add_assignable, quaternion<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<add_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_add_assignable, quaternion<T1>, quaternion<T2>> : std::bool_constant<is_high_rank_math_type_v<add_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_add_assignable, quaternion<T1>, complex<T2>> : std::bool_constant<is_high_rank_math_type_v<add_result_t<T1, T2>, T1>> {};
	// 減算代入
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_sub_assignable, quaternion<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<sub_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_sub_assignable, quaternion<T1>, quaternion<T2>> : std::bool_constant<is_high_rank_math_type_v<sub_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_sub_assignable, quaternion<T1>, complex<T2>> : std::bool_constant<is_high_rank_math_type_v<sub_result_t<T1, T2>, T1>> {};
	// 乗算代入
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_mul_assignable, quaternion<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<mul_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_mul_assignable, quaternion<T1>, quaternion<T2>> : std::bool_constant<is_high_rank_math_type_v<mul_result_t<T1, T2>, T1> && is_exist_additive_inverse_v<mul_result_t<T1, T2>>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_mul_assignable, quaternion<T1>, complex<T2>> : std::bool_constant<is_high_rank_math_type_v<mul_result_t<T1, T2>, T1> && is_exist_additive_inverse_v<mul_result_t<T1, T2>>> {};
	// 除算代入
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_div_assignable, quaternion<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<div_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_div_assignable, quaternion<T1>, quaternion<T2>> : std::bool_constant<is_high_rank_math_type_v<mul_result_t<T1, T2>, T1> && is_exist_additive_inverse_v<mul_result_t<T1, T2>> && is_skew_field_v<T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_div_assignable, quaternion<T1>, complex<T2>> : std::bool_constant<is_high_rank_math_type_v<mul_result_t<T1, T2>, T1> && is_exist_additive_inverse_v<mul_result_t<T1, T2>> && is_skew_field_v<T2>> {};


	//四元数型
	template <class T>
	class quaternion {
		template <class> friend class quaternion;
		T x_m[4];
	public:
		constexpr quaternion() : x_m{} {}
		template <class U, class = std::enable_if_t<is_rscalar_operation_v<is_assignable, quaternion, U>>>
		constexpr quaternion(const U& re) : x_m{ re } {}
		template <class U1, class U2, class U3, class U4, class = std::enable_if_t<is_standard_operation_v<is_assignable, quaternion, quaternion<common_math_type_t<U1, U2, U3, U4>>>>>
		constexpr quaternion(const U1& re, const U2& im1, const U3& im2, const U4& im3) : x_m{ T(re),T(im1),T(im2),T(im3) } {}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_assignable, quaternion, complex<U>>>>
		constexpr quaternion(const complex<U>& c) : x_m{ c[0],c[1] } {}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_assignable, quaternion, quaternion<U>>>>
		constexpr quaternion(const U& re, const vector3<U>& v) : x_m{ re,v[0],v[1],v[2] } {}
		constexpr quaternion(const quaternion& q) : x_m{ q.x_m[0],q.x_m[1],q.x_m[2],q.x_m[3] } {}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_assignable, quaternion, quaternion<U>>>>
		constexpr quaternion(const quaternion<U>& q) : x_m{ q.x_m[0],q.x_m[1],q.x_m[2],q.x_m[3] } {}

		using basis_type = T;
		using iterator = linear_iterator<T>;
		using const_iterator = linear_iterator<const T>;

		template<class Other>
		struct rebind {
			using other = quaternion<Other>;
		};

		constexpr iterator begin() noexcept { return iterator(x_m); }
		constexpr const_iterator begin() const noexcept { return const_iterator(x_m); }
		constexpr iterator end() noexcept { return iterator(x_m + 4); }
		constexpr const_iterator end() const noexcept { return const_iterator(x_m + 4); }

		//単項演算
		template <class = std::enable_if_t<is_exist_additive_inverse_v<T>>>
		constexpr quaternion operator-() const { return quaternion(-this->x_m[0], -this->x_m[1], -this->x_m[2], -this->x_m[3]); }
		constexpr quaternion operator+() const { return quaternion(*this); }
		//代入演算
		quaternion& operator=(const quaternion& q) {
			if (this != std::addressof(q)) {
				this->x_m[0] = q.x_m[0]; this->x_m[1] = q.x_m[1];
				this->x_m[2] = q.x_m[2]; this->x_m[3] = q.x_m[3];
			}
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_assignable, quaternion, quaternion<U>>>>
		quaternion& operator=(const quaternion<U>& q) {
			this->x_m[0] = q.x_m[0]; this->x_m[1] = q.x_m[1];
			this->x_m[2] = q.x_m[2]; this->x_m[3] = q.x_m[3];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_rscalar_operation_v<is_assignable, quaternion, U>>>
		quaternion& operator=(const U& n) {
			this->x_m[0] = n; this->x_m[1] = addition_traits<T>::identity_element();
			this->x_m[2] = addition_traits<T>::identity_element(); this->x_m[3] = addition_traits<T>::identity_element();
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_assignable, quaternion, complex<U>>>>
		quaternion& operator=(const complex<U>& c) {
			this->x_m[0] = c[0]; this->x_m[1] = c[1];
			this->x_m[2] = addition_traits<T>::identity_element(); this->x_m[3] = addition_traits<T>::identity_element();
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_add_assignable, quaternion, quaternion<U>>>>
		quaternion& operator+=(const quaternion<U>& q) {
			this->x_m[0] += q.x_m[0]; this->x_m[1] += q.x_m[1];
			this->x_m[2] += q.x_m[2]; this->x_m[3] += q.x_m[3];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_rscalar_operation_v<is_add_assignable, quaternion, U>>>
		quaternion& operator+=(const U& n) {
			this->x_m[0] += n;
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_add_assignable, quaternion, complex<U>>>>
		quaternion& operator+=(const complex<U>& c) {
			this->x_m[0] += c[0]; this->x_m[1] += c[1];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_sub_assignable, quaternion, quaternion<U>>>>
		quaternion& operator-=(const quaternion<U>& q) {
			this->x_m[0] -= q.x_m[0]; this->x_m[1] -= q.x_m[1];
			this->x_m[2] -= q.x_m[2]; this->x_m[3] -= q.x_m[3];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_rscalar_operation_v<is_sub_assignable, quaternion, U>>>
		quaternion& operator-=(const U& n) {
			this->x_m[0] -= n;
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_sub_assignable, quaternion, complex<U>>>>
		quaternion& operator-=(const complex<U>& c) {
			this->x_m[0] -= c[0]; this->x_m[1] -= c[1];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_mul_assignable, quaternion, quaternion<U>>>>
		quaternion& operator*=(const quaternion<U>& q) {
			//()は3次元の内積と外積
			T temp[4] = {
				this->x_m[0] * q.x_m[0] - (this->x_m[1] * q.x_m[1] + this->x_m[2] * q.x_m[2] + this->x_m[3] * q.x_m[3])
				,this->x_m[0] * q.x_m[1] + this->x_m[1] * q.x_m[0] + (this->x_m[2] * q.x_m[3] - this->x_m[3] * q.x_m[2])
				,this->x_m[0] * q.x_m[2] + this->x_m[2] * q.x_m[0] + (this->x_m[3] * q.x_m[1] - this->x_m[1] * q.x_m[3])
				,this->x_m[0] * q.x_m[3] + this->x_m[3] * q.x_m[0] + (this->x_m[1] * q.x_m[2] - this->x_m[2] * q.x_m[1])
			};
			this->x_m[0] = temp[0]; this->x_m[1] = temp[1];
			this->x_m[2] = temp[2]; this->x_m[3] = temp[3];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_rscalar_operation_v<is_mul_assignable, quaternion, U>>>
		quaternion& operator*=(const U& k) {
			this->x_m[0] *= k; this->x_m[1] *= k;
			this->x_m[2] *= k; this->x_m[3] *= k;
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_mul_assignable, quaternion, complex<U>>>>
		quaternion& operator*=(const complex<U>& c) {
			T temp[4] = {
				this->x_m[0] * c[0] - (this->x_m[1] * c[1])
				,this->x_m[0] * c[1] + this->x_m[1] * c[0]
				,this->x_m[2] * c[0] + (this->x_m[3] * c[1])
				,this->x_m[3] * c[0] + (-this->x_m[2] * c[1])
			};
			this->x_m[0] = temp[0]; this->x_m[1] = temp[1];
			this->x_m[2] = temp[2]; this->x_m[3] = temp[3];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_div_assignable, quaternion, quaternion<U>>>>
		quaternion& operator/=(const quaternion<U>& q) {
			//逆元は共役を絶対値の二乗で割る
			T temp = q.x_m[0] * q.x_m[0] + q.x_m[1] * q.x_m[1] + q.x_m[2] * q.x_m[2] + q.x_m[3] * q.x_m[3];
			return *this *= quaternion<U>(q.x_m[0] / temp, -q.x_m[1] / temp, -q.x_m[2] / temp, -q.x_m[3] / temp);
		}
		template <class U, class = std::enable_if_t<is_rscalar_operation_v<is_div_assignable, quaternion, U>>>
		quaternion& operator/=(const U& k) {
			this->x_m[0] /= k; this->x_m[1] /= k;
			this->x_m[2] /= k; this->x_m[3] /= k;
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_div_assignable, quaternion, complex<U>>>>
		quaternion& operator/=(const complex<U>& c) {
			return *this *= multiplicative_inverse(c);
		}

		//添え字演算
		const constexpr T& operator[](size_t index) const { return this->x_m[index]; }
		constexpr T& operator[](size_t index) { return this->x_m[index]; }

		//ストリーム出力
		friend std::ostream& operator<<(std::ostream& os, const quaternion& n) {
			os << '(' << n.x_m[0] << ',' << n.x_m[1] << ',' << n.x_m[2] << ',' << n.x_m[3] << ')';
			return os;
		}
		friend std::wostream& operator<<(std::wostream& os, const quaternion& n) {
			os << '(' << n.x_m[0] << ',' << n.x_m[1] << ',' << n.x_m[2] << ',' << n.x_m[3] << ')';
			return os;
		}

		template <class T>
		constexpr linear_input<iterator> operator<<(const T& value) {
			iterator itr = this->begin();
			*itr = value; ++itr;
			return linear_input<iterator>(itr, this->end());
		}
	};


	template <class U1, class U2, class U3, class U4>
	inline constexpr quaternion<common_math_type_t<U1, U2, U3, U4>> make_quaternion(U1&& re, U2&& im1, U3&& im2, U4&& im3) {
		return quaternion<common_math_type_t<U1, U2, U3, U4>>(std::forward<U1>(re), std::forward<U2>(im1), std::forward<U3>(im2), std::forward<U4>(im3));
	}


	//四元数の2項演算
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_addable, quaternion<T1>, quaternion<T2>>>>
	inline constexpr auto operator+(const quaternion<T1>& lhs, const quaternion<T2>& rhs) {
		return quaternion<add_result_t<T1, T2>>(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2], lhs[3] + rhs[3]);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = std::enable_if_t<is_standard_operation_v<is_addable, quaternion<T1>, quaternion<T2>>>>
	inline auto operator+(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1>, quaternion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2>) {
		return quaternion_parameter<add_result_t<T1, T2>, decltype(Re1() + Re2())
			, decltype(Im1_1() + Im1_2()), decltype(Im2_1() + Im2_2()), decltype(Im3_1() + Im3_2())>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_rscalar_operation_v<is_addable, quaternion<T1>, T2>>>
	inline constexpr auto operator+(const quaternion<T1>& lhs, const T2& rhs) {
		return quaternion<add_result_t<T1, T2>>(lhs[0] + rhs, lhs[1], lhs[2], lhs[3]);
	}
	template <class T1, class T2, class Re, class Im1, class Im2, class Im3, class Param, class = std::enable_if_t<is_rscalar_operation_v<is_addable, quaternion<T1>, T2>>>
	inline auto operator+(quaternion_parameter<T1, Re, Im1, Im2, Im3>, type_parameter<T2, Param> rhs) {
		return quaternion_parameter<add_result_t<T1, T2>, decltype(Re() + rhs), Im1, Im2, Im3>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_lscalar_operation_v<is_addable, T1, quaternion<T2>>>>
	inline constexpr auto operator+(const T1& lhs, const quaternion<T2>& rhs) {
		return quaternion<add_result_t<T1, T2>>(lhs + rhs[0], rhs[1], rhs[2], rhs[3]);
	}
	template <class T1, class T2, class Param, class Re, class Im1, class Im2, class Im3, class = std::enable_if_t<is_lscalar_operation_v<is_addable, T1, quaternion<T2>>>>
	inline auto operator+(type_parameter<T1, Param> lhs, quaternion_parameter<T2, Re, Im1, Im2, Im3>) {
		return quaternion_parameter<add_result_t<T1, T2>, decltype(lhs + Re()), Im1, Im2, Im3>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_addable, quaternion<T1>, complex<T2>>>>
	inline constexpr auto operator+(const quaternion<T1>& lhs, const complex<T2>& rhs) {
		return quaternion<add_result_t<T1, T2>>(lhs[0] + rhs[0], lhs[1]+ rhs[1], lhs[2], lhs[3]);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im_2, class = std::enable_if_t<is_standard_operation_v<is_addable, quaternion<T1>, complex<T2>>>>
	inline auto operator+(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1>, complex_parameter<T2, Re2, Im_2>) {
		return quaternion_parameter<add_result_t<T1, T2>, decltype(Re1() + Re2()), decltype(Im1_1() + Im_2()), Im2_1, Im3_1>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_addable, complex<T1>, quaternion<T2>>>>
	inline constexpr auto operator+(const complex<T1>& lhs, const quaternion<T2>& rhs) {
		return quaternion<add_result_t<T1, T2>>(lhs[0] + rhs[0], lhs[1] + rhs[1], rhs[2], rhs[3]);
	}
	template <class T1, class T2, class Re1, class Im_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = std::enable_if_t<is_standard_operation_v<is_addable, complex<T1>, quaternion<T2>>>>
	inline auto operator+(complex_parameter<T1, Re1, Im_1>, quaternion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2>) {
		return quaternion_parameter<add_result_t<T1, T2>, decltype(Re1() + Re2()), decltype(Im_1() + Im1_2()), Im2_2, Im3_2>();
	}

	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_subtractable, quaternion<T1>, quaternion<T2>>>>
	inline constexpr auto operator-(const quaternion<T1>& lhs, const quaternion<T2>& rhs) {
		return quaternion<sub_result_t<T1, T2>>(lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2], lhs[3] - rhs[3]);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = std::enable_if_t<is_standard_operation_v<is_subtractable, quaternion<T1>, quaternion<T2>>>>
	inline auto operator-(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1>, quaternion_parameter<T1, Re2, Im1_2, Im2_2, Im3_2>) {
		return quaternion_parameter<add_result_t<T1, T2>, decltype(Re1() - Re2())
			, decltype(Im1_1() - Im1_2()), decltype(Im2_1() - Im2_2()), decltype(Im3_1() - Im3_2())>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_rscalar_operation_v<is_subtractable, quaternion<T1>, T2>>>
	inline constexpr auto operator-(const quaternion<T1>& lhs, const T2& rhs) {
		return quaternion<sub_result_t<T1, T2>>(lhs[0] - rhs, lhs[1], lhs[2], lhs[3]);
	}
	template <class T1, class T2, class Re, class Im1, class Im2, class Im3, class Param, class = std::enable_if_t<is_rscalar_operation_v<is_subtractable, quaternion<T1>, T2>>>
	inline auto operator-(quaternion_parameter<T1, Re, Im1, Im2, Im3>, type_parameter<T2, Param> rhs) {
		return quaternion_parameter<sub_result_t<T1, T2>, decltype(Re() - rhs), Im1, Im2, Im3>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_lscalar_operation_v<is_subtractable, T1, quaternion<T2>>>>
	inline constexpr auto operator-(const T1& lhs, const quaternion<T2>& rhs) {
		return quaternion<sub_result_t<T1, T2>>(lhs - rhs[0], -rhs[1], -rhs[2], -rhs[3]);
	}
	template <class T1, class T2, class Param, class Re, class Im1, class Im2, class Im3, class = std::enable_if_t<is_lscalar_operation_v<is_subtractable, T1, quaternion<T2>>>>
	inline auto operator-(type_parameter<T1, Param> lhs, quaternion_parameter<T2, Re, Im1, Im2, Im3>) {
		return quaternion_parameter<sub_result_t<T1, T2>, decltype(lhs - Re()), decltype(-Im1()), decltype(-Im2()), decltype(-Im3())>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_subtractable, quaternion<T1>, complex<T2>>>>
	inline constexpr auto operator-(const quaternion<T1>& lhs, const complex<T2>& rhs) {
		return quaternion<sub_result_t<T1, T2>>(lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2], lhs[3]);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im_2, class = std::enable_if_t<is_standard_operation_v<is_subtractable, quaternion<T1>, complex<T2>>>>
	inline auto operator-(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1>, complex_parameter<T2, Re2, Im_2>) {
		return quaternion_parameter<sub_result_t<T1, T2>, decltype(Re1() - Re2()), decltype(Im1_1() - Im_2()), Im2_1, Im3_1>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_subtractable, complex<T1>, quaternion<T2>>>>
	inline constexpr auto operator-(const complex<T1>& lhs, const quaternion<T2>& rhs) {
		return quaternion<sub_result_t<T1, T2>>(lhs[0] - rhs[0], lhs[1] - rhs[1], -rhs[2], -rhs[3]);
	}
	template <class T1, class T2, class Re1, class Im_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = std::enable_if_t<is_standard_operation_v<is_subtractable, complex<T1>, quaternion<T2>>>>
	inline auto operator-(complex_parameter<T1, Re1, Im_1>, quaternion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2>) {
		return quaternion_parameter<sub_result_t<T1, T2>, decltype(Re1() - Re2()), decltype(Im_1() - Im1_2()), decltype(-Im2_2()), decltype(-Im3_2())>();
	}

	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_multipliable, quaternion<T1>, quaternion<T2>>>>
	inline constexpr auto operator*(const quaternion<T1>& lhs, const quaternion<T2>& rhs) {
		return quaternion<mul_result_t<T1, T2>>(
			lhs[0] * rhs[0] - (lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3] * rhs[3])
			, lhs[0] * rhs[1] + lhs[1] * rhs[0] + (lhs[2] * rhs[3] - lhs[3] * rhs[2])
			, lhs[0] * rhs[2] + lhs[2] * rhs[0] + (lhs[3] * rhs[1] - lhs[1] * rhs[3])
			, lhs[0] * rhs[3] + lhs[3] * rhs[0] + (lhs[1] * rhs[2] - lhs[2] * rhs[1]));
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = std::enable_if_t<is_standard_operation_v<is_multipliable, quaternion<T1>, quaternion<T2>>>>
	inline auto operator*(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1>, quaternion_parameter<T1, Re2, Im1_2, Im2_2, Im3_2>) {
		return quaternion_parameter<mul_result_t<T1, T2>
			, decltype(Re1() * Re2() - (Im1_1() * Im1_2()+ Im2_1() * Im2_2()+ Im3_1() * Im3_2()))
			, decltype(Re1() * Im1_2() + Im1_1() * Re2() + (Im2_1() * Im3_2() - Im3_1() * Im2_2()))
			, decltype(Re1() * Im2_2() + Im2_1() * Re2() + (Im3_1() * Im1_2() - Im1_1() * Im3_2()))
			, decltype(Re1() * Im3_2() + Im3_1() * Re2() + (Im1_1() * Im2_2() - Im2_1() * Im1_2()))>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_rscalar_operation_v<is_multipliable, quaternion<T1>, T2>>>
	inline constexpr auto operator*(const quaternion<T1>& lhs, const T2& rhs) {
		return quaternion<mul_result_t<T1, T2>>(lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs, lhs[3] * rhs);
	}
	template <class T1, class T2, class Re, class Im1, class Im2, class Im3, class Param, class = std::enable_if_t<is_rscalar_operation_v<is_multipliable, quaternion<T1>, T2>>>
	inline auto operator*(quaternion_parameter<T1, Re, Im1, Im2, Im3>, type_parameter<T2, Param> rhs) {
		return quaternion_parameter<mul_result_t<T1, T2>, decltype(Re() * rhs), decltype(Im1() * rhs), decltype(Im2() * rhs), decltype(Im3() * rhs)>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_lscalar_operation_v<is_multipliable, T1, quaternion<T2>>>>
	inline constexpr auto operator*(const T1& lhs, const quaternion<T2>& rhs) {
		return quaternion<mul_result_t<T1, T2>>(lhs * rhs[0], lhs * rhs[1], lhs * rhs[2], lhs * rhs[3]);
	}
	template <class T1, class T2, class Param, class Re, class Im1, class Im2, class Im3, class = std::enable_if_t<is_lscalar_operation_v<is_multipliable, T1, quaternion<T2>>>>
	inline auto operator*(type_parameter<T1, Param> lhs, quaternion_parameter<T2, Re, Im1, Im2, Im3>) {
		return quaternion_parameter<mul_result_t<T1, T2>, decltype(lhs * Re()), decltype(lhs * Im1()), decltype(lhs * Im2()), decltype(lhs * Im3())>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_multipliable, quaternion<T1>, complex<T2>>>>
	inline constexpr auto operator*(const quaternion<T1>& lhs, const complex<T2>& rhs) {
		return quaternion<mul_result_t<T1, T2>>(
			lhs[0] * rhs[0] - (lhs[1] * rhs[1])
			, lhs[0] * rhs[1] + lhs[1] * rhs[0]
			, lhs[2] * rhs[0] + (lhs[3] * rhs[1])
			, lhs[3] * rhs[0] + (-lhs[2] * rhs[1]));
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im_2, class = std::enable_if_t<is_standard_operation_v<is_multipliable, quaternion<T1>, complex<T2>>>>
	inline auto operator*(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1>, complex_parameter<T2, Re2, Im_2>) {
		return quaternion_parameter<mul_result_t<T1, T2>
			, decltype(Re1() * Re2() - (Im1_1() * Im_2()))
			, decltype(Re1() * Im_2() + Im1_1() * Re2())
			, decltype(Im2_1() * Re2() + (Im3_1() * Im_2()))
			, decltype(Im3_1() * Re2() + (-Im2_1() * Im_2()))>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_multipliable, complex<T1>, quaternion<T2>>>>
	inline constexpr auto operator*(const complex<T1>& lhs, const quaternion<T2>& rhs) {
		return quaternion<mul_result_t<T1, T2>>(
			lhs[0] * rhs[0] - (lhs[1] * rhs[1])
			, lhs[0] * rhs[1] + lhs[1] * rhs[0]
			, lhs[0] * rhs[2] + (-lhs[1] * rhs[3])
			, lhs[0] * rhs[3] + (lhs[1] * rhs[2]));
	}
	template <class T1, class T2, class Re1, class Im_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = std::enable_if_t<is_standard_operation_v<is_multipliable, complex<T1>, quaternion<T2>>>>
	inline auto operator*(complex_parameter<T1, Re1, Im_1>, quaternion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2>) {
		return quaternion_parameter<mul_result_t<T1, T2>
			, decltype(Re1() * Re2() - (Im_1() * Im1_2()))
			, decltype(Re1() * Im1_2() + Im_1() * Re2())
			, decltype(Re1() * Im2_2() + (-Im_1() * Im3_2()))
			, decltype(Re1() * Im3_2() + (Im_1() * Im2_2()))>();
	}

	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_divisible, quaternion<T1>, quaternion<T2>>>>
	inline constexpr auto operator/(const quaternion<T1>& lhs, const quaternion<T2>& rhs) {
		T2 temp = rhs[0] * rhs[0] + rhs[1] * rhs[1] + rhs[2] * rhs[2] + rhs[3] * rhs[3];
		return lhs * quaternion<T2>(rhs[0] / temp, -rhs[1] / temp, -rhs[2] / temp, -rhs[3] / temp);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = std::enable_if_t<is_standard_operation_v<is_divisible, quaternion<T1>, quaternion<T2>>>>
	inline auto operator/(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1> lhs, quaternion_parameter<T1, Re2, Im1_2, Im2_2, Im3_2>) {
		using type = decltype(Re2() * Re2() + Im1_2() * Im1_2() + Im2_2() * Im2_2() + Im3_2() * Im3_2());
		return lhs * quaternion_parameter<T2, decltype(Re2() / type())
			, decltype(Im1_2() / type()), decltype(Im2_2() / type()), decltype(Im3_2() / type())>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_rscalar_operation_v<is_divisible, quaternion<T1>, T2>>>
	inline constexpr auto operator/(const quaternion<T1>& lhs, const T2& rhs) {
		return quaternion<div_result_t<T1, T2>>(lhs[0] / rhs, lhs[1] / rhs, lhs[2] / rhs, lhs[3] / rhs);
	}
	template <class T1, class T2, class Re, class Im1, class Im2, class Im3, class Param, class = std::enable_if_t<is_rscalar_operation_v<is_divisible, quaternion<T1>, T2>>>
	inline auto operator/(quaternion_parameter<T1, Re, Im1, Im2, Im3>, type_parameter<T2, Param> rhs) {
		return quaternion_parameter<div_result_t<T1, T2>, decltype(Re() / rhs), decltype(Im1() / rhs), decltype(Im2() / rhs), decltype(Im3() / rhs)>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_lscalar_operation_v<is_divisible, T1, quaternion<T2>>>>
	inline constexpr auto operator/(const T1& lhs, const quaternion<T2>& rhs) {
		T2 temp = rhs[0] * rhs[0] + rhs[1] * rhs[1] + rhs[2] * rhs[2] + rhs[3] * rhs[3];
		return lhs * quaternion<T2>(rhs[0] / temp, -rhs[1] / temp, -rhs[2] / temp, -rhs[3] / temp);
	}
	template <class T1, class T2, class Param, class Re, class Im1, class Im2, class Im3, class = std::enable_if_t<is_lscalar_operation_v<is_divisible, T1, quaternion<T2>>>>
	inline auto operator/(type_parameter<T1, Param> lhs, quaternion_parameter<T2, Re, Im1, Im2, Im3>) {
		using type = decltype(Re() * Re() + Im1() * Im1() + Im2() * Im2() + Im3() * Im3());
		return lhs * quaternion_parameter<T2, decltype(Re() / type())
			, decltype(Im1() / type()), decltype(Im2() / type()), decltype(Im3() / type())>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_divisible, quaternion<T1>, complex<T2>>>>
	inline constexpr auto operator/(const quaternion<T1>& lhs, const complex<T2>& rhs) {
		return lhs * multiplicative_inverse(rhs);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im_2, class = std::enable_if_t<is_standard_operation_v<is_divisible, quaternion<T1>, complex<T2>>>>
	inline auto operator/(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1> lhs, complex_parameter<T2, Re2, Im_2>) {
		using type = decltype(Re2() * Re2() + Im_2() * Im_2());
		return lhs * complex_parameter<T2, decltype(Re2() / type()), decltype(Im_2() / type())>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_divisible, complex<T1>, quaternion<T2>>>>
	inline constexpr auto operator/(const complex<T1>& lhs, const quaternion<T2>& rhs) {
		T2 temp = rhs[0] * rhs[0] + rhs[1] * rhs[1] + rhs[2] * rhs[2] + rhs[3] * rhs[3];
		return lhs * quaternion<T2>(rhs[0] / temp, -rhs[1] / temp, -rhs[2] / temp, -rhs[3] / temp);
	}
	template <class T1, class T2, class Re1, class Im_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = std::enable_if_t<is_standard_operation_v<is_divisible, complex<T1>, quaternion<T2>>>>
	inline auto operator/(complex_parameter<T1, Re1, Im_1> lhs, quaternion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2>) {
		using type = decltype(Re2() * Re2() + Im1_2() * Im1_2() + Im2_2() * Im2_2() + Im3_2() * Im3_2());
		return lhs * quaternion_parameter<T2, decltype(Re2() / type())
			, decltype(Im1_2() / type()), decltype(Im2_2() / type()), decltype(Im3_2() / type())>();
	}


	//比較演算
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_comparable, quaternion<T1>, quaternion<T2>>>>
	inline constexpr bool operator==(const quaternion<T1>& lhs, const quaternion<T2>& rhs) {
		return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1]) && (lhs[2] == rhs[2]) && (lhs[3] == rhs[3]);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im1_2, class Im2_2, class Im3_2, std::enable_if_t<is_standard_operation_v<is_comparable, quaternion<T1>, quaternion<T2>>>>
	inline auto operator==(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1>, quaternion_parameter<T1, Re2, Im1_2, Im2_2, Im3_2>) {
		return (Re1() == Re2()) && (Im1_1() == Im1_2()) && (Im2_1() == Im2_2()) && (Im3_1() == Im3_2());
	}
	template <class T1, class T2, class = std::enable_if_t<is_rscalar_operation_v<is_comparable, quaternion<T1>, T2>>>
	inline constexpr bool operator==(const quaternion<T1>& lhs, T2& rhs) {
		return (lhs[0] == rhs) && is_absorbing_element(lhs[1]) && is_absorbing_element(lhs[2]) && is_absorbing_element(lhs[3]);
	}
	template <class T1, class T2, class Re, class Im1, class Im2, class Im3, class Param, class = std::enable_if_t<is_rscalar_operation_v<is_comparable, quaternion<T1>, T2>>>
	inline auto operator==(quaternion_parameter<T1, Re, Im1, Im2, Im3>, type_parameter<T2, Param> rhs) {
		return (Re() == rhs) && int_parameter<bool, is_absorbing_element(Im1::value)>()
			&& int_parameter<bool, is_absorbing_element(Im2::value)>() && int_parameter<bool, is_absorbing_element(Im3::value)>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_lscalar_operation_v<is_comparable, T1, quaternion<T2>>>>
	inline constexpr bool operator==(const T1& lhs, const quaternion<T2>& rhs) {
		return (lhs == rhs[0]) && is_absorbing_element(rhs[1]) && is_absorbing_element(rhs[2]) && is_absorbing_element(rhs[3]);
	}
	template <class T1, class T2, class Param, class Re, class Im1, class Im2, class Im3, class = std::enable_if_t<is_lscalar_operation_v<is_comparable, T1, quaternion<T2>>>>
	inline auto operator==(type_parameter<T1, Param> lhs, quaternion_parameter<T2, Re, Im1, Im2, Im3>) {
		return (lhs == Re()) && int_parameter<bool, is_absorbing_element(Im1::value)>()
			&& int_parameter<bool, is_absorbing_element(Im2::value)>() && int_parameter<bool, is_absorbing_element(Im3::value)>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_comparable, quaternion<T1>, complex<T2>>>>
	inline constexpr bool operator==(const quaternion<T1>& lhs, complex<T2>& rhs) {
		return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1]) && is_absorbing_element(lhs[2]) && is_absorbing_element(lhs[3]);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im_2, class = std::enable_if_t<is_standard_operation_v<is_comparable, quaternion<T1>, complex<T2>>>>
	inline auto operator==(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1>, complex_parameter<T2, Re2, Im_2>) {
		return (Re1() == Re2()) && (Im1_1() == Im_2())
			&& int_parameter<bool, is_absorbing_element(Im2_1::value)>() && int_parameter<bool, is_absorbing_element(Im3_1::value)>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_comparable, complex<T1>, quaternion<T2>>>>
	inline constexpr bool operator==(const complex<T1>& lhs, const quaternion<T2>& rhs) {
		return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1]) && is_absorbing_element(rhs[2]) && is_absorbing_element(rhs[3]);
	}
	template <class T1, class T2, class Re1, class Im_1, class Re2, class Im1_2, class Im2_2, class Im3_2, std::enable_if_t<is_standard_operation_v<is_comparable, complex<T1>, quaternion<T2>>>>
	inline auto operator==(complex_parameter<T1, Re1, Im_1>, quaternion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2>) {
		return (Re1() == Re2()) && (Im_1() == Im1_2())
			&& int_parameter<bool, is_absorbing_element(Im2_2::value)>() && int_parameter<bool, is_absorbing_element(Im3_2::value)>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_comparable, quaternion<T1>, quaternion<T2>>>>
	inline constexpr bool operator!=(const quaternion<T1>& lhs, const quaternion<T2>& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = std::enable_if_t<is_standard_operation_v<is_comparable, quaternion<T1>, quaternion<T2>>>>
	inline auto operator!=(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1> lhs, quaternion_parameter<T1, Re2, Im1_2, Im2_2, Im3_2> rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class = std::enable_if_t<is_rscalar_operation_v<is_comparable, quaternion<T1>, T2>>>
	inline constexpr bool operator!=(const quaternion<T1>& lhs, T2& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class Re, class Im1, class Im2, class Im3, class Param, class = std::enable_if_t<is_rscalar_operation_v<is_comparable, quaternion<T1>, T2>>>
	inline auto operator!=(quaternion_parameter<T1, Re, Im1, Im2, Im3> lhs, type_parameter<T2, Param> rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class = std::enable_if_t<is_lscalar_operation_v<is_comparable, T1, quaternion<T2>>>>
	inline constexpr bool operator!=(const T1& lhs, const quaternion<T2>& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class Param, class Re, class Im1, class Im2, class Im3, class = std::enable_if_t<is_lscalar_operation_v<is_comparable, T1, quaternion<T2>>>>
	inline auto operator!=(type_parameter<T1, Param> lhs, quaternion_parameter<T2, Re, Im1, Im2, Im3> rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_comparable, quaternion<T1>, complex<T2>>>>
	inline constexpr bool operator!=(const quaternion<T1>& lhs, complex<T2>& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im_2, class = std::enable_if_t<is_standard_operation_v<is_comparable, quaternion<T1>, complex<T2>>>>
	inline auto operator!=(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1> lhs, complex_parameter<T2, Re2, Im_2> rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_comparable, complex<T1>, quaternion<T2>>>>
	inline constexpr bool operator!=(const complex<T1>& lhs, const quaternion<T2>& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class Re1, class Im_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = std::enable_if_t<is_standard_operation_v<is_comparable, complex<T1>, quaternion<T2>>>>
	inline auto operator!=(complex_parameter<T1, Re1, Im_1> lhs, quaternion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2> rhs) { return !(lhs == rhs); }

	//四元数の判定
	template <class T>
	struct is_quaternion_impl : std::false_type {};
	template <class T>
	struct is_quaternion_impl<quaternion<T>> : std::true_type {};
	template <class T>
	struct is_quaternion : is_quaternion_impl<std::remove_cv_t<T>> {};
	template <class T>
	inline constexpr bool is_quaternion_v = is_quaternion<T>::value;

	//四元数の除去
	template <class T>
	struct remove_quaternion {
		using type = T;
	};
	template <class T>
	struct remove_quaternion<quaternion<T>> {
		using type = T;
	};
	template <class T>
	using remove_quaternion_t = typename remove_quaternion<T>::type;


	template <class From, class To>
	struct is_high_rank_math_type_quaternion : is_high_rank_math_type<From, typename To::basis_type> {};
	template <class From, class To>
	struct is_high_rank_math_type_quaternion<complex<From>, To> : std::bool_constant<
		//complex<From>がToの基底となる場合も含めて判定
		is_high_rank_math_type_v<complex<From>, typename To::basis_type> || is_high_rank_math_type_v<From, typename To::basis_type>
	> {};
	template <class From, class To>
	struct is_high_rank_math_type_quaternion<quaternion<From>, To> : std::bool_constant<
		//quaternion<From>がToの基底となる場合も含めて判定
		is_high_rank_math_type_v<quaternion<From>, typename To::basis_type> || is_high_rank_math_type_v<From, typename To::basis_type>
	> {};
	//Fromの型によりis_high_rank_math_type_quaternionで分岐
	template <class From, class To>
	struct is_high_rank_math_type<From, quaternion<To>> : is_high_rank_math_type_quaternion<From, quaternion<To>> {};


	//加法パラメータ取得
	template <class T>
	struct addition_traits<quaternion<T>> {
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
	struct multiplication_traits<quaternion<T>> {
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
		static constexpr bool commutative_value = false;
		//分配律
		static constexpr bool distributive_value = multiplication_traits<T>::distributive_value;
	};


	//逆元が存在するならば逆元の取得(存在しない場合は例外を出す)
	template <class T>
	struct Inverse_element<quaternion<T>> {
		static constexpr quaternion<T> _additive_inverse_(const quaternion<T>& q) {
			return quaternion<T>(additive_inverse(q[0]), additive_inverse(q[1]), additive_inverse(q[2]), additive_inverse(q[3]));
		}

		//Tに乗法逆元が存在する場合の乗法逆元
		static constexpr quaternion<T> _multiplicative_inverse_impl_(const quaternion<T>& q, std::true_type) {
			//共役を絶対値の2乗で割る
			T temp = q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3];
			return quaternion<T>(q[0] / temp, -q[1] / temp, -q[2] / temp, -q[3] / temp);
		}
		//Tの乗法逆元が存在しない場合の乗法逆元
		static constexpr quaternion<T> _multiplicative_inverse_impl_(const quaternion<T>& q, std::false_type) {
			T temp = multiplicative_inverse(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
			return quaternion<T>(q[0] * temp, -q[1] * temp, -q[2] * temp, -q[3] * temp);
		}
		static constexpr quaternion<T> _multiplicative_inverse_(const quaternion<T>& q) {
			return _multiplicative_inverse_impl_(q, std::bool_constant<is_exist_multiplicative_inverse_v<T>>());
		}
	};


	//誤差評価
	template <class T>
	struct Error_evaluation<quaternion<T>> {
		static constexpr quaternion<T> epsilon() { return quaternion<T>(Error_evaluation<T>::epsilon(), Error_evaluation<T>::epsilon()); }
		static constexpr bool _error_evaluation_(const quaternion<T>& x1, const quaternion<T>& x2) {
			return error_evaluation(x1[0], x2[0]) && error_evaluation(x1[1], x2[1]) && error_evaluation(x1[2], x2[2]) && error_evaluation(x1[3], x2[3]);
		}
	};
}


#endif