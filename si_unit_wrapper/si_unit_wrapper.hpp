#ifndef IMATH_PHYSICS_SI_UNIT_WRAPPER_HPP
#define IMATH_PHYSICS_SI_UNIT_WRAPPER_HPP


#include "IMathLib/IMathLib_config.hpp"
#include "IMathLib/utility/type_traits.hpp"
#include "IMathLib/utility/utility/tuple_type.hpp"
#include "IMathLib/math/math/math_traits.hpp"

//SI単位系のの次元解析のためのラッパー


namespace iml {
	namespace phy {

		//共通の単位
		template <imint_t>
		struct si_unit_tag {};
		//長さ
		using meter_tag = si_unit_tag<1>;
		using rmeter_tag = si_unit_tag<-1>;
		//質量
		using kilogram_tag = si_unit_tag<2>;
		using rkilogram_tag = si_unit_tag<-2>;
		//時間
		using second_tag = si_unit_tag<3>;
		using rsecond_tag = si_unit_tag<-3>;
		//電流
		using ampere_tag = si_unit_tag<4>;
		using rampere_tag = si_unit_tag<-4>;
		//熱力学温度
		using kelvin_tag = si_unit_tag<5>;
		using rkelvin_tag = si_unit_tag<-5>;
		//物質量
		using mole_tag = si_unit_tag<6>;
		using rmole_tag = si_unit_tag<-6>;
		//光度
		using candela_tag = si_unit_tag<6>;
		using rcandela_tag = si_unit_tag<-6>;


		//型がSI単位系であるか
		template <class T>
		struct is_si_unit_tag : false_type {};
		template <>
		struct is_si_unit_tag<si_unit_tag<0>> : false_type {};
		template <imint_t N>
		struct is_si_unit_tag<si_unit_tag<N>> : true_type {};
		//組み立てによるSI単位か(深層に対しても再帰的に走査)
		template <class, bool>
		struct is_si_unit_tag_impl;
		template <bool f>
		struct is_si_unit_tag_impl<type_tuple<>, f> : cat_bool<f> {};
		template <class First, class... Types, bool f>
		struct is_si_unit_tag_impl<type_tuple<First, Types...>, f>
			: is_si_unit_tag_impl<type_tuple<Types...>, is_si_unit_tag<First>::value && f> {};
		template <class... Types>
		struct is_si_unit_tag<type_tuple<Types...>> : is_si_unit_tag_impl<type_tuple<Types...>, sizeof...(Types) != 0> {};


		//任意のSI単位系の次元の取得
		template <imint_t, imint_t, class>
		struct get_si_unit_dimension_impl;
		template <imint_t M, imint_t N, class First, class... Types>
		struct get_si_unit_dimension_impl<M, N, type_tuple<First, Types...>>
			: get_si_unit_dimension_impl<M, N + is_same<si_unit_tag<M>, First>::value - is_same<si_unit_tag<-M>, First>::value, type_tuple<Types...>> {};
		template <imint_t M, imint_t N>
		struct get_si_unit_dimension_impl<M, N, type_tuple<>> {
			static constexpr imint_t value = N;
		};
		template <imint_t N, class TypeTuple>
		struct get_si_unit_dimension : get_si_unit_dimension_impl<N, 0, typename unit_type_tuple<TypeTuple>::type> {};


		//SI単位の次元を示すための型
		template <imint_t N1, imint_t N2, imint_t N3, imint_t N4, imint_t N5, imint_t N6>
		struct si_unit {};
		//SI単位の構築の補助
		template <class TypeTuple, class = enable_if<is_si_unit_tag<TypeTuple>::value>::type>
		using si_unit_supporter = si_unit<
			get_si_unit_dimension<1, TypeTuple>::value
			, get_si_unit_dimension<2, TypeTuple>::value
			, get_si_unit_dimension<3, TypeTuple>::value
			, get_si_unit_dimension<4, TypeTuple>::value
			, get_si_unit_dimension<5, TypeTuple>::value
			, get_si_unit_dimension<6, TypeTuple>::value>;


		//si_unit同士の2項演算(si名前空間のリテラルのため)
		template <imint_t M1, imint_t M2, imint_t M3, imint_t M4, imint_t M5, imint_t M6, imint_t N1, imint_t N2, imint_t N3, imint_t N4, imint_t N5, imint_t N6>
		inline si_unit<M1 + N1, M2 + N2, M3 + N3, M4 + N4, M5 + N5, M6 + N6> operator*(const si_unit<M1, M2, M3, M4, M5, M6>& t1, const si_unit<N1, N2, N3, N4, N5, N6>& t2) {
			return si_unit<M1 + N1, M2 + N2, M3 + N3, M4 + N4, M5 + N5, M6 + N6>();
		}
		template <imint_t M1, imint_t M2, imint_t M3, imint_t M4, imint_t M5, imint_t M6, imint_t N1, imint_t N2, imint_t N3, imint_t N4, imint_t N5, imint_t N6>
		inline si_unit<M1 - N1, M2 - N2, M3 - N3, M4 - N4, M5 - N5, M6 - N6> operator/(const si_unit<M1, M2, M3, M4, M5, M6>& t1, const si_unit<N1, N2, N3, N4, N5, N6>& t2) {
			return si_unit<M1 - N1, M2 - N2, M3 - N3, M4 - N4, M5 - N5, M6 - N6>();
		}


		//SI単位であるか判定
		template <class>
		struct is_si_unit : false_type {};
		template <imint_t N1, imint_t N2, imint_t N3, imint_t N4, imint_t N5, imint_t N6>
		struct is_si_unit<si_unit<N1, N2, N3, N4, N5, N6>> : true_type {};


		//2つのSI単位系の次元が一致する(si_unit_tag ver)
		template <class TypeTuple1, class TypeTuple2>
		struct is_same_dimension : cat_bool<
			(is_si_unit_tag<TypeTuple1>::value && is_si_unit_tag<TypeTuple2>::value)
			&& (get_si_unit_dimension<1, TypeTuple1>::value == get_si_unit_dimension<1, TypeTuple2>::value)
			&& (get_si_unit_dimension<2, TypeTuple1>::value == get_si_unit_dimension<2, TypeTuple2>::value)
			&& (get_si_unit_dimension<3, TypeTuple1>::value == get_si_unit_dimension<3, TypeTuple2>::value)
			&& (get_si_unit_dimension<4, TypeTuple1>::value == get_si_unit_dimension<4, TypeTuple2>::value)
			&& (get_si_unit_dimension<5, TypeTuple1>::value == get_si_unit_dimension<5, TypeTuple2>::value)
			&& (get_si_unit_dimension<6, TypeTuple1>::value == get_si_unit_dimension<6, TypeTuple2>::value)
		> {};
		//si_unit ver
		template <imint_t M1, imint_t M2, imint_t M3, imint_t M4, imint_t M5, imint_t M6, imint_t N1, imint_t N2, imint_t N3, imint_t N4, imint_t N5, imint_t N6>
		struct is_same_dimension<si_unit<M1, M2, M3, M4, M5, M6>, si_unit<N1, N2, N3, N4, N5, N6>> : false_type {};
		template <imint_t N1, imint_t N2, imint_t N3, imint_t N4, imint_t N5, imint_t N6>
		struct is_same_dimension<si_unit<N1, N2, N3, N4, N5, N6>, si_unit<N1, N2, N3, N4, N5, N6>> : true_type {};


		//無次元量の判定(si_unit_tag ver)
		template <class TypeTuple>
		struct is_dimensionless : is_same_dimension<TypeTuple, type_tuple<>> {};
		//si_unit ver
		template <imint_t N1, imint_t N2, imint_t N3, imint_t N4, imint_t N5, imint_t N6>
		struct is_dimensionless<si_unit<N1, N2, N3, N4, N5, N6>> : is_same_dimension<si_unit<N1, N2, N3, N4, N5, N6>, si_unit<0, 0, 0, 0, 0, 0>> {};


		//任意のSI単位系の次元の取得(si_unit ver)
		template <imint_t M, imint_t N1, imint_t N2, imint_t N3, imint_t N4, imint_t N5, imint_t N6>
		struct get_si_unit_dimension<M, si_unit<N1, N2, N3, N4, N5, N6>> {
			static constexpr imint_t value = at_index<M - 1, N1, N2, N3, N4, N5, N6>::value;
		};

	}


	//任意の2つの型の演算可能テスト(si_unit ver)
	template <imint_t M1, imint_t M2, imint_t M3, imint_t M4, imint_t M5, imint_t M6, imint_t N1, imint_t N2, imint_t N3, imint_t N4, imint_t N5, imint_t N6>
	struct is_calcable<phy::si_unit<M1, M2, M3, M4, M5, M6>, phy::si_unit<N1, N2, N3, N4, N5, N6>> {
		using type1 = phy::is_same_dimension<phy::si_unit<M1, M2, M3, M4, M5, M6>, phy::si_unit<N1, N2, N3, N4, N5, N6>>;
		using type2 = phy::is_dimensionless<phy::si_unit<N1, N2, N3, N4, N5, N6>>;

		static constexpr bool add_value = type1::value;
		static constexpr bool sub_value = type1::value;
		static constexpr bool mul_value = true;
		static constexpr bool div_value = true;
		static constexpr bool add_eq_value = type1::value;
		static constexpr bool sub_eq_value = type1::value;
		static constexpr bool mul_eq_value = type2::value;
		static constexpr bool div_eq_value = type2::value;
		static constexpr bool eq_value = type1::value;
		static constexpr bool lt_value = type1::value;
		static constexpr bool leq_value = type1::value;
		static constexpr bool gt_value = type1::value;
		static constexpr bool geq_value = type1::value;
	};


	//任意の2つの型の演算テスト(si_unit ver)
	template <imint_t M1, imint_t M2, imint_t M3, imint_t M4, imint_t M5, imint_t M6, imint_t N1, imint_t N2, imint_t N3, imint_t N4, imint_t N5, imint_t N6>
	struct calculation_result<phy::si_unit<M1, M2, M3, M4, M5, M6>, phy::si_unit<N1, N2, N3, N4, N5, N6>> {
		using type = is_calcable<phy::si_unit<M1, M2, M3, M4, M5, M6>, phy::si_unit<N1, N2, N3, N4, N5, N6>>;

		using add_type = typename conditional<type::add_value, phy::si_unit<M1, M2, M3, M4, M5, M6>, void>::type;
		using sub_type = typename conditional<type::sub_value, phy::si_unit<M1, M2, M3, M4, M5, M6>, void>::type;
		using mul_type = typename conditional<type::mul_value, phy::si_unit<M1 + N1, M2 + N2, M3 + N3, M4 + N4, M5 + N5, M6 + N6>, void>::type;
		using div_type = typename conditional<type::div_value, phy::si_unit<M1 - N1, M2 - N2, M3 - N3, M4 - N4, M5 - N5, M6 - N6>, void>::type;
		using add_eq_type = typename conditional<type::add_eq_value, phy::si_unit<M1, M2, M3, M4, M5, M6>, void>::type;
		using sub_eq_type = typename conditional<type::sub_eq_value, phy::si_unit<M1, M2, M3, M4, M5, M6>, void>::type;
		using mul_eq_type = typename conditional<type::mul_eq_value, phy::si_unit<M1, M2, M3, M4, M5, M6>, void>::type;
		using div_eq_type = typename conditional<type::div_eq_value, phy::si_unit<M1, M2, M3, M4, M5, M6>, void>::type;
	};


	namespace phy {

		//SI単位系のラッパー本体
		template <class, class>
		class si_unit_wrapper {};
		template <class T, imint_t N1, imint_t N2, imint_t N3, imint_t N4, imint_t N5, imint_t N6>
		class si_unit_wrapper<T, si_unit<N1, N2, N3, N4, N5, N6>> {
			template <class, class>
			friend class si_unit_wrapper;

			T x_m;				//ホールドしてる変数(tupleのvalue_holderについても要検討)


			constexpr si_unit_wrapper(const T& x) : x_m(x) {}
			template <class S, class SSIUnit, class = typename enable_if<is_inclusion<S, T>::value>::type>
			constexpr si_unit_wrapper(const S& x) : x_m(x) {}
		public:
			constexpr si_unit_wrapper() : x_m() {}
			constexpr si_unit_wrapper(const si_unit_wrapper& x) : x_m(x.x_m) {}
			template <class S>
			constexpr si_unit_wrapper(const si_unit_wrapper<S, si_unit<N1, N2, N3, N4, N5, N6>>& x) : x_m(x.x_m) {}

			using type = T;
			using si_unit_type = si_unit<N1, N2, N3, N4, N5, N6>;

			//単項演算
			template <class = typename enable_if<is_exist_add_inverse_element<T>::value>::type>
			si_unit_wrapper operator-() const { return si_unit_wrapper(-this->x_m); }
			si_unit_wrapper operator+() const { return *this; }
			//代入演算
			template <class S, class = typename enable_if<is_calcable<T, S>::add_eq_value>::type>
			si_unit_wrapper& operator+=(const si_unit_wrapper<S, si_unit_type>& x) { return si_unit_wrapper(this->x_m += x.x_m); }
			template <class S, class = typename enable_if<is_calcable<T, S>::sub_eq_value>::type>
			si_unit_wrapper& operator-=(const si_unit_wrapper<S, si_unit_type>& x) { return si_unit_wrapper(this->x_m -= x.x_m); }
			template <class S, class SSIUnit, class = typename enable_if<is_calcable<T, S>::mul_eq_value && is_dimensionless<SSIUnit>::value>::type>
			si_unit_wrapper& operator*=(const si_unit_wrapper<S, SSIUnit>& x) { return si_unit_wrapper(this->x_m *= x.x_m); }
			template <class S, class = typename enable_if<is_calcable<T, S>::mul_eq_value>::type>
			si_unit_wrapper& operator*=(const S& x) { return si_unit_wrapper(this->x_m *= x); }
			template <class S, class SSIUnit, class = typename enable_if<is_calcable<T, S>::div_eq_value && is_dimensionless<SSIUnit>::value>::type>
			si_unit_wrapper& operator/=(const si_unit_wrapper<S, SSIUnit>& x) { return si_unit_wrapper(this->x_m /= x.x_m); }
			template <class S, class = typename enable_if<is_calcable<T, S>::div_eq_value>::type>
			si_unit_wrapper& operator/=(const S& x) { return si_unit_wrapper(this->x_m /= x); }

			si_unit_wrapper& operator=(const si_unit_wrapper& x) { return si_unit_wrapper(this->x_m = x.x_m); }
			template <class S, class SSIUnit, class = typename enable_if<is_calcable<T, S>::eq_value && is_same_dimension<si_unit_type, SSIUnit>::value>::type>
			si_unit_wrapper& operator=(const si_unit_wrapper<S, SSIUnit>& x) { return si_unit_wrapper(this->x_m = x.x_m); }


			operator T&() const { return x_m; }
			constexpr T& get() const { return x_m; }
			T& get() { return x_m; }
			//オブジェクトの呼び出し
			template<class... Types>
			auto operator()(Types&&... args) const { return invoke(get(), forward<Types>(args)...); }


			//添え字による要素アクセス(型はimsize_tによる添え字による要素アクセスのみ)
			template <class = typename enable_if<is_subscript_access<T, imsize_t>::value>::type>
			const auto& operator[](imsize_t index) const {
				return si_unit_wrapper<typename subscript_access_result<T, imsize_t>::type, si_unit_type>(this->x_m[index]);
			}
			auto& operator[](imsize_t index) {
				return si_unit_wrapper<typename subscript_access_result<T, imsize_t>::type, si_unit_type>(this->x_m[index]);
			}

		};


		//任意の型がsi_unit_wrapperされているかの判定
		template <class>
		struct is_si_unit_wrapper : false_type {};
		template <class T, class SIUnit>
		struct is_si_unit_wrapper<si_unit_wrapper<T, SIUnit>> : true_type {};


		template<class T>
		struct si_unit_unwrapper_impl {
			using type = T;
		};
		template<class T, class SIUnit>
		struct si_unit_unwrapper_impl<si_unit_wrapper<T, SIUnit>> {
			using type = T;
		};
		//si_unit_wrapperのラップの解除
		template<class T>
		struct si_unit_unwrapper {
			using T1 = typename decay<T>::type;
			using type = typename si_unit_unwrapper_impl<T1>::type;
		};

		//任意の型のsi_unit_wrapperの構築
		template <class SIUnit, class T>
		inline si_unit_wrapper<T, SIUnit> make_si_unit_wrapper(const T& val) {
			si_unit_wrapper<T, SIUnit> temp;
			temp.get() = val;
			return temp;
		}


		//2項演算
		template <class U, class V, class SIUnit, class = typename enable_if<is_calcable<U, V>::add_value>::type>
		inline auto operator+(const si_unit_wrapper<U, SIUnit>& x1, const si_unit_wrapper<V, SIUnit>& x2) {
			return make_si_unit_wrapper<SIUnit>(x1.get() + x2.get());
		}
		template <class U, class V, class = typename enable_if<is_calcable<U, V>::add_value && !is_si_unit_wrapper<V>::value>::type>
		inline auto operator+(const si_unit_wrapper<U, si_unit<0, 0, 0, 0, 0, 0>>& x1, const V& x2) {
			return make_si_unit_wrapper<si_unit<0, 0, 0, 0, 0, 0>>(x1.get() + x2);
		}
		template <class U, class V, class = typename enable_if<is_calcable<U, V>::add_value && !is_si_unit_wrapper<U>::value>::type>
		inline auto operator+(const U& x1, const si_unit_wrapper<V, si_unit<0, 0, 0, 0, 0, 0>>& x2) {
			return make_si_unit_wrapper<si_unit<0, 0, 0, 0, 0, 0>>(x1 + x2.get());
		}
		template <class U, class V, class SIUnit, class = typename enable_if<is_calcable<U, V>::sub_value>::type>
		inline auto operator-(const si_unit_wrapper<U, SIUnit>& x1, const si_unit_wrapper<V, SIUnit>& x2) {
			return make_si_unit_wrapper<SIUnit>(x1.get() - x2.get());
		}
		template <class U, class V, class = typename enable_if<is_calcable<U, V>::sub_value && !is_si_unit_wrapper<V>::value>::type>
		inline auto operator-(const si_unit_wrapper<U, si_unit<0, 0, 0, 0, 0, 0>>& x1, const V& x2) {
			return make_si_unit_wrapper<si_unit<0, 0, 0, 0, 0, 0>>(x1.get() - x2);
		}
		template <class U, class V, class = typename enable_if<is_calcable<U, V>::sub_value && !is_si_unit_wrapper<U>::value>::type>
		inline auto operator-(const U& x1, const si_unit_wrapper<V, si_unit<0, 0, 0, 0, 0, 0>>& x2) {
			return make_si_unit_wrapper<si_unit<0, 0, 0, 0, 0, 0>>(x1 - x2.get());
		}
		template <class U, class USIUnit, class V, class VSIUnit, class = typename enable_if<is_calcable<U, V>::mul_value>::type>
		inline auto operator*(const si_unit_wrapper<U, USIUnit>& x1, const si_unit_wrapper<V, VSIUnit>& x2) {
			return make_si_unit_wrapper<typename calculation_result<USIUnit, VSIUnit>::mul_type>(x1.get() * x2.get());
		}
		template <class U, class USIUnit, class V, class = typename enable_if<is_calcable<U, V>::mul_value && !is_si_unit_wrapper<V>::value>::type>
		inline auto operator*(const si_unit_wrapper<U, USIUnit>& x1, const V& x2) {
			return make_si_unit_wrapper<USIUnit>(x1.get() * x2);
		}
		template <class U, class V, class VSIUnit, class = typename enable_if<is_calcable<U, V>::mul_value && !is_si_unit_wrapper<U>::value>::type>
		inline auto operator*(const U& x1, const si_unit_wrapper<V, VSIUnit>& x2) {
			return make_si_unit_wrapper<VSIUnit>(x1 * x2.get());
		}
		template <class U, class USIUnit, class V, class VSIUnit, class = typename enable_if<is_calcable<U, V>::div_value>::type>
		inline auto operator/(const si_unit_wrapper<U, USIUnit>& x1, const si_unit_wrapper<V, VSIUnit>& x2) {
			return make_si_unit_wrapper<typename calculation_result<USIUnit, VSIUnit>::div_type>(x1.get() / x2.get());
		}
		template <class U, class USIUnit, class V, class = typename enable_if<is_calcable<U, V>::div_value && !is_si_unit_wrapper<V>::value>::type>
		inline auto operator/(const si_unit_wrapper<U, USIUnit>& x1, const V& x2) {
			return make_si_unit_wrapper<USIUnit>(x1.get() / x2);
		}
		template <class U, class V, imint_t N1, imint_t N2, imint_t N3, imint_t N4, imint_t N5, imint_t N6, class = typename enable_if<is_calcable<U, V>::div_value && !is_si_unit_wrapper<U>::value>::type>
		inline auto operator/(const U& x1, const si_unit_wrapper<V, si_unit<N1, N2, N3, N4, N5, N6>>& x2) {
			return make_si_unit_wrapper<si_unit<-N1, -N2, -N3, -N4, -N5, -N6>>(x1 / x2.get());
		}
	}


	//任意の2つの型の演算可能テスト(si_unit_wrapper ver)
	template <class T1, class SIUnit1, class T2, class SIUnit2>
	struct is_calcable<phy::si_unit_wrapper<T1, SIUnit1>, phy::si_unit_wrapper<T2, SIUnit2>> {
		using type1 = phy::is_same_dimension<SIUnit1, SIUnit2>;
		using type2 = phy::is_dimensionless<SIUnit2>;
		using type3 = is_calcable<T1, T2>;

		static constexpr bool add_value = type1::value && type3::add_value;
		static constexpr bool sub_value = type1::value && type3::sub_value;
		static constexpr bool mul_value = type3::mul_value;
		static constexpr bool div_value = type3::div_value;
		static constexpr bool add_eq_value = type1::value && type3::add_eq_value;
		static constexpr bool sub_eq_value = type1::value && type3::sub_eq_value;
		static constexpr bool mul_eq_value = type2::value && type3::mul_eq_value;
		static constexpr bool div_eq_value = type2::value && type3::div_eq_value;
		static constexpr bool eq_value = type1::value && type3::eq_value;
		static constexpr bool lt_value = type1::value && type3::lt_value;
		static constexpr bool leq_value = type1::value && type3::leq_value;
		static constexpr bool gt_value = type1::value && type3::gt_value;
		static constexpr bool geq_value = type1::value && type3::geq_value;
	};


	//任意の2つの型の演算テスト(si_unit_wrapper ver)
	template <class T1, class SIUnit1, class T2, class SIUnit2>
	struct calculation_result<phy::si_unit_wrapper<T1, SIUnit1>, phy::si_unit_wrapper<T2, SIUnit2>> {
		using type1 = is_calcable<phy::si_unit_wrapper<T1, SIUnit1>, phy::si_unit_wrapper<T2, SIUnit2>>;
		using type2 = calculation_result<T1, T2>;
		using type3 = calculation_result<SIUnit1, SIUnit2>;

		using add_type = typename conditional<type1::add_value, phy::si_unit_wrapper<typename type2::add_type, typename type3::add_type>, void>::type;
		using sub_type = typename conditional<type1::sub_value, phy::si_unit_wrapper<typename type2::sub_type, typename type3::sub_type>, void>::type;
		using mul_type = typename conditional<type1::mul_value, phy::si_unit_wrapper<typename type2::mul_type, typename type3::mul_type>, void>::type;
		using div_type = typename conditional<type1::div_value, phy::si_unit_wrapper<typename type2::div_type, typename type3::div_type>, void>::type;
		using add_eq_type = typename conditional<type1::add_eq_value, phy::si_unit_wrapper<typename type2::add_eq_type, typename type3::add_eq_type>, void>::type;
		using sub_eq_type = typename conditional<type1::sub_eq_value, phy::si_unit_wrapper<typename type2::sub_eq_type, typename type3::sub_eq_type>, void>::type;
		using mul_eq_type = typename conditional<type1::mul_eq_value, phy::si_unit_wrapper<typename type2::mul_eq_type, typename type3::mul_eq_type>, void>::type;
		using div_eq_type = typename conditional<type1::div_eq_value, phy::si_unit_wrapper<typename type2::div_eq_type, typename type3::div_eq_type>, void>::type;
	};


	namespace phy {

		//リテラルとの演算をするためのsi_unitとsi_unit_wrapper間等の二項演算の定義
		template <imint_t N1, imint_t N2, imint_t N3, imint_t N4, imint_t N5, imint_t N6, class T, class SIUnit>
		inline auto operator*(const si_unit<N1, N2, N3, N4, N5, N6>& x1, const si_unit_wrapper<T, SIUnit>& x2) {
			return make_si_unit_wrapper<typename calculation_result<si_unit<N1, N2, N3, N4, N5, N6>, SIUnit>::mul_type>(x2.get());
		}
		template <class T, class SIUnit, imint_t N1, imint_t N2, imint_t N3, imint_t N4, imint_t N5, imint_t N6>
		inline auto operator*(const si_unit_wrapper<T, SIUnit>& x1, const si_unit<N1, N2, N3, N4, N5, N6>& x2) {
			return make_si_unit_wrapper<typename calculation_result<SIUnit, si_unit<N1, N2, N3, N4, N5, N6>>::mul_type>(x1.get());
		}
		template <imint_t N1, imint_t N2, imint_t N3, imint_t N4, imint_t N5, imint_t N6, class T, class = typename enable_if<!is_si_unit_wrapper<T>::value && !is_si_unit<T>::value>::type>
		inline auto operator*(const si_unit<N1, N2, N3, N4, N5, N6>& x1, const T& x2) {
			return make_si_unit_wrapper<si_unit<N1, N2, N3, N4, N5, N6>>(x2);
		}
		template <class T, imint_t N1, imint_t N2, imint_t N3, imint_t N4, imint_t N5, imint_t N6, class = typename enable_if<!is_si_unit_wrapper<T>::value && !is_si_unit<T>::value>::type>
		inline auto operator*(const T& x1, const si_unit<N1, N2, N3, N4, N5, N6>& x2) {
			return make_si_unit_wrapper<si_unit<N1, N2, N3, N4, N5, N6>>(x1);
		}
		template <imint_t N1, imint_t N2, imint_t N3, imint_t N4, imint_t N5, imint_t N6, class T, class SIUnit>
		inline auto operator/(const si_unit<N1, N2, N3, N4, N5, N6>& x1, const si_unit_wrapper<T, SIUnit>& x2) {
			return make_si_unit_wrapper<typename calculation_result<si_unit<N1, N2, N3, N4, N5, N6>, SIUnit>::div_type>(x2.get());
		}
		template <class T, class SIUnit, imint_t N1, imint_t N2, imint_t N3, imint_t N4, imint_t N5, imint_t N6>
		inline auto operator/(const si_unit_wrapper<T, SIUnit>& x1, const si_unit<N1, N2, N3, N4, N5, N6>& x2) {
			return make_si_unit_wrapper<typename calculation_result<SIUnit, si_unit<N1, N2, N3, N4, N5, N6>>::div_type>(x1.get());
		}
		template <class T, imint_t N1, imint_t N2, imint_t N3, imint_t N4, imint_t N5, imint_t N6, class = typename enable_if<!is_si_unit_wrapper<T>::value && !is_si_unit<T>::value>::type>
		inline auto operator/(const T& x1, const si_unit<N1, N2, N3, N4, N5, N6>& x2) {
			return make_si_unit_wrapper<si_unit<-N1, -N2, -N3, -N4, -N5, -N6>>(x1);
		}


		//SI単位の組み立て
		//ニュートンN(kg m / s^2)
		using newton_tag = type_tuple<kilogram_tag, meter_tag, rsecond_tag, rsecond_tag>;
		//速度v(m / s)
		using velocity_tag = type_tuple<meter_tag, rsecond_tag>;
		//加速度a(m / s^2)
		using acceleration_tag = type_tuple<meter_tag, rsecond_tag, rsecond_tag>;
		//運動量p(N s)
		using momentum_tag = type_tuple<newton_tag, second_tag>;
		//仕事J(N m)
		using joule_tag = type_tuple<newton_tag, meter_tag>;
		using energy_tag = type_tuple<newton_tag, meter_tag>;
		//仕事率W(J / s = VA)
		using watt_tag = type_tuple<joule_tag, rsecond_tag>;
		using power_tag = type_tuple<joule_tag, rsecond_tag>;


		//リテラルの列挙と演算の定義
		namespace si {
			static const si_unit_supporter<meter_tag> m;
			static const si_unit_supporter<kilogram_tag> kg;
			static const si_unit_supporter<ampere_tag> A;
			static const si_unit_supporter<kelvin_tag> K;
			static const si_unit_supporter<mole_tag> mol;
			static const si_unit_supporter<candela_tag> cd;

		}
	}
}


#endif