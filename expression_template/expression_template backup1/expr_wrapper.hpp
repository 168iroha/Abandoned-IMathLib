#ifndef IMATH_MATH_EXPRESSION_TEMPLATE_EXPR_WRAPPER_HPP
#define IMATH_MATH_EXPRESSION_TEMPLATE_EXPR_WRAPPER_HPP

#include "IMathLib/utility/type_traits.hpp"
#include "IMathLib/math/math/math_traits.hpp"


namespace iml {
	namespace op {

		//2項演算および単項演算とのみみなすことが可能な共用のタグ
		template <imsize_t>
		struct operation_tag {};
		//インクリメント(++)
		using increment_tag = operation_tag<0>;
		//デクリメント(--)
		using decrement_tag = operation_tag<1>;
		//添え字([])
		using subscript_tag = operation_tag<2>;
		//関数オブジェクト(())
		struct functor_tag {};
		//メンバアクセス(->)
		using arrow_tag = operation_tag<3>;
		//ビット反転(~)
		using bit_flip_tag = operation_tag<4>;
		//論理反転(!)
		using flip_tag = operation_tag<5>;
		//加算(+)
		using add_tag = operation_tag<6>;
		//減算(-)
		using sub_tag = operation_tag<7>;
		//乗算(*)
		using mul_tag = operation_tag<8>;
		//除算(/)
		using div_tag = operation_tag<9>;
		//剰余(%)
		using surplus_tag = operation_tag<10>;
		//メンバポインタアクセス(->*)
		using arrow_ast_tag = operation_tag<11>;
		//左シフト(<<)
		using lshift_tag = operation_tag<12>;
		//右シフト(>>)
		using rshift_tag = operation_tag<13>;
		//小なり(<)
		using less_than_tag = operation_tag<14>;
		//小なりイコール(<=)
		using less_then_equal_tag = operation_tag<15>;
		//大なり(>)
		using greater_than_tag = operation_tag<16>;
		//大なりイコール(>=)
		using greater_then_equal_tag = operation_tag<17>;
		//等号(==)
		using equal_tag = operation_tag<18>;
		//不等号(!=)
		using not_equal_tag = operation_tag<19>;
		//ビット積(&)
		using bit_and_tag = operation_tag<20>;
		//排他的ビット和(^)
		using bit_xor_tag = operation_tag<21>;
		//ビット和(|)
		using bit_or_tag = operation_tag<22>;
		//論理積(&&)
		using and_tag = operation_tag<23>;
		//論理和(||)
		using or_tag = operation_tag<24>;
		//代入(=)
		using assign_tag = operation_tag<25>;
		//加算代入
		using add_assign_tag = operation_tag<26>;
		//減算代入
		using sub_assign_tag = operation_tag<27>;
		//乗算代入
		using mul_assign_tag = operation_tag<28>;
		//除算代入
		using div_assign_tag = operation_tag<29>;
		//剰余代入
		using surplus_assign_tag = operation_tag<30>;
		//左シフト代入
		using lshift_assign_tag = operation_tag<31>;
		//右シフト代入
		using rshift_assign_tag = operation_tag<32>;
		//ビット積代入
		using bit_and_assign_tag = operation_tag<33>;
		//ビット和代入
		using bit_or_assign_tag = operation_tag<34>;
		//排他的ビット和代入
		using bit_xor_assign_tag = operation_tag<35>;
		//コンマ(,)
		using comma_tag = operation_tag<36>;


		//識別子を示すタグ
		template <size_t>
		struct ident_tag {};

		//識別子を示すタグであるかの判定
		template <class>
		struct is_ident_tag : false_type {};
		template <size_t N>
		struct is_ident_tag<ident_tag<N>> : true_type {};


		//Expression templateのためのラッパー
		template <class Op, class Expr>
		struct expr_wrapper;


		//無し(変数)
		struct none_tag {};


		//式のトップレベルのタグ
		template <class>
		struct expr_tag {
			using type = none_tag;
		};
		template <class Tag, class Expr>
		struct expr_tag<expr_wrapper<Tag, Expr>> {
			using type = Tag;
		};


		//expr_wrapperでラップされているか判定
		template <class>
		struct is_expr_wrapper : false_type {};
		template <class Op, class Expr>
		struct is_expr_wrapper<expr_wrapper<Op, Expr>> : true_type {};


		//expr_wrapperにおいて変数であることの判定
		template <class T>
		struct is_expr_variable : cat_bool<is_same<typename expr_tag<T>::type, none_tag>::value && is_expr_wrapper<T>::value> {};


		//識別子のexpr_wrapperの判定
		template <class>
		struct is_ident_expr_wrapper : false_type {};
		template <size_t N, class Expr>
		struct is_ident_expr_wrapper<expr_wrapper<ident_tag<N>, Expr>> : true_type {};


		//変数
		template <size_t N>
		struct expr_wrapper<none_tag, type_tuple<index_tuple<size_t, N>>> {
			char name_m;

			constexpr explicit expr_wrapper(char name) : name_m(name) {}

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
		//変数宣言のためのエイリアス
		template <size_t N>
		using expr_variable = expr_wrapper<none_tag, type_tuple<index_tuple<size_t, N>>>;


		//単項演算
		template <size_t N, class T>
		struct expr_wrapper<operation_tag<N>, type_tuple<T>> {
			T x_m;

			constexpr explicit expr_wrapper(const T& expr) : x_m(expr) {}

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
		//2項演算
		template <size_t N, class L, class R>
		struct expr_wrapper<operation_tag<N>, type_tuple<L, R>> {
			L lhs_m;
			R rhs_m;

			constexpr explicit expr_wrapper(const L& lhs, const R& rhs) : lhs_m(lhs), rhs_m(rhs) {}

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

		//関数オブジェクト用のexpr_wrapper
		template <class Expr, class Args, class Sequence>
		struct expr_wrapper<functor_tag, type_tuple<Expr, Args, Sequence>> {
			Expr x_m;
			Args args_m;

			template <class... Types>
			constexpr explicit expr_wrapper(const Expr& expr, Types&&... args) : x_m(expr), args_m(forward<Types>(args)...) {}

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


		//2項演算の定義(lhsとrhsの型が等しい場合は多重定義のエラーとなるためSFINAEで除外)
		template <class LOp, class LExpr, class R, class = typename enable_if<!is_ident_tag<LOp>::value && !is_ident_expr_wrapper<R>::value>::type>
		inline constexpr auto operator+(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<add_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<add_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value && !is_ident_tag<ROp>::value>::type>
		inline constexpr auto operator+(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<add_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<add_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R, class = typename enable_if<!is_ident_tag<LOp>::value && !is_ident_expr_wrapper<R>::value>::type>
		inline constexpr auto operator-(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<sub_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<sub_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value && !is_ident_tag<ROp>::value>::type>
		inline constexpr auto operator-(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<sub_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<sub_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R, class = typename enable_if<!is_ident_tag<LOp>::value && !is_ident_expr_wrapper<R>::value>::type>
		inline constexpr auto operator*(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<mul_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<mul_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value && !is_ident_tag<ROp>::value>::type>
		inline constexpr auto operator*(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<mul_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<mul_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R, class = typename enable_if<!is_ident_tag<LOp>::value && !is_ident_expr_wrapper<R>::value>::type>
		inline constexpr auto operator/(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<div_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<div_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value && !is_ident_tag<ROp>::value>::type>
		inline constexpr auto operator/(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<div_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<div_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R, class = typename enable_if<!is_ident_tag<LOp>::value && !is_ident_expr_wrapper<R>::value>::type>
		inline constexpr auto operator%(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<surplus_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<surplus_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value && !is_ident_tag<ROp>::value>::type>
		inline constexpr auto operator%(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<surplus_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<surplus_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R, class = typename enable_if<!is_ident_tag<LOp>::value && !is_ident_expr_wrapper<R>::value>::type>
		inline constexpr auto operator<<(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<lshift_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<lshift_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value && !is_ident_tag<ROp>::value>::type>
		inline constexpr auto operator<<(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<lshift_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<lshift_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R, class = typename enable_if<!is_ident_tag<LOp>::value && !is_ident_expr_wrapper<R>::value>::type>
		inline constexpr auto operator>>(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<rshift_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<rshift_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value && !is_ident_tag<ROp>::value>::type>
		inline constexpr auto operator>>(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<rshift_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<rshift_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R, class = typename enable_if<!is_ident_tag<LOp>::value && !is_ident_expr_wrapper<R>::value>::type>
		inline constexpr auto operator<(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<less_than_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<less_than_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value && !is_ident_tag<ROp>::value>::type>
		inline constexpr auto operator<(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<less_than_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<less_than_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R, class = typename enable_if<!is_ident_tag<LOp>::value && !is_ident_expr_wrapper<R>::value>::type>
		inline constexpr auto operator<=(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<less_then_equal_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<less_then_equal_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value && !is_ident_tag<ROp>::value>::type>
		inline constexpr auto operator<=(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<less_then_equal_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<less_then_equal_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R, class = typename enable_if<!is_ident_tag<LOp>::value && !is_ident_expr_wrapper<R>::value>::type>
		inline constexpr auto operator>(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<greater_than_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<greater_than_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value && !is_ident_tag<ROp>::value>::type>
		inline constexpr auto operator>(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<greater_than_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<greater_than_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R, class = typename enable_if<!is_ident_tag<LOp>::value && !is_ident_expr_wrapper<R>::value>::type>
		inline constexpr auto operator>=(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<greater_then_equal_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<greater_then_equal_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value && !is_ident_tag<ROp>::value>::type>
		inline constexpr auto operator>=(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<greater_then_equal_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<greater_then_equal_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R, class = typename enable_if<!is_ident_tag<LOp>::value && !is_ident_expr_wrapper<R>::value>::type>
		inline constexpr auto operator==(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<equal_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<equal_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value && !is_ident_tag<ROp>::value>::type>
		inline constexpr auto operator==(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<equal_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<equal_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R, class = typename enable_if<!is_ident_tag<LOp>::value && !is_ident_expr_wrapper<R>::value>::type>
		inline constexpr auto operator!=(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<not_equal_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<not_equal_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value && !is_ident_tag<ROp>::value>::type>
		inline constexpr auto operator!=(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<not_equal_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<not_equal_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R, class = typename enable_if<!is_ident_tag<LOp>::value && !is_ident_expr_wrapper<R>::value>::type>
		inline constexpr auto operator&(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<bit_and_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<bit_and_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value && !is_ident_tag<ROp>::value>::type>
		inline constexpr auto operator&(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<bit_and_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<bit_and_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R, class = typename enable_if<!is_ident_tag<LOp>::value && !is_ident_expr_wrapper<R>::value>::type>
		inline constexpr auto operator|(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<bit_or_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<bit_or_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value && !is_ident_tag<ROp>::value>::type>
		inline constexpr auto operator|(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<bit_or_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<bit_or_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R, class = typename enable_if<!is_ident_tag<LOp>::value && !is_ident_expr_wrapper<R>::value>::type>
		inline constexpr auto operator^(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<bit_xor_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<bit_xor_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value && !is_ident_tag<ROp>::value>::type>
		inline constexpr auto operator^(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<bit_xor_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<bit_xor_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R, class = typename enable_if<!is_ident_tag<LOp>::value && !is_ident_expr_wrapper<R>::value>::type>
		inline constexpr auto operator&&(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<and_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<and_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value && !is_ident_tag<ROp>::value>::type>
		inline constexpr auto operator&&(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<and_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<and_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R, class = typename enable_if<!is_ident_tag<LOp>::value && !is_ident_expr_wrapper<R>::value>::type>
		inline constexpr auto operator||(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<or_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<or_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value && !is_ident_tag<ROp>::value>::type>
		inline constexpr auto operator||(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<or_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<or_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}


		//expr_wrapperの作成
		template <class Op, class... Expr>
		inline constexpr expr_wrapper<Op, type_tuple<Expr...>> make_expr_wrapper(const Expr&... expr) {
			return expr_wrapper<Op, type_tuple<Expr...>>(expr...);
		}


		//トップレベルの演算の優先順位の定義
		//変数もしくは定数
		template <class T>
		struct operator_precedence {
			static constexpr imsize_t value = 0;
		};
		template <class T>
		struct operator_precedence<expr_wrapper<none_tag, type_tuple<T>>> {
			static constexpr imsize_t value = 0;
		};
		//単項演算
		template <class Expr>
		struct operator_precedence<expr_wrapper<add_tag, type_tuple<Expr>>> {
			static constexpr imsize_t value = 3;
		};
		template <class Expr>
		struct operator_precedence<expr_wrapper<sub_tag, type_tuple<Expr>>> {
			static constexpr imsize_t value = 3;
		};
		//2項演算
		template <class Expr1, class Expr2>
		struct operator_precedence<expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>> {
			static constexpr imsize_t value = 6;
		};
		template <class Expr1, class Expr2>
		struct operator_precedence<expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>> {
			static constexpr imsize_t value = 6;
		};
		template <class Expr1, class Expr2>
		struct operator_precedence<expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>> {
			static constexpr imsize_t value = 5;
		};
		template <class Expr1, class Expr2>
		struct operator_precedence<expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>> {
			static constexpr imsize_t value = 5;
		};
		//添え字
		template <class Expr1, class Expr2>
		struct operator_precedence<expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>> {
			static constexpr imsize_t value = 2;
		};


		//expr_wrapperのトップレベルの演算に対して下層の方が演算の優先順位が低いかの判定
		//変数もしくは定数
		template <class T>
		struct is_underlayer_low_precedence {
			static constexpr bool lhs_value = false;		//2項演算のときの左辺
			static constexpr bool rhs_value = false;		//*右辺
			static constexpr bool mhs_value = false;		//単項演算
		};
		template <class T>
		struct is_underlayer_low_precedence<expr_wrapper<none_tag, type_tuple<T>>> {
			static constexpr bool lhs_value = false;
			static constexpr bool rhs_value = false;
			static constexpr bool mhs_value = false;
		};
		//単項演算
		template <class Expr>
		struct is_underlayer_low_precedence<expr_wrapper<add_tag, type_tuple<Expr>>> {
			static constexpr bool lhs_value = false;
			static constexpr bool rhs_value = false;
			static constexpr bool mhs_value = (operator_precedence<Expr>::value > operator_precedence<expr_wrapper<add_tag, type_tuple<Expr>>>::value);
		};
		template <class Expr>
		struct is_underlayer_low_precedence<expr_wrapper<sub_tag, type_tuple<Expr>>> {
			static constexpr bool lhs_value = false;
			static constexpr bool rhs_value = false;
			static constexpr bool mhs_value = (operator_precedence<Expr>::value > operator_precedence<expr_wrapper<sub_tag, type_tuple<Expr>>>::value);
		};
		//2項演算
		template <class Expr1, class Expr2>
		struct is_underlayer_low_precedence<expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>> {
			static constexpr bool lhs_value = (operator_precedence<Expr1>::value > operator_precedence<expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>>::value);
			static constexpr bool rhs_value = (operator_precedence<Expr2>::value > operator_precedence<expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>>::value);
			static constexpr bool mhs_value = false;
		};
		template <class Expr1, class Expr2>
		struct is_underlayer_low_precedence<expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>> {
			static constexpr bool lhs_value = (operator_precedence<Expr1>::value > operator_precedence<expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>>::value);
			static constexpr bool rhs_value = (operator_precedence<Expr2>::value > operator_precedence<expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>>::value);
			static constexpr bool mhs_value = false;
		};
		template <class Expr1, class Expr2>
		struct is_underlayer_low_precedence<expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>> {
			static constexpr bool lhs_value = (operator_precedence<Expr1>::value > operator_precedence<expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>>::value);
			static constexpr bool rhs_value = (operator_precedence<Expr2>::value > operator_precedence<expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>>::value);
			static constexpr bool mhs_value = false;
		};
		template <class Expr1, class Expr2>
		struct is_underlayer_low_precedence<expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>> {
			static constexpr bool lhs_value = (operator_precedence<Expr1>::value > operator_precedence<expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>>::value);
			static constexpr bool rhs_value = (operator_precedence<Expr2>::value > operator_precedence<expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>>::value);
			static constexpr bool mhs_value = false;
		};
		//添え字(x_m[index_m]でそれぞれ左辺・右辺と扱う)
		template <class Expr1, class Expr2>
		struct is_underlayer_low_precedence<expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>> {
			static constexpr bool lhs_value = (operator_precedence<Expr1>::value > operator_precedence<expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>>::value);
			static constexpr bool rhs_value = (operator_precedence<Expr2>::value > operator_precedence<expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>>::value);
			static constexpr bool mhs_value = false;
		};




		//expr_wrapperの実行
		template <class T>
		struct Eval {
			template <class Result, class Tuple>
			static constexpr auto eval(Result&, const T& expr, Tuple&&) {
				return expr;
			}
			template <class Tuple>
			static constexpr auto eval(const T& expr, Tuple&&) {
				return expr;
			}
		};
		template <size_t N>
		struct Eval<expr_variable<N>> {
			template <class Result, class Tuple>
			static constexpr auto& eval(Result&, const expr_variable<N>&, Tuple&& t) {
				return t.get<N>();
			}
			template <class Tuple>
			static constexpr auto& eval(const expr_variable<N>&, Tuple&& t) {
				return t.get<N>();
			}
		};
		//単項演算
		template <class Expr>
		struct Eval<expr_wrapper<add_tag, type_tuple<Expr>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<add_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				return Eval<Expr>::eval(result, expr.x_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<add_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				return Eval<Expr>::eval(expr.x_m, forward<Tuple>(t));
			}
		};
		template <class Expr>
		struct Eval<expr_wrapper<sub_tag, type_tuple<Expr>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<sub_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				return -Eval<Expr>::eval(result, expr.x_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<sub_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				return -Eval<Expr>::eval(expr.x_m, forward<Tuple>(t));
			}
		};
		template <class Expr>
		struct Eval<expr_wrapper<mul_tag, type_tuple<Expr>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<mul_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				return *Eval<Expr>::eval(result, expr.x_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<mul_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				return *Eval<Expr>::eval(expr.x_m, forward<Tuple>(t));
			}
		};
		template <class Expr>
		struct Eval<expr_wrapper<flip_tag, type_tuple<Expr>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<flip_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				return !Eval<Expr>::eval(result, expr.x_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<flip_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				return !Eval<Expr>::eval(expr.x_m, forward<Tuple>(t));
			}
		};
		template <class Expr>
		struct Eval<expr_wrapper<bit_flip_tag, type_tuple<Expr>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<bit_flip_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				return ~Eval<Expr>::eval(result, expr.x_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<bit_flip_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				return ~Eval<Expr>::eval(expr.x_m, forward<Tuple>(t));
			}
		};
		//インクリメント・デクリメント
		template <class Expr>
		struct Eval<expr_wrapper<increment_tag, type_tuple<Expr>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<increment_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				return ++Eval<Expr>::eval(result, expr.x_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<increment_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				return ++Eval<Expr>::eval(expr.x_m, forward<Tuple>(t));
			}
		};
		template <class Expr>
		struct Eval<expr_wrapper<increment_tag, type_tuple<Expr, int>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<increment_tag, type_tuple<Expr, int>>& expr, Tuple&& t) {
				return Eval<Expr>::eval(result, expr.lhs_m, forward<Tuple>(t))++;
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<increment_tag, type_tuple<Expr, int>>& expr, Tuple&& t) {
				return Eval<Expr>::eval(expr.lhs_m, forward<Tuple>(t))++;
			}
		};
		template <class Expr>
		struct Eval<expr_wrapper<decrement_tag, type_tuple<Expr>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<decrement_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				return --Eval<Expr>::eval(result, expr.x_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<decrement_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				return --Eval<Expr>::eval(expr.x_m, forward<Tuple>(t));
			}
		};
		template <class Expr>
		struct Eval<expr_wrapper<decrement_tag, type_tuple<Expr, int>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<decrement_tag, type_tuple<Expr, int>>& expr, Tuple&& t) {
				return Eval<Expr>::eval(result, expr.lhs_m, forward<Tuple>(t))--;
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<decrement_tag, type_tuple<Expr, int>>& expr, Tuple&& t) {
				return Eval<Expr>::eval(expr.lhs_m, forward<Tuple>(t))--;
			}
		};
		//メンバポインタアクセス
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<arrow_ast_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<arrow_ast_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t))->*Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<arrow_ast_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t))->*Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		//添え字アクセス
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t))[Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t))];
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t))[Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t))];
			}
		};
		//代入演算
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<assign_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) = Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) = Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<add_assign_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<add_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) += Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<add_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) += Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<sub_assign_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<sub_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) -= Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<sub_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) -= Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<mul_assign_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<mul_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) *= Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<mul_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) *= Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<div_assign_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<div_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) /= Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<div_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) /= Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<surplus_assign_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<surplus_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) %= Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<surplus_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) %= Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<lshift_assign_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<lshift_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) <<= Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<lshift_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) <<= Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<rshift_assign_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<rshift_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) >>= Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<rshift_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) >>= Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<bit_and_assign_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<bit_and_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) &= Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<bit_and_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) &= Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<bit_or_assign_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<bit_or_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) |= Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<bit_or_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) |= Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<bit_xor_assign_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<bit_xor_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) ^= Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<bit_xor_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) ^= Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		//2項演算
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) + Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) + Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) - Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) - Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) * Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) * Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) / Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) / Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<surplus_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<surplus_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) % Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<surplus_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) % Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<lshift_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<lshift_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) << Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<lshift_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) << Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<rshift_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<rshift_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) >> Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<rshift_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) >> Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<less_than_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<less_than_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) < Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<less_than_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) < Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<less_then_equal_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<less_then_equal_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) <= Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<less_then_equal_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) <= Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<greater_than_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<greater_than_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) > Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<greater_than_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) > Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<greater_then_equal_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<greater_then_equal_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) >= Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<greater_then_equal_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) >= Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<equal_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<equal_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) == Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<equal_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) == Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<not_equal_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<not_equal_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) != Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<not_equal_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) != Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<bit_and_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<bit_and_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) & Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<bit_and_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) & Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<bit_or_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<bit_or_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) | Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<bit_or_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) | Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<bit_xor_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<bit_xor_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) ^ Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<bit_xor_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) ^ Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<and_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<and_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) && Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<and_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) && Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<or_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<or_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t)) || Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<or_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) || Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		//コンマ演算子
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<comma_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<comma_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				using result_type = decltype(Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t)));
				Eval<Expr1>::eval(result, expr.lhs_m, forward<Tuple>(t));
				return (result.second) ? Eval<Expr2>::eval(result, expr.rhs_m, forward<Tuple>(t)) : result_type();
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<comma_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t));
				return Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};

		//関数オブジェクト
		template <class Expr, class... Types, size_t... Indices>
		struct Eval<expr_wrapper<functor_tag, type_tuple<Expr, tuple<Types...>, index_tuple<size_t, Indices...>>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<functor_tag, type_tuple<Expr, tuple<Types...>, index_tuple<size_t, Indices...>>>& expr, Tuple&& t) {
				return invoke(Eval<Expr>::eval(result, expr.x_m, forward<Tuple>(t))
					, Eval<Types>::eval(result, expr.args_m.get<Indices>, forward<Tuple>(t))...);
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<functor_tag, type_tuple<Expr, tuple<Types...>, index_tuple<size_t, Indices...>>>& expr, Tuple&& t) {
				return invoke(Eval<Expr>::eval(expr.x_m, forward<Tuple>(t))
					, Eval<Types>::eval(expr.args_m.get<Indices>, forward<Tuple>(t))...);
			}
		};

	}
}


#endif