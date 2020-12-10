#ifndef IMATH_MATH_EXPRESSION_TEMPLATE_MATH_DIFFERENTIAL_HPP
#define IMATH_MATH_EXPRESSION_TEMPLATE_MATH_DIFFERENTIAL_HPP

#include "IMathLib/math/expression_template/expr_wrapper.hpp"


//数式微分

namespace iml {
	namespace op {
		namespace diff_op {

			//乗法零元のタグ
			struct zero_tag {
				//零元を保持
				static constexpr size_t value = 0;
			};
			//乗法単位元タグ
			struct one_tag {
				//単位元の保持
				static constexpr size_t value = 1;
			};

			template <class T>
			struct is_zero_tag : false_type {};
			template <>
			struct is_zero_tag<zero_tag> : true_type {};
			template <class T>
			struct is_one_tag : false_type {};
			template <>
			struct is_one_tag<one_tag> : true_type {};


			//加算
			template <class Expr1, class Expr2, class = void>
			struct Add {
				static constexpr auto add(const Expr1& lhs, const Expr2& rhs) -> expr_wrapper<add_tag, type_tuple<Expr1, Expr2>> {
					return expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>(lhs, rhs);
				}
			};
			template <class Expr>
			struct Add<Expr, zero_tag> {
				static constexpr auto add(const Expr& lhs, const zero_tag&) -> Expr { return lhs; }
			};
			template <class Expr>
			struct Add<zero_tag, Expr, typename enable_if<!is_zero_tag<Expr>::value>::type> {
				static constexpr auto add(const zero_tag&, const Expr& rhs) -> Expr { return rhs; }
			};
			template <class Expr>
			struct Add<Expr, one_tag, typename enable_if<!is_zero_tag<Expr>::value>::type> {
				static constexpr auto add(const Expr& lhs, const one_tag&) -> expr_wrapper<add_tag, type_tuple<Expr, size_t>> {
					return expr_wrapper<add_tag, type_tuple<Expr, size_t>>(lhs, one_tag::value);
				}
			};
			template <class Expr>
			struct Add<one_tag, Expr, typename enable_if<!is_one_tag<Expr>::value && !is_zero_tag<Expr>::value>::type> {
				static constexpr auto add(const one_tag&, const Expr& rhs) -> expr_wrapper<add_tag, type_tuple<size_t, Expr>> {
					return expr_wrapper<add_tag, type_tuple<size_t, Expr>>(one_tag::value, rhs);
				}
			};
			template <class Expr1, class Expr2>
			inline constexpr auto add(const Expr1& lhs, const Expr2& rhs) { return Add<Expr1, Expr2>::add(lhs, rhs); }


			//減算
			template <class Expr1, class Expr2, class = void>
			struct Sub {
				static constexpr auto sub(const Expr1& lhs, const Expr2& rhs) -> expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>> {
					return expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>(lhs, rhs);
				}
			};
			template <class Expr>
			struct Sub<Expr, zero_tag> {
				static constexpr auto sub(const Expr& lhs, const zero_tag&) -> Expr { return lhs; }
			};
			template <class Expr>
			struct Sub<zero_tag, Expr, typename enable_if<!is_zero_tag<Expr>::value>::type> {
				static constexpr auto sub(const zero_tag&, const Expr& rhs) -> expr_wrapper<sub_tag, type_tuple<Expr>> {
					return expr_wrapper<sub_tag, type_tuple<Expr>>(rhs);
				}
			};
			template <class Expr>
			struct Sub<Expr, one_tag, typename enable_if<!is_zero_tag<Expr>::value>::type> {
				static constexpr auto sub(const Expr& lhs, const one_tag&) -> expr_wrapper<sub_tag, type_tuple<Expr, size_t>> {
					return expr_wrapper<sub_tag, type_tuple<Expr, size_t>>(lhs, one_tag::value);
				}
			};
			template <class Expr>
			struct Sub<one_tag, Expr, typename enable_if<!is_one_tag<Expr>::value && !is_zero_tag<Expr>::value>::type> {
				static constexpr auto sub(const one_tag&, const Expr& rhs) -> expr_wrapper<sub_tag, type_tuple<size_t, Expr>> {
					return expr_wrapper<sub_tag, type_tuple<size_t, Expr>>(one_tag::value, rhs);
				}
			};
			template <class Expr1, class Expr2>
			inline constexpr auto sub(const Expr1& lhs, const Expr2& rhs) { return Sub<Expr1, Expr2>::sub(lhs, rhs); }


			//乗算
			template <class Expr1, class Expr2, class = void>
			struct Mul {
				static constexpr auto mul(const Expr1& lhs, const Expr2& rhs) -> expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>> {
					return expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>(lhs, rhs);
				}
			};
			template <class Expr>
			struct Mul<Expr, zero_tag> {
				static constexpr auto mul(const Expr& lhs, const zero_tag&) -> zero_tag { return zero_tag(); }
			};
			template <class Expr>
			struct Mul<zero_tag, Expr, typename enable_if<!is_zero_tag<Expr>::value>::type> {
				static constexpr auto mul(const zero_tag&, const Expr& rhs) -> zero_tag { return zero_tag(); }
			};
			template <class Expr>
			struct Mul<Expr, one_tag, typename enable_if<!is_zero_tag<Expr>::value>::type> {
				static constexpr auto mul(const Expr& lhs, const one_tag&) -> Expr { return lhs; }
			};
			template <class Expr>
			struct Mul<one_tag, Expr, typename enable_if<!is_one_tag<Expr>::value && !is_zero_tag<Expr>::value>::type> {
				static constexpr auto mul(const one_tag&, const Expr& rhs) -> Expr { return rhs; }
			};
			template <class Expr1, class Expr2>
			inline constexpr auto mul(const Expr1& lhs, const Expr2& rhs) { return Mul<Expr1, Expr2>::mul(lhs, rhs); }


			//除算
			template <class Expr1, class Expr2, class = void>
			struct Div {
				static constexpr auto div(const Expr1& lhs, const Expr2& rhs) -> expr_wrapper<div_tag, type_tuple<Expr1, Expr2>> {
					return expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>(lhs, rhs);
				}
			};
			template <class Expr>
			struct Div<Expr, zero_tag> {
				//呼び出されないはず
			};
			template <class Expr>
			struct Div<zero_tag, Expr, typename enable_if<!is_zero_tag<Expr>::value>::type> {
				static constexpr auto div(const zero_tag&, const Expr& rhs) -> zero_tag { return zero_tag; }
			};
			template <class Expr>
			struct Div<Expr, one_tag, typename enable_if<!is_zero_tag<Expr>::value>::type> {
				//呼び出されないはず
			};
			template <class Expr>
			struct Div<one_tag, Expr, typename enable_if<!is_one_tag<Expr>::value && !is_zero_tag<Expr>::value>::type> {
				static constexpr auto div(const one_tag&, const Expr& rhs) -> expr_wrapper<div_tag, type_tuple<size_t, Expr>> {
					return expr_wrapper<div_tag, type_tuple<Expr, size_t>>(one_tag::value, rhs);
				}
			};
			template <class Expr1, class Expr2>
			inline constexpr auto div(const Expr1& lhs, const Expr2& rhs) { return Div<Expr1, Expr2>::div(lhs, rhs); }
		}

		//数式微分
		template <size_t N, class T>
		struct Differential {
			static constexpr auto differential(const T& expr) {
				return diff_op::zero_tag();
			}
		};
		template <size_t N>
		struct Differential<N, expr_variable<N>> {
			static constexpr auto differential(const expr_variable<N>& expr) {
				return diff_op::one_tag();
			}
		};
		template <size_t N, class Expr>
		struct Differential<N, expr_wrapper<add_tag, type_tuple<Expr>>> {
			static constexpr auto differential(const expr_wrapper<add_tag, type_tuple<Expr>>& expr) {
				return Differential<N, Expr>::differential(expr.x_m);
			}
		};
		template <size_t N, class Expr>
		struct Differential<N, expr_wrapper<sub_tag, type_tuple<Expr>>> {
			static constexpr auto differential(const expr_wrapper<sub_tag, type_tuple<Expr>>& expr) {
				return -Differential<N, Expr>::differential(expr.x_m);
			}
		};
		template <size_t N, class Expr1, class Expr2>
		struct Differential<N, expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>> {
			static constexpr auto differential(const expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>& expr) {
				return diff_op::add(Differential<N, Expr1>::differential(expr.lhs_m), Differential<N, Expr2>::differential(expr.rhs_m));
			}
		};
		template <size_t N, class Expr1, class Expr2>
		struct Differential<N, expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>> {
			static constexpr auto differential(const expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>& expr) {
				return diff_op::sub(Differential<N, Expr1>::differential(expr.lhs_m), Differential<N, Expr2>::differential(expr.rhs_m));
			}
		};
		template <size_t N, class Expr1, class Expr2>
		struct Differential<N, expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>> {
			static constexpr auto differential(const expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>& expr) {
				return diff_op::add(diff_op::mul(Differential<N, Expr1>::differential(expr.lhs_m), expr.rhs_m)
					, diff_op::mul(expr.lhs_m, Differential<N, Expr2>::differential(expr.rhs_m)));
			}
		};
		template <size_t N, class Expr1, class Expr2>
		struct Differential<N, expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>> {
			static constexpr auto differential(const expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>& expr) {
				return diff_op::div(diff_op::sub(diff_op::mul(Differential<N, Expr1>::differential(expr.lhs_m), expr.rhs_m)
					, diff_op::mul(expr.lhs_m, Differential<N, Expr2>::differential(expr.rhs_m)))
					, diff_op::mul(expr.rhs_m, expr.rhs_m));
			}
		};
		template <size_t N, class Expr1, class Expr2>
		struct Differential<N, expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>> {
			static constexpr auto differential(const expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>& expr) {
				return Differential<N, Expr1>::differential(expr.lhs_m)[expr.rhs_m];
			}
		};
		template <class T>
		inline constexpr auto differential_impl(T&& expr) { return expr; }
		template <>
		inline constexpr auto differential_impl(diff_op::zero_tag&&) { return 0; }
		template <>
		inline constexpr auto differential_impl(diff_op::one_tag&&) { return 1; }
		//N番目の変数で偏微分
		template <size_t N, class T>
		inline constexpr auto differential(const T& expr) {
			return differential_impl(Differential<N, T>::differential(expr));
		}
	}
}


#endif