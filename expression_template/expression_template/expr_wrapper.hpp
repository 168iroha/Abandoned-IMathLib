#ifndef IMATH_MATH_EXPRESSION_TEMPLATE_EXPR_WRAPPER_HPP
#define IMATH_MATH_EXPRESSION_TEMPLATE_EXPR_WRAPPER_HPP

#include "IMathLib/utility/type_traits.hpp"
#include "IMathLib/utility/tuple.hpp"
#include "IMathLib/math/math/math_traits.hpp"


namespace iml {
	namespace op {

		// 2項演算および単項演算とのみみなすことが可能な共用のタグ
		template <size_t>
		struct operation_tag {};
		// 前置インクリメント(++)
		using preincrement_tag = operation_tag<0>;
		// 後置インクリメント(++)
		using postincrement_tag = operation_tag<1>;
		// 前置デクリメント(--)
		using predecrement_tag = operation_tag<2>;
		// 後置デクリメント(--)
		using postdecrement_tag = operation_tag<3>;
		// 添え字([])
		using subscript_tag = operation_tag<4>;
		// 関数オブジェクト(())
		struct functor_tag {};
		// メンバアクセス(->)
		using arrow_tag = operation_tag<5>;
		// ビット反転(~)
		using bit_flip_tag = operation_tag<6>;
		// 論理反転(!)
		using flip_tag = operation_tag<7>;
		// 加算(+)
		using add_tag = operation_tag<8>;
		// 減算(-)
		using sub_tag = operation_tag<9>;
		// 乗算(*)
		using mul_tag = operation_tag<10>;
		// 除算(/)
		using div_tag = operation_tag<11>;
		// 剰余(%)
		using surplus_tag = operation_tag<12>;
		// メンバポインタアクセス(->*)
		using arrow_ast_tag = operation_tag<13>;
		// 左シフト(<<)
		using lshift_tag = operation_tag<14>;
		// 右シフト(>>)
		using rshift_tag = operation_tag<15>;
		// 小なり(<)
		using less_than_tag = operation_tag<16>;
		// 小なりイコール(<=)
		using less_than_equal_tag = operation_tag<17>;
		// 大なり(>)
		using greater_than_tag = operation_tag<18>;
		// 大なりイコール(>=)
		using greater_than_equal_tag = operation_tag<19>;
		// 等号(==)
		using equal_tag = operation_tag<20>;
		// 不等号(!=)
		using not_equal_tag = operation_tag<21>;
		// ビット積(&)
		using bit_and_tag = operation_tag<22>;
		// 排他的ビット和(^)
		using bit_xor_tag = operation_tag<23>;
		// ビット和(|)
		using bit_or_tag = operation_tag<24>;
		// 論理積(&&)
		using and_tag = operation_tag<25>;
		// 論理和(||)
		using or_tag = operation_tag<26>;
		// 代入(=)
		using assign_tag = operation_tag<27>;
		// 加算代入(+=)
		using add_assign_tag = operation_tag<28>;
		// 減算代入(-=)
		using sub_assign_tag = operation_tag<29>;
		// 乗算代入(*=)
		using mul_assign_tag = operation_tag<30>;
		// 除算代入(/=)
		using div_assign_tag = operation_tag<31>;
		// 剰余代入(%=)
		using surplus_assign_tag = operation_tag<32>;
		// 左シフト代入(<<=)
		using lshift_assign_tag = operation_tag<33>;
		// 右シフト代入(>>=)
		using rshift_assign_tag = operation_tag<34>;
		// ビット積代入(&=)
		using bit_and_assign_tag = operation_tag<35>;
		// ビット和代入(|=)
		using bit_or_assign_tag = operation_tag<36>;
		// 排他的ビット和代入(^=)
		using bit_xor_assign_tag = operation_tag<37>;
		// コンマ(,)
		using comma_tag = operation_tag<38>;


		// 演算を示すタグであるかの判定
		template <class>
		struct is_operation_tag : std::false_type {};
		template <size_t N>
		struct is_operation_tag<operation_tag<N>> : std::true_type {};
		template <class T>
		inline constexpr bool is_operation_tag_v = is_operation_tag<T>::value;


		// 識別子を示すタグ(ifとかforとかreturnとか)
		template <size_t>
		struct ident_tag {};

		// 識別子を示すタグであるかの判定
		template <class>
		struct is_ident_tag : std::false_type {};
		template <size_t N>
		struct is_ident_tag<ident_tag<N>> : std::true_type {};
		template <class T>
		inline constexpr bool is_ident_tag_v = is_ident_tag<T>::value;


		// Expression templateのためのラッパー
		template <class Op, class Expr>
		struct expr_wrapper;


		// 無し(変数)
		struct none_tag {};


		// 式のトップレベルのタグ
		template <class>
		struct expr_tag {
			using type = none_tag;
		};
		template <class Tag, class Expr>
		struct expr_tag<expr_wrapper<Tag, Expr>> {
			using type = Tag;
		};
		template <class T>
		using expr_tag_t = typename expr_tag<T>::type;


		// expr_wrapperの判定
		template <class>
		struct is_expr_wrapper : std::false_type {};
		template <class Op, class Expr>
		struct is_expr_wrapper<expr_wrapper<Op, Expr>> : std::true_type {};
		template <class T>
		inline constexpr bool is_expr_wrapper_v = is_expr_wrapper<T>::value;
		// 識別子のexpr_wrapperの判定
		template <class>
		struct is_ident_expr_wrapper : std::false_type {};
		template <size_t N, class Expr>
		struct is_ident_expr_wrapper<expr_wrapper<ident_tag<N>, Expr>> : std::true_type {};
		template <class T>
		inline constexpr bool is_ident_expr_wrapper_v = is_ident_expr_wrapper<T>::value;


		// expr_wrapperにおいて変数であることの判定
		template <class T>
		struct is_expr_variable : std::bool_constant<std::is_same_v<expr_tag_t<T>, none_tag> && is_expr_wrapper_v<T>> {};



		// 単項演算の定義
#define EXPR_WRAPPER_UNARY_OPERATION_IMPL(NAME, OP)\
		constexpr expr_wrapper<NAME##_tag, type_tuple<expr_wrapper>> operator##OP() const {\
			return expr_wrapper<NAME##_tag, type_tuple<expr_wrapper>>(*this);\
		}
#define EXPR_WRAPPER_UNARY_OPERATION \
		EXPR_WRAPPER_UNARY_OPERATION_IMPL(add, +);\
		EXPR_WRAPPER_UNARY_OPERATION_IMPL(sub, -);\
		EXPR_WRAPPER_UNARY_OPERATION_IMPL(mul, *);\
		EXPR_WRAPPER_UNARY_OPERATION_IMPL(flip, !);\
		EXPR_WRAPPER_UNARY_OPERATION_IMPL(bit_flip, ~);

		// インクリメント・デクリメントの定義
#define EXPR_WRAPPER_INCREMENT_DECREMENT_IMPL(NAME, OP) \
		constexpr expr_wrapper<pre##NAME##_tag, type_tuple<expr_wrapper>> operator##OP() const {\
			return expr_wrapper<pre##NAME##_tag, type_tuple<expr_wrapper>>(*this);\
		}\
		constexpr expr_wrapper<post##NAME##_tag, type_tuple<expr_wrapper>> operator##OP(int) const {\
			return expr_wrapper<post##NAME##_tag, type_tuple<expr_wrapper>>(*this);\
		}
#define EXPR_WRAPPER_INCREMENT_DECREMENT \
		EXPR_WRAPPER_INCREMENT_DECREMENT_IMPL(increment, ++);\
		EXPR_WRAPPER_INCREMENT_DECREMENT_IMPL(decrement, --);

		// 2項関係の定義
#define EXPR_WRAPPER_BINARY_RELATION_IMPL(NAME, OP) \
		template <class U>\
		constexpr expr_wrapper<NAME##_tag, type_tuple<expr_wrapper, U>> operator##OP(const U& expr) const {\
			return expr_wrapper<NAME##_tag, type_tuple<expr_wrapper, U>>(*this, expr);\
		}
#define EXPR_WRAPPER_BINARY_RELATION \
		EXPR_WRAPPER_BINARY_RELATION_IMPL(arrow_ast, ->*);\
		EXPR_WRAPPER_BINARY_RELATION_IMPL(subscript, []);\
		EXPR_WRAPPER_BINARY_RELATION_IMPL(assign, =);\
		EXPR_WRAPPER_BINARY_RELATION_IMPL(add_assign, +=);\
		EXPR_WRAPPER_BINARY_RELATION_IMPL(sub_assign, -=);\
		EXPR_WRAPPER_BINARY_RELATION_IMPL(mul_assign, *=);\
		EXPR_WRAPPER_BINARY_RELATION_IMPL(div_assign, /=);\
		EXPR_WRAPPER_BINARY_RELATION_IMPL(surplus_assign, %=);\
		EXPR_WRAPPER_BINARY_RELATION_IMPL(lshift_assign, <<=);\
		EXPR_WRAPPER_BINARY_RELATION_IMPL(rshift_assign, >>=);\
		EXPR_WRAPPER_BINARY_RELATION_IMPL(bit_and_assign, &=);\
		EXPR_WRAPPER_BINARY_RELATION_IMPL(bit_or_assign, |=);\
		EXPR_WRAPPER_BINARY_RELATION_IMPL(bit_xor_assign, ^=);

		// expr_wrapperの全ての演算子のオーバーロード
#define EXPR_WRAPPER_OPERATOR_OVERLOAD \
		EXPR_WRAPPER_UNARY_OPERATION \
		EXPR_WRAPPER_INCREMENT_DECREMENT \
		EXPR_WRAPPER_BINARY_RELATION \
		/* コンマ演算子(処理を分ける) */ \
		template <class Expr>\
		constexpr expr_wrapper<comma_tag, type_tuple<expr_wrapper, Expr>> operator,(const Expr& expr) const {\
			return expr_wrapper<comma_tag, type_tuple<expr_wrapper, Expr>>(*this, expr);\
		}\
		/* 関数オブジェクト呼び出し */ \
		template <class... Args>\
		constexpr expr_wrapper<functor_tag, type_tuple<expr_wrapper, tuple<Args...>, typename tuple<Args...>::sequence_type>> operator()(Args&&... args) const {\
			return expr_wrapper<functor_tag, type_tuple<expr_wrapper, tuple<Args...>, typename tuple<Args...>::sequence_type>>(*this, std::forward<Args>(args)...);\
		}


		// 変数
		template <size_t N>
		struct expr_wrapper<none_tag, type_tuple<index_tuple<size_t, N>>> {
			char name_m;

			constexpr explicit expr_wrapper(char name) : name_m(name) {}

			EXPR_WRAPPER_OPERATOR_OVERLOAD;
		};
		// 変数宣言のためのエイリアス
		template <size_t N>
		using expr_variable = expr_wrapper<none_tag, type_tuple<index_tuple<size_t, N>>>;


		// 単項演算
		template <size_t N, class T>
		struct expr_wrapper<operation_tag<N>, type_tuple<T>> {
			T terms_m;

			constexpr explicit expr_wrapper(const T& expr) : terms_m(expr) {}

			EXPR_WRAPPER_OPERATOR_OVERLOAD;
		};
		// 2項演算
		template <size_t N, class First, class Second, class... Types>
		struct expr_wrapper<operation_tag<N>, type_tuple<First, Second, Types...>> {
			tuple<First, Second, Types...> terms_m;

			constexpr explicit expr_wrapper(const First& first, const Second& second, const Types&... args) : terms_m(first, second, args...) {}
			constexpr explicit expr_wrapper(const tuple<First, Second, Types...>& t) : terms_m(t) {}

			using tuple_type = tuple<First, Second, Types...>;

			EXPR_WRAPPER_OPERATOR_OVERLOAD;

			static constexpr size_t size() { return sizeof...(Types) + 2; }
		};
		// 関数オブジェクト
		template <class Expr, class Args, class Sequence>
		struct expr_wrapper<functor_tag, type_tuple<Expr, Args, Sequence>> {
			Expr terms_m;
			Args args_m;

			template <class... Types>
			constexpr explicit expr_wrapper(const Expr& expr, Types&&... args) : x_m(expr), args_m(std::forward<Types>(args)...) {}

			EXPR_WRAPPER_OPERATOR_OVERLOAD;
		};


		// Opによる2項演算であるかの判定
		template <class, class>
		struct is_binary_operation : std::false_type {};
		template <class Op, class First, class Second, class... Types>
		struct is_binary_operation<Op, expr_wrapper<Op, type_tuple<First, Second, Types...>>> : std::bool_constant<is_operation_tag_v<Op>> {};
		template <class Op, class Expr>
		constexpr bool is_binary_operation_v = is_binary_operation<Op, Expr>::value;


		// 数式の2項演算の木を演算子ごとにまとめるための補助
		// lhsとrhsがともにexpr_wrapperである場合
		template <class Op, class L, class R, bool = is_binary_operation_v<Op, L>, bool = is_binary_operation_v<Op, R>>
		struct Binary1 {
			// LとR共にOpではない
			static constexpr auto operation(const L& lhs, const R& rhs)  -> expr_wrapper<Op, type_tuple<L, R>> {
				return expr_wrapper<Op, type_tuple<L, R>>(lhs, rhs);
			}
		};
		template <class Op, class L1, class L2, class... Ls, class R1, class R2, class... Rs>
		struct Binary1<Op, expr_wrapper<Op, type_tuple<L1, L2, Ls...>>, expr_wrapper<Op, type_tuple<R1, R2, Rs...>>, true, true> {
			//LとR共にOpである
			static constexpr auto operation(const expr_wrapper<Op, type_tuple<L1, L2, Ls...>>& lhs, const expr_wrapper<Op, type_tuple<R1, R2, Rs...>>& rhs)  -> expr_wrapper<Op, type_tuple<tuple<L1, L2, Ls...>, tuple<R1, R2, Rs...>>> {
				return expr_wrapper<Op, type_tuple<tuple<L1, L2, Ls...>, tuple<R1, R2, Rs...>>>(lhs.terms_m, rhs.terms_m);
			}
		};
		template <class Op, class L1, class L2, class... Ls, class R>
		struct Binary1<Op, expr_wrapper<Op, type_tuple<L1, L2, Ls...>>, R, true, false> {
			//LのみOpである
			static constexpr auto operation(const expr_wrapper<Op, type_tuple<L1, L2, Ls...>>& lhs, const R& rhs)  -> expr_wrapper<Op, type_tuple<tuple<L1, L2, Ls...>, R>> {
				return expr_wrapper<Op, type_tuple<tuple<L1, L2, Ls...>, R>>(lhs.terms_m, rhs);
			}
		};
		template <class Op, class L, class R1, class R2, class... Rs>
		struct Binary1<Op, L, expr_wrapper<Op, type_tuple<R1, R2, Rs...>>, false, true> {
			//RのみOpである
			static constexpr auto operation(const L& lhs, const expr_wrapper<Op, type_tuple<R1, R2, Rs...>>& rhs)  -> expr_wrapper<Op, type_tuple<L, tuple<R1, R2, Rs...>>> {
				return expr_wrapper<Op, type_tuple<L, tuple<R1, R2, Rs...>>>(lhs, rhs.terms_m);
			}
		};
		//lhsのみがexpr_wrapperである場合
		template <class Op, class L, class R>
		struct Binary2 {
			//LがOpではない
			static constexpr auto operation(const L& lhs, const R& rhs)  -> expr_wrapper<Op, type_tuple<L, R>> {
				return expr_wrapper<Op, type_tuple<L, R>>(lhs, rhs);
			}
		};
		template <class Op, class L1, class L2, class... Ls, class R>
		struct Binary2<Op, expr_wrapper<Op, type_tuple<L1, L2, Ls...>>, R> {
			//LがOpである
			static constexpr auto operation(const expr_wrapper<Op, type_tuple<L1, L2, Ls...>>& lhs, const R& rhs)  -> expr_wrapper<Op, type_tuple<tuple<L1, L2, Ls...>, R>> {
				return expr_wrapper<Op, type_tuple<tuple<L1, L2, Ls...>, R>>(lhs.terms_m, rhs);
			}
		};
		//rhsのみがexpr_wrapperである場合
		template <class Op, class L, class R>
		struct Binary3 {
			//RがOpではない
			static constexpr auto operation(const L& lhs, const R& rhs)  -> expr_wrapper<Op, type_tuple<L, R>> {
				return expr_wrapper<Op, type_tuple<L, R>>(lhs, rhs);
			}
		};
		template <class Op, class L, class R1, class R2, class... Rs>
		struct Binary3<Op, L, expr_wrapper<Op, type_tuple<R1, R2, Rs...>>> {
			//RがOpである
			static constexpr auto operation(const L& lhs, const expr_wrapper<Op, type_tuple<R1, R2, Rs...>>& rhs)  -> expr_wrapper<Op, type_tuple<L, tuple<R1, R2, Rs...>>> {
				return expr_wrapper<Op, type_tuple<L, tuple<R1, R2, Rs...>>>(lhs, rhs.terms_m);
			}
		};


		//2項演算の定義
#define BINARY_OPERATION(NAME, OP)\
		template <class LOp, class LExpr, class ROp, class RExpr, class = std::enable_if_t<!is_ident_tag_v<LOp> && !is_ident_tag_v<ROp>>>\
		inline constexpr auto operator##OP(const expr_wrapper<LOp, LExpr>& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> decltype(Binary1<NAME##_tag, expr_wrapper<LOp, LExpr>, expr_wrapper<ROp, RExpr>>::operation(lhs, rhs)) {\
			return Binary1<NAME##_tag, expr_wrapper<LOp, LExpr>, expr_wrapper<ROp, RExpr>>::operation(lhs, rhs);\
		}\
		template <class LOp, class LExpr, class R, class = std::enable_if_t<!is_ident_tag_v<LOp> && !is_expr_wrapper_v<R>>>\
		inline constexpr auto operator##OP(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> decltype(Binary2<NAME##_tag, expr_wrapper<LOp, LExpr>, R>::operation(lhs, rhs)) {\
			return Binary2<NAME##_tag, expr_wrapper<LOp, LExpr>, R>::operation(lhs, rhs);\
		}\
		template <class L, class ROp, class RExpr, class = std::enable_if_t<!is_expr_wrapper_v<L> && !is_ident_tag_v<ROp>>>\
		inline constexpr auto operator##OP(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> decltype(Binary3<NAME##_tag, L, expr_wrapper<ROp, RExpr>>::operation(lhs, rhs)) {\
			return Binary3<NAME##_tag, L, expr_wrapper<ROp, RExpr>>::operation(lhs, rhs);\
		}
		BINARY_OPERATION(add, +);
		BINARY_OPERATION(sub, -);
		BINARY_OPERATION(mul, *);
		BINARY_OPERATION(div, /);
		BINARY_OPERATION(surplus, %);
		BINARY_OPERATION(lshift, <<);
		BINARY_OPERATION(rshift, >>);
		BINARY_OPERATION(less_than, <);
		BINARY_OPERATION(less_than_equal, <=);
		BINARY_OPERATION(greater_than, >);
		BINARY_OPERATION(greater_than_equal, >=);
		BINARY_OPERATION(equal, ==);
		BINARY_OPERATION(not_equal, !=);
		BINARY_OPERATION(bit_and, &);
		BINARY_OPERATION(bit_or, |);
		BINARY_OPERATION(bit_xor, ^);
		BINARY_OPERATION(and, &&);
		BINARY_OPERATION(or, ||);
#undef BINARY_OPERATION


		// expr_wrapperの作成
		template <class Op, class... Exprs>
		inline constexpr expr_wrapper<Op, type_tuple<Exprs...>> make_expr_wrapper(const Exprs&... exprs) {
			return expr_wrapper<Op, type_tuple<Exprs...>>(exprs...);
		}


		// トップレベルの演算の優先順位の定義
		// 変数もしくは定数
		template <class T>
		struct operator_precedence {
			static constexpr size_t value = 0;
		};
		template <class T>
		struct operator_precedence<expr_wrapper<none_tag, type_tuple<T>>> {
			static constexpr size_t value = 0;
		};
		// 単項演算
		template <class Expr>
		struct operator_precedence<expr_wrapper<add_tag, type_tuple<Expr>>> {
			static constexpr size_t value = 3;
		};
		template <class Expr>
		struct operator_precedence<expr_wrapper<sub_tag, type_tuple<Expr>>> {
			static constexpr size_t value = 3;
		};
		// 2項演算
		template <class First, class Second, class... Types>
		struct operator_precedence<expr_wrapper<add_tag, type_tuple<First, Second, Types...>>> {
			static constexpr size_t value = 6;
		};
		template <class First, class Second, class... Types>
		struct operator_precedence<expr_wrapper<sub_tag, type_tuple<First, Second, Types...>>> {
			static constexpr size_t value = 6;
		};
		template <class First, class Second, class... Types>
		struct operator_precedence<expr_wrapper<mul_tag, type_tuple<First, Second, Types...>>> {
			static constexpr size_t value = 5;
		};
		template <class First, class Second, class... Types>
		struct operator_precedence<expr_wrapper<div_tag, type_tuple<First, Second, Types...>>> {
			static constexpr size_t value = 5;
		};
		// 添え字
		template <class Expr1, class Expr2>
		struct operator_precedence<expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>> {
			static constexpr size_t value = 2;
		};


		// expr_wrapperの実行
		template <class T>
		struct Eval {
			template <class Result, class Tuple>
			static constexpr auto eval(Result&, const T& expr, Tuple&&) { return expr; }
			template <class Tuple>
			static constexpr auto eval(const T& expr, Tuple&&) { return expr; }
		};
		template <size_t N>
		struct Eval<expr_variable<N>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result&, const expr_variable<N>&, Tuple&& t) { return t.get<N>(); }
			template <class Tuple>
			static constexpr auto eval(const expr_variable<N>&, Tuple&& t) { return t.get<N>(); }
		};
		// 単項演算
#define EVAL_UNARY_OPERATION(NAME, OP)\
		template <class Expr>\
		struct Eval<expr_wrapper<NAME##_tag, type_tuple<Expr>>> {\
			template <class Result, class Tuple>\
			static constexpr auto eval(Result&, const expr_wrapper<NAME##_tag, type_tuple<Expr>>& expr, Tuple&& t) {\
				return OP##Eval<Expr>::eval(expr.terms_m, std::forward<Tuple>(t));\
			}\
			template <class Tuple>\
			static constexpr auto eval(const expr_wrapper<NAME##_tag, type_tuple<Expr>>& expr, Tuple&& t) {\
				return OP##Eval<Expr>::eval(expr.terms_m, std::forward<Tuple>(t));\
			}\
		};
		EVAL_UNARY_OPERATION(add, +);
		EVAL_UNARY_OPERATION(sub, -);
		EVAL_UNARY_OPERATION(mul, *);
		EVAL_UNARY_OPERATION(flip, !);
		EVAL_UNARY_OPERATION(bit_flip, ~);
#undef EVAL_UNARY_OPERATION
		// インクリメント・デクリメント
#define EVAL_INCREMENT_DECREMENT(NAME, OP)\
		template <class Expr>\
		struct Eval<expr_wrapper<pre##NAME##_tag, type_tuple<Expr>>> {\
			template <class Result, class Tuple>\
			static constexpr auto eval(Result&, const expr_wrapper<pre##NAME##_tag, type_tuple<Expr>>& expr, Tuple&& t) {\
				return OP##Eval<Expr>::eval(expr.terms_m, std::forward<Tuple>(t));\
			}\
			template <class Tuple>\
			static constexpr auto eval(const expr_wrapper<pre##NAME##_tag, type_tuple<Expr>>& expr, Tuple&& t) {\
				return OP##Eval<Expr>::eval(expr.terms_m, std::forward<Tuple>(t));\
			}\
		};\
		template <class Expr>\
		struct Eval<expr_wrapper<post##NAME##_tag, type_tuple<Expr>>> {\
			template <class Result, class Tuple>\
			static constexpr auto eval(Result&, const expr_wrapper<post##NAME##_tag, type_tuple<Expr>>& expr, Tuple&& t) {\
				return Eval<Expr>::eval(expr.terms_m, std::forward<Tuple>(t))##OP;\
			}\
			template <class Tuple>\
			static constexpr auto eval(const expr_wrapper<post##NAME##_tag, type_tuple<Expr>>& expr, Tuple&& t) {\
				return Eval<Expr>::eval(expr.terms_m, std::forward<Tuple>(t))##OP;\
			}\
		};
		EVAL_INCREMENT_DECREMENT(increment, ++);
		EVAL_INCREMENT_DECREMENT(decrement, --);
#undef EVAL_INCREMENT_DECREMENT
		// 添え字アクセス
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result&, const expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.terms_m.get<0>(), std::forward<Tuple>(t))[Eval<Expr2>::eval(expr.terms_m.get<1>(), std::forward<Tuple>(t))];
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.terms_m.get<0>(), std::forward<Tuple>(t))[Eval<Expr2>::eval(expr.terms_m.get<1>(), std::forward<Tuple>(t))];
			}
		};
		// 2項関係
#define EVAL_BINARY_RELATION(NAME, OP) \
		template <class Expr1, class Expr2>\
		struct Eval<expr_wrapper<NAME##_tag, type_tuple<Expr1, Expr2>>> {\
			template <class Result, class Tuple>\
			static constexpr auto eval(Result&, const expr_wrapper<NAME##_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {\
				return Eval<Expr1>::eval(expr.terms_m.get<0>(), std::forward<Tuple>(t))##OP##Eval<Expr2>::eval(expr.terms_m.get<1>(), std::forward<Tuple>(t));\
			}\
			template <class Tuple>\
			static constexpr auto eval(const expr_wrapper<NAME##_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {\
				return Eval<Expr1>::eval(expr.terms_m.get<0>(), std::forward<Tuple>(t))##OP##Eval<Expr2>::eval(expr.terms_m.get<1>(), std::forward<Tuple>(t));\
			}\
		};
		EVAL_BINARY_RELATION(arrow_ast, ->*);
		EVAL_BINARY_RELATION(assign, =);
		EVAL_BINARY_RELATION(add_assign, +=);
		EVAL_BINARY_RELATION(sub_assign, -=);
		EVAL_BINARY_RELATION(mul_assign, *=);
		EVAL_BINARY_RELATION(div_assign, /=);
		EVAL_BINARY_RELATION(surplus_assign, %=);
		EVAL_BINARY_RELATION(lshift_assign, <<=);
		EVAL_BINARY_RELATION(rshift_assign, >>=);
		EVAL_BINARY_RELATION(bit_and_assign, &=);
		EVAL_BINARY_RELATION(bit_or_assign, |=);
		EVAL_BINARY_RELATION(bit_xor_assign, ^=);
#undef EVAL_BINARY_RELATION
		// コンマ演算子
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<comma_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<comma_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				using result_type = decltype(Eval<Expr2>::eval(expr.rhs_m, std::forward<Tuple>(t)));
				Eval<Expr1>::eval(result, expr.terms_m.get<0>(), std::forward<Tuple>(t));
				return (result.second) ? Eval<Expr2>::eval(result, expr.terms_m.get<1>(), std::forward<Tuple>(t)) : result_type();
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<comma_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				Eval<Expr1>::eval(expr.terms_m.get<0>(), std::forward<Tuple>(t));
				return Eval<Expr2>::eval(expr.terms_m.get<1>(), std::forward<Tuple>(t));
			}
		};
		// 関数オブジェクト
		template <class Expr, class... Types, size_t... Indices>
		struct Eval<expr_wrapper<functor_tag, type_tuple<Expr, tuple<Types...>, index_tuple<size_t, Indices...>>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result&, const expr_wrapper<functor_tag, type_tuple<Expr, tuple<Types...>, index_tuple<size_t, Indices...>>>& expr, Tuple&& t) {
				return invoke(Eval<Expr>::eval(expr.terms_m, std::forward<Tuple>(t))
					, Eval<Types>::eval(expr.args_m.get<Indices>, std::forward<Tuple>(t))...);
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<functor_tag, type_tuple<Expr, tuple<Types...>, index_tuple<size_t, Indices...>>>& expr, Tuple&& t) {
				return invoke(Eval<Expr>::eval(expr.terms_m, std::forward<Tuple>(t))
					, Eval<Types>::eval(expr.args_m.get<Indices>, std::forward<Tuple>(t))...);
			}
		};
		// 2項関係の特に2項演算
		template <class, class T>
		struct Eval_tuple {
			template <class Tuple>
			static constexpr auto eval(const T& x, Tuple&& t, index_tuple<size_t>) { return Eval<T>::eval(x, std::forward<Tuple>(t)); }
		};
#define EVAL_BINARY_OPERATION(NAME, OP)\
		template <class... Types>\
		struct Eval_tuple<NAME##_tag, tuple<Types...>> {\
			template <class Tuple, size_t... Indices>\
			static constexpr auto eval(const tuple<Types...>& x, Tuple&& t, index_tuple<size_t, Indices...>) {\
				/* 畳み込み式で一気に展開して計算 */\
				return (...##OP##Eval_tuple<NAME##_tag, Types>::eval(x.get<Indices>(), std::forward<Tuple>(t)\
						, index_range_t<size_t, 0, tuple_size_v<Types>>()));\
			}\
		};
		EVAL_BINARY_OPERATION(add, +);
		EVAL_BINARY_OPERATION(sub, -);
		EVAL_BINARY_OPERATION(mul, *);
		EVAL_BINARY_OPERATION(div, /);
		EVAL_BINARY_OPERATION(surplus, %);
		EVAL_BINARY_OPERATION(lshift, <<);
		EVAL_BINARY_OPERATION(rshift, >>);
		EVAL_BINARY_OPERATION(less_than, <);
		EVAL_BINARY_OPERATION(less_than_equal, <=);
		EVAL_BINARY_OPERATION(greater_than, >);
		EVAL_BINARY_OPERATION(greater_than_equal, >=);
		EVAL_BINARY_OPERATION(equal, ==);
		EVAL_BINARY_OPERATION(not_equal, !=);
		EVAL_BINARY_OPERATION(bit_and, &);
		EVAL_BINARY_OPERATION(bit_or, |);
		EVAL_BINARY_OPERATION(bit_xor, ^);
		EVAL_BINARY_OPERATION(and, &&);
		EVAL_BINARY_OPERATION(or, ||);
#undef EVAL_BINARY_OPERATION
		template <class Op, class First, class Second, class... Types>
		struct Eval<expr_wrapper<Op, type_tuple<First, Second, Types...>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result&, const expr_wrapper<Op, type_tuple<First, Second, Types...>>& expr, Tuple&& t) {
				// 階層構造となっているtupleを展開する
				return Eval_tuple<Op, tuple<First, Second, Types...>>::eval(expr.terms_m, std::forward<Tuple>(t)
					, index_range_t<size_t, 0, 2 + sizeof...(Types)>());
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<Op, type_tuple<First, Second, Types...>>& expr, Tuple&& t) {
				return Eval_tuple<Op, tuple<First, Second, Types...>>::eval(expr.terms_m, std::forward<Tuple>(t)
					, index_range_t<size_t, 0, 2 + sizeof...(Types)>());
			}
		};
	
		template <class Expr, class... Types>
		inline constexpr auto eval(const Expr& expr, Types&&... args) {
			return Eval<Expr>::eval(expr, forward_as_tuple(args...));
		}



		template <class, class>
		struct Operation;
#define OPERATION_FUNCTION_UNARY_OPERATION(NAME, OP)\
		template <class Expr>\
		struct Operation<NAME##_tag, type_tuple<Expr>> {\
			static constexpr auto operation(const Expr& expr) { return OP##expr; }\
		};
		OPERATION_FUNCTION_UNARY_OPERATION(add, +);
		OPERATION_FUNCTION_UNARY_OPERATION(sub, -);
		OPERATION_FUNCTION_UNARY_OPERATION(mul, *);
#undef OPERATION_FUNCTION_UNARY_OPERATION
#define OPERATION_FUNCTION_BINARY_OPERATION(NAME, OP) \
		template <class First, class Second>\
		struct Operation<NAME##_tag, type_tuple<First, Second>> {\
			static constexpr auto operation(const First& first, const Second& second) {\
				return first##OP##second;\
			}\
		};\
		template <class First, class Second, class Third, class... Types>\
		struct Operation<NAME##_tag, type_tuple<First, Second, Third, Types...>> {\
			static constexpr auto operation(const First& first, const Second& second, const Third& third, const Types&... rest) {\
				auto temp = first##OP##second;\
				using temp_type = typename decay<decltype(temp)>::type;\
				return Operation<NAME##_tag, type_tuple<temp_type, Third, Types...>>::operation(temp, third, rest...);\
			}\
		};
		OPERATION_FUNCTION_BINARY_OPERATION(add, +);
		OPERATION_FUNCTION_BINARY_OPERATION(sub, -);
		OPERATION_FUNCTION_BINARY_OPERATION(mul, *);
		OPERATION_FUNCTION_BINARY_OPERATION(div, / );
#undef OPERATION_FUNCTION_BINARY_OPERATION
		// 演算子に対応する演算の実行
		template <class Op, class... Exprs>
		inline constexpr auto operation(const Exprs&... exprs) { return Operation<Op, type_tuple<Exprs...>>::operation(exprs...); }


		template <class, class>
		struct Reverse_operation;
#define REVERSE_OPERATION_FUNCTION_UNARY_OPERATION(NAME, OP)\
		template <class Expr>\
		struct Reverse_operation<NAME##_tag, type_tuple<Expr>> {\
			static constexpr auto operation(const Expr& expr) { return OP##expr; }\
		};
		REVERSE_OPERATION_FUNCTION_UNARY_OPERATION(add, +);
		REVERSE_OPERATION_FUNCTION_UNARY_OPERATION(sub, -);
		REVERSE_OPERATION_FUNCTION_UNARY_OPERATION(mul, *);
#undef REVERSE_OPERATION_FUNCTION_UNARY_OPERATION
#define REVERSE_OPERATION_FUNCTION_BINARY_OPERATION(NAME, OP) \
		template <class First, class Second>\
		struct Reverse_operation<NAME##_tag, type_tuple<First, Second>> {\
			static constexpr auto reverse_operation(const First& first, const Second& second) {\
				return second##OP##first;\
			}\
		};\
		template <class First, class Second, class Third, class... Types>\
		struct Reverse_operation<NAME##_tag, type_tuple<First, Second, Third, Types...>> {\
			static constexpr auto reverse_operation(const First& first, const Second& second, const Third& third, const Types&... rest) {\
				return Reverse_operation<NAME##_tag, type_tuple<Second, Third, Types...>>::reverse_operation(second, third, rest...)##OP##first;\
			}\
		};
		REVERSE_OPERATION_FUNCTION_BINARY_OPERATION(add, +);
		REVERSE_OPERATION_FUNCTION_BINARY_OPERATION(sub, -);
		REVERSE_OPERATION_FUNCTION_BINARY_OPERATION(mul, *);
		REVERSE_OPERATION_FUNCTION_BINARY_OPERATION(div, / );
#undef REVERSE_OPERATION_FUNCTION_BINARY_OPERATION
		// 演算子に対応する演算を逆順で実行
		template <class Op, class... Exprs>
		inline constexpr auto reverse_operation(const Exprs&... exprs) { return Reverse_operation<Op, type_tuple<Exprs...>>::reverse_operation(exprs...); }

	}
}


#endif