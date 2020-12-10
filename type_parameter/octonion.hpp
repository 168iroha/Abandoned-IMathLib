#ifndef IMATHLIB_H_MATH_HYPERCOMPLEX_OCTONION_HPP
#define IMATHLIB_H_MATH_HYPERCOMPLEX_OCTONION_HPP

#include "IMathLib/math/hypercomplex/quaternion.hpp"


namespace iml {

	template <class>
	class octonion;


	//八元数型のパラメータ
	template <class T, class Re, class Im1, class Im2, class Im3, class Im4, class Im5, class Im6, class Im7>
	using octonion_parameter = type_parameter<octonion<T>, type_tuple<Re, Im1, Im2, Im3, Im4, Im5, Im6, Im7>>;
	template <class T, class ReP, class ImP1, class ImP2, class ImP3, class ImP4, class ImP5, class ImP6, class ImP7>
	struct type_parameter<octonion<T>, type_tuple<type_parameter<T, ReP>, type_parameter<T, ImP1>, type_parameter<T, ImP2>, type_parameter<T, ImP3>, type_parameter<T, ImP4>, type_parameter<T, ImP5>, type_parameter<T, ImP6>, type_parameter<T, ImP7>>> {
		using type = octonion<T>;
		static constexpr type value = type(type_parameter<T, ReP>::value, type_parameter<T, ImP1>::value, type_parameter<T, ImP2>::value, type_parameter<T, ImP3>::value, type_parameter<T, ImP4>::value, type_parameter<T, ImP5>::value, type_parameter<T, ImP6>::value, type_parameter<T, ImP7>::value);

		template <class = std::enable_if_t<is_exist_additive_inverse_v<T>>>
		auto operator-() const { return octonion_parameter<T, decltype(-type_parameter<T, ReP>()), decltype(-type_parameter<T, ImP1>()), decltype(-type_parameter<T, ImP2>()), decltype(-type_parameter<T, ImP3>()), decltype(-type_parameter<T, ImP4>()), decltype(-type_parameter<T, ImP5>()), decltype(-type_parameter<T, ImP6>()), decltype(-type_parameter<T, ImP7>())>(); }
		auto operator+() const { return *this; }

		template <class T, T Val, class = std::enable_if_t<(Val >= 0) && (Val < 8)>>
			auto operator[](int_parameter<T, Val>) const { return at_type_tuple_t<Val, type_tuple<type_parameter<T, ReP>, type_parameter<T, ImP1>, type_parameter<T, ImP2>, type_parameter<T, ImP3>, type_parameter<T, ImP4>, type_parameter<T, ImP5>, type_parameter<T, ImP6>, type_parameter<T, ImP7>>>(); }
	};


	// 八元数におけるスカラー演算と標準演算の十分条件の定義
	// 加算
	template <class T1, class T2>
	struct is_lscalar_operation_impl2<is_addable, T1, octonion<T2>> : std::bool_constant<is_high_rank_math_type_v<T1, add_result_t<T1, T2>> || is_high_rank_math_type_v<add_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_addable, octonion<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<T2, add_result_t<T1, T2>> || is_high_rank_math_type_v<add_result_t<T1, T2>, T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_addable, octonion<T1>, octonion<T2>> : std::bool_constant<is_addable_v<T1, T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_addable, octonion<T1>, complex<T2>> : std::bool_constant<is_addable_v<T1, T2> && (is_high_rank_math_type_v<T1, add_result_t<T1, T2>> || is_high_rank_math_type_v<add_result_t<T1, T2>, T1>)> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_addable, complex<T1>, octonion<T2>> : std::bool_constant<is_addable_v<T1, T2> && (is_high_rank_math_type_v<T1, add_result_t<T1, T2>> || is_high_rank_math_type_v<add_result_t<T1, T2>, T1>)> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_addable, octonion<T1>, quaternion<T2>> : std::bool_constant<is_addable_v<T1, T2> && (is_high_rank_math_type_v<T1, add_result_t<T1, T2>> || is_high_rank_math_type_v<add_result_t<T1, T2>, T1>)> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_addable, quaternion<T1>, octonion<T2>> : std::bool_constant<is_addable_v<T1, T2> && (is_high_rank_math_type_v<T1, add_result_t<T1, T2>> || is_high_rank_math_type_v<add_result_t<T1, T2>, T1>)> {};
	// 減算
	template <class T1, class T2>
	struct is_lscalar_operation_impl2<is_subtractable, T1, octonion<T2>> : std::bool_constant<is_high_rank_math_type_v<T1, sub_result_t<T1, T2>> || is_high_rank_math_type_v<sub_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_subtractable, octonion<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<T2, sub_result_t<T1, T2>> || is_high_rank_math_type_v<sub_result_t<T1, T2>, T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_subtractable, octonion<T1>, octonion<T2>> : std::bool_constant<is_subtractable_v<T1, T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_subtractable, octonion<T1>, complex<T2>> : std::bool_constant<is_subtractable_v<T1, T2> && (is_high_rank_math_type_v<T1, sub_result_t<T1, T2>> || is_high_rank_math_type_v<sub_result_t<T1, T2>, T1>)> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_subtractable, complex<T1>, octonion<T2>> : std::bool_constant<is_subtractable_v<T1, T2> && (is_high_rank_math_type_v<T1, sub_result_t<T1, T2>> || is_high_rank_math_type_v<sub_result_t<T1, T2>, T1>)> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_subtractable, octonion<T1>, quaternion<T2>> : std::bool_constant<is_subtractable_v<T1, T2> && (is_high_rank_math_type_v<T1, sub_result_t<T1, T2>> || is_high_rank_math_type_v<sub_result_t<T1, T2>, T1>)> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_subtractable, quaternion<T1>, octonion<T2>> : std::bool_constant<is_subtractable_v<T1, T2> && (is_high_rank_math_type_v<T1, sub_result_t<T1, T2>> || is_high_rank_math_type_v<sub_result_t<T1, T2>, T1>)> {};
	// 乗算
	template <class T1, class T2>
	struct is_lscalar_operation_impl2<is_multipliable, T1, octonion<T2>> : std::bool_constant<is_multipliable_v<T1, T2>> {};
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_multipliable, octonion<T1>, T2> : std::bool_constant<is_multipliable_v<T1, T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_multipliable, octonion<T1>, octonion<T2>> : std::bool_constant<is_exist_additive_inverse_v<mul_result_t<T1, T2>>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_multipliable, octonion<T1>, complex<T2>> : std::bool_constant<is_exist_additive_inverse_v<mul_result_t<T1, T2>>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_multipliable, complex<T1>, octonion<T2>> : std::bool_constant<is_exist_additive_inverse_v<mul_result_t<T1, T2>>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_multipliable, octonion<T1>, quaternion<T2>> : std::bool_constant<is_exist_additive_inverse_v<mul_result_t<T1, T2>>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_multipliable, quaternion<T1>, octonion<T2>> : std::bool_constant<is_exist_additive_inverse_v<mul_result_t<T1, T2>>> {};
	// 除算
	template <class T1, class T2>
	struct is_lscalar_operation_impl2<is_divisible, T1, octonion<T2>> : std::bool_constant<is_multipliable_v<T1, T2> && is_skew_field_v<T2>> {};
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_divisible, octonion<T1>, T2> : std::bool_constant<is_divisible_v<T1, T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_divisible, octonion<T1>, octonion<T2>> : std::bool_constant<is_exist_additive_inverse_v<mul_result_t<T1, T2>> && is_skew_field_v<T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_divisible, octonion<T1>, complex<T2>> : std::bool_constant<is_exist_additive_inverse_v<mul_result_t<T1, T2>> && is_skew_field_v<T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_divisible, complex<T1>, octonion<T2>> : std::bool_constant<is_exist_additive_inverse_v<mul_result_t<T1, T2>> && is_skew_field_v<T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_divisible, octonion<T1>, quaternion<T2>> : std::bool_constant<is_exist_additive_inverse_v<mul_result_t<T1, T2>> && is_skew_field_v<T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_divisible, quaternion<T1>, octonion<T2>> : std::bool_constant<is_exist_additive_inverse_v<mul_result_t<T1, T2>> && is_skew_field_v<T2>> {};
	// 等価比較
	template <class T1, class T2>
	struct is_lscalar_operation_impl2<is_comparable, T1, octonion<T2>> : std::bool_constant<is_comparable_v<T1, T2> && is_exist_additive_identity_v<T1>> {};
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_comparable, octonion<T1>, T2> : std::bool_constant<is_comparable_v<T1, T2> && is_exist_additive_identity_v<T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_comparable, octonion<T1>, octonion<T2>> : std::bool_constant<is_comparable_v<T1, T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_comparable, octonion<T1>, complex<T2>> : std::bool_constant<is_comparable_v<T1, T2> && is_exist_additive_identity_v<T1>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_comparable, complex<T1>, octonion<T2>> : std::bool_constant<is_comparable_v<T1, T2> && is_exist_additive_identity_v<T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_comparable, octonion<T1>, quaternion<T2>> : std::bool_constant<is_comparable_v<T1, T2> && is_exist_additive_identity_v<T1>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_comparable, quaternion<T1>, octonion<T2>> : std::bool_constant<is_comparable_v<T1, T2> && is_exist_additive_identity_v<T2>> {};
	// 代入
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_assignable, octonion<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<T2, T1> && is_exist_additive_identity_v<T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_assignable, octonion<T1>, octonion<T2>> : std::bool_constant<is_high_rank_math_type_v<T2, T1> && is_exist_additive_identity_v<T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_assignable, octonion<T1>, complex<T2>> : std::bool_constant<is_high_rank_math_type_v<T2, T1> && is_exist_additive_identity_v<T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_assignable, octonion<T1>, quaternion<T2>> : std::bool_constant<is_high_rank_math_type_v<T2, T1> && is_exist_additive_identity_v<T2>> {};
	// 加算代入
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_add_assignable, octonion<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<add_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_add_assignable, octonion<T1>, octonion<T2>> : std::bool_constant<is_high_rank_math_type_v<add_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_add_assignable, octonion<T1>, complex<T2>> : std::bool_constant<is_high_rank_math_type_v<add_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_add_assignable, octonion<T1>, quaternion<T2>> : std::bool_constant<is_high_rank_math_type_v<add_result_t<T1, T2>, T1>> {};
	// 減算代入
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_sub_assignable, octonion<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<sub_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_sub_assignable, octonion<T1>, octonion<T2>> : std::bool_constant<is_high_rank_math_type_v<sub_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_sub_assignable, octonion<T1>, complex<T2>> : std::bool_constant<is_high_rank_math_type_v<sub_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_sub_assignable, octonion<T1>, quaternion<T2>> : std::bool_constant<is_high_rank_math_type_v<sub_result_t<T1, T2>, T1>> {};
	// 乗算代入
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_mul_assignable, octonion<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<mul_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_mul_assignable, octonion<T1>, octonion<T2>> : std::bool_constant<is_high_rank_math_type_v<mul_result_t<T1, T2>, T1> && is_exist_additive_inverse_v<mul_result_t<T1, T2>>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_mul_assignable, octonion<T1>, complex<T2>> : std::bool_constant<is_high_rank_math_type_v<mul_result_t<T1, T2>, T1> && is_exist_additive_inverse_v<mul_result_t<T1, T2>>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_mul_assignable, octonion<T1>, quaternion<T2>> : std::bool_constant<is_high_rank_math_type_v<mul_result_t<T1, T2>, T1> && is_exist_additive_inverse_v<mul_result_t<T1, T2>>> {};
	// 除算代入
	template <class T1, class T2>
	struct is_rscalar_operation_impl2<is_div_assignable, octonion<T1>, T2> : std::bool_constant<is_high_rank_math_type_v<div_result_t<T1, T2>, T1>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_div_assignable, octonion<T1>, octonion<T2>> : std::bool_constant<is_high_rank_math_type_v<mul_result_t<T1, T2>, T1> && is_exist_additive_inverse_v<mul_result_t<T1, T2>> && is_skew_field_v<T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_div_assignable, octonion<T1>, complex<T2>> : std::bool_constant<is_high_rank_math_type_v<mul_result_t<T1, T2>, T1> && is_exist_additive_inverse_v<mul_result_t<T1, T2>> && is_skew_field_v<T2>> {};
	template <class T1, class T2>
	struct is_standard_operation_impl2<is_div_assignable, octonion<T1>, quaternion<T2>> : std::bool_constant<is_high_rank_math_type_v<mul_result_t<T1, T2>, T1> && is_exist_additive_inverse_v<mul_result_t<T1, T2>> && is_skew_field_v<T2>> {};



	//八元数型
	template <class T>
	class octonion {
		template <class> friend class octonion;
		T x_m[8];
	public:
		constexpr octonion() : x_m{} {}
		template <class U, class = std::enable_if_t<is_rscalar_operation_v<is_assignable, octonion, U>>>
		constexpr octonion(const U& re) : x_m{ re } {}
		template <class U1, class U2, class U3, class U4, class U5, class U6, class U7, class U8, class = std::enable_if_t<is_standard_operation_v<is_assignable, octonion, octonion<common_math_type_t<U1, U2, U3, U4, U5, U6, U7, U8>>>>>
		constexpr octonion(const U1& re, const U2& im1, const U3& im2, const U4& im3, const U5& im4, const U6& im5, const U7& im6, const U8& im7) : x_m{ T(re),T(im1),T(im2),T(im3),T(im4),T(im5),T(im6),T(im7) } {}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_assignable, octonion, complex<U>>>>
		constexpr octonion(const complex<U>& c) : x_m{ c[0],c[1] } {}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_assignable, octonion, quaternion<U>>>>
		constexpr octonion(const quaternion<U>& q) : x_m{ q[0],q[1],q[2],q[3] } {}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_assignable, octonion, octonion<U>>>>
		constexpr octonion(const U& re, const vector<U, 7>& v) : x_m{ re,v[0],v[1],v[2],v[3],v[4],v[5],v[6] } {}
		constexpr octonion(const octonion& o) : x_m{ o.x_m[0],o.x_m[1],o.x_m[2],o.x_m[3],o.x_m[4],o.x_m[5],o.x_m[6],o.x_m[7] } {}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_assignable, octonion, octonion<U>>>>
		constexpr octonion(const octonion<U>& o) : x_m{ o.x_m[0],o.x_m[1],o.x_m[2],o.x_m[3],o.x_m[4],o.x_m[5],o.x_m[6],o.x_m[7] } {}

		using basis_type = T;
		using iterator = linear_iterator<T>;
		using const_iterator = linear_iterator<const T>;

		template<class Other>
		struct rebind {
			using other = octonion<Other>;
		};

		constexpr iterator begin() noexcept { return iterator(x_m); }
		constexpr const_iterator begin() const noexcept { return const_iterator(x_m); }
		constexpr iterator end() noexcept { return iterator(x_m + 8); }
		constexpr const_iterator end() const noexcept { return const_iterator(x_m + 8); }

		//単項演算の継承
		template <class = std::enable_if_t<is_exist_additive_inverse_v<T>>>
		constexpr octonion operator-() const { return octonion(-this->x_m[0], -this->x_m[1], -this->x_m[2], -this->x_m[3], -this->x_m[4], -this->x_m[5], -this->x_m[6], -this->x_m[7]); }
		constexpr octonion operator+() const { return octonion(*this); }
		//代入演算
		octonion& operator=(const octonion& o) {
			if (this != std::addressof(o)) {
				this->x_m[0] = o.x_m[0]; this->x_m[1] = o.x_m[1];
				this->x_m[2] = o.x_m[2]; this->x_m[3] = o.x_m[3];
				this->x_m[4] = o.x_m[4]; this->x_m[5] = o.x_m[5];
				this->x_m[6] = o.x_m[6]; this->x_m[7] = o.x_m[7];
			}
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_assignable, octonion, octonion<U>>>>
		octonion& operator=(const octonion<U>& o) {
			this->x_m[0] = o.x_m[0]; this->x_m[1] = o.x_m[1];
			this->x_m[2] = o.x_m[2]; this->x_m[3] = o.x_m[3];
			this->x_m[4] = o.x_m[4]; this->x_m[5] = o.x_m[5];
			this->x_m[6] = o.x_m[6]; this->x_m[7] = o.x_m[7];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_rscalar_operation_v<is_assignable, quaternion, U>>>
		octonion& operator=(const U& n) {
			this->x_m[0] = n; this->x_m[1] = addition_traits<T>::identity_element();
			this->x_m[2] = addition_traits<T>::identity_element(); this->x_m[3] = addition_traits<T>::identity_element();
			this->x_m[4] = addition_traits<T>::identity_element(); this->x_m[5] = addition_traits<T>::identity_element();
			this->x_m[6] = addition_traits<T>::identity_element(); this->x_m[7] = addition_traits<T>::identity_element();
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_assignable, octonion, complex<U>>>>
		octonion& operator=(const complex<U>& c) {
			this->x_m[0] = c[0]; this->x_m[1] = c[1];
			this->x_m[2] = addition_traits<T>::identity_element(); this->x_m[3] = addition_traits<T>::identity_element();
			this->x_m[4] = addition_traits<T>::identity_element(); this->x_m[5] = addition_traits<T>::identity_element();
			this->x_m[6] = addition_traits<T>::identity_element(); this->x_m[7] = addition_traits<T>::identity_element();
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_assignable, octonion, quaternion<U>>>>
		octonion& operator=(const quaternion<U>& q) {
			this->x_m[0] = q[0]; this->x_m[1] = q[1];
			this->x_m[2] = q[2]; this->x_m[3] = q[3];
			this->x_m[4] = addition_traits<T>::identity_element(); this->x_m[5] = addition_traits<T>::identity_element();
			this->x_m[6] = addition_traits<T>::identity_element(); this->x_m[7] = addition_traits<T>::identity_element();
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_add_assignable, octonion, octonion<U>>>>
		octonion& operator+=(const octonion<U>& o) {
			this->x_m[0] += o.x_m[0]; this->x_m[1] += o.x_m[1];
			this->x_m[2] += o.x_m[2]; this->x_m[3] += o.x_m[3];
			this->x_m[4] += o.x_m[4]; this->x_m[5] += o.x_m[5];
			this->x_m[6] += o.x_m[6]; this->x_m[7] += o.x_m[7];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_rscalar_operation_v<is_add_assignable, quaternion, U>>>
		octonion& operator+=(const U& n) {
			this->x_m[0] += n;
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_add_assignable, octonion, complex<U>>>>
		octonion& operator+=(const complex<U>& c) {
			this->x_m[0] += c[0]; this->x_m[1] += c[1];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_add_assignable, octonion, quaternion<U>>>>
		octonion& operator+=(const quaternion<U>& q) {
			this->x_m[0] += q[0]; this->x_m[1] += q[1];
			this->x_m[2] += q[2]; this->x_m[3] += q[3];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_sub_assignable, octonion, octonion<U>>>>
		octonion& operator-=(const octonion<U>& o) {
			this->x_m[0] -= o.x_m[0]; this->x_m[1] -= o.x_m[1];
			this->x_m[2] -= o.x_m[2]; this->x_m[3] -= o.x_m[3];
			this->x_m[4] -= o.x_m[4]; this->x_m[5] -= o.x_m[5];
			this->x_m[6] -= o.x_m[6]; this->x_m[7] -= o.x_m[7];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_rscalar_operation_v<is_sub_assignable, quaternion, U>>>
		octonion& operator-=(const U& n) {
			this->x_m[0] -= n;
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_sub_assignable, octonion, complex<U>>>>
		octonion& operator-=(const complex<U>& c) {
			this->x_m[0] -= c[0]; this->x_m[1] -= c[1];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_sub_assignable, octonion, quaternion<U>>>>
		octonion& operator-=(const quaternion<U>& q) {
			this->x_m[0] -= q[0]; this->x_m[1] -= q[1];
			this->x_m[2] -= q[2]; this->x_m[3] -= q[3];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_mul_assignable, octonion, octonion<U>>>>
		octonion& operator*=(const octonion<U>& o) {
			//()は7次元の内積と外積
			T temp[8] = {
				this->x_m[0] * o.x_m[0] - (this->x_m[1] * o.x_m[1] + this->x_m[2] * o.x_m[2] + this->x_m[3] * o.x_m[3] + this->x_m[4] * o.x_m[4] + this->x_m[5] * o.x_m[5] + this->x_m[6] * o.x_m[6] + this->x_m[7] * o.x_m[7])
				,this->x_m[0] * o.x_m[1] + this->x_m[1] * o.x_m[0] + (this->x_m[2] * o.x_m[3] - this->x_m[3] * o.x_m[2] - this->x_m[4] * o.x_m[5] + this->x_m[5] * o.x_m[4] - this->x_m[6] * o.x_m[7] + this->x_m[7] * o.x_m[6])
				,this->x_m[0] * o.x_m[2] + this->x_m[2] * o.x_m[0] + (-this->x_m[1] * o.x_m[3] + this->x_m[3] * o.x_m[1] - this->x_m[4] * o.x_m[6] + this->x_m[5] * o.x_m[7] + this->x_m[6] * o.x_m[4] - this->x_m[7] * o.x_m[5])
				,this->x_m[0] * o.x_m[3] + this->x_m[3] * o.x_m[0] + (this->x_m[1] * o.x_m[2] - this->x_m[2] * o.x_m[1] - this->x_m[4] * o.x_m[7] - this->x_m[5] * o.x_m[6] + this->x_m[6] * o.x_m[5] + this->x_m[7] * o.x_m[4])
				,this->x_m[0] * o.x_m[4] + this->x_m[4] * o.x_m[0] + (this->x_m[1] * o.x_m[5] + this->x_m[2] * o.x_m[6] + this->x_m[3] * o.x_m[7] - this->x_m[5] * o.x_m[1] - this->x_m[6] * o.x_m[2] - this->x_m[7] * o.x_m[3])
				,this->x_m[0] * o.x_m[5] + this->x_m[5] * o.x_m[0] + (-this->x_m[1] * o.x_m[4] - this->x_m[2] * o.x_m[7] + this->x_m[3] * o.x_m[6] + this->x_m[4] * o.x_m[1] - this->x_m[6] * o.x_m[3] + this->x_m[7] * o.x_m[2])
				,this->x_m[0] * o.x_m[6] + this->x_m[6] * o.x_m[0] + (this->x_m[1] * o.x_m[7] - this->x_m[2] * o.x_m[4] - this->x_m[3] * o.x_m[5] + this->x_m[4] * o.x_m[2] + this->x_m[5] * o.x_m[3] - this->x_m[7] * o.x_m[1])
				,this->x_m[0] * o.x_m[7] + this->x_m[7] * o.x_m[0] + (-this->x_m[1] * o.x_m[6] + this->x_m[2] * o.x_m[5] - this->x_m[3] * o.x_m[4] + this->x_m[4] * o.x_m[3] - this->x_m[5] * o.x_m[2] + this->x_m[6] * o.x_m[1])
			};
			this->x_m[0] = temp[0]; this->x_m[1] = temp[1];
			this->x_m[2] = temp[2]; this->x_m[3] = temp[3];
			this->x_m[4] = temp[4]; this->x_m[5] = temp[5];
			this->x_m[6] = temp[6]; this->x_m[7] = temp[7];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_rscalar_operation_v<is_mul_assignable, quaternion, U>>>
		octonion& operator*=(const U& k) {
			this->x_m[0] *= k; this->x_m[1] *= k;
			this->x_m[2] *= k; this->x_m[3] *= k;
			this->x_m[4] *= k; this->x_m[5] *= k;
			this->x_m[6] *= k; this->x_m[7] *= k;
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_mul_assignable, octonion, complex<U>>>>
		octonion& operator*=(const complex<U>& c) {
			T temp[8] = {
				this->x_m[0] * c[0] - (this->x_m[1] * c[1])
				,this->x_m[0] * c[1] + this->x_m[1] * c[0]
				,this->x_m[2] * c[0] + (this->x_m[3] * c[1])
				,this->x_m[3] * c[0] + (-this->x_m[2] * c[1])
				,this->x_m[4] * c[0] + (-this->x_m[5] * c[1])
				,this->x_m[5] * c[0] + (this->x_m[4] * c[1])
				,this->x_m[6] * c[0] + (-this->x_m[7] * c[1])
				,this->x_m[7] * c[0] + (this->x_m[6] * c[1])
			};
			this->x_m[0] = temp[0]; this->x_m[1] = temp[1];
			this->x_m[2] = temp[2]; this->x_m[3] = temp[3];
			this->x_m[4] = temp[4]; this->x_m[5] = temp[5];
			this->x_m[6] = temp[6]; this->x_m[7] = temp[7];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_mul_assignable, octonion, quaternion<U>>>>
		octonion& operator*=(const quaternion<U>& q) {
			T temp[8] = {
				this->x_m[0] * q[0] - (this->x_m[1] * q[1] + this->x_m[2] * q[2] + this->x_m[3] * q[3])
				,this->x_m[0] * q[1] + this->x_m[1] * q[0] + (this->x_m[2] * q[3] - this->x_m[3] * q[2])
				,this->x_m[0] * q[2] + this->x_m[2] * q[0] + (-this->x_m[1] * q[3] + this->x_m[3] * q[1])
				,this->x_m[0] * q[3] + this->x_m[3] * q[0] + (this->x_m[1] * q[2] - this->x_m[2] * q[1])
				,this->x_m[4] * q[0] + (-this->x_m[5] * q[1] - this->x_m[6] * q[2] - this->x_m[7] * q[3])
				,this->x_m[5] * q[0] + (this->x_m[4] * q[1] - this->x_m[6] * q[3] + this->x_m[7] * q[2])
				,this->x_m[6] * q[0] + (this->x_m[4] * q[2] + this->x_m[5] * q[3] - this->x_m[7] * q[1])
				,this->x_m[7] * q[0] + (this->x_m[4] * q[3] - this->x_m[5] * q[2] + this->x_m[6] * q[1])
			};
			this->x_m[0] = temp[0]; this->x_m[1] = temp[1];
			this->x_m[2] = temp[2]; this->x_m[3] = temp[3];
			this->x_m[4] = temp[4]; this->x_m[5] = temp[5];
			this->x_m[6] = temp[6]; this->x_m[7] = temp[7];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_div_assignable, octonion, octonion<U>>>>
		octonion& operator/=(const octonion<U>& o) {
			//逆元は共役を絶対値の二乗で割る
			T temp = o.x_m[0] * o.x_m[0] + o.x_m[1] * o.x_m[1] + o.x_m[2] * o.x_m[2] + o.x_m[3] * o.x_m[3] + o.x_m[4] * o.x_m[4] + o.x_m[5] * o.x_m[5] + o.x_m[6] * o.x_m[6] + o.x_m[7] * o.x_m[7];
			return *this *= octonion<U>(o.x_m[0] / temp, -o.x_m[1] / temp, -o.x_m[2] / temp, -o.x_m[3] / temp, -o.x_m[4] / temp, -o.x_m[5] / temp, -o.x_m[6] / temp, -o.x_m[7] / temp);
		}
		template <class U, class = std::enable_if_t<is_rscalar_operation_v<is_div_assignable, quaternion, U>>>
		octonion& operator/=(const U& k) {
			this->x_m[0] /= k; this->x_m[1] /= k;
			this->x_m[2] /= k; this->x_m[3] /= k;
			this->x_m[4] /= k; this->x_m[5] /= k;
			this->x_m[6] /= k; this->x_m[7] /= k;
			return *this;
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_div_assignable, octonion, complex<U>>>>
		octonion& operator/=(const complex<U>& c) {
			return *this *= multiplicative_inverse(c);
		}
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_div_assignable, octonion, quaternion<U>>>>
		octonion& operator/=(const quaternion<U>& q) {
			return *this *= multiplicative_inverse(q);
		}

		//添え字演算
		const constexpr T& operator[](size_t index) const { return this->x_m[index]; }
		constexpr T& operator[](size_t index) { return this->x_m[index]; }

		//ストリーム出力
		friend std::ostream& operator<<(std::ostream& os, const octonion& n) {
			os << '(' << n.x_m[0] << ',' << n.x_m[1] << ',' << n.x_m[2] << ',' << n.x_m[3]
				<< ',' << n.x_m[4] << ',' << n.x_m[5] << ',' << n.x_m[6] << ',' << n.x_m[7] << ')';
			return os;
		}
		friend std::wostream& operator<<(std::wostream& os, const octonion& n) {
			os << '(' << n.x_m[0] << ',' << n.x_m[1] << ',' << n.x_m[2] << ',' << n.x_m[3]
				<< ',' << n.x_m[4] << ',' << n.x_m[5] << ',' << n.x_m[6] << ',' << n.x_m[7] << ')';
			return os;
		}

		template <class T>
		constexpr linear_input<iterator> operator<<(const T& value) {
			iterator itr = this->begin();
			*itr = value; ++itr;
			return linear_input<iterator>(itr, this->end());
		}
	};


	template <class U1, class U2, class U3, class U4, class U5, class U6, class U7, class U8>
	inline constexpr octonion<common_math_type_t<U1, U2, U3, U4, U5, U6, U7, U8>> make_octonion(U1&& re, U2&& im1, U3&& im2, U4&& im3, U5&& im4, U6&& im5, U7&& im6, U8&& im7) {
		return octonion<common_math_type_t<U1, U2, U3, U4, U5, U6, U7, U8>>(std::forward<U1>(re), std::forward<U2>(im1), std::forward<U3>(im2), std::forward<U4>(im3), std::forward<U5>(im4), std::forward<U6>(im5), std::forward<U7>(im6), std::forward<U8>(im7));
	}


	//八元数の2項演算
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_addable, octonion<T1>, octonion<T2>>>>
	inline constexpr auto operator+(const octonion<T1>& lhs, const octonion<T2>& rhs) {
		return octonion<add_result_t<T1, T2>>(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2], lhs[3] + rhs[3], lhs[4] + rhs[4], lhs[5] + rhs[5], lhs[6] + rhs[6], lhs[7] + rhs[7]);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Im4_1, class Im5_1, class Im6_1, class Im7_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class Im4_2, class Im5_2, class Im6_2, class Im7_2, class = std::enable_if_t<is_standard_operation_v<is_addable, octonion<T1>, octonion<T2>>>>
	inline auto operator+(octonion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1, Im4_1, Im5_1, Im6_1, Im7_1>, octonion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2, Im4_2, Im5_2, Im6_2, Im7_2>) {
		return octonion_parameter<add_result_t<T1, T2>, decltype(Re1() + Re2()), decltype(Im1_1() + Im1_2()), decltype(Im2_1() + Im2_2()), decltype(Im3_1() + Im3_2())
			, decltype(Im4_1() + Im4_2()), decltype(Im5_1() + Im5_2()), decltype(Im6_1() + Im6_2()), decltype(Im7_1() + Im7_2())>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_rscalar_operation_v<is_addable, octonion<T1>, T2>>>
	inline constexpr auto operator+(const octonion<T1>& lhs, const T2& rhs) {
		return octonion<add_result_t<T1, T2>>(lhs[0] + rhs, lhs[1], lhs[2], lhs[3], lhs[4], lhs[5], lhs[6], lhs[7]);
	}
	template <class T1, class T2, class Re, class Im1, class Im2, class Im3, class Im4, class Im5, class Im6, class Im7, class Param, class = std::enable_if_t<is_rscalar_operation_v<is_addable, octonion<T1>, T2>>>
	inline auto operator+(octonion_parameter<T1, Re, Im1, Im2, Im3, Im4, Im5, Im6, Im7>, type_parameter<T2, Param> rhs) {
		return octonion_parameter<add_result_t<T1, T2>, decltype(Re() + rhs), Im1, Im2, Im3, Im4, Im5, Im6, Im7>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_lscalar_operation_v<is_addable, T1, octonion<T2>>>>
	inline constexpr auto operator+(const T1& lhs, const octonion<T2>& rhs) {
		return octonion<add_result_t<T1, T2>>(lhs + rhs[0], rhs[1], rhs[2], rhs[3], rhs[4], rhs[5], rhs[6], rhs[7]);
	}
	template <class T1, class T2, class Param, class Re, class Im1, class Im2, class Im3, class Im4, class Im5, class Im6, class Im7, class = std::enable_if_t<is_lscalar_operation_v<is_addable, T1, octonion<T2>>>>
	inline auto operator+(type_parameter<T1, Param> lhs, octonion_parameter<T2, Re, Im1, Im2, Im3, Im4, Im5, Im6, Im7>) {
		return octonion_parameter<add_result_t<T1, T2>, decltype(lhs + Re()), Im1, Im2, Im3, Im4, Im5, Im6, Im7>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_addable, octonion<T1>, complex<T2>>>>
	inline constexpr auto operator+(const octonion<T1>& lhs, const complex<T2>& rhs) {
		return octonion<add_result_t<T1, T2>>(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2], lhs[3], lhs[4], lhs[5], lhs[6], lhs[7]);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Im4_1, class Im5_1, class Im6_1, class Im7_1, class Re2, class Im_2, class = std::enable_if_t<is_standard_operation_v<is_addable, octonion<T1>, complex<T2>>>>
	inline auto operator+(octonion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1, Im4_1, Im5_1, Im6_1, Im7_1>, complex_parameter<T2, Re2, Im_2>) {
		return octonion_parameter<add_result_t<T1, T2>, decltype(Re1() + Re2()), decltype(Im1_1() + Im_2()), Im2_1, Im3_1, Im4_1, Im5_1, Im6_1, Im7_1>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_addable, complex<T1>, octonion<T2>>>>
	inline constexpr auto operator+(const complex<T1>& lhs, const octonion<T2>& rhs) {
		return octonion<add_result_t<T1, T2>>(lhs[0] + rhs[0], lhs[1] + rhs[1], rhs[2], rhs[3], rhs[4], rhs[5], rhs[6], rhs[7]);
	}
	template <class T1, class T2, class Re1, class Im_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class Im4_2, class Im5_2, class Im6_2, class Im7_2, class = std::enable_if_t<is_standard_operation_v<is_addable, complex<T1>, octonion<T2>>>>
	inline auto operator+(complex_parameter<T1, Re1, Im_1>, octonion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2, Im4_2, Im5_2, Im6_2, Im7_2>) {
		return octonion_parameter<add_result_t<T1, T2>, decltype(Re1() + Re2()), decltype(Im_1() + Im1_2()), Im2_2, Im3_2, Im4_2, Im5_2, Im6_2, Im7_2>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_addable, octonion<T1>, quaternion<T2>>>>
	inline constexpr auto operator+(const octonion<T1>& lhs, const quaternion<T2>& rhs) {
		return octonion<add_result_t<T1, T2>>(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2], lhs[3] + rhs[3], lhs[4], lhs[5], lhs[6], lhs[7]);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Im4_1, class Im5_1, class Im6_1, class Im7_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = std::enable_if_t<is_standard_operation_v<is_addable, octonion<T1>, quaternion<T2>>>>
	inline auto operator+(octonion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1, Im4_1, Im5_1, Im6_1, Im7_1>, quaternion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2>) {
		return octonion_parameter<add_result_t<T1, T2>, decltype(Re1() + Re2()), decltype(Im1_1() + Im1_2()), decltype(Im2_1() + Im2_2()), decltype(Im3_1() + Im3_2()), Im4_1, Im5_1, Im6_1, Im7_1>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_addable, quaternion<T1>, octonion<T2>>>>
	inline constexpr auto operator+(const quaternion<T1>& lhs, const octonion<T2>& rhs) {
		return octonion<add_result_t<T1, T2>>(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2], lhs[3] + rhs[3], rhs[4], rhs[5], rhs[6], rhs[7]);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class Im4_2, class Im5_2, class Im6_2, class Im7_2, class = std::enable_if_t<is_standard_operation_v<is_addable, quaternion<T1>, octonion<T2>>>>
	inline auto operator+(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1>, octonion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2, Im4_2, Im5_2, Im6_2, Im7_2>) {
		return octonion_parameter<add_result_t<T1, T2>, decltype(Re1() + Re2()), decltype(Im1_1() + Im1_2()), decltype(Im2_1() + Im2_2()), decltype(Im3_1() + Im3_2()), Im4_2, Im5_2, Im6_2, Im7_2>();
	}

	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_subtractable, octonion<T1>, octonion<T2>>>>
	inline constexpr auto operator-(const octonion<T1>& lhs, const octonion<T2>& rhs) {
		return octonion<sub_result_t<T1, T2>>(lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2], lhs[3] - rhs[3], lhs[4] - rhs[4], lhs[5] - rhs[5], lhs[6] - rhs[6], lhs[7] - rhs[7]);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Im4_1, class Im5_1, class Im6_1, class Im7_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class Im4_2, class Im5_2, class Im6_2, class Im7_2, class = std::enable_if_t<is_standard_operation_v<is_subtractable, octonion<T1>, octonion<T2>>>>
	inline auto operator-(octonion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1, Im4_1, Im5_1, Im6_1, Im7_1>, octonion_parameter<T1, Re2, Im1_2, Im2_2, Im3_2, Im4_2, Im5_2, Im6_2, Im7_2>) {
		return octonion_parameter<add_result_t<T1, T2>, decltype(Re1() - Re2()), decltype(Im1_1() - Im1_2()), decltype(Im2_1() - Im2_2()), decltype(Im3_1() - Im3_2())
			, decltype(Im4_1() - Im4_2()), decltype(Im5_1() - Im5_2()), decltype(Im6_1() - Im6_2()), decltype(Im7_1() - Im7_2())>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_rscalar_operation_v<is_subtractable, octonion<T1>, T2>>>
	inline constexpr auto operator-(const octonion<T1>& lhs, const T2& rhs) {
		return octonion<sub_result_t<T1, T2>>(lhs[0] - rhs, lhs[1], lhs[2], lhs[3], lhs[4], lhs[5], lhs[6], lhs[7]);
	}
	template <class T1, class T2, class Re, class Im1, class Im2, class Im3, class Im4, class Im5, class Im6, class Im7, class Param, class = std::enable_if_t<is_rscalar_operation_v<is_subtractable, octonion<T1>, T2>>>
	inline auto operator-(octonion_parameter<T1, Re, Im1, Im2, Im3, Im4, Im5, Im6, Im7>, type_parameter<T2, Param> rhs) {
		return octonion_parameter<sub_result_t<T1, T2>, decltype(Re() - rhs), Im1, Im2, Im3, Im4, Im5, Im6, Im7>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_lscalar_operation_v<is_subtractable, T1, octonion<T2>>>>
	inline constexpr auto operator-(const T1& lhs, const octonion<T2>& rhs) {
		return octonion<sub_result_t<T1, T2>>(lhs - rhs[0], -rhs[1], -rhs[2], -rhs[3], -rhs[4], -rhs[5], -rhs[6], -rhs[7]);
	}
	template <class T1, class T2, class Param, class Re, class Im1, class Im2, class Im3, class Im4, class Im5, class Im6, class Im7, class = std::enable_if_t<is_lscalar_operation_v<is_subtractable, T1, octonion<T2>>>>
	inline auto operator-(type_parameter<T1, Param> lhs, octonion_parameter<T2, Re, Im1, Im2, Im3, Im4, Im5, Im6, Im7>) {
		return octonion_parameter<sub_result_t<T1, T2>, decltype(lhs - Re()), decltype(-Im1()), decltype(-Im2()), decltype(-Im3()), decltype(-Im4()), decltype(-Im5()), decltype(-Im6()), decltype(-Im7())>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_subtractable, octonion<T1>, complex<T2>>>>
	inline constexpr auto operator-(const octonion<T1>& lhs, const complex<T2>& rhs) {
		return octonion<sub_result_t<T1, T2>>(lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2], lhs[3], lhs[4], lhs[5], lhs[6], lhs[7]);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Im4_1, class Im5_1, class Im6_1, class Im7_1, class Re2, class Im_2, class = std::enable_if_t<is_standard_operation_v<is_subtractable, octonion<T1>, complex<T2>>>>
	inline auto operator-(octonion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1, Im4_1, Im5_1, Im6_1, Im7_1>, complex_parameter<T2, Re2, Im_2>) {
		return octonion_parameter<sub_result_t<T1, T2>, decltype(Re1() - Re2()), decltype(Im1_1() - Im_2()), Im2_1, Im3_1, Im4_1, Im5_1, Im6_1, Im7_1>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_subtractable, complex<T1>, octonion<T2>>>>
	inline constexpr auto operator-(const complex<T1>& lhs, const octonion<T2>& rhs) {
		return octonion<sub_result_t<T1, T2>>(lhs[0] - rhs[0], lhs[1] - rhs[1], -rhs[2], -rhs[3], -rhs[4], -rhs[5], -rhs[6], -rhs[7]);
	}
	template <class T1, class T2, class Re1, class Im_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class Im4_2, class Im5_2, class Im6_2, class Im7_2, class = std::enable_if_t<is_standard_operation_v<is_subtractable, complex<T1>, octonion<T2>>>>
	inline auto operator-(complex_parameter<T1, Re1, Im_1>, octonion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2, Im4_2, Im5_2, Im6_2, Im7_2>) {
		return octonion_parameter<sub_result_t<T1, T2>, decltype(Re1() - Re2()), decltype(Im_1() - Im1_2()), decltype(-Im2_2()), decltype(-Im3_2()), decltype(-Im4_2()), decltype(-Im5_2()), decltype(-Im6_2()), decltype(-Im7_2())>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_subtractable, octonion<T1>, quaternion<T2>>>>
	inline constexpr auto operator-(const octonion<T1>& lhs, const quaternion<T2>& rhs) {
		return octonion<sub_result_t<T1, T2>>(lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2], lhs[3] - rhs[3], lhs[4], lhs[5], lhs[6], lhs[7]);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Im4_1, class Im5_1, class Im6_1, class Im7_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = std::enable_if_t<is_standard_operation_v<is_subtractable, octonion<T1>, quaternion<T2>>>>
	inline auto operator-(octonion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1, Im4_1, Im5_1, Im6_1, Im7_1>, quaternion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2>) {
		return octonion_parameter<sub_result_t<T1, T2>, decltype(Re1() - Re2()), decltype(Im1_1() - Im1_2()), decltype(Im2_1() - Im2_2()), decltype(Im3_1() - Im3_2()), Im4_1, Im5_1, Im6_1, Im7_1>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_subtractable, quaternion<T1>, octonion<T2>>>>
	inline constexpr auto operator-(const quaternion<T1>& lhs, const octonion<T2>& rhs) {
		return octonion<sub_result_t<T1, T2>>(lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2], lhs[3] - rhs[3], -rhs[4], -rhs[5], -rhs[6], -rhs[7]);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class Im4_2, class Im5_2, class Im6_2, class Im7_2, class = std::enable_if_t<is_standard_operation_v<is_subtractable, quaternion<T1>, octonion<T2>>>>
	inline auto operator-(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1>, octonion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2, Im4_2, Im5_2, Im6_2, Im7_2>) {
		return octonion_parameter<sub_result_t<T1, T2>, decltype(Re1() - Re2()), decltype(Im1_1() - Im1_2()), decltype(Im2_1() - Im2_2()), decltype(Im3_1() - Im3_2()), decltype(-Im4_2()), decltype(-Im5_2()), decltype(-Im6_2()), decltype(-Im7_2())>();
	}

	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_multipliable, octonion<T1>, octonion<T2>>>>
	inline constexpr auto operator*(const octonion<T1>& lhs, const octonion<T2>& rhs) {
		return octonion<mul_result_t<T1, T2>>(lhs[0] * rhs[0] - (lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3] * rhs[3] + lhs[4] * rhs[4] + lhs[5] * rhs[5] + lhs[6] * rhs[6] + lhs[7] * rhs[7])
			, lhs[0] * rhs[1] + lhs[1] * rhs[0] + (lhs[2] * rhs[3] - lhs[3] * rhs[2] - lhs[4] * rhs[5] + lhs[5] * rhs[4] - lhs[6] * rhs[7] + lhs[7] * rhs[6])
			, lhs[0] * rhs[2] + lhs[2] * rhs[0] + (-lhs[1] * rhs[3] + lhs[3] * rhs[1] - lhs[4] * rhs[6] + lhs[5] * rhs[7] + lhs[6] * rhs[4] - lhs[7] * rhs[5])
			, lhs[0] * rhs[3] + lhs[3] * rhs[0] + (lhs[1] * rhs[2] - lhs[2] * rhs[1] - lhs[4] * rhs[7] - lhs[5] * rhs[6] + lhs[6] * rhs[5] + lhs[7] * rhs[4])
			, lhs[0] * rhs[4] + lhs[4] * rhs[0] + (lhs[1] * rhs[5] + lhs[2] * rhs[6] + lhs[3] * rhs[7] - lhs[5] * rhs[1] - lhs[6] * rhs[2] - lhs[7] * rhs[3])
			, lhs[0] * rhs[5] + lhs[5] * rhs[0] + (-lhs[1] * rhs[4] - lhs[2] * rhs[7] + lhs[3] * rhs[6] + lhs[4] * rhs[1] - lhs[6] * rhs[3] + lhs[7] * rhs[2])
			, lhs[0] * rhs[6] + lhs[6] * rhs[0] + (lhs[1] * rhs[7] - lhs[2] * rhs[4] - lhs[3] * rhs[5] + lhs[4] * rhs[2] + lhs[5] * rhs[3] - lhs[7] * rhs[1])
			, lhs[0] * rhs[7] + lhs[7] * rhs[0] + (-lhs[1] * rhs[6] + lhs[2] * rhs[5] - lhs[3] * rhs[4] + lhs[4] * rhs[3] - lhs[5] * rhs[2] + lhs[6] * rhs[1]));
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Im4_1, class Im5_1, class Im6_1, class Im7_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class Im4_2, class Im5_2, class Im6_2, class Im7_2, class = std::enable_if_t<is_standard_operation_v<is_multipliable, octonion<T1>, octonion<T2>>>>
	inline auto operator*(octonion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1, Im4_1, Im5_1, Im6_1, Im7_1>, octonion_parameter<T1, Re2, Im1_2, Im2_2, Im3_2, Im4_2, Im5_2, Im6_2, Im7_2>) {
		return octonion_parameter<mul_result_t<T1, T2>
			, decltype(Re1() * Re2() - (Im1_1() * Im1_2() + Im2_1() * Im2_2() + Im3_1() * Im3_2() + Im4_1() * Im4_2() + Im5_1() * Im5_2() + Im6_1() * Im6_2() + Im7_1() * Im7_2()))
			, decltype(Re1() * Im1_2() + Im1_1() * Re2() + (Im2_1() * Im3_2() - Im3_1() * Im2_2() - Im4_1() * Im5_2() + Im5_1() * Im4_2() - Im6_1() * Im7_2() + Im7_1() * Im6_2()))
			, decltype(Re1() * Im2_2() + Im2_1() * Re2() + (-Im1_1() * Im3_2() + Im3_1() * Im1_2() - Im4_1() * Im6_2() + Im5_1() * Im7_2() + Im6_1() * Im4_2() - Im7_1() * Im5_2()))
			, decltype(Re1() * Im3_2() + Im3_1() * Re2() + (Im1_1() * Im2_2() - Im2_1() * Im1_2() - Im4_1() * Im7_2() - Im5_1() * Im6_2() + Im6_1() * Im5_2() + Im7_1() * Im4_2()))
			, decltype(Re1() * Im4_2() + Im4_1() * Re2() + (Im1_1() * Im5_2() + Im2_1() * Im6_2() + Im3_1() * Im7_2() - Im5_1() * Im1_2() - Im6_1() * Im2_2() - Im7_1() * Im3_2()))
			, decltype(Re1() * Im5_2() + Im5_1() * Re2() + (-Im1_1() * Im4_2() - Im2_1() * Im7_2() + Im3_1() * Im6_2() + Im4_1() * Im1_2() - Im6_1() * Im3_2() + Im7_1() * Im2_2()))
			, decltype(Re1() * Im6_2() + Im6_1() * Re2() + (Im1_1() * Im7_2() - Im2_1() * Im4_2() - Im3_1() * Im5_2() + Im4_1() * Im2_2() + Im5_1() * Im3_2() - Im7_1() * Im1_2()))
			, decltype(Re1() * Im7_2() + Im7_1() * Re2() + (-Im1_1() * Im6_2() + Im2_1() * Im5_2() - Im3_1() * Im4_2() + Im4_1() * Im3_2() - Im5_1() * Im2_2() + Im6_1() * Im1_2()))>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_rscalar_operation_v<is_multipliable, octonion<T1>, T2>>>
	inline constexpr auto operator*(const octonion<T1>& lhs, const T2& rhs) {
		return octonion<mul_result_t<T1, T2>>(lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs, lhs[3] * rhs, lhs[4] * rhs, lhs[5] * rhs, lhs[6] * rhs, lhs[7] * rhs);
	}
	template <class T1, class T2, class Re, class Im1, class Im2, class Im3, class Im4, class Im5, class Im6, class Im7, class Param, class = std::enable_if_t<is_rscalar_operation_v<is_multipliable, octonion<T1>, T2>>>
	inline auto operator*(octonion_parameter<T1, Re, Im1, Im2, Im3, Im4, Im5, Im6, Im7>, type_parameter<T2, Param> rhs) {
		return octonion_parameter<mul_result_t<T1, T2>, decltype(Re() * rhs), decltype(Im1() * rhs), decltype(Im2() * rhs), decltype(Im3() * rhs), decltype(Im4() * rhs), decltype(Im5() * rhs), decltype(Im6() * rhs), decltype(Im7() * rhs)>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_lscalar_operation_v<is_multipliable, T1, octonion<T2>>>>
	inline constexpr auto operator*(const T1& lhs, const octonion<T2>& rhs) {
		return octonion<mul_result_t<T1, T2>>(lhs * rhs[0], lhs * rhs[1], lhs * rhs[2], lhs * rhs[3], lhs * rhs[4], lhs * rhs[5], lhs * rhs[6], lhs * rhs[7]);
	}
	template <class T1, class T2, class Param, class Re, class Im1, class Im2, class Im3, class Im4, class Im5, class Im6, class Im7, class = std::enable_if_t<is_lscalar_operation_v<is_multipliable, T1, octonion<T2>>>>
	inline auto operator*(type_parameter<T1, Param> lhs, octonion_parameter<T2, Re, Im1, Im2, Im3, Im4, Im5, Im6, Im7>) {
		return octonion_parameter<mul_result_t<T1, T2>, decltype(lhs * Re()), decltype(lhs * Im1()), decltype(lhs * Im2()), decltype(lhs * Im3()), decltype(lhs * Im4()), decltype(lhs * Im5()), decltype(lhs * Im6()), decltype(lhs * Im7())>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_multipliable, octonion<T1>, complex<T2>>>>
	inline constexpr auto operator*(const octonion<T1>& lhs, const complex<T2>& rhs) {
		return octonion<mul_result_t<T1, T2>>(lhs[0] * rhs[0] - (lhs[1] * rhs[1])
			, lhs[0] * rhs[1] + lhs[1] * rhs[0]
			, lhs[2] * rhs[0] + (lhs[3] * rhs[1])
			, lhs[3] * rhs[0] + (-lhs[2] * rhs[1])
			, lhs[4] * rhs[0] + (-lhs[5] * rhs[1])
			, lhs[5] * rhs[0] + (lhs[4] * rhs[1])
			, lhs[6] * rhs[0] + (-lhs[7] * rhs[1])
			, lhs[7] * rhs[0] + (lhs[6] * rhs[1]));
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Im4_1, class Im5_1, class Im6_1, class Im7_1, class Re2, class Im_2, class = std::enable_if_t<is_standard_operation_v<is_multipliable, octonion<T1>, complex<T2>>>>
	inline auto operator*(octonion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1, Im4_1, Im5_1, Im6_1, Im7_1>, complex_parameter<T2, Re2, Im_2>) {
		return octonion_parameter<mul_result_t<T1, T2>
			, decltype(Re1() * Re2() - (Im1_1() * Im_2()))
			, decltype(Re1() * Im_2() + Im1_1() * Re2())
			, decltype(Im2_1() * Re2() + (Im3_1() * Im_2()))
			, decltype(Im3_1() * Re2() + (-Im2_1() * Im_2()))
			, decltype(Im4_1() * Re2() + (-Im5_1() * Im_2()))
			, decltype(Im5_1() * Re2() + (Im4_1() * Im_2()))
			, decltype(Im6_1() * Re2() + (-Im7_1() * Im_2()))
			, decltype(Im7_1() * Re2() + (Im6_1() * Im_2()))>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_multipliable, complex<T1>, octonion<T2>>>>
	inline constexpr auto operator*(const complex<T1>& lhs, const octonion<T2>& rhs) {
		return octonion<mul_result_t<T1, T2>>(lhs[0] * rhs[0] - (lhs[1] * rhs[1])
			, lhs[0] * rhs[1] + lhs[1] * rhs[0]
			, lhs[0] * rhs[2] + (-lhs[1] * rhs[3])
			, lhs[0] * rhs[3] + (lhs[1] * rhs[2])
			, lhs[0] * rhs[4] + (lhs[1] * rhs[5])
			, lhs[0] * rhs[5] + (-lhs[1] * rhs[4])
			, lhs[0] * rhs[6] + (lhs[1] * rhs[7])
			, lhs[0] * rhs[7] + (-lhs[1] * rhs[6]));
	}
	template <class T1, class T2, class Re1, class Im_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class Im4_2, class Im5_2, class Im6_2, class Im7_2, class = std::enable_if_t<is_standard_operation_v<is_multipliable, complex<T1>, octonion<T2>>>>
	inline auto operator*(complex_parameter<T1, Re1, Im_1>, octonion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2, Im4_2, Im5_2, Im6_2, Im7_2>) {
		return octonion_parameter<mul_result_t<T1, T2>
			, decltype(Re1() * Re2() - (Im_1() * Im1_2()))
			, decltype(Re1() * Im1_2() + Im_1() * Re2())
			, decltype(Re1() * Im2_2() + (-Im_1() * Im3_2()))
			, decltype(Re1() * Im3_2() + (Im_1() * Im2_2()))
			, decltype(Re1() * Im4_2() + (Im_1() * Im5_2()))
			, decltype(Re1() * Im5_2() + (-Im_1() * Im4_2()))
			, decltype(Re1() * Im6_2() + (Im_1() * Im7_2()))
			, decltype(Re1() * Im7_2() + (-Im_1() * Im6_2()))>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_multipliable, octonion<T1>, quaternion<T2>>>>
	inline constexpr auto operator*(const octonion<T1>& lhs, const quaternion<T2>& rhs) {
		return octonion<mul_result_t<T1, T2>>(lhs[0] * rhs[0] - (lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3] * rhs[3])
			, lhs[0] * rhs[1] + lhs[1] * rhs[0] + (lhs[2] * rhs[3] - lhs[3] * rhs[2])
			, lhs[0] * rhs[2] + lhs[2] * rhs[0] + (-lhs[1] * rhs[3] + lhs[3] * rhs[1])
			, lhs[0] * rhs[3] + lhs[3] * rhs[0] + (lhs[1] * rhs[2] - lhs[2] * rhs[1])
			, lhs[4] * rhs[0] + (-lhs[5] * rhs[1] - lhs[6] * rhs[2] - lhs[7] * rhs[3])
			, lhs[5] * rhs[0] + (lhs[4] * rhs[1] - lhs[6] * rhs[3] + lhs[7] * rhs[2])
			, lhs[6] * rhs[0] + (lhs[4] * rhs[2] + lhs[5] * rhs[3] - lhs[7] * rhs[1])
			, lhs[7] * rhs[0] + (lhs[4] * rhs[3] - lhs[5] * rhs[2] + lhs[6] * rhs[1]));
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Im4_1, class Im5_1, class Im6_1, class Im7_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = std::enable_if_t<is_standard_operation_v<is_multipliable, octonion<T1>, quaternion<T2>>>>
	inline auto operator*(octonion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1, Im4_1, Im5_1, Im6_1, Im7_1>, quaternion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2>) {
		return octonion_parameter<mul_result_t<T1, T2>
			, decltype(Re1() * Re2() - (Im1_1() * Im1_2() + Im2_1() * Im2_2() + Im3_1() * Im3_2()))
			, decltype(Re1() * Im1_2() + Im1_1() * Re2() + (Im2_1() * Im3_2() - Im3_1() * Im2_2()))
			, decltype(Re1() * Im2_2() + Im2_1() * Re2() + (-Im1_1() * Im3_2() + Im3_1() * Im1_2()))
			, decltype(Re1() * Im3_2() + Im3_1() * Re2() + (Im1_1() * Im2_2() - Im2_1() * Im1_2()))
			, decltype(Im4_1() * Re2() + (-Im5_1() * Im1_2() - Im6_1() * Im2_2() - Im7_1() * Im3_2()))
			, decltype(Im5_1() * Re2() + (Im4_1() * Im1_2() - Im6_1() * Im3_2() + Im7_1() * Im2_2()))
			, decltype(Im6_1() * Re2() + (Im4_1() * Im2_2() + Im5_1() * Im3_2() - Im7_1() * Im1_2()))
			, decltype(Im7_1() * Re2() + (Im4_1() * Im3_2() - Im5_1() * Im2_2() + Im6_1() * Im1_2()))>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_multipliable, quaternion<T1>, octonion<T2>>>>
	inline constexpr auto operator*(const quaternion<T1>& lhs, const octonion<T2>& rhs) {
		return octonion<mul_result_t<T1, T2>>(lhs[0] * rhs[0] - (lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3] * rhs[3])
			, lhs[0] * rhs[1] + lhs[1] * rhs[0] + (lhs[2] * rhs[3] - lhs[3] * rhs[2])
			, lhs[0] * rhs[2] + lhs[2] * rhs[0] + (-lhs[1] * rhs[3] + lhs[3] * rhs[1])
			, lhs[0] * rhs[3] + lhs[3] * rhs[0] + (lhs[1] * rhs[2] - lhs[2] * rhs[1])
			, lhs[0] * rhs[4] + (lhs[1] * rhs[5] + lhs[2] * rhs[6] + lhs[3] * rhs[7])
			, lhs[0] * rhs[5] + (-lhs[1] * rhs[4] - lhs[2] * rhs[7] + lhs[3] * rhs[6])
			, lhs[0] * rhs[6] + (lhs[1] * rhs[7] - lhs[2] * rhs[4] - lhs[3] * rhs[5])
			, lhs[0] * rhs[7] + (-lhs[1] * rhs[6] + lhs[2] * rhs[5] - lhs[3] * rhs[4]));
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class Im4_2, class Im5_2, class Im6_2, class Im7_2, class = std::enable_if_t<is_standard_operation_v<is_multipliable, quaternion<T1>, octonion<T2>>>>
	inline auto operator*(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1>, octonion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2, Im4_2, Im5_2, Im6_2, Im7_2>) {
		return octonion_parameter<mul_result_t<T1, T2>
			, decltype(Re1() * Re2() - (Im1_1() * Im1_2() + Im2_1() * Im2_2() + Im3_1() * Im3_2()))
			, decltype(Re1() * Im1_2() + Im1_1() * Re2() + (Im2_1() * Im3_2() - Im3_1() * Im2_2()))
			, decltype(Re1() * Im2_2() + Im2_1() * Re2() + (-Im1_1() * Im3_2() + Im3_1() * Im1_2()))
			, decltype(Re1() * Im3_2() + Im3_1() * Re2() + (Im1_1() * Im2_2() - Im2_1() * Im1_2()))
			, decltype(Re1() * Im4_2() + (Im1_1() * Im5_2() + Im2_1() * Im6_2() + Im3_1() * Im7_2()))
			, decltype(Re1() * Im5_2() + (-Im1_1() * Im4_2() - Im2_1() * Im7_2() + Im3_1() * Im6_2()))
			, decltype(Re1() * Im6_2() + (Im1_1() * Im7_2() - Im2_1() * Im4_2() - Im3_1() * Im5_2()))
			, decltype(Re1() * Im7_2() + (-Im1_1() * Im6_2() + Im2_1() * Im5_2() - Im3_1() * Im4_2()))>();
	}

	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_divisible, octonion<T1>, octonion<T2>>>>
	inline constexpr auto operator/(const octonion<T1>& lhs, const octonion<T2>& rhs) {
		T2 temp = rhs[0] * rhs[0] + rhs[1] * rhs[1] + rhs[2] * rhs[2] + rhs[3] * rhs[3] + rhs[4] * rhs[4] + rhs[5] * rhs[5] + rhs[6] * rhs[6] + rhs[7] * rhs[7];
		return lhs * octonion<T2>(rhs[0] / temp, -rhs[1] / temp, -rhs[2] / temp, -rhs[3] / temp, -rhs[4] / temp, -rhs[5] / temp, -rhs[6] / temp, -rhs[7] / temp);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Im4_1, class Im5_1, class Im6_1, class Im7_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class Im4_2, class Im5_2, class Im6_2, class Im7_2, class = std::enable_if_t<is_standard_operation_v<is_divisible, octonion<T1>, octonion<T2>>>>
	inline auto operator/(octonion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1, Im4_1, Im5_1, Im6_1, Im7_1> lhs, octonion_parameter<T1, Re2, Im1_2, Im2_2, Im3_2, Im4_2, Im5_2, Im6_2, Im7_2>) {
		using type = decltype(Re2() * Re2() + Im1_2() * Im1_2() + Im2_2() * Im2_2() + Im3_2() * Im3_2() + Im4_2() * Im4_2() + Im5_2() * Im5_2() + Im6_2() * Im6_2() + Im7_2() * Im7_2());
		return lhs * octonion_parameter<T2, decltype(Re2() / type()), decltype(Im1_2() / type()), decltype(Im2_2() / type()), decltype(Im3_2() / type()), decltype(Im4_2() / type()), decltype(Im5_2() / type()), decltype(Im6_2() / type()), decltype(Im7_2() / type())>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_rscalar_operation_v<is_divisible, octonion<T1>, T2>>>
	inline constexpr auto operator/(const octonion<T1>& lhs, const T2& rhs) {
		return octonion<div_result_t<T1, T2>>(lhs[0] / rhs, lhs[1] / rhs, lhs[2] / rhs, lhs[3] / rhs, lhs[4] / rhs, lhs[5] / rhs, lhs[6] / rhs, lhs[7] / rhs);
	}
	template <class T1, class T2, class Re, class Im1, class Im2, class Im3, class Im4, class Im5, class Im6, class Im7, class Param, class = std::enable_if_t<is_rscalar_operation_v<is_divisible, octonion<T1>, T2>>>
	inline auto operator/(octonion_parameter<T1, Re, Im1, Im2, Im3, Im4, Im5, Im6, Im7>, type_parameter<T2, Param> rhs) {
		return octonion_parameter<div_result_t<T1, T2>, decltype(Re() / rhs), decltype(Im1() / rhs), decltype(Im2() / rhs), decltype(Im3() / rhs), decltype(Im4() / rhs), decltype(Im5() / rhs), decltype(Im6() / rhs), decltype(Im7() / rhs)>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_lscalar_operation_v<is_divisible, T1, quaternion<T2>>>>
	inline constexpr auto operator/(const T1& lhs, const octonion<T2>& rhs) {
		T2 temp = rhs[0] * rhs[0] + rhs[1] * rhs[1] + rhs[2] * rhs[2] + rhs[3] * rhs[3] + rhs[4] * rhs[4] + rhs[5] * rhs[5] + rhs[6] * rhs[6] + rhs[7] * rhs[7];
		return lhs * octonion<T2>(rhs[0] / temp, -rhs[1] / temp, -rhs[2] / temp, -rhs[3] / temp, -rhs[4] / temp, -rhs[5] / temp, -rhs[6] / temp, -rhs[7] / temp);
	}
	template <class T1, class T2, class Param, class Re, class Im1, class Im2, class Im3, class Im4, class Im5, class Im6, class Im7, class = std::enable_if_t<is_lscalar_operation_v<is_divisible, T1, quaternion<T2>>>>
	inline auto operator/(type_parameter<T1, Param> lhs, octonion_parameter<T2, Re, Im1, Im2, Im3, Im4, Im5, Im6, Im7>) {
		using type = decltype(Re() * Re() + Im1() * Im1() + Im2() * Im2() + Im3() * Im3() + Im4() * Im4() + Im5() * Im5() + Im6() * Im6() + Im7() * Im7());
		return lhs * octonion_parameter<T2, decltype(Re() / type()), decltype(Im1() / type()), decltype(Im2() / type()), decltype(Im3() / type()), decltype(Im4() / type()), decltype(Im5() / type()), decltype(Im6() / type()), decltype(Im7() / type())>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_divisible, octonion<T1>, complex<T2>>>>
	inline constexpr auto operator/(const octonion<T1>& lhs, const complex<T2>& rhs) {
		return lhs * multiplicative_inverse(rhs);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Im4_1, class Im5_1, class Im6_1, class Im7_1, class Re2, class Im_2, class = std::enable_if_t<is_standard_operation_v<is_divisible, octonion<T1>, complex<T2>>>>
	inline auto operator/(octonion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1, Im4_1, Im5_1, Im6_1, Im7_1> lhs, complex_parameter<T2, Re2, Im_2>) {
		using type = decltype(Re2() * Re2() + Im_2() * Im_2());
		return lhs * complex_parameter<T2, decltype(Re2() / type()), decltype(Im_2() / type())>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_divisible, complex<T1>, octonion<T2>>>>
	inline constexpr auto operator/(const complex<T1>& lhs, const octonion<T2>& rhs) {
		T2 temp = rhs[0] * rhs[0] + rhs[1] * rhs[1] + rhs[2] * rhs[2] + rhs[3] * rhs[3] + rhs[4] * rhs[4] + rhs[5] * rhs[5] + rhs[6] * rhs[6] + rhs[7] * rhs[7];
		return lhs * octonion<T2>(rhs[0] / temp, -rhs[1] / temp, -rhs[2] / temp, -rhs[3] / temp, -rhs[4] / temp, -rhs[5] / temp, -rhs[6] / temp, -rhs[7] / temp);
	}
	template <class T1, class T2, class Re1, class Im_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class Im4_2, class Im5_2, class Im6_2, class Im7_2, class = std::enable_if_t<is_standard_operation_v<is_divisible, complex<T1>, octonion<T2>>>>
	inline auto operator/(complex_parameter<T1, Re1, Im_1> lhs, octonion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2, Im4_2, Im5_2, Im6_2, Im7_2>) {
		using type = decltype(Re2() * Re2() + Im1_2() * Im1_2() + Im2_2() * Im2_2() + Im3_2() * Im3_2() + Im4_2() * Im4_2() + Im5_2() * Im5_2() + Im6_2() * Im6_2() + Im7_2() * Im7_2());
		return lhs * octonion_parameter<T2, decltype(Re2() / type()), decltype(Im1_2() / type()), decltype(Im2_2() / type()), decltype(Im3_2() / type()), decltype(Im4_2() / type()), decltype(Im5_2() / type()), decltype(Im6_2() / type()), decltype(Im7_2() / type())>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_divisible, octonion<T1>, quaternion<T2>>>>
	inline constexpr auto operator/(const octonion<T1>& lhs, const quaternion<T2>& rhs) {
		return lhs * multiplicative_inverse(rhs);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Im4_1, class Im5_1, class Im6_1, class Im7_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = std::enable_if_t<is_standard_operation_v<is_divisible, octonion<T1>, quaternion<T2>>>>
	inline auto operator/(octonion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1, Im4_1, Im5_1, Im6_1, Im7_1> lhs, quaternion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2>) {
		using type = decltype(Re2() * Re2() + Im1_2() * Im1_2() + Im2_2() * Im2_2() + Im3_2() * Im3_2());
		return lhs * quaternion_parameter<T2, decltype(Re2() / type()), decltype(Im1_2() / type()), decltype(Im2_2() / type()), decltype(Im3_2() / type())>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_divisible, quaternion<T1>, octonion<T2>>>>
	inline constexpr auto operator/(const quaternion<T1>& lhs, const octonion<T2>& rhs) {
		T2 temp = rhs[0] * rhs[0] + rhs[1] * rhs[1] + rhs[2] * rhs[2] + rhs[3] * rhs[3] + rhs[4] * rhs[4] + rhs[5] * rhs[5] + rhs[6] * rhs[6] + rhs[7] * rhs[7];
		return lhs * octonion<T2>(rhs[0] / temp, -rhs[1] / temp, -rhs[2] / temp, -rhs[3] / temp, -rhs[4] / temp, -rhs[5] / temp, -rhs[6] / temp, -rhs[7] / temp);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class Im4_2, class Im5_2, class Im6_2, class Im7_2, class = std::enable_if_t<is_standard_operation_v<is_divisible, quaternion<T1>, octonion<T2>>>>
	inline auto operator/(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1> lhs, octonion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2, Im4_2, Im5_2, Im6_2, Im7_2>) {
		using type = decltype(Re2() * Re2() + Im1_2() * Im1_2() + Im2_2() * Im2_2() + Im3_2() * Im3_2() + Im4_2() * Im4_2() + Im5_2() * Im5_2() + Im6_2() * Im6_2() + Im7_2() * Im7_2());
		return lhs * octonion_parameter<T2, decltype(Re2() / type()), decltype(Im1_2() / type()), decltype(Im2_2() / type()), decltype(Im3_2() / type()), decltype(Im4_2() / type()), decltype(Im5_2() / type()), decltype(Im6_2() / type()), decltype(Im7_2() / type())>();
	}


	//比較演算
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_comparable, octonion<T1>, octonion<T2>>>>
	inline constexpr bool operator==(const octonion<T1>& lhs, const octonion<T2>& rhs) {
		return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1]) && (lhs[2] == rhs[2]) && (lhs[3] == rhs[3])
			&& (lhs[4] == rhs[4]) && (lhs[5] == rhs[5]) && (lhs[6] == rhs[6]) && (lhs[7] == rhs[7]);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Im4_1, class Im5_1, class Im6_1, class Im7_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class Im4_2, class Im5_2, class Im6_2, class Im7_2, class = std::enable_if_t<is_standard_operation_v<is_comparable, octonion<T1>, octonion<T2>>>>
	inline auto operator==(octonion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1, Im4_1, Im5_1, Im6_1, Im7_1>, octonion_parameter<T1, Re2, Im1_2, Im2_2, Im3_2, Im4_2, Im5_2, Im6_2, Im7_2>) {
		return (Re1() == Re2()) && (Im1_1() == Im1_2()) && (Im2_1() == Im2_2()) && (Im3_1() == Im3_2())
			&& (Im4_1() == Im4_2()) && (Im5_1() == Im5_2()) && (Im6_1() == Im6_2()) && (Im7_1() == Im7_2());
	}
	template <class T1, class T2, class = std::enable_if_t<is_rscalar_operation_v<is_comparable, octonion<T1>, T2>>>
	inline constexpr bool operator==(const octonion<T1>& lhs, T2& rhs) {
		return (lhs[0] == rhs) && is_absorbing_element(lhs[1]) && is_absorbing_element(lhs[2]) && is_absorbing_element(lhs[3])
			&& is_absorbing_element(lhs[4]) && is_absorbing_element(lhs[5]) && is_absorbing_element(lhs[6]) && is_absorbing_element(lhs[7]);
	}
	template <class T1, class T2, class Re, class Im1, class Im2, class Im3, class Im4, class Im5, class Im6, class Im7, class Param, class = std::enable_if_t<is_rscalar_operation_v<is_comparable, octonion<T1>, T2>>>
	inline auto operator==(octonion_parameter<T1, Re, Im1, Im2, Im3, Im4, Im5, Im6, Im7>, type_parameter<T2, Param> rhs) {
		return (Re() == rhs) && int_parameter<bool, is_absorbing_element(Im1::value)>() && int_parameter<bool, is_absorbing_element(Im2::value)>() && int_parameter<bool, is_absorbing_element(Im3::value)>()
			&& int_parameter<bool, is_absorbing_element(Im4::value)>() && int_parameter<bool, is_absorbing_element(Im5::value)>() && int_parameter<bool, is_absorbing_element(Im6::value)>() && int_parameter<bool, is_absorbing_element(Im7::value)>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_lscalar_operation_v<is_comparable, T1, octonion<T2>>>>
	inline constexpr bool operator==(const T1& lhs, const octonion<T2>& rhs) {
		return (lhs[0] == rhs) && is_absorbing_element(lhs[1]) && is_absorbing_element(rhs[2]) && is_absorbing_element(rhs[3])
			&& is_absorbing_element(rhs[4]) && is_absorbing_element(rhs[5]) && is_absorbing_element(rhs[6]) && is_absorbing_element(rhs[7]);
	}
	template <class T1, class T2, class Param, class Re, class Im1, class Im2, class Im3, class Im4, class Im5, class Im6, class Im7, class = std::enable_if_t<is_lscalar_operation_v<is_comparable, T1, octonion<T2>>>>
	inline auto operator==(type_parameter<T1, Param> lhs, octonion_parameter<T2, Re, Im1, Im2, Im3, Im4, Im5, Im6, Im7>) {
		return (lhs == Re()) && int_parameter<bool, is_absorbing_element(Im1::value)>() && int_parameter<bool, is_absorbing_element(Im2::value)>() && int_parameter<bool, is_absorbing_element(Im3::value)>()
			&& int_parameter<bool, is_absorbing_element(Im4::value)>() && int_parameter<bool, is_absorbing_element(Im5::value)>() && int_parameter<bool, is_absorbing_element(Im6::value)>() && int_parameter<bool, is_absorbing_element(Im7::value)>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_comparable, octonion<T1>, complex<T2>>>>
	inline constexpr bool operator==(const octonion<T1>& lhs, complex<T2>& rhs) {
		return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1]) && is_absorbing_element(lhs[2]) && is_absorbing_element(lhs[3])
			&& is_absorbing_element(lhs[4]) && is_absorbing_element(lhs[5]) && is_absorbing_element(lhs[6]) && is_absorbing_element(lhs[7]);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Im4_1, class Im5_1, class Im6_1, class Im7_1, class Re2, class Im_2, class = std::enable_if_t<is_standard_operation_v<is_comparable, octonion<T1>, complex<T2>>>>
	inline auto operator==(octonion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1, Im4_1, Im5_1, Im6_1, Im7_1>, complex_parameter<T2, Re2, Im_2>) {
		return (Re1() == Re2()) && (Im1_1() == Im_2()) && int_parameter<bool, is_absorbing_element(Im2_1::value)>() && int_parameter<bool, is_absorbing_element(Im3_1::value)>()
			&& int_parameter<bool, is_absorbing_element(Im4_1::value)>() && int_parameter<bool, is_absorbing_element(Im5_1::value)>() && int_parameter<bool, is_absorbing_element(Im6_1::value)>() && int_parameter<bool, is_absorbing_element(Im7_1::value)>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_comparable, complex<T1>, octonion<T2>>>>
	inline constexpr bool operator==(const complex<T1>& lhs, const octonion<T2>& rhs) {
		return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1]) && is_absorbing_element(rhs[2]) && is_absorbing_element(rhs[3])
			&& is_absorbing_element(rhs[4]) && is_absorbing_element(rhs[5]) && is_absorbing_element(rhs[6]) && is_absorbing_element(rhs[7]);
	}
	template <class T1, class T2, class Re1, class Im_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class Im4_2, class Im5_2, class Im6_2, class Im7_2, class = std::enable_if_t<is_standard_operation_v<is_comparable, complex<T1>, octonion<T2>>>>
	inline auto operator==(complex_parameter<T1, Re1, Im_1>, octonion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2, Im4_2, Im5_2, Im6_2, Im7_2>) {
		return (Re1() == Re2()) && (Im_1() == Im1_2()) && int_parameter<bool, is_absorbing_element(Im2_2::value)>() && int_parameter<bool, is_absorbing_element(Im3_2::value)>()
			&& int_parameter<bool, is_absorbing_element(Im4_2::value)>() && int_parameter<bool, is_absorbing_element(Im5_2::value)>() && int_parameter<bool, is_absorbing_element(Im6_2::value)>() && int_parameter<bool, is_absorbing_element(Im7_2::value)>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_comparable, octonion<T1>, quaternion<T2>>>>
	inline constexpr bool operator==(const octonion<T1>& lhs, quaternion<T2>& rhs) {
		return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1]) && (lhs[2] == rhs[2]) && (lhs[3] == rhs[3])
			&& is_absorbing_element(lhs[4]) && is_absorbing_element(lhs[5]) && is_absorbing_element(lhs[6]) && is_absorbing_element(lhs[7]);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Im4_1, class Im5_1, class Im6_1, class Im7_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = std::enable_if_t<is_standard_operation_v<is_comparable, octonion<T1>, quaternion<T2>>>>
	inline auto operator==(octonion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1, Im4_1, Im5_1, Im6_1, Im7_1>, quaternion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2>) {
		return (Re1() == Re2()) && (Im1_1() == Im1_2()) && (Im2_1() == Im2_2()) && (Im3_1() == Im3_2())
			&& int_parameter<bool, is_absorbing_element(Im4_1::value)>() && int_parameter<bool, is_absorbing_element(Im5_1::value)>() && int_parameter<bool, is_absorbing_element(Im6_1::value)>() && int_parameter<bool, is_absorbing_element(Im7_1::value)>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_comparable, quaternion<T1>, octonion<T2>>>>
	inline constexpr bool operator==(const quaternion<T1>& lhs, const octonion<T2>& rhs) {
		return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1]) && (lhs[2] == rhs[2]) && (lhs[3] == rhs[3])
			&& is_absorbing_element(rhs[4]) && is_absorbing_element(rhs[5]) && is_absorbing_element(rhs[6]) && is_absorbing_element(rhs[7]);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class Im4_2, class Im5_2, class Im6_2, class Im7_2, class = std::enable_if_t<is_standard_operation_v<is_comparable, quaternion<T1>, octonion<T2>>>>
	inline auto operator==(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1>, octonion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2, Im4_2, Im5_2, Im6_2, Im7_2>) {
		return (Re1() == Re2()) && (Im1_1() == Im1_2()) && (Im2_1() == Im2_2()) && (Im3_1() == Im3_2())
			&& int_parameter<bool, is_absorbing_element(Im4_2::value)>() && int_parameter<bool, is_absorbing_element(Im5_2::value)>() && int_parameter<bool, is_absorbing_element(Im6_2::value)>() && int_parameter<bool, is_absorbing_element(Im7_2::value)>();
	}
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_comparable, octonion<T1>, octonion<T2>>>>
	inline constexpr bool operator!=(const octonion<T1>& lhs, const octonion<T2>& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Im4_1, class Im5_1, class Im6_1, class Im7_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class Im4_2, class Im5_2, class Im6_2, class Im7_2, class = std::enable_if_t<is_standard_operation_v<is_comparable, octonion<T1>, octonion<T2>>>>
	inline auto operator!=(octonion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1, Im4_1, Im5_1, Im6_1, Im7_1> lhs, octonion_parameter<T1, Re2, Im1_2, Im2_2, Im3_2, Im4_2, Im5_2, Im6_2, Im7_2> rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class = std::enable_if_t<is_rscalar_operation_v<is_comparable, octonion<T1>, T2>>>
	inline constexpr bool operator!=(const octonion<T1>& lhs, T2& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class Re, class Im1, class Im2, class Im3, class Im4, class Im5, class Im6, class Im7, class Param, class = std::enable_if_t<is_rscalar_operation_v<is_comparable, octonion<T1>, T2>>>
	inline auto operator!=(octonion_parameter<T1, Re, Im1, Im2, Im3, Im4, Im5, Im6, Im7> lhs, type_parameter<T2, Param> rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class = std::enable_if_t<is_lscalar_operation_v<is_comparable, T1, octonion<T2>>>>
	inline constexpr bool operator!=(const T1& lhs, const octonion<T2>& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class Param, class Re, class Im1, class Im2, class Im3, class Im4, class Im5, class Im6, class Im7, class = std::enable_if_t<is_lscalar_operation_v<is_comparable, T1, octonion<T2>>>>
	inline auto operator!=(type_parameter<T1, Param> lhs, octonion_parameter<T2, Re, Im1, Im2, Im3, Im4, Im5, Im6, Im7> rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_comparable, octonion<T1>, complex<T2>>>>
	inline constexpr bool operator!=(const octonion<T1>& lhs, complex<T2>& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Im4_1, class Im5_1, class Im6_1, class Im7_1, class Re2, class Im_2, class = std::enable_if_t<is_standard_operation_v<is_comparable, octonion<T1>, complex<T2>>>>
	inline auto operator!=(octonion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1, Im4_1, Im5_1, Im6_1, Im7_1> lhs, complex_parameter<T2, Re2, Im_2> rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_comparable, complex<T1>, octonion<T2>>>>
	inline constexpr bool operator!=(const complex<T1>& lhs, const octonion<T2>& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class Re1, class Im_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class Im4_2, class Im5_2, class Im6_2, class Im7_2, class = std::enable_if_t<is_standard_operation_v<is_comparable, complex<T1>, octonion<T2>>>>
	inline auto operator!=(complex_parameter<T1, Re1, Im_1> lhs, octonion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2, Im4_2, Im5_2, Im6_2, Im7_2> rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_comparable, octonion<T1>, quaternion<T2>>>>
	inline constexpr bool operator!=(const octonion<T1>& lhs, quaternion<T2>& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Im4_1, class Im5_1, class Im6_1, class Im7_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = std::enable_if_t<is_standard_operation_v<is_comparable, octonion<T1>, quaternion<T2>>>>
	inline auto operator!=(octonion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1, Im4_1, Im5_1, Im6_1, Im7_1> lhs, quaternion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2> rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class = std::enable_if_t<is_standard_operation_v<is_comparable, quaternion<T1>, octonion<T2>>>>
	inline constexpr bool operator!=(const quaternion<T1>& lhs, const octonion<T2>& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class Im4_2, class Im5_2, class Im6_2, class Im7_2, class = std::enable_if_t<is_standard_operation_v<is_comparable, quaternion<T1>, octonion<T2>>>>
	inline auto operator!=(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1> lhs, octonion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2, Im4_2, Im5_2, Im6_2, Im7_2> rhs) { return !(lhs == rhs); }


	//八元数の判定
	template <class T>
	struct is_octonion_impl : std::false_type {};
	template <class T>
	struct is_octonion_impl<octonion<T>> : std::true_type {};
	template <class T>
	struct is_octonion : is_octonion_impl<std::remove_cv_t<T>> {};
	template <class T>
	inline constexpr bool is_octonion_v = is_octonion<T>::value;

	//八元数の除去
	template <class T>
	struct remove_octonion {
		using type = T;
	};
	template <class T>
	struct remove_octonion<octonion<T>> {
		using type = T;
	};
	template <class T>
	using remove_octonion_t = typename remove_octonion<T>::type;


	template <class From, class To>
	struct is_high_rank_math_type_octonion : is_high_rank_math_type<From, typename To::basis_type> {};
	template <class From, class To>
	struct is_high_rank_math_type_octonion<complex<From>, To> : std::bool_constant<
		//complex<From>がToの基底となる場合も含めて判定
		is_high_rank_math_type_v<complex<From>, typename To::basis_type> || is_high_rank_math_type_v<From, typename To::basis_type>
	> {};
	template <class From, class To>
	struct is_high_rank_math_type_octonion<quaternion<From>, To> : std::bool_constant<
		//quaternion<From>がToの基底となる場合も含めて判定
		is_high_rank_math_type_v<quaternion<From>, typename To::basis_type> || is_high_rank_math_type_v<From, typename To::basis_type>
	> {};
	template <class From, class To>
	struct is_high_rank_math_type_octonion<octonion<From>, To> : std::bool_constant<
		//octonion<From>がToの基底となる場合も含めて判定
		is_high_rank_math_type_v<octonion<From>, typename To::basis_type> || is_high_rank_math_type_v<From, typename To::basis_type>
	> {};
	//Fromの型によりis_high_rank_math_type_octonionで分岐
	template <class From, class To>
	struct is_high_rank_math_type<From, octonion<To>> : is_high_rank_math_type_octonion<From, octonion<To>> {};


	//加法パラメータ取得
	template <class T>
	struct addition_traits<octonion<T>> {
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
	struct multiplication_traits<octonion<T>> {
		//単位元
		template <class = std::enable_if_t<is_exist_multiplicative_identity_v<T>>>
		static constexpr auto identity_element() { return addition_traits<T>::identity_element(); }
		//吸収元
		template <class = std::enable_if_t<is_exist_absorbing_element_v<T>>>
		static constexpr T absorbing_element() { return T(); }
		//結合律
		static constexpr bool associative_value = false;
		//消約律
		static constexpr bool cancellative_value = multiplication_traits<T>::cancellative_value;
		//可換律
		static constexpr bool commutative_value = false;
		//分配律
		static constexpr bool distributive_value = multiplication_traits<T>::distributive_value;
	};


	//逆元が存在するならば逆元の取得(存在しない場合は例外を出す)
	template <class T>
	struct Inverse_element<octonion<T>> {
		static constexpr octonion<T> _additive_inverse_(const octonion<T>& o) {
			return octonion<T>(additive_inverse(o[0]), additive_inverse(o[1]), additive_inverse(o[2]), additive_inverse(o[3]), additive_inverse(o[4]), additive_inverse(o[5]), additive_inverse(o[6]), additive_inverse(o[7]));
		}

		//Tに乗法逆元が存在する場合の乗法逆元
		static constexpr octonion<T> _multiplicative_inverse_impl_(const octonion<T>& o, std::true_type) {
			//共役を絶対値の2乗で割る
			T temp = o[0] * o[0] + o[1] * o[1] + o[2] * o[2] + o[3] * o[3] + o[4] * o[4] + o[5] * o[5] + o[6] * o[6] + o[7] * o[7];
			return octonion<T>(o[0] / temp, -o[1] / temp, -o[2] / temp, -o[3] / temp, -o[4] / temp, -o[5] / temp, -o[6] / temp, -o[7] / temp);
		}
		//Tの乗法逆元が存在しない場合の乗法逆元
		static constexpr octonion<T> _multiplicative_inverse_impl_(const octonion<T>& o, std::false_type) {
			T temp = multiplicative_inverse(o[0] * o[0] + o[1] * o[1] + o[2] * o[2] + o[3] * o[3] + o[4] * o[4] + o[5] * o[5] + o[6] * o[6] + o[7] * o[7]);
			return octonion<T>(o[0] * temp, -o[1] * temp, -o[2] * temp, -o[3] * temp, -o[4] * temp, -o[5] * temp, -o[6] * temp, -o[7] * temp);
		}
		static constexpr octonion<T> _multiplicative_inverse_(const octonion<T>& o) {
			return _multiplicative_inverse_impl_(o, std::bool_constant<is_exist_multiplicative_inverse_v<T>>());
		}
	};


	//誤差評価
	template <class T>
	struct Error_evaluation<octonion<T>> {
		static constexpr octonion<T> epsilon() { return octonion<T>(Error_evaluation<T>::epsilon(), Error_evaluation<T>::epsilon()); }
		static constexpr bool _error_evaluation_(const octonion<T>& x1, const octonion<T>& x2) {
			return error_evaluation(x1[0], x2[0]) && error_evaluation(x1[1], x2[1]) && error_evaluation(x1[2], x2[2]) && error_evaluation(x1[3], x2[3]) && error_evaluation(x1[4], x2[4]) && error_evaluation(x1[5], x2[5]) && error_evaluation(x1[6], x2[6]) && error_evaluation(x1[7], x2[7]);
		}
	};
}

#endif