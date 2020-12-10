#ifndef IMATH_MATH_EXPRESSION_TEMPLATE_MATH_DISTRIBUTIVE_HPP
#define IMATH_MATH_EXPRESSION_TEMPLATE_MATH_DISTRIBUTIVE_HPP

#include "IMathLib/math/expression_template/expr_wrapper.hpp"


namespace iml {
	namespace op {

		//分配則の適用
		template <class>
		struct Distributive1;
		template <class T, bool = is_underlayer_low_precedence<T>::lhs_value, bool = is_underlayer_low_precedence<T>::rhs_value>
		struct Distributive2 {
			static constexpr auto distributive2(const T& expr) {
				return expr;
			}
		};
		template <class T, bool F1, bool F2>
		struct Distributive2<expr_wrapper<none_tag, type_tuple<T>>, F1, F2> {
			static constexpr auto distributive2(const expr_wrapper<none_tag, type_tuple<T>>& expr) {
				return expr;
			}
		};
		template <class Expr, bool F1, bool F2>
		struct Distributive2<expr_wrapper<add_tag, type_tuple<Expr>>, F1, F2> {
			static constexpr auto distributive2(const expr_wrapper<add_tag, type_tuple<Expr>>& expr) {
				return +Distributive1<Expr>::distributive1(expr.x_m);
			}
		};
		template <class Expr, bool F1, bool F2>
		struct Distributive2<expr_wrapper<sub_tag, type_tuple<Expr>>, F1, F2> {
			static constexpr auto distributive2(const expr_wrapper<sub_tag, type_tuple<Expr>>& expr) {
				return -Distributive1<Expr>::distributive1(expr.x_m);
			}
		};
		template <class Expr1, class Expr2, bool F1, bool F2>
		struct Distributive2<expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>, F1, F2> {
			static constexpr auto distributive2(const expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>& expr) {
				return Distributive1<Expr1>::distributive1(expr.lhs_m) + Distributive1<Expr2>::distributive1(expr.rhs_m);
			}
		};
		template <class Expr1, class Expr2, bool F1, bool F2>
		struct Distributive2<expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>, F1, F2> {
			static constexpr auto distributive2(const expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>& expr) {
				return Distributive1<Expr1>::distributive1(expr.lhs_m) - Distributive1<Expr2>::distributive1(expr.rhs_m);
			}
		};
		template <class Expr1, class Expr2>
		struct Distributive2<expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>, false, false> {
			static constexpr auto distributive2(const expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>& expr) {
				return Distributive1<Expr1>::distributive1(expr.lhs_m) * Distributive1<Expr2>::distributive1(expr.rhs_m);
			}
		};
		template <class Expr1, class Expr2>
		struct Distributive2<expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>, true, false> {
			static constexpr auto distributive2(const expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>& expr) {
				//exprの左には孫が存在することを用いて戻り値型の補助を作成
				using lhs_lhs_type = decltype(expr.lhs_m.lhs_m);
				using lhs_rhs_type = decltype(expr.lhs_m.rhs_m);
				auto temp1 = Distributive1<Expr2>::distributive1(expr.rhs_m);
				auto temp2 = Distributive1<lhs_lhs_type>::distributive1(expr.lhs_m.lhs_m);
				auto temp3 = Distributive1<lhs_rhs_type>::distributive1(expr.lhs_m.rhs_m);

				auto temp4 = Distributive1<expr_wrapper<mul_tag, type_tuple<decltype(temp2), decltype(temp1)>>>::distributive1(
					make_expr_wrapper<mul_tag>(temp2, temp1)
				);
				auto temp5 = Distributive1<expr_wrapper<mul_tag, type_tuple<decltype(temp3), decltype(temp1)>>>::distributive1(
					make_expr_wrapper<mul_tag>(temp3, temp1)
				);
				return make_expr_wrapper<typename expr_tag<Expr1>::type>(temp4, temp5);
			}
		};
		template <class Expr1, class Expr2>
		struct Distributive2<expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>, false, true> {
			static constexpr auto distributive2(const expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>& expr) {
				//exprの右には孫が存在することを用いて戻り値型の補助を作成
				using rhs_lhs_type = decltype(expr.rhs_m.lhs_m);
				using rhs_rhs_type = decltype(expr.rhs_m.rhs_m);
				auto temp1 = Distributive1<Expr1>::distributive1(expr.lhs_m);
				auto temp2 = Distributive1<rhs_lhs_type>::distributive1(expr.rhs_m.lhs_m);
				auto temp3 = Distributive1<rhs_rhs_type>::distributive1(expr.rhs_m.rhs_m);

				auto temp4 = Distributive1<expr_wrapper<mul_tag, type_tuple<decltype(temp1), decltype(temp2)>>>::distributive1(
					make_expr_wrapper<mul_tag>(temp1, temp2)
				);
				auto temp5 = Distributive1<expr_wrapper<mul_tag, type_tuple<decltype(temp1), decltype(temp3)>>>::distributive1(
					make_expr_wrapper<mul_tag>(temp1, temp3)
				);
				return make_expr_wrapper<typename expr_tag<Expr2>::type>(temp4, temp5);
			}
		};
		template <class Expr1, class Expr2>
		struct Distributive2<expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>, true, true> {
			static constexpr auto distributive2(const expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>& expr) {
				//exprの左右には孫が存在することを用いて戻り値型の補助を作成
				using lhs_lhs_type = decltype(expr.lhs_m.lhs_m);
				using lhs_rhs_type = decltype(expr.lhs_m.rhs_m);
				using rhs_lhs_type = decltype(expr.rhs_m.lhs_m);
				using rhs_rhs_type = decltype(expr.rhs_m.rhs_m);
				auto temp1 = Distributive1<lhs_lhs_type>::distributive1(expr.lhs_m.lhs_m);
				auto temp2 = Distributive1<lhs_rhs_type>::distributive1(expr.lhs_m.rhs_m);
				auto temp3 = Distributive1<rhs_lhs_type>::distributive1(expr.rhs_m.lhs_m);
				auto temp4 = Distributive1<rhs_rhs_type>::distributive1(expr.rhs_m.rhs_m);
				//左辺を優先的に展開
				auto temp5 = Distributive1<expr_wrapper<mul_tag, type_tuple<decltype(temp1), decltype(temp3)>>>::distributive1(
					make_expr_wrapper<mul_tag>(temp1, temp3)
				);
				auto temp6 = Distributive1<expr_wrapper<mul_tag, type_tuple<decltype(temp1), decltype(temp4)>>>::distributive1(
					make_expr_wrapper<mul_tag>(temp1, temp4)
				);
				auto temp7 = Distributive1<expr_wrapper<mul_tag, type_tuple<decltype(temp2), decltype(temp3)>>>::distributive1(
					make_expr_wrapper<mul_tag>(temp2, temp3)
				);
				auto temp8 = Distributive1<expr_wrapper<mul_tag, type_tuple<decltype(temp2), decltype(temp4)>>>::distributive1(
					make_expr_wrapper<mul_tag>(temp2, temp4)
				);
				auto temp9 = make_expr_wrapper<typename expr_tag<Expr2>::type>(temp5, temp6);
				auto temp10 = make_expr_wrapper<typename expr_tag<Expr2>::type>(temp7, temp8);
				return make_expr_wrapper<typename expr_tag<Expr1>::type>(temp9, temp10);
			}
		};
		template <class Expr1, class Expr2>
		struct Distributive2<expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>, false, false> {
			static constexpr auto distributive2(const expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>& expr) {
				return Distributive1<Expr1>::distributive1(expr.lhs_m) / Distributive1<Expr2>::distributive1(expr.rhs_m);
			}
		};
		template <class Expr1, class Expr2>
		struct Distributive2<expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>, true, false> {
			static constexpr auto distributive2(const expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>& expr) {
				//exprの左には孫が存在することを用いて戻り値型の補助を作成
				using lhs_lhs_type = decltype(expr.lhs_m.lhs_m);
				using lhs_rhs_type = decltype(expr.lhs_m.rhs_m);
				auto temp1 = Distributive1<Expr2>::distributive1(expr.rhs_m);
				auto temp2 = Distributive1<lhs_lhs_type>::distributive1(expr.lhs_m.lhs_m);
				auto temp3 = Distributive1<lhs_rhs_type>::distributive1(expr.lhs_m.rhs_m);

				auto temp4 = Distributive1<expr_wrapper<div_tag, type_tuple<decltype(temp2), decltype(temp1)>>>::distributive1(
					make_expr_wrapper<div_tag>(temp2, temp1)
				);
				auto temp5 = Distributive1<expr_wrapper<div_tag, type_tuple<decltype(temp3), decltype(temp1)>>>::distributive1(
					make_expr_wrapper<div_tag>(temp3, temp1)
				);
				return make_expr_wrapper<typename expr_tag<Expr1>::type>(temp4, temp5);
			}
		};
		template <class Expr1, class Expr2>
		struct Distributive2<expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>, false, true> {
			static constexpr auto distributive2(const expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>& expr) {
				//a/(b+c)に対して分配則を適用することは不可
				return Distributive1<Expr1>::distributive1(expr.lhs_m) / Distributive1<Expr2>::distributive1(expr.rhs_m);
			}
		};
		template <class Expr1, class Expr2>
		struct Distributive2<expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>, true, true> {
			static constexpr auto distributive2(const expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>& expr) {
				//exprの左には孫が存在することを用いて戻り値型の補助を作成
				using lhs_lhs_type = decltype(expr.lhs_m.lhs_m);
				using lhs_rhs_type = decltype(expr.lhs_m.rhs_m);
				auto temp1 = Distributive1<Expr2>::distributive1(expr.rhs_m);
				auto temp2 = Distributive1<lhs_lhs_type>::distributive1(expr.lhs_m.lhs_m);
				auto temp3 = Distributive1<lhs_rhs_type>::distributive1(expr.lhs_m.rhs_m);

				auto temp4 = Distributive1<expr_wrapper<div_tag, type_tuple<decltype(temp2), decltype(temp1)>>>::distributive1(
					make_expr_wrapper<div_tag>(temp2, temp1)
				);
				auto temp5 = Distributive1<expr_wrapper<div_tag, type_tuple<decltype(temp3), decltype(temp1)>>>::distributive1(
					make_expr_wrapper<div_tag>(temp3, temp1)
				);
				return make_expr_wrapper<typename expr_tag<Expr1>::type>(temp4, temp5);
			}
		};
		template <class Expr1, class Expr2, bool F1, bool F2>
		struct Distributive2<expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>, F1, F2> {
			static constexpr auto distributive2(const expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>& expr) {
				return Distributive1<Expr1>::distributive1(expr.x_m)[Distributive1<Expr2>::distributive1(expr.index_m)];
			}
		};
		//下の階層から評価するためのもの(要は上の階層は遅延評価するということ)
		template <class T>
		struct Distributive1 {
			static constexpr auto distributive1(const T& expr) {
				return Distributive2<T>::distributive2(expr);
			}
		};
		template <class T>
		struct Distributive1<expr_wrapper<none_tag, type_tuple<T>>> {
			static constexpr auto distributive1(const expr_wrapper<none_tag, type_tuple<T>>& expr) {
				return Distributive2<expr_wrapper<none_tag, type_tuple<T>>>::distributive2(expr);
			}
		};
		template <class Expr>
		struct Distributive1<expr_wrapper<add_tag, type_tuple<Expr>>> {
			static constexpr auto distributive1(const expr_wrapper<add_tag, type_tuple<Expr>>& expr) {
				auto temp = Distributive1<Expr>::distributive1(expr.x_m);
				return Distributive2<expr_wrapper<add_tag, type_tuple<decltype(temp)>>>::distributive2(temp);
			}
		};
		template <class Expr>
		struct Distributive1<expr_wrapper<sub_tag, type_tuple<Expr>>> {
			static constexpr auto distributive1(const expr_wrapper<sub_tag, type_tuple<Expr>>& expr) {
				auto temp = Distributive1<Expr>::distributive1(expr.x_m);
				return Distributive2<expr_wrapper<sub_tag, type_tuple<decltype(temp)>>>::distributive2(temp);
			}
		};
		template <class Expr1, class Expr2>
		struct Distributive1<expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>> {
			static constexpr auto distributive1(const expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>& expr) {
				auto temp1 = Distributive1<Expr1>::distributive1(expr.lhs_m);
				auto temp2 = Distributive1<Expr2>::distributive1(expr.rhs_m);
				return Distributive2<expr_wrapper<add_tag, type_tuple<decltype(temp1), decltype(temp2)>>>::distributive2(
					make_expr_wrapper<add_tag>(temp1, temp2)
				);
			}
		};
		template <class Expr1, class Expr2>
		struct Distributive1<expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>> {
			static constexpr auto distributive1(const expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>& expr) {
				auto temp1 = Distributive1<Expr1>::distributive1(expr.lhs_m);
				auto temp2 = Distributive1<Expr2>::distributive1(expr.rhs_m);
				return Distributive2<expr_wrapper<sub_tag, type_tuple<decltype(temp1), decltype(temp2)>>>::distributive2(
					make_expr_wrapper<sub_tag>(temp1, temp2)
				);
			}
		};
		template <class Expr1, class Expr2>
		struct Distributive1<expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>> {
			static constexpr auto distributive1(const expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>& expr) {
				auto temp1 = Distributive1<Expr1>::distributive1(expr.lhs_m);
				auto temp2 = Distributive1<Expr2>::distributive1(expr.rhs_m);
				return Distributive2<expr_wrapper<mul_tag, type_tuple<decltype(temp1), decltype(temp2)>>>::distributive2(
					make_expr_wrapper<mul_tag>(temp1, temp2)
				);
			}
		};
		template <class Expr1, class Expr2>
		struct Distributive1<expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>> {
			static constexpr auto distributive1(const expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>& expr) {
				auto temp1 = Distributive1<Expr1>::distributive1(expr.lhs_m);
				auto temp2 = Distributive1<Expr2>::distributive1(expr.rhs_m);
				return Distributive2<expr_wrapper<div_tag, type_tuple<decltype(temp1), decltype(temp2)>>>::distributive2(
					make_expr_wrapper<div_tag>(temp1, temp2)
				);
			}
		};
		template <class Expr1, class Expr2>
		struct Distributive1<expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>> {
			static constexpr auto distributive1(const expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>& expr) {
				auto temp1 = Distributive1<Expr1>::distributive1(expr.x_m);
				auto temp2 = Distributive1<Expr2>::distributive1(expr.index_m);
				return Distributive2<expr_wrapper<subscript_tag, type_tuple<decltype(temp1), decltype(temp2)>>>::distributive2(
					make_expr_wrapper<subscript_tag>(temp1, temp2)
				);
			}
		};
		template <class T>
		auto distributive(const T& expr) { return Distributive1<T>::distributive1(expr); }

	}
}


#endif