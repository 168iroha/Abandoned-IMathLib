#ifndef IMATH_MATH_EXPRESSION_TEMPLATE_MATH_COS_HPP
#define IMATH_MATH_EXPRESSION_TEMPLATE_MATH_COS_HPP

#include "IMathLib/math/expression_template/expr_wrapper.hpp"
#include "IMathLib/math/expression_template/math/math.hpp"
#include "IMathLib/math/expression_template/math/differential.hpp"
#include "IMathLib/math/math.hpp"

namespace iml {

	template <class Op, class Expr>
	struct Cos<op::expr_wrapper<Op, Expr>> {
		using expr_type = op::expr_wrapper<Op, Expr>;
		using result_type = op::expr_wrapper<op::math_function_tag, type_tuple<Cos<expr_type>, tuple<expr_type>, typename tuple<expr_type>::sequence_type>>;

		static constexpr result_type _cos_(const expr_type& x) {
			return result_type(x);
		}

		template <class... Types>
		static constexpr auto eval(Types&&... args) { return cos(std::forward<Types>(args)...); }
	};

	namespace op {

		//cosの偏導関数
		template <size_t N, class Expr>
		struct Differential<N, expr_wrapper<math_function_tag, type_tuple<iml::Cos<Expr>, tuple<Expr>, index_tuple<size_t, 0>>>> {
			static constexpr auto differential(const expr_wrapper<math_function_tag, type_tuple<iml::Cos<Expr>, tuple<Expr>, index_tuple<size_t, 0>>>& expr) {
				return -iml::sin(expr.args_m.get<0>())*Differential<N, Expr>::differential(expr.args_m.get<0>());
			}
		};
	}
}


#endif