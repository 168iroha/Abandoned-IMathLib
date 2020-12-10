#ifndef IMATH_MATH_EXPRESSION_TEMPLATE_MATH_ASSOCIATIVE_HPP
#define IMATH_MATH_EXPRESSION_TEMPLATE_MATH_ASSOCIATIVE_HPP

#include "IMathLib/math/expression_template/expr_wrapper.hpp"


//結合法則の適用

namespace iml {
	namespace op {

		//bool:式のトップレベルに対して結合法則が作用可能か判定
		//定数及び変数
		template <class Op, class T, bool = is_same<typename expr_tag<T>::type, Op>::value>
		struct Associative {
			static constexpr auto associative(const T& expr) { return expr; }
		};
		template <class Op, size_t N, bool F>
		struct Associative<Op, expr_variable<N>, F> {
			static constexpr auto associative(const expr_variable<N>& expr) { return expr; }
		};
		//単項演算
		template <class Op, class Expr, bool F>
		struct Associative<Op, expr_wrapper<add_tag, type_tuple<Expr>>, F> {
			using expr_type = expr_wrapper<add_tag, type_tuple<Expr>>;
			static constexpr auto associative(const expr_type& expr) { return +Associative<Op, Expr>::associative(expr.terms); }
		};
		template <class Op, class Expr, bool F>
		struct Associative<Op, expr_wrapper<sub_tag, type_tuple<Expr>>, F> {
			using expr_type = expr_wrapper<sub_tag, type_tuple<Expr>>;
			static constexpr auto associative(const expr_type& expr) { return -Associative<Op, Expr>::associative(expr.terms); }
		};

		//シーケンスからtupleの構築
		template <class Tuple1, class Tuple2, size_t... Indices1, size_t... Indices2, size_t... Indices3>
		constexpr auto Associative_tuple2_impl(const Tuple1& t1, const Tuple2& t2, index_tuple<size_t, Indices1...>, index_tuple<size_t, Indices2...>, index_tuple<size_t, Indices3...>) {
			return make_tuple(t1.get<Indices1>()..., t2.get<Indices2>()..., t1.get<Indices3>()...);
		}


		//タプルの要素が分配可能かの識別
		//Op:式のトップレベルの演算と結合対象の演算でそれぞれは一致する
		//bool1:走査の終端判定，bool2:走査する要素がタプルか判定
		template <class Op, class Tuple, size_t N = 0, bool = (N == tuple_size<Tuple>::value - 1), bool = is_tuple<typename at_type_tuple<N, Tuple>::type>::value>
		struct Associative_tuple2 {};
		//<false, false>
		template <class Op, class Tuple, size_t N>
		struct Associative_tuple2<Op, Tuple, N, false, false> {
			static constexpr auto associative(const Tuple& x) {
				return Associative_tuple2<Op, Tuple, N + 1>::associative(x);
			}
		};
		//<true, false>
		template <class Op, class Tuple, size_t N>
		struct Associative_tuple2<Op, Tuple, N, true, false> {
			static constexpr auto associative(const Tuple& x) {
				return expr_wrapper<Op, typename Tuple::type_tuple_type>(x);
			}
		};
		//<false, true>
		template <class Op, class Tuple, size_t N>
		struct Associative_tuple2<Op, Tuple, N, false, true> {
			static constexpr auto associative(const Tuple& x) {
				//左右から結合するためのシーケンスの用意
				using low_index = typename index_range<size_t, 0, N>::type;
				using high_index = typename index_range<size_t, N + 1, Tuple::size()>::type;
				//タプルに対して結合法則の適用
				auto temp = Associative_tuple2<Op, typename at_type_tuple<N, Tuple>::type>::associative(x.get<N>());
				//tempを展開するためのシーケンス
				using temp_tuple_type = typename decay<decltype(temp.terms_m)>::type;
				using temp_sequence = typename temp_tuple_type::sequence_type;
				auto temp2 = Associative_tuple2_impl(x, temp.terms_m, low_index(), temp_sequence(), high_index());
				using temp2_type = typename decay<decltype(temp2)>::type;

				return Associative_tuple2<Op, temp2_type, N + temp_sequence::value>::associative(temp2);
			}
		};
		//<true, true>
		template <class Op, class Tuple, size_t N>
		struct Associative_tuple2<Op, Tuple, N, true, true> {
			static constexpr auto associative(const Tuple& x) {
				//左右から結合するためのシーケンスの用意
				using low_index = typename index_range<size_t, 0, N>::type;
				using high_index = typename index_range<size_t, N + 1, Tuple::size()>::type;
				//タプルに対して結合法則の適用
				auto temp = Associative_tuple2<Op, typename at_type_tuple<N, Tuple>::type>::associative(x.get<N>());
				//tempを展開するためのシーケンス
				using temp_tuple_type = typename decay<decltype(temp.terms_m)>::type;
				using temp_sequence = typename temp_tuple_type::sequence_type;
				auto temp2 = Associative_tuple2_impl(x, temp, low_index(), temp_sequence(), high_index());
				using temp2_type = typename decay<decltype(temp2)>::type;

				return expr_wrapper<Op, typename temp2_type::type_tuple_type>(temp2);
			}
		};


		//タプルの要素を結合法則を適用済みのものにして再構成
		//Op1:再構成で結合する演算，Op2:結合法則を作用させる演算
		//bool1:走査の終端判定，bool2:走査する要素がタプルか判定
		//<false, false>
		template <class Op1, class Op2, class Tuple, size_t N = 0, bool = (N == tuple_size<Tuple>::value - 1), bool = is_tuple<typename at_type_tuple<N, Tuple>::type>::value>
		struct Associative_tuple1 {
			static constexpr auto associative(const Tuple& x) {
				return reverse_operation<Op1>(Associative<Op2, typename at_type_tuple<N, Tuple>::type>::associative(x.get<N>())
					, Associative_tuple1<Op1, Op2, Tuple, N + 1>::associative(x));
			}
		};
		//<true, false>
		template <class Op1, class Op2, class Tuple, size_t N>
		struct Associative_tuple1<Op1, Op2, Tuple, N, true, false> {
			static constexpr auto associative(const Tuple& x) {
				return Associative<Op2, typename at_type_tuple<N, Tuple>::type>::associative(x.get<N>());
			}
		};
		//<false, true>
		template <class Op1, class Op2, class Tuple, size_t N>
		struct Associative_tuple1<Op1, Op2, Tuple, N, false, true> {
			static constexpr auto associative(const Tuple& x) {
				//更にタプルを展開
				return reverse_operation<Op1>(Associative_tuple1<Op1, Op2, typename at_type_tuple<N, Tuple>::type>::associative(x.get<N>())
					, Associative_tuple1<Op1, Op2, Tuple, N + 1>::associative(x));
			}
		};
		//<true, true>
		template <class Op1, class Op2, class Tuple, size_t N>
		struct Associative_tuple1<Op1, Op2, Tuple, N, true, true> {
			static constexpr auto associative(const Tuple& x) {
				//更にタプルを展開
				return Associative_tuple1<Op1, Op2, typename at_type_tuple<N, Tuple>::type>::associative(x.get<N>());
			}
		};

		//2項演算
#define ASSOCIATIVE_BINARY_OPERATION(NAME)\
		template <class Op, class First, class Second, class... Types>\
		struct Associative<Op, expr_wrapper<NAME##_tag, type_tuple<First, Second, Types...>>, true> {\
			using expr_type = expr_wrapper<NAME##_tag, type_tuple<First, Second, Types...>>;\
			static constexpr auto associative(const expr_type& expr) {\
				/*予めタプルの要素に対して結合法則を適用*/\
				auto temp = Associative_tuple1<NAME##_tag, Op, tuple<First, Second, Types...>>::associative(expr.terms_m);\
				using temp_tuple_type = typename decay<decltype(temp.terms_m)>::type;\
				return Associative_tuple2<NAME##_tag, temp_tuple_type>::associative(temp.terms_m);\
			}\
		};\
		template <class Op, class First, class Second, class... Types>\
		struct Associative<Op, expr_wrapper<NAME##_tag, type_tuple<First, Second, Types...>>, false> {\
			using expr_type = expr_wrapper<NAME##_tag, type_tuple<First, Second, Types...>>;\
			static constexpr auto associative(const expr_type& expr) {\
				return Associative_tuple1<NAME##_tag, Op, tuple<First, Second, Types...>>::associative(expr.terms_m);\
			}\
		};
		ASSOCIATIVE_BINARY_OPERATION(add);
		ASSOCIATIVE_BINARY_OPERATION(sub);
		ASSOCIATIVE_BINARY_OPERATION(mul);
		ASSOCIATIVE_BINARY_OPERATION(div);
#undef ASSOCIATIVE_BINARY_OPERATION

		template <class Op, class Expr>
		constexpr auto associative(const Expr& expr) { return Associative<Op, Expr>::associative(expr); }
	}
}


#endif