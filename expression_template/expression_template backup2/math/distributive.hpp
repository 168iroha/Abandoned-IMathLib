#ifndef IMATH_MATH_EXPRESSION_TEMPLATE_MATH_DISTRIBUTIVE_HPP
#define IMATH_MATH_EXPRESSION_TEMPLATE_MATH_DISTRIBUTIVE_HPP

#include "IMathLib/math/expression_template/expr_wrapper.hpp"


//分配法則の適用

namespace iml {
	namespace op {


		//変数及び定数
		template <class T>
		struct Distributive {
			static constexpr auto distributive(const T& expr) { return expr; }
		};
		template <size_t N>
		struct Distributive<expr_variable<N>> {
			static constexpr auto distributive(const expr_variable<N>& expr) { return expr; }
		};
		//単項演算
		template <class Expr>
		struct Distributive<expr_wrapper<add_tag, type_tuple<Expr>>> {
			using expr_type = expr_wrapper<add_tag, type_tuple<Expr>>;
			static constexpr auto distributive(const expr_type& expr) {
				auto temp = 1 * expr.terms_m;
				using temp_type = typename decay<decltype(temp)>::type;
				return Distributive<temp_type>::distributive(temp);
			}
		};
		template <class Expr>
		struct Distributive<expr_wrapper<sub_tag, type_tuple<Expr>>> {
			using expr_type = expr_wrapper<sub_tag, type_tuple<Expr>>;
			static constexpr auto distributive(const expr_type& expr) {
				auto temp = -1 * expr.terms_m;
				using temp_type = typename decay<decltype(temp)>::type;
				return Distributive<temp_type>::distributive(temp);
			}
		};
		//加減算
		template <class First, class Second, class... Types>
		struct Distributive<expr_wrapper<add_tag, type_tuple<First, Second, Types...>>> {
			using expr_type = expr_wrapper<add_tag, type_tuple<First, Second, Types...>>;
			static constexpr auto distributive(const expr_type& expr) { return expr; }
		};
		template <class First, class Second, class... Types>
		struct Distributive<expr_wrapper<sub_tag, type_tuple<First, Second, Types...>>> {
			using expr_type = expr_wrapper<sub_tag, type_tuple<First, Second, Types...>>;
			static constexpr auto distributive(const expr_type& expr) { return expr; }
		};


		//分配法則を作用
		//Op1:分配する演算(乗除算等)，Op2:分配される演算(加減算等)
		template <class, class, class T, size_t, class, class, bool = is_tuple<T>::value>
		struct Distributive_tuple3 {};
		template <class Op1, class Op2, class T, size_t N, size_t... Indices1, size_t... Indices2>
		struct Distributive_tuple3<Op1, Op2, T, N, index_tuple<size_t, Indices1...>, index_tuple<size_t, Indices2...>, false> {
			template <class Tuple>
			static constexpr auto distributive(const Tuple& t, const T& x) {
				return make_expr_wrapper<Op1>(t.get<Indices1>()..., x, t.get<Indices2>()...);
			}
		};
		template <class Op1, class Op2, class... Types, size_t N, size_t... Indices1, size_t... Indices2>
		struct Distributive_tuple3<Op1, Op2, tuple<Types...>, N, index_tuple<size_t, Indices1...>, index_tuple<size_t, Indices2...>, true> {
			template <class Tuple, class First>
			static constexpr auto distributive_impl(const Tuple& t, const First& first) {
				return Distributive_tuple3<Op1, Op2, First, N
					, index_tuple<size_t, Indices1...>, index_tuple<size_t, Indices2...>>::distributive(t, first);
			}
			template <class Tuple, class First, class Second, class... Args>
			static constexpr auto distributive_impl(const Tuple& t, const First& first, const Second& second, const Args&... args) {
				return reverse_operation<Op2>(distributive_impl(t, first), distributive_impl(t, second, args...));
			}

			//Distributiveのためのapply
			template <class Tuple, size_t... Indices>
			static constexpr auto apply(const Tuple& t, const tuple<Types...>& x, index_tuple<size_t, Indices...>) {
				return invoke(distributive_impl<Tuple, Types...>, t, x.get<Indices>()...);
			}
			template <class Tuple>
			static constexpr auto distributive(const Tuple& t, const tuple<Types...>& x) {
				return apply(t, x, typename tuple<Types...>::sequence_type());
			}
		};

		template <class Expr, class Tuple, size_t N = 0, bool = (N == tuple_size<Tuple>::value - 1), bool = (operator_precedence<typename at_type_tuple<N, Tuple>::type>::value > operator_precedence<Expr>::value)>
		struct Distributive_tuple2 {};


		//タプルの全ての要素に対してDistributive_tuple2を作用させてOp2で結合
		//Expr1:分配する演算(乗除算等)，Expr2:分配される演算(加減算等)
		template <class Expr1, class Expr2, class T, size_t N>
		struct Distributive_tuple2_impl {
			static constexpr auto distributive(const T& x) {
				return Distributive_tuple2<Expr1, typename T::tuple_type, N + 1>::distributive(x.terms_m);
			}
		};
		template <class Expr1, class Expr2, class... Types, size_t N>
		struct Distributive_tuple2_impl<Expr1, Expr2, tuple<Types...>, N> {
			using op2_type = typename expr_tag<Expr2>::type;

			template <class First>
			static constexpr auto distributive_impl(const First& first) {
				return Distributive_tuple2_impl<Expr1, Expr2, First, N>::distributive(first);
			}
			template <class First, class Second, class... Args>
			static constexpr auto distributive_impl(const First& first, const Second& second, const Args&... args) {
				return reverse_operation<op2_type>(distributive_impl(first), distributive_impl(second, args...));
			}

			static constexpr auto distributive(const tuple<Types...>& x) {
				return apply(distributive_impl<Types...>, x);
			}
		};

		//タプルの要素が展開可能かの識別
		//bool1:走査の終端判定，bool2:分配可能判定
		//<false, false>
		template <class Expr, class Tuple, size_t N>
		struct Distributive_tuple2<Expr, Tuple, N, false, false> {
			static constexpr auto distributive(const Tuple& x) {
				return Distributive_tuple2<Expr, Tuple, N + 1>::distributive(x);
			}
		};
		//<true, false>
		template <class Expr, class Tuple, size_t N>
		struct Distributive_tuple2<Expr, Tuple, N, true, false> {
			using op_type = typename expr_tag<Expr>::type;

			static constexpr auto distributive(const Tuple& x) {
				return expr_wrapper<op_type, typename Tuple::type_tuple_type>(x);
			}
		};
		//<false, true>
		template <class Expr, class Tuple, size_t N>
		struct Distributive_tuple2<Expr, Tuple, N, false, true> {
			using op_type = typename expr_tag<Expr>::type;

			static constexpr auto distributive(const Tuple& x) {
				//左右からかけるためのシーケンスの用意
				using low_index = typename index_range<size_t, 0, N>::type;
				using high_index = typename index_range<size_t, N + 1, Tuple::size()>::type;
				//分配法則の適用
				using tuple_type = typename decay<decltype(x.get<N>().terms_m)>::type;
				auto temp = Distributive_tuple3<op_type, typename expr_tag<typename at_type_tuple<N, Tuple>::type>::type, tuple_type
					, N, low_index, high_index>::distributive(x, x.get<N>().terms_m);

				using temp_type = typename decay<decltype(temp.terms_m)>::type;
				return Distributive_tuple2_impl<Expr, typename at_type_tuple<N, Tuple>::type
					, temp_type, N>::distributive(temp.terms_m);
			}
		};
		//<true, true>
		template <class Expr, class Tuple, size_t N>
		struct Distributive_tuple2<Expr, Tuple, N, true, true> {
			using op_type = typename expr_tag<Expr>::type;

			static constexpr auto distributive(const Tuple& x) {
				//左右からかけるためのシーケンスの用意
				using low_index = typename index_range<size_t, 0, N>::type;
				using high_index = typename index_range<size_t, N + 1, Tuple::size()>::type;
				//分配法則の適用
				using tuple_type = typename decay<decltype(x.get<N>().terms_m)>::type;
				return Distributive_tuple3<op_type, typename expr_tag<typename at_type_tuple<N, Tuple>::type>::type, tuple_type
					, N, low_index, high_index>::distributive(x, x.get<N>().terms_m);
			}
		};


		//タプルの要素を分配法則を適用済みのものにして再構成
		//bool1:走査の終端判定，bool2:走査する要素がタプルか判定
		//<false, false>
		template <class Op, class Tuple, size_t N = 0, bool = (N == tuple_size<Tuple>::value - 1), bool = is_tuple<typename at_type_tuple<N, Tuple>::type>::value>
		struct Distributive_tuple1 {
			static constexpr auto distributive(const Tuple& x) {
				return reverse_operation<Op>(Distributive<typename at_type_tuple<N, Tuple>::type>::distributive(x.get<N>())
					, Distributive_tuple1<Op, Tuple, N + 1>::distributive(x));
			}
		};
		//<true, false>
		template <class Op, class Tuple, size_t N>
		struct Distributive_tuple1<Op, Tuple, N, true, false> {
			static constexpr auto distributive(const Tuple& x) {
				return Distributive<typename at_type_tuple<N, Tuple>::type>::distributive(x.get<N>());
			}
		};
		//<false, true>
		template <class Op, class Tuple, size_t N>
		struct Distributive_tuple1<Op, Tuple, N, false, true> {
			static constexpr auto distributive(const Tuple& x) {
				//更にタプルを展開
				return reverse_operation<Op>(Distributive_tuple1<Op, typename at_type_tuple<N, Tuple>::type>::distributive(x.get<N>())
					, Distributive_tuple1<Op, Tuple, N + 1>::distributive(x));
			}
		};
		//<true, true>
		template <class Op, class Tuple, size_t N>
		struct Distributive_tuple1<Op, Tuple, N, true, true> {
			static constexpr auto distributive(const Tuple& x) {
				//更にタプルを展開
				return Distributive_tuple1<Op, typename at_type_tuple<N, Tuple>::type>::distributive(x.get<N>());
			}
		};

		//乗除算
		template <class First, class Second, class... Types>
		struct Distributive<expr_wrapper<mul_tag, type_tuple<First, Second, Types...>>> {
			using expr_type = expr_wrapper<mul_tag, type_tuple<First, Second, Types...>>;

			static constexpr auto distributive(const expr_type& expr) {
				//予めタプルの要素に対して分配法則を適用
				auto temp = Distributive_tuple1<mul_tag, tuple<First, Second, Types...>>::distributive(expr.terms_m);
				using temp_tuple_type = typename decay<decltype(temp.terms_m)>::type;
				//メインの分配法則の適用と式の再構築
				return Distributive_tuple2<expr_type, temp_tuple_type>::distributive(temp.terms_m);
			}
		};
		template <class First, class Second, class... Types>
		struct Distributive<expr_wrapper<div_tag, type_tuple<First, Second, Types...>>> {
			using expr_type = expr_wrapper<div_tag, type_tuple<First, Second, Types...>>;

			static constexpr auto distributive(const expr_type& expr) {
				//予めタプルの要素に対して分配法則を適用
				auto temp = Distributive_tuple1<div_tag, tuple<First, Second, Types...>>::distributive(expr.terms_m);
				using temp_tuple_type = typename decay<decltype(temp.terms_m)>::type;
				//メインの分配法則の適用と式の再構築
				return Distributive_tuple2<expr_type, temp_tuple_type>::distributive(temp.terms_m);
			}
		};
		template <class T>
		inline constexpr auto distributive(const T& expr) { return Distributive<T>::distributive(expr); }

	}
}


#endif