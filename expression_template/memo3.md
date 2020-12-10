 - [Expression templateと数式処理](https://qiita.com/168irohairoha/items/beecd2e12a56dd0eda3c)
 - [Expression templateと数式処理2](https://qiita.com/168irohairoha/items/79c2fd468a7b5458103f)

の続き物。
今回は数式処理を中心にやる。そして、その数式処理を上手くやるにはExpression templateの設計から見直す必要がある。
Lambda内で変数宣言とか変数のキャプチャはやらない。それだけで十分に長い。

C++14でも実装することは可能である(というか一回した)が、とても見ていられない程度に再帰が出てくるため、畳み込み式を用いるためにC++17で実装をする。

大分、間が空いたからどこまでやったか覚えてない。

# 動機
考えたことの整理のために書こうと思った。

# Expression templateの設計の見直し
## 修正した理論
さて、極端な例ではあるが、現段階では
$$
a+b+c+d\ \Rightarrow\ +(+(+(a,b),c),d)
$$
のように構文木が作成される。これは無駄が多く非常に良くない。全て加法によって結合されているのだから
$$
+(a,b,c,d)
$$
のように構文木が作成されるべきである。ちなみに、これはポーランド記法に括弧をつけて多少見やすくしたものである。
他の例として
$$
\begin{aligned}
abc+def+ghi\ &\Rightarrow\ \times(a,b,c)+\times(d,e,f)+\times(g,h,i)\\\
&\Rightarrow\ +(\times (a,b,c),\times (d,e,f),\times (g,h,i))
\end{aligned}
$$
とかできるのが理想。
また、実際の代数的構造は八元数のように結合法則が成り立たない場合もあるため、実際の構文木の構築には付加情報を与える必要がある。
実際にプログラム上で動かすには以下のようなイメージで計算をする。
$$
\begin{aligned}
(a+b)+(c+(d+e))+f\ &\Rightarrow\ +(a,b)+(c++(d,e))+f \\\
&\Rightarrow\ +(a,b)++(c,(d,e))+f \\\
&\Rightarrow\ +((a,b),(c,(d,e)))+f \\\
&\Rightarrow\ +((a,b),(c,(d,e)),f)
\end{aligned}
$$
このように、右から作用する演算子に対するタプルをそのまま加えるだけである。このようなタプルを加えるといった単純な操作だけで、同じ演算子による演算が予めまとめられる。
これにより、木を浅くする≒テンプレート再帰深度の低減といった効果が得られる。ただし、同じ演算子による演算、最初の例のような
$$
\begin{aligned}
a+b+c+d\ &\Rightarrow\ +(a,b)+c+d \\\
&\Rightarrow\ +(a,b,c)+d \\\
&\Rightarrow\ +(a,b,c,d)
\end{aligned}
$$
でなければ効果は薄い(少なくとも前のExpression templateの実装よりは悪くなることはない)が、数式処理に対してはいろいろと都合がいい。
例えば、交換法則は演算子に対応するタプルの内部を入れ替えるだけであり、結合法則も同様である。分配法則も分配するための項がタプルとして全て既知であることから処理の単純化を図ることができる。

また、`Eval`を実行するときも演算子に対するタプルの階層が深いところから再帰的に実行するだけである。

## 実装
まずは、2項演算についての`expr_wrapper`の修正をする。これまでは左辺と右辺の2項のみを保持していたが、前述したような理論による構築法では2個以上の可変長のテンプレート引数をもつこととなる。そのため、変数の保持には`tuple`が活躍する。
また、そのタプルを構成するための2項演算も厳密に推論をさせるために色々と書き換える必要がある。そのため、まずは加算を例にとってその種類を列挙する。ちなみにlhsとrhsは2項演算に渡される引数である。

<ul>
<li>lhsとrhsがともに`expr_wrapper`である。
  <ul>
    <li>lhsとrhsがともに加算の項を示す。ex)lhs=a+b, rhs=c+d</li>
    <li>lhsのみが加算の項を示す。ex)lhs=a+b, rhs=c-d</li>
    <li>rhsのみが加算の項を示す。ex)lhs=a-d, rhs=c+d</li>
    <li>どちらも加算の項ではない。ex)lhs=a-d, rhs=c-d</li>
  </ul>
  </li>
  <li>lhsのみが`expr_wrapper`である。
  <ul>
    <li>lhsが加算の項を示す。ex)lhs=a+b</li>
    <li>lhsが加算の項ではない。ex)lhs=a-b</li>
  </ul>
  </li>
  <li>rhsのみが`expr_wrapper`である。
  <ul>
    <li>rhsが加算の項を示す。ex)rhs=a+b</li>
    <li>rhsが加算の項ではない。ex)rhs=a-b</li>
  </ul>
  </li>
  <li>lhsとrhsがともに`expr_wrapper`ではない。
  <ul>
    <li>通常の2項演算が作用。</li>
  </ul>
  </li>
</ul>

このように9パターン存在するが、実際に実装をするのは一番最後を除いた8パターンである。
また、プログラム上ではタプルを用いるが、
$$
a+b+c+d\ \Rightarrow\ +(a,b,c,d)
$$
というように素直に記述した場合、プログラム上で可変長引数は先頭からしか取り出すことはできないため
$$
+(a,b,c,d)\ \Rightarrow\ a++(b,c,d)\ \Rightarrow\ a+(b++(c,d))\ \Rightarrow\ a+(b+(c+d))
$$
となり、演算順番が逆順になるといった問題点がある。そこで、スタックのように
$$
a+b+c+d\ \Rightarrow\ +(d,c,b,a)
$$
とすることで
$$
+(d,c,b,a)\ \Rightarrow\ +(c,b,a)+d\ \Rightarrow\ (+(b,a)+c)+d\ \Rightarrow\ ((a+b)+c)+d
$$
のようにこの問題を解決する。2項のときはこのような問題は生じなかったが、多項になると生じるのである。
流石にここでSFINAE全開で分岐するとグローバル領域の汚染がとても見てられないものになるため、添え物程度のSFINAEにするために以下のようにする。

```C++
		//Opによる2項演算であるかの判定
		template <class, class>
		struct is_binary_operation : false_type {};
		template <class Op, class First, class Second, class... Types>
		struct is_binary_operation<Op, expr_wrapper<Op, type_tuple<First, Second, Types...>>> : cat_bool<is_operation_tag<Op>::value> {};


		//lhsとrhsがともにexpr_wrapperである場合
		template <class Op, class L, class R, bool = is_binary_operation<Op, L>::value, bool = is_binary_operation<Op, R>::value>
		struct Binary1 {
			//LとR共にOpではない
			static constexpr auto operation(const L& lhs, const R& rhs)  -> expr_wrapper<Op, type_tuple<R, L>> {
				return expr_wrapper<Op, type_tuple<R, L>>(rhs, lhs);
			}
		};
		template <class Op, class L1, class L2, class... Ls, class R1, class R2, class... Rs>
		struct Binary1<Op, expr_wrapper<Op, type_tuple<L1, L2, Ls...>>, expr_wrapper<Op, type_tuple<R1, R2, Rs...>>, true, true> {
			//LとR共にOpである
			static constexpr auto operation(const expr_wrapper<Op, type_tuple<L1, L2, Ls...>>& lhs, const expr_wrapper<Op, type_tuple<R1, R2, Rs...>>& rhs)  -> expr_wrapper<Op, type_tuple<tuple<R1, R2, Rs...>, tuple<L1, L2, Ls...>>> {
				return expr_wrapper<Op, type_tuple<tuple<R1, R2, Rs...>, tuple<L1, L2, Ls...>>>(rhs.terms_m, lhs.terms_m);
			}
		};
		template <class Op, class L1, class L2, class... Ls, class R>
		struct Binary1<Op, expr_wrapper<Op, type_tuple<L1, L2, Ls...>>, R, true, false> {
			//LのみOpである
			static constexpr auto operation(const expr_wrapper<Op, type_tuple<L1, L2, Ls...>>& lhs, const R& rhs)  -> expr_wrapper<Op, type_tuple<R, tuple<L1, L2, Ls...>>> {
				return expr_wrapper<Op, type_tuple<R, tuple<L1, L2, Ls...>>>(rhs, lhs.terms_m);
			}
		};
		template <class Op, class L, class R1, class R2, class... Rs>
		struct Binary1<Op, L, expr_wrapper<Op, type_tuple<R1, R2, Rs...>>, false, true> {
			//RのみOpである
			static constexpr auto operation(const L& lhs, const expr_wrapper<Op, type_tuple<R1, R2, Rs...>>& rhs)  -> expr_wrapper<Op, type_tuple<tuple<R1, R2, Rs...>, L>> {
				return expr_wrapper<Op, type_tuple<tuple<R1, R2, Rs...>, L>>(rhs.terms_m, lhs);
			}
		};
		//lhsのみがexpr_wrapperである場合
		template <class Op, class L, class R>
		struct Binary2 {
			//LがOpではない
			static constexpr auto operation(const L& lhs, const R& rhs)  -> expr_wrapper<Op, type_tuple<R, L>> {
				return expr_wrapper<Op, type_tuple<R, L>>(rhs, lhs);
			}
		};
		template <class Op, class L1, class L2, class... Ls, class R>
		struct Binary2<Op, expr_wrapper<Op, type_tuple<L1, L2, Ls...>>, R> {
			//LがOpである
			static constexpr auto operation(const expr_wrapper<Op, type_tuple<L1, L2, Ls...>>& lhs, const R& rhs)  -> expr_wrapper<Op, type_tuple<R, tuple<L1, L2, Ls...>>> {
				return expr_wrapper<Op, type_tuple<R, tuple<L1, L2, Ls...>>>(rhs, lhs.terms_m);
			}
		};
		//rhsのみがexpr_wrapperである場合
		template <class Op, class L, class R>
		struct Binary3 {
			//RがOpではない
			static constexpr auto operation(const L& lhs, const R& rhs)  -> expr_wrapper<Op, type_tuple<R, L>> {
				return expr_wrapper<Op, type_tuple<R, L>>(rhs, lhs);
			}
		};
		template <class Op, class L, class R1, class R2, class... Rs>
		struct Binary3<Op, L, expr_wrapper<Op, type_tuple<R1, R2, Rs...>>> {
			//RがOpである
			static constexpr auto operation(const L& lhs, const expr_wrapper<Op, type_tuple<R1, R2, Rs...>>& rhs)  -> expr_wrapper<Op, type_tuple<tuple<R1, R2, Rs...>, L>> {
				return expr_wrapper<Op, type_tuple<tuple<R1, R2, Rs...>, L>>(rhs.terms_m, lhs);
			}
		};
```

SFINAEは便利だから使うけど、使わなくても十分に実現可能なら極力使わない。
というわけでこれを用いた場合における2項演算の加算の場合の定義を示す。

```C++
		template <class LOp, class LExpr, class ROp, class RExpr, class = typename enable_if<!is_ident_tag<LOp>::value && !is_ident_tag<ROp>::value>::type>
		constexpr auto operator+(const expr_wrapper<LOp, LExpr>& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> decltype(Binary1<add_tag, expr_wrapper<LOp, LExpr>, expr_wrapper<ROp, RExpr>>::operation(lhs, rhs)) {
			return Binary1<add_tag, expr_wrapper<LOp, LExpr>, expr_wrapper<ROp, RExpr>>::operation(lhs, rhs);
		}
		template <class LOp, class LExpr, class R, class = typename enable_if<!is_ident_tag<LOp>::value && !is_expr_wrapper<R>::value>::type>
		constexpr auto operator+(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> decltype(Binary2<add_tag, expr_wrapper<LOp, LExpr>, R>::operation(lhs, rhs)) {
			return Binary2<add_tag, expr_wrapper<LOp, LExpr>, R>::operation(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value && !is_ident_tag<ROp>::value>::type>
		constexpr auto operator+(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> decltype(Binary3<add_tag, L, expr_wrapper<ROp, RExpr>>::operation(lhs, rhs)) {
			return Binary3<add_tag, L, expr_wrapper<ROp, RExpr>>::operation(lhs, rhs);
		}
```

また、全ての2項演算については以下のようにマクロを用いて定義をすることができる。

```C++
#define BINARY_OPERATION(NAME, OP)\
		template <class LOp, class LExpr, class ROp, class RExpr, class = typename enable_if<!is_ident_tag<LOp>::value && !is_ident_tag<ROp>::value>::type>\
		constexpr auto operator##OP(const expr_wrapper<LOp, LExpr>& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> decltype(Binary1<NAME##_tag, expr_wrapper<LOp, LExpr>, expr_wrapper<ROp, RExpr>>::operation(lhs, rhs)) {\
			return Binary1<NAME##_tag, expr_wrapper<LOp, LExpr>, expr_wrapper<ROp, RExpr>>::operation(lhs, rhs);\
		}\
		template <class LOp, class LExpr, class R, class = typename enable_if<!is_ident_tag<LOp>::value && !is_expr_wrapper<R>::value>::type>\
		constexpr auto operator##OP(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> decltype(Binary2<NAME##_tag, expr_wrapper<LOp, LExpr>, R>::operation(lhs, rhs)) {\
			return Binary2<NAME##_tag, expr_wrapper<LOp, LExpr>, R>::operation(lhs, rhs);\
		}\
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value && !is_ident_tag<ROp>::value>::type>\
		constexpr auto operator##OP(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> decltype(Binary3<NAME##_tag, L, expr_wrapper<ROp, RExpr>>::operation(lhs, rhs)) {\
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
		BINARY_OPERATION(less_then_equal, <=);
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
```

セミコロンを付けてるのは何となく。ただ単に文末にセミコロンを付けるといった統一性が欲しかっただけ。
そして、これに対応するように`expr_wrapper`の方も改良する。
まずは前回の`expr_wrapper`内に存在した演算子のオーバーロード群を1つのマクロにまとめる。
また、このときインクリメントとデクリメントについての`operation_tag`を無駄を省くために前置と後置で区別した。

```C++
		//単項演算の定義
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

		//インクリメント・デクリメントの定義
#define EXPR_WRAPPER_INCREMENT_DECREMENT_IMPL(NAME, OP) \
		constexpr expr_wrapper<pre##NAME##_tag, type_tuple<expr_wrapper>> operator##OP() const {\
			return expr_wrapper<pre##NAME##_tag, type_tuple<expr_wrapper>>(*this);\
		}\
		constexpr expr_wrapper<post##NAME##_tag, type_tuple<expr_wrapper>> operator##OP(int) const {\
			return expr_wrapper<post##NAME##_tag, type_tuple<expr_wrapper>>(*this);\
		}\

#define EXPR_WRAPPER_INCREMENT_DECREMENT \
		EXPR_WRAPPER_INCREMENT_DECREMENT_IMPL(increment, ++);\
		EXPR_WRAPPER_INCREMENT_DECREMENT_IMPL(decrement, --);

		//2項関係の定義
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

		//expr_wrapperの全ての演算子のオーバーロード
#define EXPR_WRAPPER_OPERATOR_OVERLOAD \
		EXPR_WRAPPER_UNARY_OPERATION \
		EXPR_WRAPPER_INCREMENT_DECREMENT \
		EXPR_WRAPPER_BINARY_RELATION \
		/*コンマ演算子*/ \
		template <class Expr>\
		constexpr expr_wrapper<comma_tag, type_tuple<expr_wrapper, Expr>> operator,(const Expr& expr) const {\
			return expr_wrapper<comma_tag, type_tuple<expr_wrapper, Expr>>(*this, expr);\
		}\
		/*関数オブジェクト呼び出し*/ \
		template <class... Args>\
		constexpr expr_wrapper<functor_tag, type_tuple<expr_wrapper, tuple<Args...>, typename tuple<Args...>::sequence_type>> operator()(Args&&... args) const {\
			return expr_wrapper<functor_tag, type_tuple<expr_wrapper, tuple<Args...>, typename tuple<Args...>::sequence_type>>(*this, forward<Args>(args)...);\
		}
```

これにより`expr_wrapper`内に`EXPR_WRAPPER_OPERATOR_OVERLOAD`を定義することで演算子のオーバーロードが全て定義される。また、マクロ名の衝突も十分に考えられるが、`EXPR_WRAPPER_OPERATOR_OVERLOAD`で衝突するとか当たり屋でしかないと思う。
というわけで、2項演算の`expr_wrapper`は以下のようになる。

```C++
		//2項演算
		template <size_t N, class First, class Second, class... Types>
		struct expr_wrapper<operation_tag<N>, type_tuple<First, Second, Types...>> {
			tuple<First, Second, Types...> terms_m;

			constexpr explicit expr_wrapper(const First& first, const Second& second, const Types&... args) : terms_m(first, second, args...) {}
			constexpr explicit expr_wrapper(const tuple<First, Second, Types...>& t) : terms_m(t) {}

			using tuple_type = tuple<First, Second, Types...>;

			EXPR_WRAPPER_OPERATOR_OVERLOAD;
		};
```

後は`Eval`の定義を列挙するだけである。2項演算は1回後回しにして2項演算以外の場合を示す。

```C++
		//expr_wrapperの実行
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
		//単項演算
#define EVAL_UNARY_OPERATION(NAME, OP)\
		template <class Expr>\
		struct Eval<expr_wrapper<NAME##_tag, type_tuple<Expr>>> {\
			template <class Result, class Tuple>\
			static constexpr auto eval(Result&, const expr_wrapper<NAME##_tag, type_tuple<Expr>>& expr, Tuple&& t) {\
				return OP##Eval<Expr>::eval(expr.terms_m, forward<Tuple>(t));\
			}\
			template <class Tuple>\
			static constexpr auto eval(const expr_wrapper<NAME##_tag, type_tuple<Expr>>& expr, Tuple&& t) {\
				return OP##Eval<Expr>::eval(expr.terms_m, forward<Tuple>(t));\
			}\
		};
		EVAL_UNARY_OPERATION(add, +);
		EVAL_UNARY_OPERATION(sub, -);
		EVAL_UNARY_OPERATION(mul, *);
		EVAL_UNARY_OPERATION(flip, !);
		EVAL_UNARY_OPERATION(bit_flip, ~);
#undef EVAL_UNARY_OPERATION
		//インクリメント・デクリメント
#define EVAL_INCREMENT_DECREMENT(NAME, OP)\
		template <class Expr>\
		struct Eval<expr_wrapper<pre##NAME##_tag, type_tuple<Expr>>> {\
			template <class Result, class Tuple>\
			static constexpr auto eval(Result&, const expr_wrapper<pre##NAME##_tag, type_tuple<Expr>>& expr, Tuple&& t) {\
				return OP##Eval<Expr>::eval(expr.terms_m, forward<Tuple>(t));\
			}\
			template <class Tuple>\
			static constexpr auto eval(const expr_wrapper<pre##NAME##_tag, type_tuple<Expr>>& expr, Tuple&& t) {\
				return OP##Eval<Expr>::eval(expr.terms_m, forward<Tuple>(t));\
			}\
		};\
		template <class Expr>\
		struct Eval<expr_wrapper<post##NAME##_tag, type_tuple<Expr>>> {\
			template <class Result, class Tuple>\
			static constexpr auto eval(Result&, const expr_wrapper<post##NAME##_tag, type_tuple<Expr>>& expr, Tuple&& t) {\
				return Eval<Expr>::eval(expr.terms_m, forward<Tuple>(t))##OP;\
			}\
			template <class Tuple>\
			static constexpr auto eval(const expr_wrapper<post##NAME##_tag, type_tuple<Expr>>& expr, Tuple&& t) {\
				return Eval<Expr>::eval(expr.terms_m, forward<Tuple>(t))##OP;\
			}\
		};
		EVAL_INCREMENT_DECREMENT(increment, ++);
		EVAL_INCREMENT_DECREMENT(decrement, --);
#undef EVAL_INCREMENT_DECREMENT
		//添え字アクセス
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result&, const expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.terms_m.get<0>(), forward<Tuple>(t))[Eval<Expr2>::eval(expr.terms_m.get<1>(), forward<Tuple>(t))];
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.terms_m.get<0>(), forward<Tuple>(t))[Eval<Expr2>::eval(expr.terms_m.get<1>(), forward<Tuple>(t))];
			}
		};
		//2項関係
#define EVAL_BINARY_RELATION(NAME, OP) \
		template <class Expr1, class Expr2>\
		struct Eval<expr_wrapper<NAME##_tag, type_tuple<Expr1, Expr2>>> {\
			template <class Result, class Tuple>\
			static constexpr auto eval(Result&, const expr_wrapper<NAME##_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {\
				return Eval<Expr1>::eval(expr.terms_m.get<0>(), forward<Tuple>(t))##OP##Eval<Expr2>::eval(expr.terms_m.get<1>(), forward<Tuple>(t));\
			}\
			template <class Tuple>\
			static constexpr auto eval(const expr_wrapper<NAME##_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {\
				return Eval<Expr1>::eval(expr.terms_m.get<0>(), forward<Tuple>(t))##OP##Eval<Expr2>::eval(expr.terms_m.get<1>(), forward<Tuple>(t));\
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
		//コンマ演算子
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<comma_tag, type_tuple<Expr1, Expr2>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result& result, const expr_wrapper<comma_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				using result_type = decltype(Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t)));
				Eval<Expr1>::eval(result, expr.terms_m.get<0>(), forward<Tuple>(t));
				return (result.second) ? Eval<Expr2>::eval(result, expr.terms_m.get<1>(), forward<Tuple>(t)) : result_type();
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<comma_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				Eval<Expr1>::eval(expr.terms_m.get<0>(), forward<Tuple>(t));
				return Eval<Expr2>::eval(expr.terms_m.get<1>(), forward<Tuple>(t));
			}
		};
		//関数オブジェクト
		template <class Expr, class... Types, size_t... Indices>
		struct Eval<expr_wrapper<functor_tag, type_tuple<Expr, tuple<Types...>, index_tuple<size_t, Indices...>>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result&, const expr_wrapper<functor_tag, type_tuple<Expr, tuple<Types...>, index_tuple<size_t, Indices...>>>& expr, Tuple&& t) {
				return invoke(Eval<Expr>::eval(expr.terms_m, forward<Tuple>(t))
					, Eval<Types>::eval(expr.args_m.get<Indices>, forward<Tuple>(t))...);
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<functor_tag, type_tuple<Expr, tuple<Types...>, index_tuple<size_t, Indices...>>>& expr, Tuple&& t) {
				return invoke(Eval<Expr>::eval(expr.terms_m, forward<Tuple>(t))
					, Eval<Types>::eval(expr.args_m.get<Indices>, forward<Tuple>(t))...);
			}
		};
```

このとき、`return`識別子を許容しない構文で`result`を`eval`の引数に渡さないことで、その構文を除去している。

次に2項演算の`Eval`を定義する。というわけで理論通りに記述した加算の場合のコードを示す。

```C++
		template <class T>
		struct Eval_tuple {
			template <class Tuple>
			static constexpr auto eval(Tuple&& t, const T& x) { return Eval<T>::eval(x, forward<Tuple>(t)); }
		};
		template <class... Types>
		struct Eval_tuple<tuple<Types...>> {
			template <class Tuple, class First>
			static constexpr auto eval_impl(Tuple&& t, const First& first) {
				return Eval_tuple<First>::eval(forward<Tuple>(t), first);
			}
			template <class Tuple, class First, class Second, class... Args>
			static constexpr auto eval_impl(Tuple&& t, const First& first, const Second& second, const Args&... args) {
				return eval_impl(forward<Tuple>(t), second, args...) + eval_impl(forward<Tuple>(t), first);
			}

			//Evalのためのapply
			template <class Tuple, size_t... Indices>
			static constexpr auto apply(Tuple&& t, const tuple<Types...>& x, index_tuple<size_t, Indices...>) {
				return invoke(eval_impl<Tuple, Types...>, forward<Tuple>(t), x.get<Indices>()...);
			}
			template <class Tuple>
			static constexpr auto eval(Tuple&& t, const tuple<Types...>& x) {
				return apply(forward<Tuple>(t), x, typename tuple<Types...>::sequence_type());
			}
		};
		//2項関係の特に2項演算
		template <class First, class Second, class... Types>
		struct Eval<expr_wrapper<add_tag, type_tuple<First, Second, Types...>>> {
			template <class Result, class Tuple>
			static constexpr auto eval(Result&, const expr_wrapper<add_tag, type_tuple<First, Second, Types...>>& expr, Tuple&& t) {
				return Eval_tuple<tuple<First, Second, Types...>>::eval(forward<Tuple>(t), expr.terms_m);
			}
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<add_tag, type_tuple<First, Second, Types...>>& expr, Tuple&& t) {
				return Eval_tuple<tuple<First, Second, Types...>>::eval(forward<Tuple>(t), expr.terms_m);
			}
		};
```

というわけで、これをマクロを用いて構築する。

```C++
		//2項関係の特に2項演算
		template <class, class T>
		struct Eval_tuple {
			template <class Tuple>
			static constexpr auto eval(Tuple&& t, const T& x) { return Eval<T>::eval(x, forward<Tuple>(t)); }
		};
#define EVAL_BINARY_OPERATION(NAME, OP)\
		template <class... Types>\
		struct Eval_tuple<NAME##_tag, tuple<Types...>> {\
			template <class Tuple, class First>\
			static constexpr auto eval_impl(Tuple&& t, const First& first) {\
				return Eval_tuple<NAME##_tag, First>::eval(forward<Tuple>(t), first);\
			}\
			template <class Tuple, class First, class Second, class... Args>\
			static constexpr auto eval_impl(Tuple&& t, const First& first, const Second& second, const Args&... args) {\
				return eval_impl(forward<Tuple>(t), second, args...)##OP##eval_impl(forward<Tuple>(t), first);\
			}\
			/*Evalのためのapply*/\
			template <class Tuple, size_t... Indices>\
			static constexpr auto apply(Tuple&& t, const tuple<Types...>& x, index_tuple<size_t, Indices...>) {\
				return invoke(eval_impl<Tuple, Types...>, forward<Tuple>(t), x.get<Indices>()...);\
			}\
			template <class Tuple>\
			static constexpr auto eval(Tuple&& t, const tuple<Types...>& x) {\
				return apply(forward<Tuple>(t), x, typename tuple<Types...>::sequence_type());\
			}\
		};\
		template <class First, class Second, class... Types>\
		struct Eval<expr_wrapper<NAME##_tag, type_tuple<First, Second, Types...>>> {\
			template <class Result, class Tuple>\
			static constexpr auto eval(Result&, const expr_wrapper<NAME##_tag, type_tuple<First, Second, Types...>>& expr, Tuple&& t) {\
				return Eval_tuple<NAME##_tag, tuple<First, Second, Types...>>::eval(forward<Tuple>(t), expr.terms_m);\
			}\
			template <class Tuple>\
			static constexpr auto eval(const expr_wrapper<NAME##_tag, type_tuple<First, Second, Types...>>& expr, Tuple&& t) {\
				return Eval_tuple<NAME##_tag, tuple<First, Second, Types...>>::eval(forward<Tuple>(t), expr.terms_m);\
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
```

これで一通り完成。

# 数式処理
構文木の構成を大幅に変えたため、それらに合わせた処理方法をしなければならない。
## 数式の出力
今までの数式の出力では$a+(b+c)$のような出力には対応していなかったが、今回のような設計により比較的容易に出力可能となる。例によって四則演算と添え字アクセスのみ対応する。
仕様が変わったのは2項演算についてのみであるため、そのコードのみを示す。

```C++
		//2項演算
		template <class, class T>
		struct Estring_tuple {
			static constexpr auto estring(const T& x) { return Estring<T>::estring(x); }
		};
#define ESTRIGN_BINARY_OPERATION(NAME, OP)\
		template <class... Types>\
		struct Estring_tuple<NAME##_tag, tuple<Types...>> {\
			template <class First>\
			static constexpr auto estring_impl2(const First& first) {\
				std::stringstream ss;\
				if (is_tuple<First>::value || (operator_precedence<First>::value >= operator_precedence<expr_wrapper<NAME##_tag, type_tuple<Types...>>>::value)) {\
					ss << "(" << Estring_tuple<NAME##_tag, First>::estring(first) << ')';\
				}\
				else {\
					ss << Estring_tuple<NAME##_tag, First>::estring(first);\
				}\
				return ss.str();\
			}\
			template <class First>\
			static constexpr auto estring_impl1(const First& first) {\
				std::stringstream ss;\
				ss << Estring_tuple<NAME##_tag, First>::estring(first);\
				return ss.str();\
			}\
			template <class First, class Second, class... Args>\
			static constexpr auto estring_impl1(const First& first, const Second& second, const Args&... args) {\
				std::stringstream ss;\
				/*先頭には括弧を付けない((a+b)+cみたいのは無し)*/\
				ss << estring_impl1(second, args...) << (#OP) << estring_impl2(first);\
				return ss.str();\
			}\
			static constexpr auto estring(const tuple<Types...>& x) {\
				return apply(estring_impl1<Types...>, x);\
			}\
		};\
		template <class First, class Second, class... Types>\
		struct Estring<expr_wrapper<NAME##_tag, type_tuple<First, Second, Types...>>> {\
			static constexpr auto estring(const expr_wrapper<NAME##_tag, type_tuple<First, Second, Types...>>& expr) {\
				return Estring_tuple<NAME##_tag, tuple<First, Second, Types...>>::estring(expr.terms_m);\
			}\
		};
		ESTRIGN_BINARY_OPERATION(add, +);
		ESTRIGN_BINARY_OPERATION(sub, -);
		ESTRIGN_BINARY_OPERATION(mul, *);
		ESTRIGN_BINARY_OPERATION(div, /);
#undef ESTRIGN_BINARY_OPERATION
```

本質的には`Eval`と同じ構造をしている。ただ、若干再帰の構造が変わっているのと前回までのバグを修正している。


## 基本的な式の整理
式の整理ということで主に結合法則、分配法則、交換法則といったものが対称になるが、実装上法則の濫用は認めないといった方針をとる。例えば分配法則であんもくてきに結合法則を使用するとか。
どのように使うかを例えるなら、ペアノシステムから自然数を構成し演算を定義するみたいなそんな感じの使い方。普段私たちはそれらをあまり意識せずに数式を変形したりしているため、それらの式変形法を自動的にまとめてフィッティングさせるようなルーチンをも必要になるだろう。

### 分配法則の適用
分配法則というのは、少しだけ抽象的に扱えば、写像$f$を写像$g$で分配するといったことであれば
$$
h:g(a,b,f(c,d,e),f,g)\to f(g(a,b,c,f,g),g(a,b,d,f,g),g(a,b,e,f,g))
$$
といった操作をする写像$h$である。実は乗法的とかそんなものは上の関係式さえ成り立てばあまり関係無かったりするが、そこまで抽象的なものを扱うつもりはない。$f$と$g$の記号が重複しているのは気にない。
また、実装をするときには前の$h$を例にとれば、$g$の保持するタプルのそれぞれの要素が分配可能ならその要素(タプル)の要素に対して分配をしていき、それぞれを結合するといった操作をする。例を示すと
$$
\begin{aligned}
a\times (b+c)\times d\times (e-f)&\to a\times (b+c)\times d\times (e-f)\ \ \ (aは分配不可) \\\
&\to a\times b\times d\times (e-f)+a\times c\times d\times (e-f) \\\
&\to a\times b\times d\times (e-f)+a\times c\times d\times (e-f)\ \ \ (dは分配不可) \\\
&\to (a\times b\times d\times e - a\times b\times d\times f) +(a\times c\times d\times e-a\times c\times d\times f)
\end{aligned}
$$
みたいな感じ。また、これらの操作は再帰的(というより多分帰納的)に行うことをも留意する。
また、単項演算については
$$
-(a+b\times c)\to -1\times (a+b\times c)
$$
のように考えることで乗算と等価に考えることができる。
というわけで、以下コード。

```C++
		//変数及び定数
		template <class T>
		struct Distributive {
			static constexpr auto distributive(const T& expr) { return expr; }
		};
		template <size_t N>
		struct Distributive<expr_variable<N>> {
			static constexpr auto distributive(const expr_variable<N>& expr) { return expr; }
		};
		//単項演算
		template <class Expr>
		struct Distributive<expr_wrapper<add_tag, type_tuple<Expr>>> {
			using expr_type = expr_wrapper<add_tag, type_tuple<Expr>>;
			static constexpr auto distributive(const expr_type& expr) {
				auto temp = 1 * expr.terms_m;
				using temp_type = typename decay<decltype(temp)>::type;
				return Distributive<temp_type>::distributive(temp);
			}
		};
		template <class Expr>
		struct Distributive<expr_wrapper<sub_tag, type_tuple<Expr>>> {
			using expr_type = expr_wrapper<sub_tag, type_tuple<Expr>>;
			static constexpr auto distributive(const expr_type& expr) {
				auto temp = -1 * expr.terms_m;
				using temp_type = typename decay<decltype(temp)>::type;
				return Distributive<temp_type>::distributive(temp);
			}
		};
		//加減算
		template <class First, class Second, class... Types>
		struct Distributive<expr_wrapper<add_tag, type_tuple<First, Second, Types...>>> {
			using expr_type = expr_wrapper<add_tag, type_tuple<First, Second, Types...>>;
			static constexpr auto distributive(const expr_type& expr) { return expr; }
		};
		template <class First, class Second, class... Types>
		struct Distributive<expr_wrapper<sub_tag, type_tuple<First, Second, Types...>>> {
			using expr_type = expr_wrapper<sub_tag, type_tuple<First, Second, Types...>>;
			static constexpr auto distributive(const expr_type& expr) { return expr; }
		};


		//分配法則を作用
		//Op1:分配する演算(乗除算等)，Op2:分配される演算(加減算等)
		template <class, class, class T, size_t, class, class, bool = is_tuple<T>::value>
		struct Distributive_tuple3 {};
		template <class Op1, class Op2, class T, size_t N, size_t... Indices1, size_t... Indices2>
		struct Distributive_tuple3<Op1, Op2, T, N, index_tuple<size_t, Indices1...>, index_tuple<size_t, Indices2...>, false> {
			template <class Tuple>
			static constexpr auto distributive(const Tuple& t, const T& x) {
				return make_expr_wrapper<Op1>(t.get<Indices1>()..., x, t.get<Indices2>()...);
			}
		};
		template <class Op1, class Op2, class... Types, size_t N, size_t... Indices1, size_t... Indices2>
		struct Distributive_tuple3<Op1, Op2, tuple<Types...>, N, index_tuple<size_t, Indices1...>, index_tuple<size_t, Indices2...>, true> {
			template <class Tuple, class First>
			static constexpr auto distributive_impl(const Tuple& t, const First& first) {
				return Distributive_tuple3<Op1, Op2, First, N
					, index_tuple<size_t, Indices1...>, index_tuple<size_t, Indices2...>>::distributive(t, first);
			}
			template <class Tuple, class First, class Second, class... Args>
			static constexpr auto distributive_impl(const Tuple& t, const First& first, const Second& second, const Args&... args) {
				return reverse_operation<Op2>(distributive_impl(t, first), distributive_impl(t, second, args...));
			}

			//Distributiveのためのapply
			template <class Tuple, size_t... Indices>
			static constexpr auto apply(const Tuple& t, const tuple<Types...>& x, index_tuple<size_t, Indices...>) {
				return invoke(distributive_impl<Tuple, Types...>, t, x.get<Indices>()...);
			}
			template <class Tuple>
			static constexpr auto distributive(const Tuple& t, const tuple<Types...>& x) {
				return apply(t, x, typename tuple<Types...>::sequence_type());
			}
		};

		template <class Expr, class Tuple, size_t N = 0, bool = (N == tuple_size<Tuple>::value - 1), bool = (operator_precedence<typename at_type_tuple<N, Tuple>::type>::value > operator_precedence<Expr>::value)>
		struct Distributive_tuple2 {};


		//タプルの全ての要素に対してDistributive_tuple2を作用させてOp2で結合
		//Expr1:分配する演算(乗除算等)，Expr2:分配される演算(加減算等)
		template <class Expr1, class Expr2, class T, size_t N>
		struct Distributive_tuple2_impl {
			static constexpr auto distributive(const T& x) {
				return Distributive_tuple2<Expr1, typename T::tuple_type, N + 1>::distributive(x.terms_m);
			}
		};
		template <class Expr1, class Expr2, class... Types, size_t N>
		struct Distributive_tuple2_impl<Expr1, Expr2, tuple<Types...>, N> {
			using op2_type = typename expr_tag<Expr2>::type;

			template <class First>
			static constexpr auto distributive_impl(const First& first) {
				return Distributive_tuple2_impl<Expr1, Expr2, First, N>::distributive(first);
			}
			template <class First, class Second, class... Args>
			static constexpr auto distributive_impl(const First& first, const Second& second, const Args&... args) {
				return reverse_operation<op2_type>(distributive_impl(first), distributive_impl(second, args...));
			}

			static constexpr auto distributive(const tuple<Types...>& x) {
				return apply(distributive_impl<Types...>, x);
			}
		};

		//タプルの要素が分配可能かの識別
		//bool1:走査の終端判定，bool2:分配可能判定
		//<false, false>
		template <class Expr, class Tuple, size_t N>
		struct Distributive_tuple2<Expr, Tuple, N, false, false> {
			static constexpr auto distributive(const Tuple& x) {
				return Distributive_tuple2<Expr, Tuple, N + 1>::distributive(x);
			}
		};
		//<true, false>
		template <class Expr, class Tuple, size_t N>
		struct Distributive_tuple2<Expr, Tuple, N, true, false> {
			using op_type = typename expr_tag<Expr>::type;

			static constexpr auto distributive(const Tuple& x) {
				return expr_wrapper<op_type, typename Tuple::type_tuple_type>(x);
			}
		};
		//<false, true>
		template <class Expr, class Tuple, size_t N>
		struct Distributive_tuple2<Expr, Tuple, N, false, true> {
			using op_type = typename expr_tag<Expr>::type;

			static constexpr auto distributive(const Tuple& x) {
				//左右からかけるためのシーケンスの用意
				using low_index = typename index_range<size_t, 0, N>::type;
				using high_index = typename index_range<size_t, N + 1, Tuple::size()>::type;
				//分配法則の適用
				using tuple_type = typename decay<decltype(x.get<N>().terms_m)>::type;
				auto temp = Distributive_tuple3<op_type, typename expr_tag<typename at_type_tuple<N, Tuple>::type>::type, tuple_type
					, N, low_index, high_index>::distributive(x, x.get<N>().terms_m);

				using temp_type = typename decay<decltype(temp.terms_m)>::type;
				return Distributive_tuple2_impl<Expr, typename at_type_tuple<N, Tuple>::type
					, temp_type, N>::distributive(temp.terms_m);
			}
		};
		//<true, true>
		template <class Expr, class Tuple, size_t N>
		struct Distributive_tuple2<Expr, Tuple, N, true, true> {
			using op_type = typename expr_tag<Expr>::type;

			static constexpr auto distributive(const Tuple& x) {
				//左右からかけるためのシーケンスの用意
				using low_index = typename index_range<size_t, 0, N>::type;
				using high_index = typename index_range<size_t, N + 1, Tuple::size()>::type;
				//分配法則の適用
				using tuple_type = typename decay<decltype(x.get<N>().terms_m)>::type;
				return Distributive_tuple3<op_type, typename expr_tag<typename at_type_tuple<N, Tuple>::type>::type, tuple_type
					, N, low_index, high_index>::distributive(x, x.get<N>().terms_m);
			}
		};


		//タプルの要素を分配法則を適用済みのものにして再構成
		//bool1:走査の終端判定，bool2:走査する要素がタプルか判定
		//<false, false>
		template <class Op, class Tuple, size_t N = 0, bool = (N == tuple_size<Tuple>::value - 1), bool = is_tuple<typename at_type_tuple<N, Tuple>::type>::value>
		struct Distributive_tuple1 {
			static constexpr auto distributive(const Tuple& x) {
				return reverse_operation<Op>(Distributive<typename at_type_tuple<N, Tuple>::type>::distributive(x.get<N>())
					, Distributive_tuple1<Op, Tuple, N + 1>::distributive(x));
			}
		};
		//<true, false>
		template <class Op, class Tuple, size_t N>
		struct Distributive_tuple1<Op, Tuple, N, true, false> {
			static constexpr auto distributive(const Tuple& x) {
				return Distributive<typename at_type_tuple<N, Tuple>::type>::distributive(x.get<N>());
			}
		};
		//<false, true>
		template <class Op, class Tuple, size_t N>
		struct Distributive_tuple1<Op, Tuple, N, false, true> {
			static constexpr auto distributive(const Tuple& x) {
				//更にタプルを展開
				return reverse_operation<Op>(Distributive_tuple1<Op, typename at_type_tuple<N, Tuple>::type>::distributive(x.get<N>())
					, Distributive_tuple1<Op, Tuple, N + 1>::distributive(x));
			}
		};
		//<true, true>
		template <class Op, class Tuple, size_t N>
		struct Distributive_tuple1<Op, Tuple, N, true, true> {
			static constexpr auto distributive(const Tuple& x) {
				//更にタプルを展開
				return Distributive_tuple1<Op, typename at_type_tuple<N, Tuple>::type>::distributive(x.get<N>());
			}
		};

		//乗除算
		template <class First, class Second, class... Types>
		struct Distributive<expr_wrapper<mul_tag, type_tuple<First, Second, Types...>>> {
			using expr_type = expr_wrapper<mul_tag, type_tuple<First, Second, Types...>>;

			static constexpr auto distributive(const expr_type& expr) {
				//予めタプルの要素に対して分配法則を適用
				auto temp = Distributive_tuple1<mul_tag, tuple<First, Second, Types...>>::distributive(expr.terms_m);
				using temp_tuple_type = typename decay<decltype(temp.terms_m)>::type;
				//メインの分配法則の適用と式の再構築
				return Distributive_tuple2<expr_type, temp_tuple_type>::distributive(temp.terms_m);
			}
		};
		template <class First, class Second, class... Types>
		struct Distributive<expr_wrapper<div_tag, type_tuple<First, Second, Types...>>> {
			using expr_type = expr_wrapper<div_tag, type_tuple<First, Second, Types...>>;

			static constexpr auto distributive(const expr_type& expr) {
				//予めタプルの要素に対して分配法則を適用
				auto temp = Distributive_tuple1<div_tag, tuple<First, Second, Types...>>::distributive(expr.terms_m);
				using temp_tuple_type = typename decay<decltype(temp.terms_m)>::type;
				//メインの分配法則の適用と式の再構築
				return Distributive_tuple2<expr_type, temp_tuple_type>::distributive(temp.terms_m);
			}
		};
		template <class T>
		inline constexpr auto distributive(const T& expr) { return Distributive<T>::distributive(expr); }
```

ちなみに、`reverse_operation`は与えられた任意の演算子で引数全てを逆順から演算するものである。

```text
reverse_operation(a,b,c)→c+b+a
```

みたいな。
このような実装方法をとることにより、任意の演算で任意の演算を分配するといったコードへの拡張をも容易になる。

ここで、1つ留意しなければならないのは
$$
a\times(b+(c+d))=a\times b+(a\times c+a\times d)
$$
となるように処理していることである。このとき、括弧は外すことはできず、仮に外したものが演算結果とするならば、それは結合法則の適用を意味しており、このような処理の濫用は認めることはできない。

どうでもいいことであるが自分も試行錯誤する上では結構難しく感じた。ただ、出来上がったコードは意外と単純。難しくなっている要因は、基本的に全て再帰コードであることと、括弧を考慮した`expr_wrapper`の仕組みを導入したからである。
括弧を考慮した仕組みを導入しないと次の結合法則が分配法則がとても簡単に感じるほど複雑になる。あと簡約法則もかなり複雑になる。もちろん前の構造でもできない訳ではないが、やるなら簡単の方がいいだろう。

### 結合法則の適用
結合法則といっているが、結局のところ同じ演算子で結合されていれば
$$
a+(b+c)\to a+b+c
$$
のように括弧を外すだけなため、原理は非常に簡単。2項演算の`expr_wrapper`の保持しているタプルの中にタプルをもたないように展開するだけである。
再帰ベースだから手続き言語だと理論は簡単でも実装はやや面倒。あくまでも理論が単純なだけである。

```C++
		//bool:式のトップレベルに対して結合法則が作用可能か判定
		//定数及び変数
		template <class Op, class T, bool = is_same<typename expr_tag<T>::type, Op>::value>
		struct Associative {
			static constexpr auto associative(const T& expr) { return expr; }
		};
		template <class Op, size_t N, bool F>
		struct Associative<Op, expr_variable<N>, F> {
			static constexpr auto associative(const expr_variable<N>& expr) { return expr; }
		};
		//単項演算
		template <class Op, class Expr, bool F>
		struct Associative<Op, expr_wrapper<add_tag, type_tuple<Expr>>, F> {
			using expr_type = expr_wrapper<add_tag, type_tuple<Expr>>;
			static constexpr auto associative(const expr_type& expr) { return +Associative<Op, Expr>::associative(expr.terms); }
		};
		template <class Op, class Expr, bool F>
		struct Associative<Op, expr_wrapper<sub_tag, type_tuple<Expr>>, F> {
			using expr_type = expr_wrapper<sub_tag, type_tuple<Expr>>;
			static constexpr auto associative(const expr_type& expr) { return -Associative<Op, Expr>::associative(expr.terms); }
		};

		//シーケンスからtupleの構築
		template <class Tuple1, class Tuple2, size_t... Indices1, size_t... Indices2, size_t... Indices3>
		constexpr auto Associative_tuple2_impl(const Tuple1& t1, const Tuple2& t2, index_tuple<size_t, Indices1...>, index_tuple<size_t, Indices2...>, index_tuple<size_t, Indices3...>) {
			return make_tuple(t1.get<Indices1>()..., t2.get<Indices2>()..., t1.get<Indices3>()...);
		}


		//タプルの要素が分配可能かの識別
		//Op:式のトップレベルの演算と結合対象の演算でそれぞれは一致する
		//bool1:走査の終端判定，bool2:走査する要素がタプルか判定
		template <class Op, class Tuple, size_t N = 0, bool = (N == tuple_size<Tuple>::value - 1), bool = is_tuple<typename at_type_tuple<N, Tuple>::type>::value>
		struct Associative_tuple2 {};
		//<false, false>
		template <class Op, class Tuple, size_t N>
		struct Associative_tuple2<Op, Tuple, N, false, false> {
			static constexpr auto associative(const Tuple& x) {
				return Associative_tuple2<Op, Tuple, N + 1>::associative(x);
			}
		};
		//<true, false>
		template <class Op, class Tuple, size_t N>
		struct Associative_tuple2<Op, Tuple, N, true, false> {
			static constexpr auto associative(const Tuple& x) {
				return expr_wrapper<Op, typename Tuple::type_tuple_type>(x);
			}
		};
		//<false, true>
		template <class Op, class Tuple, size_t N>
		struct Associative_tuple2<Op, Tuple, N, false, true> {
			static constexpr auto associative(const Tuple& x) {
				//左右から結合するためのシーケンスの用意
				using low_index = typename index_range<size_t, 0, N>::type;
				using high_index = typename index_range<size_t, N + 1, Tuple::size()>::type;
				//タプルに対して結合法則の適用
				auto temp = Associative_tuple2<Op, typename at_type_tuple<N, Tuple>::type>::associative(x.get<N>());
				//tempを展開するためのシーケンス
				using temp_tuple_type = typename decay<decltype(temp.terms_m)>::type;
				using temp_sequence = typename temp_tuple_type::sequence_type;
				auto temp2 = Associative_tuple2_impl(x, temp.terms_m, low_index(), temp_sequence(), high_index());
				using temp2_type = typename decay<decltype(temp2)>::type;

				return Associative_tuple2<Op, temp2_type, N + temp_sequence::value>::associative(temp2);
			}
		};
		//<true, true>
		template <class Op, class Tuple, size_t N>
		struct Associative_tuple2<Op, Tuple, N, true, true> {
			static constexpr auto associative(const Tuple& x) {
				//左右から結合するためのシーケンスの用意
				using low_index = typename index_range<size_t, 0, N>::type;
				using high_index = typename index_range<size_t, N + 1, Tuple::size()>::type;
				//タプルに対して結合法則の適用
				auto temp = Associative_tuple2<Op, typename at_type_tuple<N, Tuple>::type>::associative(x.get<N>());
				//tempを展開するためのシーケンス
				using temp_tuple_type = typename decay<decltype(temp.terms_m)>::type;
				using temp_sequence = typename temp_tuple_type::sequence_type;
				auto temp2 = Associative_tuple2_impl(x, temp, low_index(), temp_sequence(), high_index());
				using temp2_type = typename decay<decltype(temp2)>::type;

				return expr_wrapper<Op, typename temp2_type::type_tuple_type>(temp2);
			}
		};


		//タプルの要素を結合法則を適用済みのものにして再構成
		//Op1:再構成で結合する演算，Op2:結合法則を作用させる演算
		//bool1:走査の終端判定，bool2:走査する要素がタプルか判定
		//<false, false>
		template <class Op1, class Op2, class Tuple, size_t N = 0, bool = (N == tuple_size<Tuple>::value - 1), bool = is_tuple<typename at_type_tuple<N, Tuple>::type>::value>
		struct Associative_tuple1 {
			static constexpr auto associative(const Tuple& x) {
				return reverse_operation<Op1>(Associative<Op2, typename at_type_tuple<N, Tuple>::type>::associative(x.get<N>())
					, Associative_tuple1<Op1, Op2, Tuple, N + 1>::associative(x));
			}
		};
		//<true, false>
		template <class Op1, class Op2, class Tuple, size_t N>
		struct Associative_tuple1<Op1, Op2, Tuple, N, true, false> {
			static constexpr auto associative(const Tuple& x) {
				return Associative<Op2, typename at_type_tuple<N, Tuple>::type>::associative(x.get<N>());
			}
		};
		//<false, true>
		template <class Op1, class Op2, class Tuple, size_t N>
		struct Associative_tuple1<Op1, Op2, Tuple, N, false, true> {
			static constexpr auto associative(const Tuple& x) {
				//更にタプルを展開
				return reverse_operation<Op1>(Associative_tuple1<Op1, Op2, typename at_type_tuple<N, Tuple>::type>::associative(x.get<N>())
					, Associative_tuple1<Op1, Op2, Tuple, N + 1>::associative(x));
			}
		};
		//<true, true>
		template <class Op1, class Op2, class Tuple, size_t N>
		struct Associative_tuple1<Op1, Op2, Tuple, N, true, true> {
			static constexpr auto associative(const Tuple& x) {
				//更にタプルを展開
				return Associative_tuple1<Op1, Op2, typename at_type_tuple<N, Tuple>::type>::associative(x.get<N>());
			}
		};

		//2項演算
#define ASSOCIATIVE_BINARY_OPERATION(NAME)\
		template <class Op, class First, class Second, class... Types>\
		struct Associative<Op, expr_wrapper<NAME##_tag, type_tuple<First, Second, Types...>>, true> {\
			using expr_type = expr_wrapper<NAME##_tag, type_tuple<First, Second, Types...>>;\
			static constexpr auto associative(const expr_type& expr) {\
				/*予めタプルの要素に対して結合法則を適用*/\
				auto temp = Associative_tuple1<NAME##_tag, Op, tuple<First, Second, Types...>>::associative(expr.terms_m);\
				using temp_tuple_type = typename decay<decltype(temp.terms_m)>::type;\
				return Associative_tuple2<NAME##_tag, temp_tuple_type>::associative(temp.terms_m);\
			}\
		};\
		template <class Op, class First, class Second, class... Types>\
		struct Associative<Op, expr_wrapper<NAME##_tag, type_tuple<First, Second, Types...>>, false> {\
			using expr_type = expr_wrapper<NAME##_tag, type_tuple<First, Second, Types...>>;\
			static constexpr auto associative(const expr_type& expr) {\
				return Associative_tuple1<NAME##_tag, Op, tuple<First, Second, Types...>>::associative(expr.terms_m);\
			}\
		};
		ASSOCIATIVE_BINARY_OPERATION(add);
		ASSOCIATIVE_BINARY_OPERATION(sub);
		ASSOCIATIVE_BINARY_OPERATION(mul);
		ASSOCIATIVE_BINARY_OPERATION(div);
#undef ASSOCIATIVE_BINARY_OPERATION

		template <class Op, class Expr>
		constexpr auto associative(const Expr& expr) { return Associative<Op, Expr>::associative(expr); }
```

やはり走査をするためのコードが長い。ただ、変数に対する代数的構造が異なる場合では同じ演算子でも作用する演算は異なるため、あまり「いい操作」であるとはいえない。その辺りは後述する。

###交換法則の適用について
さて、交換法則を実装する方法であるが、これはどのようなルールに則って数式に交換法則を適用させるかというのが難しい問題である。、当たり前ではあるがそもそもどの項を対象にして交換法則を適用するかといった操作は困難である。
そのため、現状では交換法則について実装しない。

### 考察1
現在、式の整理を厳密に行うにはあまりにも型情報が不足している。例えば、ベクトルや実数であれば和について交換法則や結合法則を自由に適用することができるが、四元数や行列は積について交換法則を自由に適用することはできない(もちろん単位元を含む交換法則が適用可能な部分群は存在する)。

さて、このような問題はC++のテンプレートがあまりにも自由すぎることから起きる問題であり、C++プログラマはテンプレートを用いる際にはそのテンプレートの機能を制限して制御するために、メタ関数等を用いて特定の特性を持った型のみをテンプレートで扱うといったことで日夜格闘している(と勝手ながら思っている)。
そこで、現在のExpression templateを振り返ると、変数にはどんな型でも代入することができ、あまりにも自由すぎるといったことからいくつかの機能は実装することができないのである(できなくはないがあまりにも自由すぎて実装するべきではない)。つまり、Expression templateの変数型を制限する構文を定義すればいいということがわかる。別に、変数全てが実数であるとすればこのような必要はないのだが、そのような仕様ではあまりにも不便であろう。少なくともベクトルや行列は日常的に用いるし、突如有限体上のベクトルとか出されたらいろいろとやってられない。

最も典型的なテンプレートを制限するための構文は

```C++
//複素数型に制限
template <class T>
auto hoge(complex<T>) {}
//ポインタ型に制限
template <class T>
auto huga(T*) {}
```

といったものであろう。これらをLambdaの引数で再現するというのはできない訳ではないが、今回は数式を対象としているため数式の変数そのものに対して型を推論させる仕組みが必要であるが、あらゆる場合を考えることは不可能である。
そこで、考えやすくするために数式に含まれる変数について仮定を列挙する。

 - 全ての数式に含まれる変数(`x[5]`等は添え字アクセス後の型)は代数的構造である
 - 変数は高々有限個である
 - 式は計算可能である

これらは当たり前であるが、今後の一般化および拡張を考える上では重要である。
また、減算と除算はそれぞれ加算と乗算の逆演算であり、減算もしくは除算が定義されるならば加法逆元もしくは乗法逆元が存在するべきである。これより、以下の仮定を与えてもいいだろう。

- 逆演算が定義されるならば元の演算で逆元が存在する

この仮定の真価は
$$
-(-a)\to a
$$
とすることの許可や
$$
a/b\to a\times \frac{1}{b},\ \ \ a-b\to a+(-b)
$$
といった式変形を許容するといったところにある。

### 考察2
考察1と分けたのはそもそもの話題が異なるからである。
さて、ここで一旦Expression templateの型の制限云々を忘れることとする。そもそも今回のような用途におけるExpression templateを使用するのはライブラリ設計者側ではなくユーザ側であり、ライブラリ設計者側はこれらの機能を提供するだけで実際に用いることはない。
そのため、変数の型は既知のはずである。C++のLambdaのような柔軟さが足りないと思われるかもしれないが、そもそも目的が数式処理であることから必要以上の柔軟さはあるべきではない。むしろ中途半端に

```C++
//N次元ベクトル型を示すタグのつもり
template <size_t N>
struct vector_tag {};
//他の等価な候補(アーベル群かつ加群であることを示す)
template <size_t N>
using vector_tag = type_tuple<type_tuple<group_tag, abel_tag>, module_tag, dimension_tag<N>>;
```

といったものを採用してしまうと色々と冗長すぎて面倒である。後者はいい案なのかもしれないが、次元情報が型情報に対して非標準的すぎて却下。かといって次元情報いらないのではと思うと外積代数で詰んでしまう。
というわけで、現状はいい案がないということと、そもそもある程度厳密な型情報がないと法則が適用可能であるか判定できないため直接想定される型情報を与えるべきだろう。例を与えておくと

```
vector<vector<double, M>, N>
```

のようにベクトルの定義に反するものでも自然にスカラー乗法を定義したりすることができるため、このようなものでもプログラム上では柔軟に記述することができるべきである。実際、このようなベクトル上のベクトルは剛体力学における変位と回転角を示すベクトル(および擬ベクトル)を1つの拡張されたベクトルとして扱うといった場合で見ることができる(実際には2つの独立した3次元ベクトルの直和から生成されるベクトルと考えればベクトルの定義に反しないがプログラム上ではそうもいかない)。

そのため、あらゆる代数的構造で結合法則や交換法則が適用可能か等のパラメータを予め定義していて分配法則等の適用時にコンパイルエラーを吐くようにすればいい。プログラム上で予め分配法則が適用可能か定義するのではなく、演算子のオーバーロードによる定義からそれを判定するようなメタ関数を作成可能かといった命題もあるが、それは事実上偽である。
これについては態々説明する必要はないと思うが、排中律に関する事柄が解になると思われる。実数の公理を仮定して背理等で非構成的な証明は机上では可能であるが、関数のように入力と出力しかわからない場合では必然的に構成的な証明をせざる得なくなり無事に詰む。

## 数値リテラル型の定義
分配法則では単項演算について
$$
-(a+b\times c)\to -1\times (a+b\times c)\to -1\times a+-1\times(b\times c)
$$
と処理されるが、どうにかして
$$
-1\times a+-1\times(b\times c)\to -a+-(b\times c)
$$
としたいものである。欲を言えば円周率やネイピア数といった数学定数も厳密に扱うことができるべきである。
数式微分では予め`zero_tag`や`one_tag`を定義することで数式の最適化を施していたが、これを一般化することにより数値リテラル型を定義する。

## 加法と乗法のみで式を結合する
考察1で述べたことであるが、仮定より逆演算は元の演算の逆元で表現することができる。すなわち、全ての式は加算と乗算で表現することができる。というわけでこれを実装する。

## 簡約法則の適用
本当は式の整理に含めるべきなのだが、まぁ仕方ない。これに関しては他の式の整理に対して例外的な処理をするからである。主に暗黙的な交換法則の適用。

# 終わりに
大分ソースコードが綺麗にまとまってきたように思える。そろそろ応用例にも取り組んでみたいとは思っているが、それをするにはまだいろいろと足りない。といっても今回の内容も面倒なだけでそこまで難しくないから誰かがもう少し頑張って色々やってくれる気もする。
ただ、誰得記事なので記事を書くモチベーションが上がらない。というか時代に乗ってない内容なのだから仕方のないことではあるが、それはそれとして記事を書くモチベーションが上がらない。まぁ、金になるような内容ならそもそも記事を書かないが。といっても私のやってることは金になるものなんてまずないのだが。
