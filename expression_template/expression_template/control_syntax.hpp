#ifndef IMATH_MATH_EXPRESSION_TEMPLATE_CONTROL_SYNTAX_HPP
#define IMATH_MATH_EXPRESSION_TEMPLATE_CONTROL_SYNTAX_HPP

#include "IMathLib/math/expression_template/expr_wrapper.hpp"
#include "IMathLib/utility/tuple.hpp"


//if文等の制御構文の定義

namespace iml {
	namespace op {


		//if用のスコープ([])
		using if_scope_tag = ident_tag<0>;
		//if文(if)
		using if_tag = ident_tag<1>;


		//ifためのスコープ
		template <class Terms, class Content, class... Orders>
		struct expr_wrapper<if_scope_tag, type_tuple<pair<Terms, Content>, Orders...>>
			: expr_wrapper<if_tag, type_tuple<pair<Terms, Content>, Orders...>> {
			using else_type = expr_wrapper<if_tag, type_tuple<type_tuple<bool>, pair<Terms, Content>, Orders...>>;
			else_type _else_;

			constexpr explicit expr_wrapper(const tuple<pair<Terms, Content>, Orders...>& x)
				: _else_(true, x), expr_wrapper<if_tag, type_tuple<pair<Terms, Content>, Orders...>>(x) {}

			//else if文の宣言
			template <class U>
			constexpr expr_wrapper<if_tag, type_tuple<type_tuple<U>, pair<Terms, Content>, Orders...>> _else_if_(const U& expr) {
				return expr_wrapper<if_tag, type_tuple<type_tuple<U>, pair<Terms, Content>, Orders...>>(expr, x_m);
			}

			//コンマ演算子
			template <class Expr>
			constexpr expr_wrapper<comma_tag, type_tuple<expr_wrapper, Expr>> operator,(const Expr& expr) const {
				return expr_wrapper<comma_tag, type_tuple<expr_wrapper, Expr>>(*this, expr);
			}
		};
		//ifを生成するためのメソッド(Exprは条件式)
		template <class Expr, class Order, class... Orders>
		struct expr_wrapper<if_tag, type_tuple<type_tuple<Expr>, Order, Orders...>> {
			Expr x_m;
			tuple<Order, Orders...> rest_m;

			constexpr explicit expr_wrapper(const Expr& expr, const tuple<Order, Orders...>& rest) : x_m(expr), rest_m(rest) {}

			template <class U>
			constexpr expr_wrapper<if_scope_tag, type_tuple<pair<Expr, U>, Order, Orders...>> operator[](const U& expr) const {
				return expr_wrapper<if_scope_tag, type_tuple<pair<Expr, U>, Order, Orders...>>(
					iml::tuple_cat(tuple<pair<Expr, U>>(pair<Expr, U>(x_m, expr)), rest_m));
			}
		};
		template <class Expr>
		struct expr_wrapper<if_tag, type_tuple<type_tuple<Expr>>> {
			Expr x_m;

			constexpr explicit expr_wrapper(const Expr& expr) : x_m(expr) {}

			template <class U>
			constexpr expr_wrapper<if_scope_tag, type_tuple<pair<Expr, U>>> operator[](const U& expr) const {
				return expr_wrapper<if_scope_tag, type_tuple<pair<Expr, U>>>(tuple<pair<Expr, U>>(pair<Expr, U>(x_m, expr)));
			}
		};
		//if文
		template <class Terms, class Content, class... Orders>
		struct expr_wrapper<if_tag, type_tuple<pair<Terms, Content>, Orders...>> {
			tuple<pair<Terms, Content>, Orders...> x_m;

			constexpr explicit expr_wrapper(const tuple<pair<Terms, Content>, Orders...>& x) : x_m(x) {}
		};
		template <class Expr>
		constexpr auto _if_(const Expr& expr) { return expr_wrapper<if_tag, type_tuple<type_tuple<Expr>>>(expr); }


		//ifのためのEval
		template <size_t N, class T, bool = (N + 1 < T::value)>
		struct Eval_if;
		template <size_t N, class... Orders>
		struct Eval_if<N, type_tuple<Orders...>, true> {
			//戻り値はelse ifおよびelseを実行させるか否か
			template <class Result, class Tuple>
			static constexpr bool eval_if(Result& result, const tuple<Orders...>& expr, Tuple&& t) {
				bool temp = Eval_if<N + 1, type_tuple<Orders...>>::eval_if(result, expr, std::forward<Tuple>(t));
				if (result.second && Eval<typename decay<decltype(expr.get<N>().first)>::type>::eval(result, expr.get<N>().first, std::forward<Tuple>(t)) && temp) {
					Eval<typename decay<decltype(expr.get<N>().second)>::type>::eval(result, expr.get<N>().second, std::forward<Tuple>(t));
					return false;
				}
				return temp;
			}
			template <class Tuple>
			static constexpr bool eval_if(const tuple<Orders...>& expr, Tuple&& t) {
				bool temp = Eval_if<N + 1, type_tuple<Orders...>>::eval_if(expr, std::forward<Tuple>(t));
				if (Eval<typename decay<decltype(expr.get<N>().first)>::type>::eval(expr.get<N>().first, std::forward<Tuple>(t)) && temp) {
					Eval<typename decay<decltype(expr.get<N>().second)>::type>::eval(expr.get<N>().second, std::forward<Tuple>(t));
					return false;
				}
				return temp;
			}
		};
		template <size_t N, class... Orders>
		struct Eval_if<N, type_tuple<Orders...>, false> {
			//戻り値はelse ifおよびelseを実行させるか否か
			template <class Result, class Tuple>
			static constexpr bool eval_if(Result& result, const tuple<Orders...>& expr, Tuple&& t) {
				if (result.second && Eval<typename decay<decltype(expr.get<N>().first)>::type>::eval(result, expr.get<N>().first, std::forward<Tuple>(t))) {
					Eval<typename decay<decltype(expr.get<N>().second)>::type>::eval(result, expr.get<N>().second, std::forward<Tuple>(t));
					return false;
				}
				return true;
			}
			template <class Tuple>
			static constexpr bool eval_if(const tuple<Orders...>& expr, Tuple&& t) {
				if (Eval<typename decay<decltype(expr.get<N>().first)>::type>::eval(expr.get<N>().first, std::forward<Tuple>(t))) {
					Eval<typename decay<decltype(expr.get<N>().second)>::type>::eval(expr.get<N>().second, std::forward<Tuple>(t));
					return false;
				}
				return true;
			}
		};
		//if_tagの場合のEval
		template <class... Orders>
		struct Eval<expr_wrapper<if_tag, type_tuple<Orders...>>> {
			template <class Result, class Tuple>
			static constexpr void eval(Result& result, const expr_wrapper<if_tag, type_tuple<Orders...>>& expr, Tuple&& t) {
				Eval_if<0, type_tuple<Orders...>>::eval_if(result, expr.x_m, std::forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr void eval(const expr_wrapper<if_tag, type_tuple<Orders...>>& expr, Tuple&& t) {
				Eval_if<0, type_tuple<Orders...>>::eval_if(expr.x_m, std::forward<Tuple>(t));
			}
		};
		//if_scope_tagの場合のEval
		template <class... Orders>
		struct Eval<expr_wrapper<if_scope_tag, type_tuple<Orders...>>> {
			template <class Result, class Tuple>
			static constexpr void eval(Result& result, const expr_wrapper<if_scope_tag, type_tuple<Orders...>>& expr, Tuple&& t) {
				Eval_if<0, type_tuple<Orders...>>::eval_if(result, expr.x_m, std::forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr void eval(const expr_wrapper<if_scope_tag, type_tuple<Orders...>>& expr, Tuple&& t) {
				Eval_if<0, type_tuple<Orders...>>::eval_if(expr.x_m, std::forward<Tuple>(t));
			}
		};



		//while文(while)
		using while_tag = ident_tag<2>;


		template <class Expr>
		struct expr_wrapper<while_tag, type_tuple<Expr>> {
			Expr x_m;

			constexpr explicit expr_wrapper(const Expr& expr) : x_m(expr) {}

			template <class U>
			constexpr expr_wrapper<while_tag, type_tuple<Expr, U>> operator[](const U& expr) const {
				return expr_wrapper<while_tag, type_tuple<Expr, U>>(x_m, expr);
			}
		};
		template <class Expr>
		constexpr auto _while_(const Expr& expr) { return expr_wrapper<while_tag, type_tuple<Expr>>(expr); }

		//while文
		template <class Terms, class Content>
		struct expr_wrapper<while_tag, type_tuple<Terms, Content>> {
			Terms terms_m;
			Content content_m;

			constexpr explicit expr_wrapper(const Terms& terms, const Content& content) : terms_m(terms), content_m(content) {}
		};

		//whileに対するEval
		template <class Terms, class Content>
		struct Eval<expr_wrapper<while_tag, type_tuple<Terms, Content>>> {
			template <class Result, class Tuple>
			static constexpr void eval(Result& result, const expr_wrapper<while_tag, type_tuple<Terms, Content>>& expr, Tuple&& t) {
				while (result.second && Eval<Terms>::eval(result, expr.terms_m, std::forward<Tuple>(t))) {
					Eval<Content>::eval(result, expr.content_m, std::forward<Tuple>(t));
				}
			}
			template <class Tuple>
			static constexpr void eval(const expr_wrapper<while_tag, type_tuple<Terms, Content>>& expr, Tuple&& t) {
				while (Eval<Terms>::eval(expr.terms_m, std::forward<Tuple>(t))) {
					Eval<Content>::eval(expr.content_m, std::forward<Tuple>(t));
				}
			}
		};



		//do-while文(do-while)
		using do_while_tag = ident_tag<3>;

		template <class Expr>
		struct expr_wrapper<do_while_tag, type_tuple<Expr>> {
			Expr x_m;

			constexpr explicit expr_wrapper(const Expr& x_m) : x_m(x_m) {}

			//条件式の記述
			template <class U>
			expr_wrapper<do_while_tag, type_tuple<U, Expr>> _while_(const U& expr) {
				return expr_wrapper<do_while_tag, type_tuple<U, Expr>>(expr, x_m);
			}
		};
		//do-while文
		template <class Terms, class Content>
		struct expr_wrapper<do_while_tag, type_tuple<Terms, Content>> {
			Terms terms_m;
			Content content_m;

			constexpr explicit expr_wrapper(const Terms& terms, const Content& content) : terms_m(terms), content_m(content) {}
		};
		//do-while文を生成するためのメソッド
		struct Do_while {
			template <class U>
			constexpr expr_wrapper<do_while_tag, type_tuple<U>> operator[](const U& expr) const {
				return expr_wrapper<do_while_tag, type_tuple<U>>(expr);
			}
		};
		static constexpr Do_while _do_;

		//do-whileに対するEval
		template <class Terms, class Content>
		struct Eval<expr_wrapper<do_while_tag, type_tuple<Terms, Content>>> {
			template <class Result, class Tuple>
			static constexpr void eval(Result& result, const expr_wrapper<do_while_tag, type_tuple<Terms, Content>>& expr, Tuple&& t) {
				do {
					Eval<Content>::eval(result, expr.content_m, std::forward<Tuple>(t));
				} while (result.second && Eval<Terms>::eval(result, expr.terms_m, std::forward<Tuple>(t)));
			}
			template <class Tuple>
			static constexpr void eval(const expr_wrapper<do_while_tag, type_tuple<Terms, Content>>& expr, Tuple&& t) {
				do {
					Eval<Content>::eval(expr.content_m, std::forward<Tuple>(t));
				} while (Eval<Terms>::eval(expr.terms_m, std::forward<Tuple>(t)));
			}
		};

	}

}


#endif