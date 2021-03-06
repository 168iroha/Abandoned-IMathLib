 - [Expression templateと数式処理](https://qiita.com/168irohairoha/items/beecd2e12a56dd0eda3c)

の続き物。結局続いた。数式処理要素は非常に薄い。
やるのは数式微分の拡張だけ。添え物程度。

# 動機
多変数関数化する方法とか色々思いついた。あとやっぱり`shared_ptr`を使って変数のリソースの保持するのは許されないと思った(小並)。
アイディアは`std::bind`とか実際に演算グラフを書いたりして得ている。

# 準備
そのままいきなりすべて作るには前提知識（？）なるものが大分不足するかと思われる。

## 理想的な設計
というわけで、まずはどんな感じに使いたいか、あくまでも理想のコード。

```C++
int main() {
	//変数のプレースホルダ（引数は文字列出力のための変数名）
	auto x = iml::expr_variable<0>('x');
	auto y = iml::expr_variable<1>('y');
	auto f = x * x * y + y * y;

	//あたかも変数x,yがテンプレートのように扱える（複素数もOK）
	//そして，evalの代わりに関数オブジェクト
	std::cout << f(10, 0.8) << std::endl;
	std::cout << f(10, complex<double>(1, 5)) << std::endl;
	return 0;
}
```

現実はこんなにうまくはいかない、多分。
C++のラムダ式は意地でも使わない。だって、数式処理ができない。まぁ、できる言語というのがそもそもおかしい。
また、原理上はどうあがいてもExpression templateで関数を扱うというのは不可能である。そこで、数学関数を全て自作することにより

```C++
	auto f = iml::sin(x * x * y) + y * y;
```

みたいな感じなことができるようにする。なお、ここでは関数と数学関数を差別的に扱っている。なぜならば、数学関数は一貫して共通の設計をすることができるからである。それは後でやる。

## bindの仕組み
さて、理想的なコードでプレースホルダと関数の引数の関係云々を扱ったが、それを実現するには`bind`の仕組みが大きな助けになるだろう、多分。
というわけで、`bind`の仕組みを考えつつ関数の`bind`を実装する。クラスのメソッド等については面倒なためやらない。
とりあえず、関数オブジェクトを返すということで、直ちに以下のように実装するということがわかる。

```C++
	//変数のためのプレースホルダ
	template <size_t N>
	struct ph {};

	template <class F, class... Types>
	class binder_wrapper {
		typename decay<F>::type f_m;
		tuple<typename decay<Types>::type...> args_m;
	public:
		explicit constexpr binder_wrapper(F&& f, Types&&... args)
			: f_m(forward<F>(f)), args_m(forward<Types>(args)...) {}

		template <class... UTypes>
		constexpr auto operator()(UTypes&&... args) {
			//なんとかして呼び出す
		}
	};

	template <class F, class... Types>
	constexpr binder_wrapper<F, Types...> bind(F&& f, Types&&... args) {
		return binder_wrapper<F, Types...>(forward<F>(f), forward<Types>(args)...);
	}
```

何となく`binder`と`wrapper`で意味が被っている気がするのは無視。
このとき標準ライブラリのように`bind`を実装するには、関数呼び出し時にタプル`args_m`の各要素を調べ、その要素がプレースホルダのとき、そのプレースホルダのインデックスに対応する関数オブジェクトに与えられた引数を取り出して、それを`invoke`すればいい。
多分、文で書くよりコード見た方が理解が早い。以下、全コード。

```C++
	//変数のためのプレースホルダ
	template <size_t N>
	struct ph {};

	
	//プレースホルダを置き換えるためのやつ
	template <class T>
	struct Binder_fix {
		template <class Tuple>
		static constexpr auto binder_fix(T& arg, Tuple&&) { return arg; }
	};
	template <size_t N>
	struct Binder_fix<ph<N>> {
		template <class Tuple>
		static constexpr auto binder_fix(ph<N>&, Tuple&& arg) { return arg.get<N>(); }
	};
	template <class F, class Tuple, class UTuple, size_t... Indices>
	constexpr auto binder_invoke(F& f, Tuple& bt, UTuple&& ut, index_tuple<size_t, Indices...>&&) {
		return invoke(f, Binder_fix<typename Tuple::template at_type<Indices>::type>::binder_fix(bt.get<Indices>(), forward<UTuple>(ut))...);
	}

	template <class F, class... Types>
	class binder_wrapper {
		typename decay<F>::type f_m;
		tuple<typename decay<Types>::type...> args_m;
		//引数のシーケンス
		using sequence = typename index_range<size_t, 0, sizeof...(Types)>::type;
	public:
		explicit constexpr binder_wrapper(F&& f, Types&&... args)
			: f_m(forward<F>(f)), args_m(forward<Types>(args)...) {}

		template <class... UTypes>
		constexpr auto operator()(UTypes&&... args) {
			return binder_invoke(f_m, args_m, forward_as_tuple(forward<UTypes>(args)...), sequence());
		}
	};

	template <class F, class... Types>
	constexpr binder_wrapper<F, Types...> bind(F&& f, Types&&... args) {
		return binder_wrapper<F, Types...>(forward<F>(f), forward<Types>(args)...);
	}
```

関数オブジェクトの引数について`forward_as_tuple`をしているのがミソである。ちなみに、`tuple::at_type`というのは`tuple`の任意のインデックスの型を得るものである。
実際の`bind`の実装とは違うかもしれないが、自分はこうして実装している。また、`tuple`のメンバで`get<N>()`をもっているのは`tuple`が自作であるためである。
この手法はExpression templateを関数オブジェクトとして利用するのに応用できる。といっても、`bind`ほど複雑にはならないが。

## 関数の遅延評価について
前述でExpression templateで関数を扱うことは不可能であると述べたが、それは少し語弊がある。自明な扱うことができない関数としては、引数に`expre_wrapper`をもつことができない関数であるが、これは本質的ではない。
そして、本質的に不可能であるという解は**停止性問題**にある。まぁ、プログラムをやるエンジニアなら絶対知ってることだし、調べればすぐ出てくるからここでわざわざ述べる必要はない。調べてすぐ解決できるようなものはわざわざ書くなんて烏滸がましい。

というわけで、数式として関数に引数を渡すことは不可能であると決定したが、あくまでも値として渡せば計算可能である。つまり、数式と数値を橋渡しするものを作れば解決である。それは、`bind`の実装方法が大いに役立つだろう。
しかし、数式処理というものをしたいのが目的（？）であるため、このような解決法では数式処理ではなく数値解析となってしまう。そのため、数学関数に関してはやや特殊な実装となるが、数学関数の特性と非常に親和性がある。それについては後述する。
それ以外の関数については面倒というか、そこまでは求めていないため実装しない。


# Expression templateの改良
## 変数型の改良
変数型といえば`expr_wrapper<none_tag, type_tuple<T>>`のことであるが、これは内部で`shared_ptr`な変数を保持している。このような場面で動的にメモリを確保するのはあまりよろしくないことであるため、プレースホルダに置き換える。
という感じにプレースホルダに書き換える大義名分を得る。

このようにすることで、実際に遅延評価をするまでは`expr_wrapper`が計算可能であるかはわからない。すなわち実際に計算可能であるかを判定する必要がない。つまり、前回の「計算可能な構文木」というのはなくす方向で行けるだろう。
というわけで、以下のようなコードにする。

```C++
	//変数
	template <size_t N>
	struct expr_wrapper<none_tag, type_tuple<index_tuple<size_t, N>>> {
		char name_m;

		constexpr expr_wrapper(char name) : name_m(name) {}

		constexpr expr_wrapper<sub_tag, type_tuple<expr_wrapper>> operator-() const {
			return expr_wrapper<sub_tag, type_tuple<expr_wrapper>>(*this);
		}
		constexpr expr_wrapper<add_tag, type_tuple<expr_wrapper>> operator+() const {
			return expr_wrapper<add_tag, type_tuple<expr_wrapper>>(*this);
		}

		template <class U>
		constexpr expr_wrapper<subscript_tag, type_tuple<expr_wrapper, U>> operator[](const U& index) const {
			return expr_wrapper<subscript_tag, type_tuple<expr_wrapper, U>>(*this, index);
		}
	};
```

他の部分特殊化でも同様。`name_m`をどうにかしたいけど数式の出力でいろいろ困る。これを改善するには出力インターフェースレベルでの改善を要するためやらない。

## 複数の処理の記述
まずは、`expr_wrapper`が複数の処理を記述することができるようにする。これは、コンマ演算子のオーバーロードによって実現することができる。
以下は変数の場合の例と`comma_tag`に対する部分特殊化である。

```C++
	//コンマ(,)
	using comma_tag = operation_tag<5>;


	//変数
	template <size_t N>
	struct expr_wrapper<none_tag, type_tuple<index_tuple<size_t, N>>> {
		//略

		template <class U>
		constexpr expr_wrapper<comma_tag, type_tuple<expr_wrapper, U>> operator,(const U& expr) const {
			return expr_wrapper<comma_tag, type_tuple<expr_wrapper, U>>(*this, expr);
		}

		//略
	};
```

後はこれをうまく対応させるような`Eval`を定義するだけである。

```C++
	template <class Expr1, class Expr2>
	struct Eval<expr_wrapper<comma_tag, type_tuple<Expr1, Expr2>>> {
		template <class Tuple>
		static constexpr return eval(const expr_wrapper<comma_tag, type_tuple<Expr1, Expr2>>& expr) {
			Eval<Expr1>::eval(expr.lhs_m);
			return Eval<Expr2>::eval(expr.rhs_m);
		}
	};
```

これで複数の処理をコンマにより複数の処理を記述することができる。


## 関数オブジェクトの定義
これは非常に簡単。シーケンスを必要としない`bind`の仕組みそのままである。前述で`eval`なくすとかあったが、`eval`の機能自体はそのまま用いて実装する。
しかし、`expr_wrapper`の全てに対して関数オブジェクトを定義すると、

```C++
	auto x = iml::expr_variable<0>('x');
	auto y = iml::expr_variable<1>('y');
	//xの関数オブジェクト呼び出しをする式
	auto f = x(y, 10);
```

といった操作ができなくなる。
というわけで、`expr_wrapper`の別の部分特殊化によって式の呼び出しができるようにする。
また、記号の濫用を防ぐために、Expression templateに関する全てを新しい名前空間`op`に対して定義をした。

```C++
		//expr_wrapperの実行
		template <class T>
		struct Eval {
			template <class Tuple>
			static constexpr auto eval(const T& expr, Tuple&&) {
				return expr;
			}
		};
		template <size_t N>
		struct Eval<expr_wrapper<none_tag, type_tuple<index_tuple<size_t, N>>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<none_tag, type_tuple<index_tuple<size_t, N>>>&, Tuple&& t) {
				return t.get<N>();
			}
		};
		template <class Expr>
		struct Eval<expr_wrapper<add_tag, type_tuple<Expr>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<add_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				return Eval<Expr>::eval(expr.x_m);
			}
		};
		template <class Expr>
		struct Eval<expr_wrapper<sub_tag, type_tuple<Expr>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<sub_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				return -Eval<Expr>::eval(expr.x_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) + Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) - Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) * Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) / Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t))[Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t))];
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<comma_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<comma_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t));
				return Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};


		//ラムダ式
		template <class Expr>
		struct lambda_functor {
			Expr x_m;

			constexpr lambda_functor(const Expr& expr) : x_m(expr) {}

			//オブジェクトの呼び出し
			template <class... Types>
			constexpr auto operator()(Types&&... args) { return Eval<Expr>::eval(x_m, forward_as_tuple(forward<Types>(args)...)); }
			template <class... Types>
			constexpr auto operator()(Types&&... args) const { return Eval<Expr>::eval(x_m, forward_as_tuple(forward<Types>(args)...)); }
		};
```

しかし、

```C++
		template <class Expr>
		constexpr auto lambda(const Expr& expr) {
			return expr_wrapper<lambda_tag, type_tuple<Expr>>(expr);
		}
```

のようなものを作成すると、複数の処理を`()`内で記述することにより、コンマ演算子は引数リストとして評価されてしまう。そのため、`[]`内に記述するべきである。
というわけでそれを再現するための以下コード。

```C++
		//lambda_functorを生成するためのメソッド
		struct Lambda {
			template <class Expr>
			constexpr lambda_functor<Expr> operator[](const Expr& expr) const {
				return lambda_functor<Expr>(expr);
			}
		};
		static constexpr Lambda lambda;


		//ラムダ式の評価
		template <class Expr>
		struct Eval<lambda_functor<Expr>> {
			template <class Tuple>
			static constexpr auto eval(const lambda_functor<Expr>& expr, Tuple&& t) {
				return Eval<Expr>::eval(expr.x_m, forward<Tuple>(t));
			}
		};
```

これにより、

```C++
int main() {

	auto x = iml::op::expr_variable<0>('x');
	auto y = iml::op::expr_variable<1>('y');
	//暗黙的にx+yが戻り値
	auto f = iml::op::lambda[
		x * x * y + y * y, x + y
	];

	std::cout << f(10, 1) << std::endl;

	return 0;
}
```

というように扱うことができる。



## operation_tag設計の見直し
現状の`operation_tag`の実装では`operation_tag`の定義の追加に合わせて同じ内容の`expr_wrapper`を書くのは非生産的である。`Eval`については処理が全て違うため問題ないが、`expre_wrapper`は冗長すぎる。
というわけで、単項演算との推論の弊害となっている`none_tag`を`operation_tag`を用いないことで解決する。このとき、`expr_variable`関数を廃止してその同名で変数用のエイリアスを与える。

```C++
	//無し(変数)
	struct none_tag {};


	//変数
	template <size_t N>
	struct expr_wrapper<none_tag, type_tuple<index_tuple<size_t, N>>>{
		//略
	};
	//変数宣言のためのエイリアス
	template <size_t N>
	using expr_variable = expr_wrapper<none_tag, type_tuple<index_tuple<size_t, N>>>;
```

`none_tag`を差別化したことにより、単項演算と2項演算についての`expr_wrapper`が

```C++
		//単項演算
		template <size_t N, class T>
		struct expr_wrapper<operation_tag<N>, type_tuple<T>> {
			//略
		};
		//2項演算
		template <size_t N, class L, class R>
		struct expr_wrapper<operation_tag<N>, type_tuple<L, R>> {
			//略
		};
```

のように簡単に記述することができるようになり、他の演算子を定義したときも単項演算もしくは2項演算と扱うことができるならば、`epxr_wrapper`はそのまま用いることができる。
このとき、`expr_traits`は既に意味を成さないため、演算のタグを取得するためのメソッドに書き換える。

```C++
		//式のトップレベルのタグ
		template <class>
		struct expr_tag {
			using type = none_tag;
		};
		template <class Tag, class Expr>
		struct expr_tag<expr_wrapper<Tag, Expr>> {
			using type = Tag;
		};


		//expr_wrapperにおいて変数であることの判定
		template <class T>
		struct is_expr_variable : cat_bool<is_same<typename expr_tag<T>::type, none_tag>::value && is_expr_wrapper<T>::value> {};
```

このとき、`is_expr_variable`をこんなに面倒に作ってるかというと、今後の仕様変更に対しても不変な要素で作成するコンセプトの元である。

また、全てのオーバーロード可能な演算子のタグを定義する。
<details><summary>演算子のタグの定義一覧</summary><div>

```C++
		//2項演算および単項演算とのみみなすことが可能な共用のタグ
		template <size_t>
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
		using less_than_equal_tag = operation_tag<15>;
		//大なり(>)
		using greater_than_tag = operation_tag<16>;
		//大なりイコール(>=)
		using greater_than_equal_tag = operation_tag<17>;
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
		//加算代入(+=)
		using add_assign_tag = operation_tag<26>;
		//減算代入(-=)
		using sub_assign_tag = operation_tag<27>;
		//乗算代入(*=)
		using mul_assign_tag = operation_tag<28>;
		//除算代入(/=)
		using div_assign_tag = operation_tag<29>;
		//剰余代入(%=)
		using surplus_assign_tag = operation_tag<30>;
		//左シフト代入(<<=)
		using lshift_assign_tag = operation_tag<31>;
		//右シフト代入(>>=)
		using rshift_assign_tag = operation_tag<32>;
		//ビット積代入(&=)
		using bit_and_assign_tag = operation_tag<33>;
		//ビット和代入(|=)
		using bit_or_assign_tag = operation_tag<34>;
		//排他的ビット和代入(^=)
		using bit_xor_assign_tag = operation_tag<35>;
		//コンマ(,)
		using comma_tag = operation_tag<36>;
```
</div></details>

関数オブジェクトについては可変個数の演算と考えられ、単項演算もしくは2項演算のどちらにも該当しないため、`operation_tag`とは別の定義を与えている。
あくまでもこれらは演算の定義であるため、制御構文等では別途定義する。
同様にして演算のタグに対応する`expr_wrapper`と2項演算、`Eval`の定義をも示す。といってもほとんど同じものだが、ほぼ全部コピペのためとても長い。
`->`について定義しない理由は後述する。

<details><summary>`expr_wrapper`の定義一覧</summary><div>

```C++
		//変数
		template <size_t N>
		struct expr_wrapper<none_tag, type_tuple<index_tuple<size_t, N>>> {
			char name_m;

			constexpr expr_wrapper(char name) : name_m(name) {}

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

		//単項演算
		template <size_t N, class T>
		struct expr_wrapper<operation_tag<N>, type_tuple<T>> {
			T x_m;

			constexpr expr_wrapper(const T& expr) : x_m(expr) {}

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

			constexpr expr_wrapper(const L& lhs, const R& rhs) : lhs_m(lhs), rhs_m(rhs) {}

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
```
</div></details>

<details><summary>2項演算の定義一覧</summary><div>

```C++
		//2項演算の定義(lhsとrhsの型が等しい場合は多重定義のエラーとなるためSFINAEで除外)
		template <class LOp, class LExpr, class R>
		constexpr auto operator+(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<add_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<add_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value>::type>
		constexpr auto operator+(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<add_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<add_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R>
		constexpr auto operator-(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<sub_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<sub_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value>::type>
		constexpr auto operator-(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<sub_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<sub_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R>
		constexpr auto operator*(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<mul_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<mul_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value>::type>
		constexpr auto operator*(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<mul_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<mul_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R>
		constexpr auto operator/(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<div_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<div_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value>::type>
		constexpr auto operator/(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<div_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<div_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R>
		constexpr auto operator%(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<surplus_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<surplus_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value>::type>
		constexpr auto operator%(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<surplus_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<surplus_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R>
		constexpr auto operator<<(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<lshift_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<lshift_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value>::type>
		constexpr auto operator<<(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<lshift_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<lshift_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R>
		constexpr auto operator>>(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<rshift_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<rshift_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value>::type>
		constexpr auto operator>>(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<rshift_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<rshift_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R>
		constexpr auto operator<(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<less_than_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<less_than_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value>::type>
		constexpr auto operator<(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<less_than_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<less_than_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R>
		constexpr auto operator<=(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<less_than_equal_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<less_than_equal_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value>::type>
		constexpr auto operator<=(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<less_than_equal_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<less_than_equal_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R>
		constexpr auto operator>(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<greater_than_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<greater_than_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value>::type>
		constexpr auto operator>(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<greater_than_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<greater_than_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R>
		constexpr auto operator>=(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<greater_than_equal_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<greater_than_equal_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value>::type>
		constexpr auto operator>=(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<greater_than_equal_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<greater_than_equal_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R>
		constexpr auto operator==(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<equal_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<equal_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value>::type>
		constexpr auto operator==(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<equal_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<equal_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R>
		constexpr auto operator!=(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<not_equal_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<not_equal_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value>::type>
		constexpr auto operator!=(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<not_equal_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<not_equal_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R>
		constexpr auto operator&(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<bit_and_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<bit_and_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value>::type>
		constexpr auto operator&(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<bit_and_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<bit_and_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R>
		constexpr auto operator|(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<bit_or_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<bit_or_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value>::type>
		constexpr auto operator|(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<bit_or_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<bit_or_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R>
		constexpr auto operator^(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<bit_xor_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<bit_xor_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value>::type>
		constexpr auto operator^(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<bit_xor_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<bit_xor_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R>
		constexpr auto operator&&(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<and_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<and_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value>::type>
		constexpr auto operator&&(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<and_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<and_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
		template <class LOp, class LExpr, class R>
		constexpr auto operator||(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<or_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
			return expr_wrapper<or_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
		}
		template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value>::type>
		constexpr auto operator||(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<or_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
			return expr_wrapper<or_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
		}
```
</div></details>

<details><summary>`Eval`の定義一覧</summary><div>

```C++
		//expr_wrapperの実行
		template <class T>
		struct Eval {
			template <class Tuple>
			static constexpr auto eval(const T& expr, Tuple&&) {
				return expr;
			}
		};
		template <size_t N>
		struct Eval<expr_variable<N>> {
			template <class Tuple>
			static constexpr auto& eval(const expr_variable<N>&, Tuple&& t) {
				return t.get<N>();
			}
		};
		//単項演算
		template <class Expr>
		struct Eval<expr_wrapper<add_tag, type_tuple<Expr>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<add_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				return Eval<Expr>::eval(expr.x_m, forward<Tuple>(t));
			}
		};
		template <class Expr>
		struct Eval<expr_wrapper<sub_tag, type_tuple<Expr>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<sub_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				return -Eval<Expr>::eval(expr.x_m, forward<Tuple>(t));
			}
		};
		template <class Expr>
		struct Eval<expr_wrapper<mul_tag, type_tuple<Expr>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<mul_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				return *Eval<Expr>::eval(expr.x_m, forward<Tuple>(t));
			}
		};
		template <class Expr>
		struct Eval<expr_wrapper<flip_tag, type_tuple<Expr>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<flip_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				return !Eval<Expr>::eval(expr.x_m, forward<Tuple>(t));
			}
		};
		template <class Expr>
		struct Eval<expr_wrapper<bit_flip_tag, type_tuple<Expr>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<bit_flip_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				return ~Eval<Expr>::eval(expr.x_m, forward<Tuple>(t));
			}
		};
		//インクリメント・デクリメント
		template <class Expr>
		struct Eval<expr_wrapper<increment_tag, type_tuple<Expr>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<increment_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				return ++Eval<Expr>::eval(expr.x_m, forward<Tuple>(t));
			}
		};
		template <class Expr>
		struct Eval<expr_wrapper<increment_tag, type_tuple<Expr, int>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<increment_tag, type_tuple<Expr, int>>& expr, Tuple&& t) {
				return Eval<Expr>::eval(expr.lhs_m, forward<Tuple>(t))++;
			}
		};
		template <class Expr>
		struct Eval<expr_wrapper<decrement_tag, type_tuple<Expr>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<decrement_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				return --Eval<Expr>::eval(expr.x_m, forward<Tuple>(t));
			}
		};
		template <class Expr>
		struct Eval<expr_wrapper<decrement_tag, type_tuple<Expr, int>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<decrement_tag, type_tuple<Expr, int>>& expr, Tuple&& t) {
				return Eval<Expr>::eval(expr.lhs_m, forward<Tuple>(t))--;
			}
		};
		//メンバポインタアクセス
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<arrow_ast_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<arrow_ast_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t))->*Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		//添え字アクセス
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t))[Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t))];
			}
		};
		//代入演算
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<assign_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) = Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<add_assign_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<add_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) += Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<sub_assign_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<sub_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) -= Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<mul_assign_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<mul_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) *= Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<div_assign_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<div_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) /= Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<surplus_assign_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<surplus_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) %= Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<lshift_assign_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<lshift_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) <<= Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<rshift_assign_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<rshift_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) >>= Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<bit_and_assign_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<bit_and_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) &= Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<bit_or_assign_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<bit_or_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) |= Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<bit_xor_assign_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<bit_xor_assign_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) ^= Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		//2項演算
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) + Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) - Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) * Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) / Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<surplus_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<surplus_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) % Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<lshift_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<lshift_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) << Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<rshift_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<rshift_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) >> Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<less_than_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<less_than_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) < Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<less_than_equal_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<less_than_equal_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) <= Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<greater_than_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<greater_than_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) > Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<greater_than_equal_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<greater_than_equal_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) >= Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<equal_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<equal_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) == Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<not_equal_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<not_equal_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) != Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<bit_and_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<bit_and_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) & Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<bit_or_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<bit_or_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) | Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<bit_xor_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<bit_xor_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) ^ Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<and_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<and_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) && Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<or_tag, type_tuple<Expr1, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<or_tag, type_tuple<Expr1, Expr2>>& expr, Tuple&& t) {
				return Eval<Expr1>::eval(expr.lhs_m, forward<Tuple>(t)) || Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
		//コンマ演算子
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<comma_tag, type_tuple<Expr1, Expr2>>> {
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
```
</div></details>

ただただ全ての演算について記述しただけだから長い。
現状は明らかに代入演算等が`constexpr`等が要因で意味を成していないが、今は無視する。それを回避するためにはExpression template Lambda内で変数宣言するといったものを定式化する必要が生じたりするためである。
実際は変数の機能がなくてもそれを再現することができる。以下のコードはそのイメージ。

```C++
int main() {

	auto x = iml::op::expr_variable<0>('x');
	//あたかもLambda内で使うauto型の変数yの宣言
	auto y = x * x + 1;
	//xを引数に持つLambda式f
	auto f = iml::op::lambda[
		(y + x) * x
	];
	//1110
	std::cout << f(10) << std::endl;

	return 0;
}
```

なぜ、再現できるものを作るかっていうと、スコープの問題の解決をするためである。

また、このコードではコピー不可のオブジェクトに対してLambdaを構築することができない。それを解決するには2項演算等を右辺値参照と左辺値参照とで処理を分け、`expr_wrapper`で保持する変数が参照型か否かを厳密に分類する必要がある。
この辺りは変数のキャプチャのことをも交えて実装していきたいため次回以降で。
また、型推論を厳密に行うために全ての構造体およびクラスのコンストラクタについて`explicit`を付けなければならない。

## より使いやすくする
数式のみを扱うのであれば現状で充分である。しかし、ラムダ式のように自由に扱いたいならば現状ではいろいろと機能が足りない。そのため、それらを実装する。

### if文
`if`使えたらすごい便利、作ろう。
実際にスコープ`{}`は作ることはできないため、`[]`によって再現をする。このような設計の元、`Eval`を上手く部分特殊化させるために、`subscript_tag`とは別物のスコープ用のタグ`scope_tag`を定義することでこれを作成する。
このようなスコープの再現は`lambda`との統一性をも取れている。

```C++
		//識別子を示すタグ
		template <size_t N>
		struct ident_tag {};


		//if用のスコープ([])
		using if_scope_tag = ident_tag<0>;
		//if文(if)
		using if_tag = ident_tag<1>;


		//スコープのためのexpr_wrapper
		template <class L, class R>
		struct expr_wrapper<scope_tag, type_tuple<L, R>> {
			L lhs_m;
			R rhs_m;

			constexpr explicit expr_wrapper(const L& lhs, const R& rhs) : lhs_m(lhs), rhs_m(rhs) {}

			//コンマ演算子
			template <class Expr>
			constexpr expr_wrapper<comma_tag, type_tuple<expr_wrapper, Expr>> operator,(const Expr& expr) const {
				return expr_wrapper<comma_tag, type_tuple<expr_wrapper, Expr>>(*this, expr);
			}
		};
		//ifのためのexpr_wrapper
		template <class Expr>
		struct expr_wrapper<if_tag, type_tuple<Expr>> {
			Expr x_m;

			constexpr explicit expr_wrapper(const Expr& expr) : x_m(expr) {}

			template <class U>
			constexpr expr_wrapper<scope_tag, type_tuple<expr_wrapper, U>> operator[](const U& expr) const {
				return expr_wrapper<scope_tag, type_tuple<expr_wrapper, U>>(*this, expr);
			}
		};
		template <class Expr>
		constexpr auto _if_(const Expr& expr) { return expr_wrapper<if_tag, type_tuple<Expr>>(expr); }


		//ifのためのEval
		template <class Expr1, class Expr2>
		struct Eval<expr_wrapper<scope_tag, type_tuple<expr_wrapper<if_tag, type_tuple<Expr1>>, Expr2>>> {
			template <class Tuple>
			static constexpr auto eval(const expr_wrapper<scope_tag, type_tuple<expr_wrapper<if_tag, type_tuple<Expr1>>, Expr2>>& expr, Tuple&& t) {
				if (Eval<Expr1>::eval(expr.lhs_m.x_m, forward<Tuple>(t))) Eval<Expr2>::eval(expr.rhs_m, forward<Tuple>(t));
			}
		};
```

まぁ簡単。
大体こんな感じで使うことができる。

```C++
int main() {

	auto x = iml::op::expr_variable<0>('x');
	auto y = iml::op::expr_variable<1>('y');
	auto f = iml::op::lambda[
		iml::op::_if_(x + y)[
			//何か処理を書かないとエラー
		]
	];
```

しかし、生成される`expr_wrapper`について、この場合はどう考えてもスコープ情報はいらないし、`if`で大量の入れ子を作ってしまうのはあまりよろしくない。
というわけで、イメージとして

```text
expr_wrapper<if_tag, type_tuple<type_tuple<content_n>, type_tuple<terms_n-1, content_n-1>, ・・・type_tuple<terms_1, content_1>>>
```

のような条件とスコープ内の内容のペアの組として扱うように`if`を設計する。また、$n,n-1,\cdots, 1$のように逆順になっているのは、トップレベルの`type_tuple`がスタックとして扱っているためである。要は命令のスタック。
また、

```text
else = else if(1)
```

と等価とみなせることを用いて実装する。
まずは、`if`文の構築のためのコード。

```C++
		//識別子を示すタグ
		template <size_t>
		struct ident_tag {};


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
```

また、`Eval`だけでは`if`を評価することは不可能であるため、`Eval_if`を作成することによりこれを解決する。

```C++
		//ifのためのEval
		template <size_t N, class T, bool = (N + 1 < T::value)>
		struct Eval_if;
		template <size_t N, class... Orders>
		struct Eval_if<N, type_tuple<Orders...>, true> {
			//戻り値はelse ifおよびelseを実行させるか否か
			template <class Tuple>
			static constexpr bool eval_if(const tuple<Orders...>& expr, Tuple&& t) {
				bool temp = Eval_if<N + 1, type_tuple<Orders...>>::eval_if(expr, forward<Tuple>(t));
				if (Eval<typename decay<decltype(expr.get<N>().first)>::type>::eval(expr.get<N>().first, forward<Tuple>(t)) && temp) {
					Eval<typename decay<decltype(expr.get<N>().second)>::type>::eval(expr.get<N>().second, forward<Tuple>(t));
					return false;
				}
				return temp;
			}
		};
		template <size_t N, class... Orders>
		struct Eval_if<N, type_tuple<Orders...>, false> {
			//戻り値はelse ifおよびelseを実行させるか否か
			template <class Tuple>
			static constexpr bool eval_if(const tuple<Orders...>& expr, Tuple&& t) {
				if (Eval<typename decay<decltype(expr.get<N>().first)>::type>::eval(expr.get<N>().first, forward<Tuple>(t))) {
					Eval<typename decay<decltype(expr.get<N>().second)>::type>::eval(expr.get<N>().second, forward<Tuple>(t));
					return false;
				}
				return true;
			}
		};
		//if_tagの場合のEval
		template <class... Orders>
		struct Eval<expr_wrapper<if_tag, type_tuple<Orders...>>> {
			template <class Tuple>
			static constexpr void eval(const expr_wrapper<if_tag, type_tuple<Orders...>>& expr, Tuple&& t) {
				Eval_if<0, type_tuple<Orders...>>::eval_if(expr.x_m, forward<Tuple>(t));
			}
		};
		//if_scope_tagの場合のEval
		template <class... Orders>
		struct Eval<expr_wrapper<if_scope_tag, type_tuple<Orders...>>> {
			template <class Tuple>
			static constexpr void eval(const expr_wrapper<if_scope_tag, type_tuple<Orders...>>& expr, Tuple&& t) {
				Eval_if<0, type_tuple<Orders...>>::eval_if(expr.x_m, forward<Tuple>(t));
			}
		};
```

これらの出力テストをする場合、`cout`はコピー不可オブジェクトのため、

```C++
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
			static constexpr void eval(Result& result ,const expr_wrapper<cout_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				std::cout << Eval<Expr>::eval(result, expr.x_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr void eval(const expr_wrapper<cout_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				std::cout << Eval<Expr>::eval(expr.x_m, forward<Tuple>(t));
			}
		};
```

のようにホルダのような振る舞いをするものを作成することで

```C++
int main() {

	auto x = iml::op::expr_variable<0>('x');
	auto y = iml::op::expr_variable<1>('y');
	auto f = iml::op::lambda[
		iml::op::_if_(x + y)[
			iml::op::ch << std::string("if check")
		]._else_if_(x - y)[
			iml::op::ch << std::string("else if check")
		]._else_[
			iml::op::ch << std::string("else check")
		]
	];

	//first time:else if check
	std::cout << "first time:"; f(-10, 10); std::cout << std::endl;
	//second time:if check
	std::cout << "second time:"; f(-1, 10); std::cout << std::endl;
	//third time:else check
	std::cout << "third time:"; f(0, 0); std::cout << std::endl;

	return 0;
}
```

のようにテストすることができる。
ちなみに、`if_tag`等を`ident_tag`の特殊な場合で定義しているのは、通常の`expr_wrapper`との2項演算を推論から除外するためである。そのため、`expr_wrapper`との2項演算に`enable_if`を用いて推論の候補から除外している。ただし、コンマ演算子は例外である。


### while文
`if`文が作れれば`while`文は非常に簡単。ただし、変数をまだ扱うことができないため実質意味ない。

```C++
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
			template <class Tuple>
			static constexpr void eval(const expr_wrapper<while_tag, type_tuple<Terms, Content>>& expr, Tuple&& t) {
				while (Eval<Terms>::eval(expr.terms_m, forward<Tuple>(t))) {
					Eval<Content>::eval(expr.content_m, forward<Tuple>(t));
				}
			}
		};
```

ついでに`do-while`文も。

```C++
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
			template <class Tuple>
			static constexpr void eval(const expr_wrapper<do_while_tag, type_tuple<Terms, Content>>& expr, Tuple&& t) {
				do {
					Eval<Content>::eval(expr.content_m, forward<Tuple>(t));
				} while (Eval<Terms>::eval(expr.terms_m, forward<Tuple>(t)));
			}
		};
```

変数およびキャプチャを実装していないため原理上テストはできないが、これで動くはず。


### return識別子
複数行の処理があるなら`return`が欲しくなる。
当然ながらLambda内の全ての`return`に対する型は全て同じにならなければならない。つまり、その戻り値型推論を行わなければならく面倒。なので今回はやらない。

まずは、`return`に戻り値を作用させるためのコードを示す。

```C++
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

```

これにより

```text
_return_ << (hoge + huga)
```

のような`return`に対して戻り値を入力をするといった構文が定義できる。このように演算子をいい意味で濫用することにより**識別子との間に独自の構文を定義することができる**。あくまでも今回は`return`のみではあるが。
今回は戻り値型推論をサボタージュするため、Lambdaの方を多少工夫する。具体的には後置戻り値型モドキを作る。その実装は以下のようになる。

```C++
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
				using result_type = typename decay<decltype(Eval<typename Result::expr_type>::eval(type_m.expr_m, forward_as_tuple(forward<Types>(args)...)))>::type;
				//boolはreturnフラグが立っていないか
				pair<result_type, bool> result(result_type(), true);
				Eval<Expr>::eval(result, x_m, forward_as_tuple(forward<Types>(args)...));
				return result.first;
			}
			template <class... Types>
			constexpr auto operator()(Types&&... args) const {
				//戻り値型の決定
				using result_type = typename decay<decltype(Eval<typename Result::expr_type>::eval(type_m.expr_m, forward_as_tuple(forward<Types>(args)...)))>::type;
				//boolはreturnフラグが立っていないか
				pair<result_type, bool> result(result_type(), true);
				Eval<Expr>::eval(result, x_m, forward_as_tuple(forward<Types>(args)...));
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
```

そして、`Eval`には戻り値情報`pair<result_type, bool>`を引数に含んだ場合のオーバーロード作成するだけである。実装方法としては戻り値情報のフラグが`false`のときは処理を実行しないということであるが、本質的に処理を分岐すればいいのはコンマ演算子についてと`if`や`while`といった制御構文についてのみである。
まずは、`return`に対する`Eval`のコードを示す。

```C++
		//returnに対するEval
		template <class Expr>
		struct Eval<expr_wrapper<return_tag, type_tuple<Expr>>> {
			template <class Result, class Tuple>
			static constexpr void eval(Result& result, const expr_wrapper<return_tag, type_tuple<Expr>>& expr, Tuple&& t) {
				result.first = Eval<Expr>::eval(result, expr.x_m, forward<Tuple>(t));
				result.second = false;
			}
			template <class Tuple>
			static constexpr void eval(const expr_wrapper<return_tag, type_tuple<Expr>>&, Tuple&&) {}
		};
```

次に、コンマ演算子の場合について。

```C++
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
```

あと、一応制御構文の`Eval`の一覧も。

<details><summary>制御構文の`Eval`の実装一覧</summary><div>

```C++
		//ifのためのEval
		template <size_t N, class T, bool = (N + 1 < T::value)>
		struct Eval_if;
		template <size_t N, class... Orders>
		struct Eval_if<N, type_tuple<Orders...>, true> {
			//戻り値はelse ifおよびelseを実行させるか否か
			template <class Result, class Tuple>
			static constexpr bool eval_if(Result& result, const tuple<Orders...>& expr, Tuple&& t) {
				bool temp = Eval_if<N + 1, type_tuple<Orders...>>::eval_if(result, expr, forward<Tuple>(t));
				if (result.second && Eval<typename decay<decltype(expr.get<N>().first)>::type>::eval(result, expr.get<N>().first, forward<Tuple>(t)) && temp) {
					Eval<typename decay<decltype(expr.get<N>().second)>::type>::eval(result, expr.get<N>().second, forward<Tuple>(t));
					return false;
				}
				return temp;
			}
			template <class Tuple>
			static constexpr bool eval_if(const tuple<Orders...>& expr, Tuple&& t) {
				bool temp = Eval_if<N + 1, type_tuple<Orders...>>::eval_if(expr, forward<Tuple>(t));
				if (Eval<typename decay<decltype(expr.get<N>().first)>::type>::eval(expr.get<N>().first, forward<Tuple>(t)) && temp) {
					Eval<typename decay<decltype(expr.get<N>().second)>::type>::eval(expr.get<N>().second, forward<Tuple>(t));
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
				if (result.second && Eval<typename decay<decltype(expr.get<N>().first)>::type>::eval(result, expr.get<N>().first, forward<Tuple>(t))) {
					Eval<typename decay<decltype(expr.get<N>().second)>::type>::eval(result, expr.get<N>().second, forward<Tuple>(t));
					return false;
				}
				return true;
			}
			template <class Tuple>
			static constexpr bool eval_if(const tuple<Orders...>& expr, Tuple&& t) {
				if (Eval<typename decay<decltype(expr.get<N>().first)>::type>::eval(expr.get<N>().first, forward<Tuple>(t))) {
					Eval<typename decay<decltype(expr.get<N>().second)>::type>::eval(expr.get<N>().second, forward<Tuple>(t));
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
				Eval_if<0, type_tuple<Orders...>>::eval_if(result, expr.x_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr void eval(const expr_wrapper<if_tag, type_tuple<Orders...>>& expr, Tuple&& t) {
				Eval_if<0, type_tuple<Orders...>>::eval_if(expr.x_m, forward<Tuple>(t));
			}
		};
		//if_scope_tagの場合のEval
		template <class... Orders>
		struct Eval<expr_wrapper<if_scope_tag, type_tuple<Orders...>>> {
			template <class Result, class Tuple>
			static constexpr void eval(Result& result, const expr_wrapper<if_scope_tag, type_tuple<Orders...>>& expr, Tuple&& t) {
				Eval_if<0, type_tuple<Orders...>>::eval_if(result, expr.x_m, forward<Tuple>(t));
			}
			template <class Tuple>
			static constexpr void eval(const expr_wrapper<if_scope_tag, type_tuple<Orders...>>& expr, Tuple&& t) {
				Eval_if<0, type_tuple<Orders...>>::eval_if(expr.x_m, forward<Tuple>(t));
			}
		};


		//whileに対するEval
		template <class Terms, class Content>
		struct Eval<expr_wrapper<while_tag, type_tuple<Terms, Content>>> {
			template <class Result, class Tuple>
			static constexpr void eval(Result& result, const expr_wrapper<while_tag, type_tuple<Terms, Content>>& expr, Tuple&& t) {
				while (result.second && Eval<Terms>::eval(result, expr.terms_m, forward<Tuple>(t))) {
					Eval<Content>::eval(result, expr.content_m, forward<Tuple>(t));
				}
			}
			template <class Tuple>
			static constexpr void eval(const expr_wrapper<while_tag, type_tuple<Terms, Content>>& expr, Tuple&& t) {
				while (Eval<Terms>::eval(expr.terms_m, forward<Tuple>(t))) {
					Eval<Content>::eval(expr.content_m, forward<Tuple>(t));
				}
			}
		};


		//do-whileに対するEval
		template <class Terms, class Content>
		struct Eval<expr_wrapper<do_while_tag, type_tuple<Terms, Content>>> {
			template <class Result, class Tuple>
			static constexpr void eval(Result& result, const expr_wrapper<do_while_tag, type_tuple<Terms, Content>>& expr, Tuple&& t) {
				do {
					Eval<Content>::eval(result, expr.content_m, forward<Tuple>(t));
				} while (result.second && Eval<Terms>::eval(result, expr.terms_m, forward<Tuple>(t)));
			}
			template <class Tuple>
			static constexpr void eval(const expr_wrapper<do_while_tag, type_tuple<Terms, Content>>& expr, Tuple&& t) {
				do {
					Eval<Content>::eval(expr.content_m, forward<Tuple>(t));
				} while (Eval<Terms>::eval(expr.terms_m, forward<Tuple>(t)));
			}
		};
```
</div></details>

これでLambdaの戻り値を定義できる。ちなみに`return`がないときは戻り値型に対するデフォルトコンストラクタが返る。
まぁ、エラーは吐いてくれるがとても解読はできなく、参考にできるのはどの行かだけである。こういう時に、`static_assert`を用いるべきであるが、今回は戻り値型推論をやっていないためそれはできない。やはり、戻り値型推論はするべきである。
ちなみに戻り値型推論は、構文解析と同様にして型推論はコンパイラがやってくれるため、全ての`return`に作用する戻り値の型をリスト化して、それらを比較することで実現できるが、まぁこれが面倒。

### 余談
現実的な問題として、`->*`演算子はクラスメンバへのアクセスに

```C++
struct hoge {
	int menber_data;

	int member_function(int);
};

x = iml::expr_variable<0>('x');
(&x->*&hoge::member_function)(100);		//引数100でメンバ関数アクセス
&x->*&hoge::member_data;			//メンバ変数へのアクセス
```

のようなコードを記述するといった場合が考えられるが、本質的に`->`演算子をを用いることはない。つまり、`expr_wrapper`における`->`演算子のオーバーロードは自由に利用することができる。
このようなものを用いた独自の構文をも定義することが可能である。

## まとめ
とりあえず、何ができるのかといったことのメモのようなもの。

```C++
int main(int argc, char* argv[]) {

	auto x = iml::op::expr_variable<0>('x');
	auto y = iml::op::expr_variable<1>('y');
	//戻り値型がxの型で最大値を返すLambda
	auto f = iml::op::lambda -> result_info(x) [
		iml::op::_if_(x < y)[
			iml::op::_return_ << y
		]._else_[
			iml::op::_return_ << x
		]
	];

	//10
	std::cout << f(10, 1) << std::endl;
	//100
	std::cout << f(10, 100) << std::endl;

	return 0;
}
```

しかも、Constant expressionできる。

# 数式微分
というわけで、添え物程度の数式処理をやる。
## 数学関数の実装
さて、例えば正弦であれば実数や複素数、四元数といった代数的構造に対して定義可能である。しかし、関数テンプレートは部分特殊化が不可であるため、関数単体では厳密に型推論を行うことができない。
そこで、型推論のために構造体を経由することにより実装するのが一般的である。以下はその実装である。

```C++
//デフォルト(実数)のsin
template <class T>
struct Sin {
	static constexpr T _sin_(const T&) {
		//略
	}
}
//複素数のsin
template <class T>
struct Sin<complex<T>> {
	static constexpr complex<T> _sin_(const complex<T>&) {
		//略
	}
}
//四元数のsin
template <class T>
struct Sin<quaternion<T>> {
	static constexpr quaternion<T> _sin_(const quaternion<T>&) {
		//略
	}
}

//実際に呼び出すもの
template <class T>
constexpr auto sin(const T& x) { return Sin<T>::_sin_(x); }
```

ちなみに構造体内で関数名にアンダースコア付けているのは`_sin_`等の内部で再帰を用いることを考慮した場合の対策である。
このとき、構造体がテンプレート引数に持たせることで`expr_wrapper`が数学関数に対応させることが可能であることがわかる。さらに、その構造体内、例えば`Sin`であれば`_sin_`とは別名の`eval`関数を設けることで`expr_wrapper`における数学関数の処理の統一化を図ることができる。

```C++
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
```

まぁ、極めて単純。ほとんどコピペレベル。そして、`Eval`に対応させるように`expr_wrapper`における数学関数を定義する。
例として正弦と余弦を定義する。

```C++
	template <class Op, class Expr>
	struct Sin<op::expr_wrapper<Op, Expr>> {
		using expr_type = op::expr_wrapper<Op, Expr>;
		using result_type = op::expr_wrapper<op::math_function_tag, type_tuple<Sin<expr_type>, tuple<expr_type>, typename tuple<expr_type>::sequence_type>>;

		static constexpr result_type _sin_(const expr_type& x) {
			return result_type(x);
		}

		template <class... Types>
		static constexpr auto eval(Types&&... args) { return sin(forward<Types>(args)...); }
	};

	template <class Op, class Expr>
	struct Cos<op::expr_wrapper<Op, Expr>> {
		using expr_type = op::expr_wrapper<Op, Expr>;
		using result_type = op::expr_wrapper<op::math_function_tag, type_tuple<Cos<expr_type>, tuple<expr_type>, typename tuple<expr_type>::sequence_type>>;

		static constexpr result_type _cos_(const expr_type& x) {
			return result_type(x);
		}

		template <class... Types>
		static constexpr auto eval(Types&&... args) { return cos(forward<Types>(args)...); }
	};
```

まぁ、これも定義通り。`eval`で元の関数本体を呼び出すところがネックかもしれない。
前の例に則れば

```C++
int main(int argc, char* argv[]) {

	auto x = iml::op::expr_variable<0>('x');
	auto y = iml::op::expr_variable<1>('y');
	//戻り値型がxの型で最大値を返すLambda
	auto f = iml::op::lambda -> result_info(double()) [
		iml::op::_if_(x < y)[
			iml::op::_return_ << iml::sin(y)
		]._else_[
			iml::op::_return_ << iml::cos(x)
		]
	];

	std::cout << f(10, 1) << std::endl;
	std::cout << f(10, 100) << std::endl;

	return 0;
}
```

のようなコードを記述することができる。

## 偏微分の実装
前回は`expr_wrapper`に対して微分をしたが、今回もLambdaではなく`expr_wrapper`に対して偏微分をする。特に理由はない。
また、前提として四則演算+添え字のみの式に対して偏微分演算が適用可能であり、式として演算不可である場合を考慮しない。ちなみに、添え字については変数に次元をもった場合の将来性を考慮したものであり、現在はまったくもって意味がない。
実装はどの番号の変数を微分するかであるため大して前回と変更点はない。というわけで、前回から微々たる修正を加えた以下コード。

```C++
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
			constexpr auto add(const Expr1& lhs, const Expr2& rhs) { return Add<Expr1, Expr2>::add(lhs, rhs); }


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
			constexpr auto sub(const Expr1& lhs, const Expr2& rhs) { return Sub<Expr1, Expr2>::sub(lhs, rhs); }


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
			constexpr auto mul(const Expr1& lhs, const Expr2& rhs) { return Mul<Expr1, Expr2>::mul(lhs, rhs); }


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
			constexpr auto div(const Expr1& lhs, const Expr2& rhs) { return Div<Expr1, Expr2>::div(lhs, rhs); }
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
		//N番目の変数で偏微分
		template <size_t N, class T>
		constexpr auto differential(const T& expr) { return Differential<N, T>::differential(expr); }
```

`zero`と`one`構造体でテンプレート引数を廃止されたのは仕様上残当。というか可微分多様体の時点で実数と考えてしまっても問題ないと思う。
また、これに関しては前回からではあるが、`differential`の戻り値型が`zero_tag`もしくは`one_tag`の場合には、そのまま戻り値として返されるため期待したような結果が得られない。
というわけで以下のようにする。

```C++
		template <class T>
		constexpr auto differential_impl(T&& expr) { return expr; }
		template <>
		constexpr auto differential_impl(diff_op::zero_tag&&) { return 0; }
		template <>
		constexpr auto differential_impl(diff_op::one_tag&&) { return 1; }
		//N番目の変数で偏微分
		template <size_t N, class T>
		inline constexpr auto differential(const T& expr) {
			return differential_impl(Differential<N, T>::differential(expr));
		}
```

ちなみに、この数式微分はExpression templateを利用しているためConstant expression指定して用いることが前提であり、`differential_impl`を経由したデメリットは一切ない(と思われる)。

## 数学関数の偏微分
最後に数学関数を微分する。といっても、数学関数用の`expr_wrapper`は実装したため、`Differential`を部分特殊化して数学関数の各々の導関数を定義するだけである。

```C++
		//sinの偏導関数
		template <size_t N, class Expr>
		struct Differential<N, expr_wrapper<math_function_tag, type_tuple<iml::Sin<Expr>, tuple<Expr>, index_tuple<size_t, 0>>>> {
			static constexpr auto differential(const expr_wrapper<math_function_tag, type_tuple<iml::Sin<Expr>, tuple<Expr>, index_tuple<size_t, 0>>>& expr) {
				return iml::cos(expr.args_m.get<0>())*Differential<N, Expr>::differential(expr.args_m.get<0>());
			}
		};


		//cosの偏導関数
		template <size_t N, class Expr>
		struct Differential<N, expr_wrapper<math_function_tag, type_tuple<iml::Cos<Expr>, tuple<Expr>, index_tuple<size_t, 0>>>> {
			static constexpr auto differential(const expr_wrapper<math_function_tag, type_tuple<iml::Cos<Expr>, tuple<Expr>, index_tuple<size_t, 0>>>& expr) {
				return -iml::sin(expr.args_m.get<0>())*Differential<N, Expr>::differential(expr.args_m.get<0>());
			}
		};
```

また、今回は一切触れてこなかったが、`Estring`も`Sin`と`Cos`について部分特殊化することで数式を文字列として出力できる。

```C++
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
```

というわけで、以下のようなコードを実行することができる。

```C++
int main() {

	constexpr auto x = iml::op::expr_variable<0>('x');
	constexpr auto y = iml::op::expr_variable<1>('y');
	constexpr auto z = x * iml::sin(x*y);
	constexpr auto dzdx = iml::op::differential<0>(z);
	//result:sin(x*y)+x*cos(x*y)*y
	std::cout <<"result:"<< dzdx << std::endl;

	return 0;
}
```

これで今回の分は完成。

#終わりに
内容はそこそこ簡単ではあるが、とにかく場合分けが多いため非常に面倒。おとなしく#defineとか使えばもうちょっと簡単に記述できるけど、現在`#define`はフラグメント以外で使わない主義なのでこのまま。#defineでないと実装できないものは例外だが。
また、本文内でも書いたが、Lambda内で変数宣言したり変数をキャプチャといったことは次回以降。やるかどうかは例によって未定。
何かBoost.Lambdaで同じことできるらしいことを教えてもらったが、英語読めないからわからん。

ちなみに、`tuple`と`type_tuple`が混同させているのは意味論的なものである。
多分もう少し内容を他の記事と分割した方が良かったかも。
