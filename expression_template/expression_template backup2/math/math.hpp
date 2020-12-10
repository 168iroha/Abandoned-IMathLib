#ifndef IMATH_MATH_EXPRESSION_TEMPLATE_MATH_MATH_HPP
#define IMATH_MATH_EXPRESSION_TEMPLATE_MATH_MATH_HPP

#include "IMathLib/math/expression_template/expr_wrapper.hpp"
#include "IMathLib/utility/tuple.hpp"


//Expression templateにおける数学関数

namespace iml {
	namespace op {

		//数学関数のタグ
		struct math_function_tag {};


		//数学関数のexpr_wrapper
		template <class Struct, class Args, class Sequence>
		struct expr_wrapper<math_function_tag, type_tuple<Struct, Args, Sequence>> {
			Args args_m;

			template <class... Types>
			constexpr explicit expr_wrapper(Types&&... args) : args_m(forward<Types>(args)...) {}

			EXPR_WRAPPER_OPERATOR_OVERLOAD;
		};


		//数学関数のEval
		template <class Struct, class... Types, size_t... Indices>
		struct Eval<expr_wrapper<math_function_tag, type_tuple<Struct, tuple<Types...>, index_tuple<size_t, Indices...>>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<math_function_tag, type_tuple<Struct, tuple<Types...>, index_tuple<size_t, Indices...>>>& expr, Tuple&& t) {
				return Struct::eval(Eval<Types>::eval(expr.args_m.get<Indices>(), forward<Tuple>(t))...);
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<math_function_tag, type_tuple<Struct, tuple<Types...>, index_tuple<size_t, Indices...>>>& expr, Tuple&& t) {
				return Struct::eval(Eval<Types>::eval(expr.args_m.get<Indices>(), forward<Tuple>(t))...);
			}
		};

	}
}


#endif