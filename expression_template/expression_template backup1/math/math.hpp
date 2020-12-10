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


			//単項演算
			constexpr expr_wrapper<add_tag, type_tuple<expr_wrapper>> operator+() const {
				return expr_wrapper<add_tag, type_tuple<expr_wrapper>>(*this);
			}
			constexpr expr_wrapper<sub_tag, type_tuple<expr_wrapper>> operator-() const {
				return expr_wrapper<sub_tag, type_tuple<expr_wrapper>>(*this);
			}
			constexpr expr_wrapper<mul_tag, type_tuple<expr_wrapper>> operator*() const {
				return expr_wrapper<mul_tag, type_tuple<expr_wrapper>>(*this);
			}
			constexpr expr_wrapper<flip_tag, type_tuple<expr_wrapper>> operator!() const {
				return expr_wrapper<flip_tag, type_tuple<expr_wrapper>>(*this);
			}
			constexpr expr_wrapper<bit_flip_tag, type_tuple<expr_wrapper>> operator~() const {
				return expr_wrapper<bit_flip_tag, type_tuple<expr_wrapper>>(*this);
			}
			//インクリメント・デクリメント
			constexpr expr_wrapper<increment_tag, type_tuple<expr_wrapper>> operator++() const {
				return expr_wrapper<increment_tag, type_tuple<expr_wrapper>>(*this);
			}
			constexpr expr_wrapper<increment_tag, type_tuple<expr_wrapper, int>> operator++(int) const {
				return expr_wrapper<increment_tag, type_tuple<expr_wrapper>>(*this, 0);
			}
			constexpr expr_wrapper<decrement_tag, type_tuple<expr_wrapper>> operator--() const {
				return expr_wrapper<decrement_tag, type_tuple<expr_wrapper>>(*this);
			}
			constexpr expr_wrapper<decrement_tag, type_tuple<expr_wrapper, int>> operator--(int) const {
				return expr_wrapper<decrement_tag, type_tuple<expr_wrapper>>(*this, 0);
			}

			//メンバポインタアクセス
			template <class U>
			constexpr expr_wrapper<arrow_ast_tag, type_tuple<expr_wrapper, U>> operator->*(const U& expr) const {
				return expr_wrapper<arrow_ast_tag, type_tuple<expr_wrapper, U>>(*this, expr);
			}
			//添え字アクセス
			template <class U>
			constexpr expr_wrapper<subscript_tag, type_tuple<expr_wrapper, U>> operator[](const U& index) const {
				return expr_wrapper<subscript_tag, type_tuple<expr_wrapper, U>>(*this, index);
			}

			//代入演算
			template <class U>
			constexpr expr_wrapper<assign_tag, type_tuple<expr_wrapper, U>> operator=(const U& expr) const {
				return expr_wrapper<assign_tag, type_tuple<expr_wrapper, U>>(*this, expr);
			}
			template <class U>
			constexpr expr_wrapper<add_assign_tag, type_tuple<expr_wrapper, U>> operator+=(const U& expr) const {
				return expr_wrapper<add_assign_tag, type_tuple<expr_wrapper, U>>(*this, expr);
			}
			template <class U>
			constexpr expr_wrapper<sub_assign_tag, type_tuple<expr_wrapper, U>> operator-=(const U& expr) const {
				return expr_wrapper<sub_assign_tag, type_tuple<expr_wrapper, U>>(*this, expr);
			}
			template <class U>
			constexpr expr_wrapper<mul_assign_tag, type_tuple<expr_wrapper, U>> operator*=(const U& expr) const {
				return expr_wrapper<mul_assign_tag, type_tuple<expr_wrapper, U >>(*this, expr);
			}
			template <class U>
			constexpr expr_wrapper<div_assign_tag, type_tuple<expr_wrapper, U>> operator/=(const U& expr) const {
				return expr_wrapper<div_assign_tag, type_tuple<expr_wrapper, U>>(*this, expr);
			}
			template <class U>
			constexpr expr_wrapper<surplus_assign_tag, type_tuple<expr_wrapper, U>> operator%=(const U& expr) const {
				return expr_wrapper<surplus_assign_tag, type_tuple<expr_wrapper, U>>(*this, expr);
			}
			template <class U>
			constexpr expr_wrapper<lshift_assign_tag, type_tuple<expr_wrapper, U>> operator<<=(const U& expr) const {
				return expr_wrapper<lshift_assign_tag, type_tuple<expr_wrapper, U>>(*this, expr);
			}
			template <class U>
			constexpr expr_wrapper<rshift_assign_tag, type_tuple<expr_wrapper, U>> operator>>=(const U& expr) const {
				return expr_wrapper<rshift_assign_tag, type_tuple<expr_wrapper, U>>(*this, expr);
			}
			template <class U>
			constexpr expr_wrapper<bit_and_assign_tag, type_tuple<expr_wrapper, U>> operator&=(const U& expr) const {
				return expr_wrapper<bit_and_assign_tag, type_tuple<expr_wrapper, U>>(*this, expr);
			}
			template <class U>
			constexpr expr_wrapper<bit_or_assign_tag, type_tuple<expr_wrapper, U>> operator|=(const U& expr) const {
				return expr_wrapper<bit_or_assign_tag, type_tuple<expr_wrapper, U>>(*this, expr);
			}
			template <class U>
			constexpr expr_wrapper<bit_xor_assign_tag, type_tuple<expr_wrapper, U>> operator^=(const U& expr) const {
				return expr_wrapper<bit_xor_assign_tag, type_tuple<expr_wrapper, U>>(*this, expr);
			}

			//コンマ演算子
			template <class Expr>
			constexpr expr_wrapper<comma_tag, type_tuple<expr_wrapper, Expr>> operator,(const Expr& expr) const {
				return expr_wrapper<comma_tag, type_tuple<expr_wrapper, Expr>>(*this, expr);
			}

			//関数オブジェクトの呼び出し
			template <class... Types>
			constexpr expr_wrapper<functor_tag, type_tuple<expr_wrapper, tuple<Types...>, typename tuple<Types...>::sequence_type>> operator()(Types&&... args) const {
				return expr_wrapper<functor_tag, type_tuple<expr_wrapper, tuple<Types...>, typename tuple<Types...>::sequence_type>>(*this, forward<Types>(args)...);
			}
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