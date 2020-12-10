#ifndef IMATHLIB_H_MATH_EXPRESSION_TEMPLATE_HPP
#define IMATHLIB_H_MATH_EXPRESSION_TEMPLATE_HPP

#include "IMathLib/math/expression_template/control_syntax.hpp"
#include "IMathLib/math/expression_template/expr_wrapper.hpp"
#include "IMathLib/math/expression_template/return.hpp"


//数式で数学関数を扱うためのもの
#include "IMathLib/math/expression_template/math/associative.hpp"
#include "IMathLib/math/expression_template/math/cos.hpp"
#include "IMathLib/math/expression_template/math/differential.hpp"
#include "IMathLib/math/expression_template/math/distributive.hpp"
#include "IMathLib/math/expression_template/math/math.hpp"
#include "IMathLib/math/expression_template/math/sin.hpp"


namespace iml {
	namespace op {

		//expr_wrapperのための数式の文字列生成
		template <class T>
		struct Estring {
			using expr_type = T;
			static auto estring(const expr_type& expr) {
				std::stringstream ss;
				ss << expr;
				return ss.str();
			}
		};
		template <class T>
		struct Estring<expr_wrapper<none_tag, type_tuple<T>>> {
			using expr_type = expr_wrapper<none_tag, type_tuple<T>>;
			static auto estring(const expr_type& expr) {
				std::stringstream ss;
				ss << expr.name_m;
				return ss.str();
			}
		};
		//単項演算
		template <class Expr>
		struct Estring<expr_wrapper<add_tag, type_tuple<Expr>>> {
			using expr_type = expr_wrapper<add_tag, type_tuple<Expr>>;
			static auto estring(const expr_type& expr) {
				std::stringstream ss;
				if constexpr (operator_precedence<Expr>::value >= operator_precedence<expr_type>::value) {
					ss << "+(" << Estring<Expr>::estring(expr.terms_m) << ')';
				}
				else {
					ss << '+' << Estring<Expr>::estring(expr.terms_m);
				}
				return ss.str();
			}
		};
		template <class Expr>
		struct Estring<expr_wrapper<sub_tag, type_tuple<Expr>>> {
			using expr_type = expr_wrapper<sub_tag, type_tuple<Expr>>;
			static auto estring(const expr_type& expr) {
				std::stringstream ss;
				if constexpr (operator_precedence<Expr>::value >= operator_precedence<expr_type>::value) {
					ss << "-(" << Estring<Expr>::estring(expr.terms_m) << ')';
				}
				else {
					ss << '-' << Estring<Expr>::estring(expr.terms_m);
				}
				return ss.str();
			}
		};
		//2項演算
		template <class, class T>
		struct Estring_tuple {
			static constexpr auto estring(const T& x, index_tuple<size_t>) { return Estring<T>::estring(x); }
		};
#define ESTRIGN_BINARY_OPERATION(NAME, OP)\
		template <class First, class... Types>\
		struct Estring_tuple<NAME##_tag, tuple<First, Types...>> {\
			template <class T>\
			static constexpr auto estring_impl(const T& x) {\
				std::stringstream ss;\
				if constexpr (is_tuple_v<T> || (operator_precedence<T>::value >= operator_precedence<expr_wrapper<NAME##_tag, type_tuple<First, Types...>>>::value)) {\
					ss << "(" << Estring_tuple<NAME##_tag, T>::estring(x, index_range_t<size_t, 1, tuple_size_v<T>>()) << ')';\
				}\
				else {\
					ss << Estring_tuple<NAME##_tag, T>::estring(x, index_range_t<size_t, 1, tuple_size_v<T>>());\
				}\
				return ss.str();\
			}\
			template <size_t... Indices>\
			static constexpr auto estring(const tuple<First, Types...>& x, index_tuple<size_t, Indices...>) {\
				std::stringstream ss;\
				ss << Estring_tuple<NAME##_tag, First>::estring(x.get<0>(), index_range_t<size_t, 1, tuple_size_v<First>>());\
				(ss << ... << ((#OP) + estring_impl(x.get<Indices>())));\
				return ss.str();\
			}\
		};
		ESTRIGN_BINARY_OPERATION(add, +);
		ESTRIGN_BINARY_OPERATION(sub, -);
		ESTRIGN_BINARY_OPERATION(mul, *);
		ESTRIGN_BINARY_OPERATION(div, / );
#undef ESTRIGN_BINARY_OPERATION
		template <class Op, class First, class Second, class... Types>
		struct Estring<expr_wrapper<Op, type_tuple<First, Second, Types...>>> {
			static constexpr auto estring(const expr_wrapper<Op, type_tuple<First, Second, Types...>>& expr) {
				return Estring_tuple<Op, tuple<First, Second, Types...>>::estring(expr.terms_m
					, index_range_t<size_t, 1, 2 + sizeof...(Types)>());
			}
		};

		template <class Expr1, class Expr2>
		struct Estring<expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>> {
			using expr_type = expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>;
			static auto estring(const expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>& expr) {
				std::stringstream ss;
				if constexpr (operator_precedence<Expr1>::value > operator_precedence<expr_type>::value) {
					ss << "(" << Estring<Expr1>::estring(expr.terms_m.get<0>()) << ')';
				}
				else {
					ss << Estring<Expr1>::estring(expr.get<0>());
				}
				ss << '[' << Estring<Expr2>::estring(expr.get<1>()) << ']';
				return ss.str();
			}
		};
		template <class T>
		auto estring(const T& expr) { return Estring<T>::estring(expr); }

		template <class T, class = std::enable_if_t<is_expr_wrapper<T>::value>>
		std::ostream& operator<<(std::ostream& os, const T& expr) {
			os << estring(expr);
			return os;
		}

	}
}

namespace iml {
	namespace op {

		//std::coutを識別子として登録
		using cout_tag = ident_tag<100>;

		//std::coutがコピー不可オブジェクトのためそのホルダのようなもの(intは型推論を機能させるためのダミー)
		template <>
		struct expr_wrapper<cout_tag, type_tuple<int, int>> {
			template <class Expr>
			expr_wrapper<cout_tag, type_tuple<Expr>> operator<<(const Expr& expr) {
				return expr_wrapper<cout_tag, type_tuple<Expr>>(expr);
			}
		};
		expr_wrapper<cout_tag, type_tuple<int, int>> ch;

		template <class Expr>
		struct Eval<expr_wrapper<cout_tag, type_tuple<Expr>>> {
			template <class Result, class Tuple>
			static constexpr void eval(Result& result, const expr_wrapper<cout_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				std::cout << Eval<Expr>::eval(result, expr.x_m, std::forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr void eval(const expr_wrapper<cout_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				std::cout << Eval<Expr>::eval(expr.x_m, std::forward<Tuple>(t));
			}
		};



		template <class Expr>
		struct Estring<expr_wrapper<math_function_tag, type_tuple<iml::Sin<Expr>, tuple<Expr>, index_tuple<size_t, 0>>>> {
			using expr_type = expr_wrapper<math_function_tag, type_tuple<iml::Sin<Expr>, tuple<Expr>, index_tuple<size_t, 0>>>;
			static auto estring(const expr_type& expr) {
				std::stringstream ss;
				ss << "sin(" << Estring<Expr>::estring(expr.args_m.get<0>()) << ")";
				return ss.str();
			}
		};
		template <class Expr>
		struct Estring<expr_wrapper<math_function_tag, type_tuple<iml::Cos<Expr>, tuple<Expr>, index_tuple<size_t, 0>>>> {
			using expr_type = expr_wrapper<math_function_tag, type_tuple<iml::Cos<Expr>, tuple<Expr>, index_tuple<size_t, 0>>>;
			static auto estring(const expr_type& expr) {
				std::stringstream ss;
				ss << "cos(" << Estring<Expr>::estring(expr.args_m.get<0>()) << ")";
				return ss.str();
			}
		};
	}
}

#endif