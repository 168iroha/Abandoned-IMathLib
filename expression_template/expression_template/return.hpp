#ifndef IMATH_MATH_EXPRESSION_TEMPLATE_RETURN_HPP
#define IMATH_MATH_EXPRESSION_TEMPLATE_RETURN_HPP

#include "IMathLib/math/expression_template/expr_wrapper.hpp"
#include "IMathLib/math/expression_template/control_syntax.hpp"
#include "IMathLib/utility/tuple.hpp"


//return文の記述

namespace iml {
	namespace op {


		//return識別子(return)
		using return_tag = ident_tag<4>;


		//return
		template <class Expr>
		struct expr_wrapper<return_tag, type_tuple<Expr>> {
			Expr x_m;

			constexpr explicit expr_wrapper(const Expr& expr) : x_m(expr) {}
		};

		//returnを生成するためのメソッド(intはダミー)
		template <>
		struct expr_wrapper<return_tag, type_tuple<int, int>> {
			//returnに対する入力という意
			template <class T>
			constexpr expr_wrapper<return_tag, type_tuple<T>> operator<<(const T& expr) const {
				return expr_wrapper<return_tag, type_tuple<T>>(expr);
			}
		};
		static constexpr expr_wrapper<return_tag, type_tuple<int, int>> _return_;

		//returnに対するEval
		template <class Expr>
		struct Eval<expr_wrapper<return_tag, type_tuple<Expr>>> {
			template <class Result, class Tuple>
			static constexpr void eval(Result& result, const expr_wrapper<return_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				result.first = Eval<Expr>::eval(result, expr.x_m, std::forward<Tuple>(t));
				result.second = false;
			}
			template <class Tuple>
			static constexpr void eval(const expr_wrapper<return_tag, type_tuple<Expr>>&, Tuple&&) {}
		};


		//戻り値型推論
		//template <class T, class = typename enable_if<is_same< typename expr_tag<T>::type, return_tag>::value>::type>
		//struct return_type_inference {};


		//戻り値型情報
		template <class T>
		struct Result_info {
			T expr_m;

			using expr_type = T;

			constexpr explicit Result_info(const T& expr) : expr_m(expr) {}
		};


		//ラムダ式
		template <class Expr, class Result>
		struct lambda_functor {
			Expr x_m;
			Result type_m;			//戻り値型情報

			constexpr explicit lambda_functor(const Expr& expr, const Result& type) : x_m(expr), type_m(type) {}

			//オブジェクトの呼び出し
			template <class... Types>
			constexpr auto operator()(Types&&... args) {
				//戻り値型の決定
				using result_type = typename decay<decltype(Eval<typename Result::expr_type>::eval(type_m.expr_m, forward_as_tuple(std::forward<Types>(args)...)))>::type;
				//boolはreturnフラグが立っていないか
				pair<result_type, bool> result(result_type(), true);
				Eval<Expr>::eval(result, x_m, forward_as_tuple(std::forward<Types>(args)...));
				return result.first;
			}
			template <class... Types>
			constexpr auto operator()(Types&&... args) const {
				//戻り値型の決定
				using result_type = typename decay<decltype(Eval<typename Result::expr_type>::eval(type_m.expr_m, forward_as_tuple(std::forward<Types>(args)...)))>::type;
				//boolはreturnフラグが立っていないか
				pair<result_type, bool> result(result_type(), true);
				Eval<Expr>::eval(result, x_m, forward_as_tuple(std::forward<Types>(args)...));
				return result.first;
			}
		};


		//auto識別子
		struct Auto {};
		static constexpr Auto _auto_;


		//lambda_functorを生成するためのメソッド
		template <class ResultT>
		struct Lambda2 {
			ResultT type_m;			//戻り値型情報

			constexpr explicit Lambda2(const ResultT& expr) : type_m(expr) {}

			//戻り値型情報により戻り値型を決定してLambdaを構築(現状は未実装)
			template <class Expr>
			constexpr lambda_functor<Expr, ResultT> operator[](const Expr& expr) const {
				return lambda_functor<Expr, ResultT>(expr, type_m);
			}
		};
		struct Lambda1 {

			//戻り値型情報の登録
			template <class Expr>
			constexpr Lambda2<Result_info<Expr>> result_info(const Expr& expr) const {
				return Lambda2<Result_info<Expr>>(Result_info<Expr>(expr));
			}

			//Lambdaの後置戻り値型を再現
			const constexpr Lambda1* operator->() const { return this; }
			
			//戻り値型推論により戻り値型を決定してLambdaを構築(現状は未実装)
			template <class Expr>
			constexpr lambda_functor<Expr, Result_info<Auto>> operator[](const Expr& expr) const {
				return lambda_functor<Expr, Result_info<Auto>>(expr, Result_info<Auto>(_auto_));
			}
		};
		static constexpr Lambda1 lambda;


		//ラムダ式の評価
		template <class Expr, class Result>
		struct Eval<lambda_functor<Expr, Result>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const lambda_functor<Expr, Result>& expr, Tuple&& t) {
				return Eval<Expr>::eval(result, expr.x_m, std::forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const lambda_functor<Expr, Result>& expr, Tuple&& t) {
				return Eval<Expr>::eval(expr.x_m, std::forward<Tuple>(t));
			}
		};

	}

}


#endif