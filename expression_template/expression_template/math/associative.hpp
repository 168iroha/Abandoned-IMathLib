#ifndef IMATH_MATH_EXPRESSION_TEMPLATE_MATH_ASSOCIATIVE_HPP
#define IMATH_MATH_EXPRESSION_TEMPLATE_MATH_ASSOCIATIVE_HPP

#include "IMathLib/math/expression_template/expr_wrapper.hpp"


// 結合法則の適用
namespace iml {
	namespace op {

		// bool:式のトップレベルに対して結合法則が適用可能か判定
		// 定数及び変数(結合法則適用なし)
		template <class Op, class T, bool = std::is_same_v<expr_tag_t<T>, Op>>
		struct Associative {
			static constexpr auto associative(const T& expr) { return expr; }
		};
		template <class Op, size_t N, bool F>
		struct Associative<Op, expr_variable<N>, F> {
			static constexpr auto associative(const expr_variable<N>& expr) { return expr; }
		};
		// 単項演算(結合法則適用なし)
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


		// シーケンスからtupleの構築
		template <class Tuple1, class Tuple2, size_t... Indices1, size_t... Indices2, size_t... Indices3>
		inline constexpr auto Associative_tuple2_impl(const Tuple1& t1, const Tuple2& t2, index_tuple<size_t, Indices1...>, index_tuple<size_t, Indices2...>, index_tuple<size_t, Indices3...>) {
			return make_tuple(t1.get<Indices1>()..., t2.get<Indices2>()..., t1.get<Indices3>()...);
		}

		// タプルの要素が分配可能かの識別
		// Op:式のトップレベルの演算と結合対象の演算でそれぞれは一致する
		// bool1:走査の終端判定，bool2:走査する要素がタプルか判定
		template <class Op, class Tuple, size_t N = 0, bool = (N == tuple_size_v<Tuple> - 1), bool = is_tuple_v<at_type_tuple_t<N, Tuple>>>
		struct Associative_tuple2 {};
		// <false, false>
		template <class Op, class Tuple, size_t N>
		struct Associative_tuple2<Op, Tuple, N, false, false> {
			static constexpr auto associative(const Tuple& x) {
				return Associative_tuple2<Op, Tuple, N + 1>::associative(x);
			}
		};
		// <true, false>
		template <class Op, class Tuple, size_t N>
		struct Associative_tuple2<Op, Tuple, N, true, false> {
			static constexpr auto associative(const Tuple& x) {
				return expr_wrapper<Op, typename Tuple::type_tuple_type>(x);
			}
		};
		// <false, true>
		template <class Op, class Tuple, size_t N>
		struct Associative_tuple2<Op, Tuple, N, false, true> {
			static constexpr auto associative(const Tuple& x) {
				// 左右から結合するためのシーケンスの用意
				using low_index = index_range_t<size_t, 0, N>;
				using high_index = index_range_t<size_t, N + 1, tuple_size_v<Tuple>>;
				// タプルに対して結合法則の適用
				auto temp = Associative_tuple2<Op, at_type_tuple_t<N, Tuple>>::associative(x.get<N>());
				// tempを展開するためのシーケンス
				using temp_sequence = typename std::decay_t<decltype(temp.terms_m)>::sequence_type;
				// 結合法則の適用
				auto temp2 = Associative_tuple2_impl(x, temp.terms_m, low_index(), temp_sequence(), high_index());

				return Associative_tuple2<Op, std::decay_t<decltype(temp2)>, N + temp_sequence::value>::associative(temp2);
			}
		};
		// <true, true>
		template <class Op, class Tuple, size_t N>
		struct Associative_tuple2<Op, Tuple, N, true, true> {
			static constexpr auto associative(const Tuple& x) {
				// 左右から結合するためのシーケンスの用意
				using low_index = index_range_t<size_t, 0, N>;
				using high_index = index_range_t<size_t, N + 1, tuple_size_v<Tuple>>;
				// タプルに対して結合法則の適用
				auto temp = Associative_tuple2<Op, at_type_tuple_t<N, Tuple>>::associative(x.get<N>());
				// tempを展開するためのシーケンス
				using temp_sequence = typename std::decay_t<decltype(temp.terms_m)>::sequence_type;
				// 結合法則の適用
				auto temp2 = Associative_tuple2_impl(x, temp.terms_m, low_index(), temp_sequence(), high_index());

				return expr_wrapper<Op, typename std::decay_t<decltype(temp2)>::type_tuple_type>(temp2);
			}
		};


		// タプルの各要素を結合法則を適用して再構成
		// Op1:再構成で結合する演算, Op2:結合法則を適用させる演算
		template <class Op1, class Op2, class T>
		struct Associative_tuple1 {
			static constexpr auto associative(const T& x, index_tuple<size_t>) { return Associative<Op2, T>::associative(x); }
		};
#define ASSOCIATIVE_TUPLE1(NAME, OP)\
		template <class Op2, class... Types>\
		struct Associative_tuple1<NAME##_tag, Op2, tuple<Types...>> {\
			template <size_t... Indices>\
			static constexpr auto associative(const tuple<Types...>& x, index_tuple<size_t, Indices...>) {\
				/* 畳み込み式で一気に展開して計算 */\
				return (...##OP##Associative_tuple1<NAME##_tag, Op2, Types>::associative(x.get<Indices>(), index_range_t<size_t, 0, tuple_size_v<Types>>()));\
			}\
		};
		ASSOCIATIVE_TUPLE1(add, +);
		ASSOCIATIVE_TUPLE1(sub, -);
		ASSOCIATIVE_TUPLE1(mul, *);
		ASSOCIATIVE_TUPLE1(div, /);
#undef ASSOCIATIVE_TUPLE1

		// 2項演算
		// Opと式のトップレベルの2項演算子が一致するため結合法則を適用
		template <class Op1, class Op2, class First, class Second, class... Types>
			struct Associative<Op1, expr_wrapper<Op2, type_tuple<First, Second, Types...>>, true> {
			using expr_type = expr_wrapper<Op2, type_tuple<First, Second, Types...>>;

			static constexpr auto associative(const expr_type& expr) {
				// 予め各タプルの要素に対して結合法則を適用
				auto temp = Associative_tuple1<Op2, Op1, tuple<First, Second, Types...>>::associative(expr.terms_m
					, index_range_t<size_t, 0, 2 + sizeof...(Types)>());
				// 各タプルの要素で結合法則を適用
				return Associative_tuple2<Op2, std::decay_t<decltype(temp.terms_m)>>::associative(temp.terms_m);
			}
		};
		// Opと式のトップレベルの2項演算子が一致しないため結合法則を適用しない
		template <class Op1, class Op2, class First, class Second, class... Types>
		struct Associative<Op1, expr_wrapper<Op2, type_tuple<First, Second, Types...>>, false> {
			using expr_type = expr_wrapper<Op2, type_tuple<First, Second, Types...>>;

			static constexpr auto associative(const expr_type& expr) {
				// タプルの要素に対して結合法則を適用
				return Associative_tuple1<Op2, Op1, tuple<First, Second, Types...>>::associative(expr.terms_m
						, index_range_t<size_t, 0, 2 + sizeof...(Types)>());
			}
		};

		template <class Op, class Expr>
		constexpr auto associative(const Expr& expr) { return Associative<Op, Expr>::associative(expr); }
	}
}


#endif