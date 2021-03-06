# 動機
唐突にExpressionTemplateがやりたくなって、意外と面白かったからもう少し頑張ってみようと思った。
結果、怪文書みたいのができた。

# Expression templateの実装
## はじめに
というわけで、まずはExpression templateを実装するが、それに対していくつかの制約を設ける。設ける制約は、`reference_wrapper`のようなラッパーとしての実装と、許容する演算は加減乗除の四則演算および単項演算と添え字アクセスのみといった方針をとる。例外として、シフト演算子によるストリーム出力を許容する。
場合によっては、インクリメントとデクリメントの演算子を許容してもいいだろう。

## 構文木の構築
通常、ソースコードに記述された演算は構文解析を通して意味解析等の後に実行することで達成される。そのため、構文解析をして構文木を作成する。
このとき、C++のソースコードから構文木を構成しなければならないが、C++にはテンプレートおよびオペレータオーバーロードといった機能があるため、それは割と容易に達成することができる。
まずは演算子を示すプレースホルダを定義する。

```C++
namespace iml {

	//演算子のタグ
	template <size_t>
	struct operation_tag {};
	//無し
	using none_tag = operation_tag<0>;
	//加算(+)
	using add_tag = operation_tag<1>;
	//減算(-)
	using sub_tag = operation_tag<2>;
	//乗算(*)
	using mul_tag = operation_tag<3>;
	//除算(/)
	using div_tag = operation_tag<4>;
	//添え字([])
	using subscript_tag = operation_tag<5>;
}
```

これにより、演算子とその関係は`operation_tag`で共通で扱うことができる。それぞれの演算子でプレースホルダを作るよりは建設的な方法だと思うし、自分はよくやる。
具体的には以下のような扱いができるといった利点である。

```C++
namespace iml {

	//Expression templateのためのラッパー
	template <class Op, class Expr>
	struct expr_wrapper;
	//変数
	template <class T>
	struct expr_wrapper<none_tag, type_tuple<T>> {};
	//単項演算
	template <class T>
	struct expr_wrapper<add_tag, type_tuple<T>> {};
	template <class T>
	struct expr_wrapper<sub_tag, type_tuple<T>> {};
	//2項演算
	template <size_t N, class L, class R>
	struct expr_wrapper<operation_tag<N>, type_tuple<L, R>> {};
	//添え字アクセス
	template <class T, class S>
	struct expr_wrapper<subscript_tag, type_tuple<T, S>> {};


	//expr_wrapperでラップされているか判定
	template <class>
	struct is_expr_wrapper : false_type {};
	template <size_t N, class Expr, class... Exprs>
	struct is_expr_wrapper<expr_wrapper<operation_tag<N>, type_tuple<Expr, Exprs...>>> : true_type {};
}
```

これは`expr_wrapper`で構文木を作成するためのクラスであるが、そのクラスによりラップされているということが簡潔に記述することができる。ちなみに、`type_tuple`というのは型の`tuple`のようなものである。また、添え字アクセスについては型`T`を型`S`で添え字アクセスするという意味でテンプレート引数を2つとる。
これを元にして、`expr_wrapper`の具体的な中身を構成する。
以下は変数における場合の例である。以後、全ての`expr_wrapper`で共通の事柄は変数の場合を例にとって示す。

```C++
	//変数
	template <class T>
	struct expr_wrapper<none_tag, type_tuple<T>> {
		T x_m;

		constexpr expr_wrapper(const T& x) : x_m(x) {}
	};
```

次に、具体的に構文木を作成する方法を示す。といっても、それは2項演算を定義するだけでコンパイラが構文木を作成するように動いてくれる。というわけで以下コード。

```C++
	//2項演算の定義(lとrの型が等しい場合は多重定義のエラーとなるためSFINAEで除外)
	template <class LOp, class LExpr, class R>
	auto operator+(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) {
		return expr_wrapper<add_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
	}
	template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value>::type>
	auto operator+(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) {
		return expr_wrapper<add_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
	}
	template <class LOp, class LExpr, class R>
	auto operator-(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) {
		return expr_wrapper<sub_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
	}
	template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value>::type>
	auto operator-(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) {
		return expr_wrapper<sub_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
	}
	template <class LOp, class LExpr, class R>
	auto operator*(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) {
		return expr_wrapper<mul_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
	}
	template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value>::type>
	auto operator*(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) {
		return expr_wrapper<mul_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
	}
	template <class LOp, class LExpr, class R>
	auto operator/(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) {
		return expr_wrapper<div_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
	}
	template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value>::type>
	auto operator/(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) {
		return expr_wrapper<div_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
	}
```

まぁ、きわめて単純。でもわざわざ`enable_if`でSFINAEをしたくないからといって構造体を経由してもいいが、余計なコピーが増えるし、2項演算のオペレータオーバーロードはグローバル領域に定義されるから面倒な問題云々が発生するから無理。その辺のC++の仕様が変わってくれるとありがたかったりする。
実際に構文木が作成されていることの確認は`typeid`を用いることでできる。というわけで、以下検証用コード。

```C++
namespace iml {
	//expr_wrapper用の変数
	template <class T>
	expr_wrapper<none_tag, type_tuple<T>> expr_variable(const T& val) {
		return expr_wrapper<none_tag, type_tuple<T>>(val);
	}
}

int main() {
	auto x = iml::expr_variable(10.);
	auto y = (x*x + 5) / x * 8;
	std::cout << typeid(y).name() << std::endl;
	return 0;
}
```

出力は、とりあえず見やすさ重視のため`type_tuple`を省略と`operation_tag`の置き換えを施している。

```text
expr_wrapper<
	mul_tag,
	<
		expr_wrapper<
			div_tag,
			<
				expr_wrapper<
					add_tag,
					<
						expr_wrapper<
							mul_tag,
							<
								expr_wrapper<
									none_tag,
									<double>
								>,
								expr_wrapper<
									none_tag,
									<double>
								>
							>
						>,
						int
					>
				>,
				expr_wrapper<
					none_tag,
					<double>
				>
			>
		>,
		int
	>
>
```

というわけで実際に構文木が生成できることが確認できる。
また、`expr_wrapper`を容易に構築するためのメソッドを作っておくと色々と記述が楽になる。

```C++
	//expr_wrapperの作成
	template <class Op, class... Expr>
	expr_wrapper<Op, type_tuple<Expr...>> make_expr_wrapper(const Expr&... expr) {
		return expr_wrapper<Op, type_tuple<Expr...>>(expr...);
	}
```

次に、単項演算の構文木の構築するためのメソッドを構築する。単項演算子のオーバーロードに限ってはクラス内で定義する必要があるため、以下のように記述する。

```C++
	//変数
	template <class T>
	struct expr_wrapper<none_tag, type_tuple<T>> {
		const T& x_m;

		constexpr expr_wrapper(const T& x) : x_m(x) {}

		expr_wrapper<sub_tag, type_tuple<expr_wrapper>> operator-() const {
			return expr_wrapper<sub_tag, type_tuple<expr_wrapper>>(*this);
		}
		expr_wrapper<add_tag, type_tuple<expr_wrapper>> operator+() const {
			return expr_wrapper<add_tag, type_tuple<expr_wrapper>>(*this);
		}
	};
```

最後に、添え字アクセスのメソッドを作成する。

```C++
	//変数
	template <class T>
	struct expr_wrapper<none_tag, type_tuple<T>> {
		T x_m;

		constexpr expr_wrapper(const T& x) : x_m(x) {}

		expr_wrapper<sub_tag, type_tuple<expr_wrapper>> operator-() const {
			return expr_wrapper<sub_tag, type_tuple<expr_wrapper>>(*this);
		}
		expr_wrapper<add_tag, type_tuple<expr_wrapper>> operator+() const {
			return expr_wrapper<add_tag, type_tuple<expr_wrapper>>(*this);
		}

		template <class U>
		expr_wrapper<subscript_tag, type_tuple<expr_wrapper, U>> operator[](const U& index) const {
			return expr_wrapper<subscript_tag, type_tuple<expr_wrapper, U>>(*this, index);
		}
	};
```

これで構文木を作成するためのメソッドは全て記述することができた。なお、まだ最適化の余地があるが、現状は無視するとする。



## 計算可能な構文木の構築
構文木を作成することはできたが、それが演算可能であるとは限らない。そのため、構文木を演算可能なものにする方法を考える。これは所謂意味解析に当たるものであるが、セオリー通りの意味解析ではC++ではコンパイルエラーを読むのがもはや不可能に近くなる。
そこで、Expression templateが自動で構築することができることから、構文木の構築段階で演算が不可の場合は構築しないでエラーを出力するといった方針をとる。
演算が不可であるかを判定するには、式の演算結果型のエイリアスを`expr_wrapper`内部で保持すればいい。あとは`expr_wrapper`ではない演算結果型のために、式の特性を示す`expr_traits`を定義する。
感覚的にはイテレータでラップされていない型をも考慮して、イテレータの種類を取得するのに`iterator_traits`の`iterator_category`を経由するのと同じようなことをやろうとしている。
式の特性といっても演算結果型と式が演算可能であるかくらいしか思い浮かばないが、現状は演算結果型の定義だけで充分である。
というわけで以下`expr_traits`のコード。

```C++
	//式の特性
	template <class T>
	struct expr_traits {
		using result_type = T;							//演算結果型
	};
	template <size_t N, class Expr, class... Exprs>
	struct expr_traits<expr_wrapper<operation_tag<N>, type_tuple<Expr, Exprs...>>> {
		using result_type = typename expr_wrapper<operation_tag<N>, type_tuple<Expr, Exprs...>>::result_type;
	};
```

後は、各演算パターンに合わせて`expr_traits`を経由したりしなかったりしながら`result_type`を定義する。

```C++
		//expr_wrapper<none_tag, type_tuple<T>>
		using result_type = T;
		//expr_wrapper<add_tag, type_tuple<T>>
		using result_type = typename expr_traits<T>::result_type;
		//expr_wrapper<sub_tag, type_tuple<T>>
		using result_type = typename expr_traits<T>::result_type;
		//expr_wrapper<add_tag, type_tuple<L, R>>
		using result_type = typename calculation_result<typename expr_traits<L>::result_type, typename expr_traits<R>::result_type>::add_type;
		//expr_wrapper<sub_tag, type_tuple<L, R>>
		using result_type = typename calculation_result<typename expr_traits<L>::result_type, typename expr_traits<R>::result_type>::sub_type;
		//expr_wrapper<mul_tag, type_tuple<L, R>>
		using result_type = typename calculation_result<typename expr_traits<L>::result_type, typename expr_traits<R>::result_type>::mul_type;
		//expr_wrapper<div_tag, type_tuple<L, R>>
		using result_type = typename calculation_result<typename expr_traits<L>::result_type, typename expr_traits<R>::result_type>::div_type;
		//expr_wrapper<subscript_tag, type_tuple<T, S>>
		using result_type = typename subscript_access_result<typename expr_traits<T>::result_type, typename expr_traits<S>::result_type>::type;
```

なお、`calculation_result`は2つの型の演算結果型を提供するメタで、`subscript_access_result`は型`T`を型`S`で添え字アクセスしたときの結果型の参照を除去したものを取得するメタである。
後は、この定数値を用いて型を定義できないようにすればいい。`static_assert`を用いて

```C++
	//変数
	template <class T>
	struct expr_wrapper<none_tag, type_tuple<T>> {
		T x_m;

		constexpr expr_wrapper(const T& x) : x_m(x) {}

		using result_type = T;

		static_assert(!is_same<result_type, void>::value, "expression error.");

		expr_wrapper<sub_tag, type_tuple<expr_wrapper>> operator-() const {
			return expr_wrapper<sub_tag, type_tuple<expr_wrapper>>(*this);
		}
		expr_wrapper<add_tag, type_tuple<expr_wrapper>> operator+() const {
			return expr_wrapper<add_tag, type_tuple<expr_wrapper>>(*this);
		}

		template <class U>
		expr_wrapper<subscript_tag, type_tuple<expr_wrapper, U>> operator[](const U& index) const {
			return expr_wrapper<subscript_tag, type_tuple<expr_wrapper, U>>(*this, index);
		}
	};
```

のように定義してもいいが、[ここ](https://cpprefjp.github.io/lang/cpp11/sfinae_expressions.html)にあるようなコードを書いてSFINAEを用いることを前提としたとき、問答無用でコンパイルエラーが発生するため、`static_assert`を用いた実装は記述しない。
これが原因で自分はクラスに対して`static_assert`を用いることはない。この辺のC++の仕様をどうにかしてほしい。もはや自分の中では`static_assert`はゴミと化している。もちろん、使える場面ではしっかり使うが。`static_assert`は属性構文と合わせると美味しい。
話題を戻して、`static_assert`を用いない場合は、`expr_wrapper`のSFINAEのためのテンプレート引数を用意してする必要がある。まずは、`is_calcable`の`expr_wrapper`用の`is_expr_calcable`を定義する。`is_calcable`とは、2つの型が演算可能であるかを判定するメタである。

```C++
	template <class L, class R>
	struct is_expr_calcable {
		using type = typename is_calcable<typename expr_traits<L>::result_type, typename expr_traits<R>::result_type>::type;

		static constexpr bool add_value = type::add_value;
		static constexpr bool sub_value = type::sub_value;
		static constexpr bool mul_value = type::mul_value;
		static constexpr bool div_value = type::div_value;
	};
```

これを用いて`expr_wrapper`を修正する。変数用の`expr_wrapper`は演算可能という概念からは離れるため、2項演算用の`expr_wrapper`についてを示す。

```C++
	//2項演算
	template <class L, class R>
	struct expr_wrapper<add_tag, type_tuple<L, R>, typename enable_if<is_expr_calcable<L, R>::add_value>::type> {
		L lhs_m;
		R rhs_m;

		constexpr expr_wrapper(const L& lhs, const R& rhs) : lhs_m(lhs), rhs_m(rhs) {}

		using result_type = typename calculation_result<typename expr_traits<L>::result_type, typename expr_traits<R>::result_type>::add_type;

		expr_wrapper<sub_tag, type_tuple<expr_wrapper>> operator-() const {
			return expr_wrapper<sub_tag, type_tuple<expr_wrapper>>(*this);
		}
		expr_wrapper<add_tag, type_tuple<expr_wrapper>> operator+() const {
			return expr_wrapper<add_tag, type_tuple<expr_wrapper>>(*this);
		}

		template <class U>
		expr_wrapper<subscript_tag, type_tuple<expr_wrapper, U>> operator[](const U& index) const {
			return expr_wrapper<subscript_tag, type_tuple<expr_wrapper, U>>(*this, index);
		}
	};
```

最後に、2項演算に対してSFINAEが動くようにする。他でも共通であるため、加算についてのみ示す。

```C++
	template <class LOp, class LExpr, class R>
	auto operator+(const expr_wrapper<LOp, LExpr>& lhs, const R& rhs) -> expr_wrapper<add_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>> {
		return expr_wrapper<add_tag, type_tuple<expr_wrapper<LOp, LExpr>, R>>(lhs, rhs);
	}
	template <class L, class ROp, class RExpr, class = typename enable_if<!is_expr_wrapper<L>::value>::type>
	auto operator+(const L& lhs, const expr_wrapper<ROp, RExpr>& rhs) -> expr_wrapper<add_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>> {
		return expr_wrapper<add_tag, type_tuple<L, expr_wrapper<ROp, RExpr>>>(lhs, rhs);
	}
```

後置きにしているのは個人的都合。個人的にはSFINAEさせるのは後置きで、それ以外は前置きで戻り値型を書いてメンテナンス性を上げるといった感じ。


## 式の評価
式の評価というのは要は式を実行するということ。よく、`=`とか`+=`といった代入演算が呼び出されたときに演算を実行するものが多いと思われる。今回はとりあえずそれを実行する関数を作成する。
その前に、変数の仕様について見直す。所謂式の評価というのは「遅延評価」であるため、変数についても「遅延設定」なるものができるべきである。というわけで`shared_ptr`を用いた実装をする。

```C++
	template <class T>
	struct expr_wrapper<none_tag, type_tuple<T>> {
		shared_ptr<T> x_m;
		char name_m;

		constexpr expr_wrapper(const T& x, char name) : x_m(new T(x)), name_m(name) {}

		using result_type = T;

		expr_wrapper<sub_tag, type_tuple<expr_wrapper>> operator-() const {
			return expr_wrapper<sub_tag, type_tuple<expr_wrapper>>(*this);
		}
		expr_wrapper<add_tag, type_tuple<expr_wrapper>> operator+() const {
			return expr_wrapper<add_tag, type_tuple<expr_wrapper>>(*this);
		}

		template <class U>
		expr_wrapper<subscript_tag, type_tuple<expr_wrapper, U>> operator[](const U& index) const {
			return expr_wrapper<subscript_tag, type_tuple<expr_wrapper, U>>(*this, index);
		}

		expr_wrapper& operator=(const T& x) { (*this)() = x; return *this; }

		//リソースの参照の取得
		const T& operator()() const { return *this->x_m.get(); }
		T& operator()() { return *this->x_m.get(); }
	};
```

この式をただ単に実行するだけというのは比較的単純で、再帰的に演算をするだけである。以下単純なコード。

```C++
	//expr_wrapperの実行
	template <class T>
	struct Eval {
		static constexpr T eval(const T& expr) {
			return expr;
		}
	};
	template <class T>
	struct Eval<expr_wrapper<none_tag, type_tuple<T>>> {
		static constexpr T eval(const expr_wrapper<none_tag, type_tuple<T>>& expr) {
			return expr();
		}
	};
	template <class Expr>
	struct Eval<expr_wrapper<add_tag, type_tuple<Expr>>> {
		static constexpr auto eval(const expr_wrapper<add_tag, type_tuple<Expr>>& expr) {
			return Eval<Expr>::eval(expr.x_m);
		}
	};
	template <class Expr>
	struct Eval<expr_wrapper<sub_tag, type_tuple<Expr>>> {
		static constexpr auto eval(const expr_wrapper<sub_tag, type_tuple<Expr>>& expr) {
			return -Eval<Expr>::eval(expr.x_m);
		}
	};
	template <class Expr1, class Expr2>
	struct Eval<expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>> {
		static constexpr auto eval(const expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>& expr) {
			return Eval<Expr1>::eval(expr.lhs_m) + Eval<Expr2>::eval(expr.rhs_m);
		}
	};
	template <class Expr1, class Expr2>
	struct Eval<expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>> {
		static constexpr auto eval(const expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>& expr) {
			return Eval<Expr1>::eval(expr.lhs_m) - Eval<Expr2>::eval(expr.rhs_m);
		}
	};
	template <class Expr1, class Expr2>
	struct Eval<expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>> {
		static constexpr auto eval(const expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>& expr) {
			return Eval<Expr1>::eval(expr.lhs_m) * Eval<Expr2>::eval(expr.rhs_m);
		}
	};
	template <class Expr1, class Expr2>
	struct Eval<expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>> {
		static constexpr auto eval(const expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>& expr) {
			return Eval<Expr1>::eval(expr.lhs_m) / Eval<Expr2>::eval(expr.rhs_m);
		}
	};
	template <class Expr1, class Expr2>
	struct Eval<expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>> {
		static constexpr auto eval(const expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>& expr) {
			return Eval<Expr1>::eval(expr.x_m)[Eval<Expr2>::eval(expr.index_m)];
		}
	};
	template <class T>
	auto eval(const T& expr) { return Eval<T>::eval(expr); }
```

とりあえず使い方としては

```C++
int main() {
	auto x = iml::expr_variable(10.);
	auto y = (5 * x + 15) / 10;
	x = 100;			//変数を書き換えてみる
	//51.5
	std::cout << iml::eval(y) << std::endl;

	return 0;
}
```

みたいな使い方ができる。
これで式を評価するための評価関数`eval`が完成する。なんとなくラムダ式の超劣化版みたいな。
これでExpression template完成。

# 数式処理
ここからが今回のメイン。普通のプログラマには何も使い物にならない内容。あるのはロマンだけ。
でも数式処理は詳しくないから簡単な例だけ。

## 数式の出力
まず、数式処理のチェックに構文木は見ていられないため、数式の文字列で出力できるようにする。
全ての式に括弧を付けるというだけであれば非常に楽であるが、それではいけない。使い勝手が悪い。実装方法としては、演算の優先順位を予め定義しておき、構文木の親より子の方が演算の優先順位が低いならば括弧をつければいい。
とりあえず[ここ](https://ja.cppreference.com/w/cpp/language/operator_precedence)を参考に演算の優先順位を定義する。
どうでもいいことであればC++20で三方比較演算子が追加される(?)のは初めて知った。

```C++
	//トップレベルの演算の優先順位の定義
	//変数もしくは定数
	template <class T>
	struct operator_precedence {
		static constexpr size_t value = 0;
	};
	template <class T>
	struct operator_precedence<expr_wrapper<none_tag, type_tuple<T>>> {
		static constexpr size_t value = 0;
	};
	//単項演算
	template <class Expr>
	struct operator_precedence<expr_wrapper<add_tag, type_tuple<Expr>>> {
		static constexpr size_t value = 3;
	};
	template <class Expr>
	struct operator_precedence<expr_wrapper<sub_tag, type_tuple<Expr>>> {
		static constexpr size_t value = 3;
	};
	//2項演算
	template <class Expr1, class Expr2>
	struct operator_precedence<expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>> {
		static constexpr size_t value = 6;
	};
	template <class Expr1, class Expr2>
	struct operator_precedence<expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>> {
		static constexpr size_t value = 6;
	};
	template <class Expr1, class Expr2>
	struct operator_precedence<expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>> {
		static constexpr size_t value = 5;
	};
	template <class Expr1, class Expr2>
	struct operator_precedence<expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>> {
		static constexpr size_t value = 5;
	};
	//添え字
	template <class Expr1, class Expr2>
	struct operator_precedence<expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>> {
		static constexpr size_t value = 2;
	};
```

さて、これで文字列の生成の準備が整ったが、文字列操作の全てを実装するのは面倒であるため`stringstream`を用いて実装する。本質としては`eval`の実装とほとんど同じである。

```C++
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
			ss << expr();
			return ss.str();
		}
	};
	template <class Expr>
	struct Estring<expr_wrapper<add_tag, type_tuple<Expr>>> {
		using expr_type = expr_wrapper<add_tag, type_tuple<Expr>>;
		static auto estring(const expr_type& expr) {
			std::stringstream ss;
			if (operator_precedence<Expr>::value > operator_precedence<expr_type>::value) {
				ss << "+(" << Estring<Expr>::estring(expr.x_m) << ')';
			}
			else {
				ss << '+' << Estring<Expr>::estring(expr.x_m);
			}
			return ss.str();
		}
	};
	template <class Expr>
	struct Estring<expr_wrapper<sub_tag, type_tuple<Expr>>> {
		using expr_type = expr_wrapper<sub_tag, type_tuple<Expr>>;
		static auto estring(const expr_type& expr) {
			std::stringstream ss;
			if (operator_precedence<Expr>::value > operator_precedence<expr_type>::value) {
				ss << "-(" << Estring<Expr>::estring(expr.x_m) << ')';
			}
			else {
				ss << '-' << Estring<Expr>::estring(expr.x_m);
			}
			return ss.str();
		}
	};
	template <class Expr1, class Expr2>
	struct Estring<expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>> {
		using expr_type = expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>;
		static auto estring(const expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>& expr) {
			std::stringstream ss;
			if (operator_precedence<Expr1>::value > operator_precedence<expr_type>::value) {
				ss << "(" << Estring<Expr1>::estring(expr.lhs_m) << ')';
			}
			else {
				ss << Estring<Expr1>::estring(expr.lhs_m);
			}
			ss << '+';
			if (operator_precedence<Expr2>::value > operator_precedence<expr_type>::value) {
				ss << "(" << Estring<Expr2>::estring(expr.rhs_m) << ')';
			}
			else {
				ss << Estring<Expr2>::estring(expr.rhs_m);
			}
			return ss.str();
		}
	};
	template <class Expr1, class Expr2>
	struct Estring<expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>> {
		using expr_type = expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>;
		static auto estring(const expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>& expr) {
			std::stringstream ss;
			if (operator_precedence<Expr1>::value > operator_precedence<expr_type>::value) {
				ss << "(" << Estring<Expr1>::estring(expr.lhs_m) << ')';
			}
			else {
				ss << Estring<Expr1>::estring(expr.lhs_m);
			}
			ss << '-';
			if (operator_precedence<Expr2>::value > operator_precedence<expr_type>::value) {
				ss << "(" << Estring<Expr2>::estring(expr.rhs_m) << ')';
			}
			else {
				ss << Estring<Expr2>::estring(expr.rhs_m);
			}
			return ss.str();
		}
	};
	template <class Expr1, class Expr2>
	struct Estring<expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>> {
		using expr_type = expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>;
		static auto estring(const expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>& expr) {
			std::stringstream ss;
			if (operator_precedence<Expr1>::value > operator_precedence<expr_type>::value) {
				ss << "(" << Estring<Expr1>::estring(expr.lhs_m) << ')';
			}
			else {
				ss << Estring<Expr1>::estring(expr.lhs_m);
			}
			ss << '*';
			if (operator_precedence<Expr2>::value > operator_precedence<expr_type>::value) {
				ss << "(" << Estring<Expr2>::estring(expr.rhs_m) << ')';
			}
			else {
				ss << Estring<Expr2>::estring(expr.rhs_m);
			}
			return ss.str();
		}
	};
	template <class Expr1, class Expr2>
	struct Estring<expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>> {
		using expr_type = expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>;
		static auto estring(const expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>& expr) {
			std::stringstream ss;
			if (operator_precedence<Expr1>::value > operator_precedence<expr_type>::value) {
				ss << "(" << Estring<Expr1>::estring(expr.lhs_m) << ')';
			}
			else {
				ss << Estring<Expr1>::estring(expr.lhs_m);
			}
			ss << '/';
			if (operator_precedence<Expr2>::value > operator_precedence<expr_type>::value) {
				ss << "(" << Estring<Expr2>::estring(expr.rhs_m) << ')';
			}
			else {
				ss << Estring<Expr2>::estring(expr.rhs_m);
			}
			return ss.str();
		}
	};
	template <class Expr1, class Expr2>
	struct Estring<expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>> {
		using expr_type = expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>;
		static auto estring(const expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>& expr) {
			std::stringstream ss;
			if (operator_precedence<Expr1>::value > operator_precedence<expr_type>::value) {
				ss << "(" << Estring<Expr1>::estring(expr.x_m) << ')';
			}
			else {
				ss << Estring<Expr1>::estring(expr.x_m);
			}
			ss << '[' << Estring<Expr2>::estring(expr.index_m) << ']';
			return ss.str();
		}
	};
	template <class T>
	auto estring(const T& expr) { return Estring<T>::estring(expr); }
```

また、これは出力ストリームをオーバーロードして

```C++
	template <class T, class = typename enable_if<is_expr_wrapper<T>::value>::type>
	std::ostream& operator<<(std::ostream& os, const T& expr) {
		os << estring(expr);
		return os;
	}
```

としてもいいだろう。というか、ストリームを使うならするべき。
これで完成と行きたいところであるが、このままではせっかく作った数式の変数までもが保持する値で出力される。
というわけで変数名を保持できるようにする。

```C++
	//変数
	template <class T>
	struct expr_wrapper<none_tag, type_tuple<T>> {
		T x_m;
		char name_m;

		constexpr expr_wrapper(const T& x, char name) : x_m(x), name_m(name) {}

		using result_type = T;

		expr_wrapper<sub_tag, type_tuple<expr_wrapper>> operator-() const {
			return expr_wrapper<sub_tag, type_tuple<expr_wrapper>>(*this);
		}
		expr_wrapper<add_tag, type_tuple<expr_wrapper>> operator+() const {
			return expr_wrapper<add_tag, type_tuple<expr_wrapper>>(*this);
		}

		template <class U>
		expr_wrapper<subscript_tag, type_tuple<expr_wrapper, U>> operator[](const U& index) const {
			return expr_wrapper<subscript_tag, type_tuple<expr_wrapper, U>>(*this, index);
		}

		expr_wrapper& operator=(const T& x) { this->x_m = x; return *this; }
	};
```

また、`expr_variable`を変数名を指定できるようにする。

```C++
	//expr_wrapper用の変数
	template <class T>
	expr_wrapper<none_tag, type_tuple<T>> expr_variable(const T& val, char name = 'x') {
		return expr_wrapper<none_tag, type_tuple<T>>(val, name);
	}
```

そして、文字列出力のやつを以下のように書き換える。

```C++
	template <class T>
	struct Estring<expr_wrapper<none_tag, type_tuple<T>>> {
		using expr_type = expr_wrapper<none_tag, type_tuple<T>>;
		static auto estring(const expr_type& expr) {
			std::stringstream ss;
			ss << expr.name_m;
			return ss.str();
		}
	};
```

これで数式を文字列として出力することができる。
こんな感じで使うことができる。

```C++
int main() {
    auto x = iml::expr_variable(10., 'z');
    auto y = -(5 * x + 15) / 10;
    //-(5*z+15)/10
    std::cout << y << std::endl;

    return 0;
}
```

しかし、

```C++
int main() {
    auto x = iml::expr_variable(10., 'x');
    auto y = 1 / (x * x);
    //1/x*x
    std::cout << y << std::endl;

    return 0;
}
```

となり、除算と乗算の複合に関しては正しく表示されない。そのため、以下のようにすることでこの問題を解消する。

```C++
	template <class Expr1, class Expr2>
	struct Estring<expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>> {
		using expr_type = expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>;
		static auto estring(const expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>& expr) {
			std::stringstream ss;
			//等号を付け加えた
			if (operator_precedence<Expr1>::value >= operator_precedence<expr_type>::value) {
				ss << "(" << Estring<Expr1>::estring(expr.lhs_m) << ')';
			}
			else {
				ss << Estring<Expr1>::estring(expr.lhs_m);
			}
			ss << '/';
			//等号を付け加えた
			if (operator_precedence<Expr2>::value >= operator_precedence<expr_type>::value) {
				ss << "(" << Estring<Expr2>::estring(expr.rhs_m) << ')';
			}
			else {
				ss << Estring<Expr2>::estring(expr.rhs_m);
			}
			return ss.str();
		}
	};
```


## 式の整理
さて、せっかく`expr_wrapper`は変数をもつことができるのだから式の整理ができるべきである。というか式の整理は数式処理で最も基本的なことであり、最も難しい。
そのため、比較的簡単に実装できるもののみを扱う。

### 分配法則
まずやることは積の分配である。積の分配とはこんなの。
$$
a\times(b+c+d)=a\times b+a\times c+a\times d
$$
商も同様。要は、式の構文木で親より子の方が優先順位が低いなら子に分配するといったもの。
イメージとしては上の数式を例にとって以下の感じの再帰的操作をする。
<img src="https://i.imgur.com/1urblIi.png" title="tree"></img>
一般に考えればこのようになるが、線型演算でなければ成り立たない。もっと詳しく言うと環であるということ。例えば添え字アクセスはどう考えても分配法則は成り立たない。その判定系はかなり面倒であることと、今回のテーマとはかけ離れるため省略する。
これの実装も本質的には`eval`と同じである。ただこれはテンプレート引数の内容が変わるため1から再構成する必要がある。さらに、これは下の階層から分配法則を適用させていくため、分配可能かを評価する式は下の階層から分配法則を適用した結果の式であるため、渡された式をそのまま分配可能かを評価してはならない。
そのため、下からの計算完了を待つために2段階で計算する必要がある。マスタースレーブみたいな~~(絶対違う)~~。

```C++
	//分配則の適用
	template <class>
	struct Distributive1;
	template <class T, bool = is_underlayer_low_precedence<T>::lhs_value, bool = is_underlayer_low_precedence<T>::rhs_value>
	struct Distributive2 {
		static constexpr auto distributive2(const T& expr) {
			return expr;
		}
	};
	template <class T, bool F1, bool F2>
	struct Distributive2<expr_wrapper<none_tag, type_tuple<T>>, F1, F2> {
		static constexpr auto distributive2(const expr_wrapper<none_tag, type_tuple<T>>& expr) {
			return expr;
		}
	};
	template <class Expr, bool F1, bool F2>
	struct Distributive2<expr_wrapper<add_tag, type_tuple<Expr>>, F1, F2> {
		static constexpr auto distributive2(const expr_wrapper<add_tag, type_tuple<Expr>>& expr) {
			return +Distributive1<Expr>::distributive1(expr.x_m);
		}
	};
	template <class Expr, bool F1, bool F2>
	struct Distributive2<expr_wrapper<sub_tag, type_tuple<Expr>>, F1, F2> {
		static constexpr auto distributive2(const expr_wrapper<sub_tag, type_tuple<Expr>>& expr) {
			return -Distributive1<Expr>::distributive1(expr.x_m);
		}
	};
	template <class Expr1, class Expr2, bool F1, bool F2>
	struct Distributive2<expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>, F1, F2> {
		static constexpr auto distributive2(const expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>& expr) {
			return Distributive1<Expr1>::distributive1(expr.lhs_m) + Distributive1<Expr2>::distributive1(expr.rhs_m);
		}
	};
	template <class Expr1, class Expr2, bool F1, bool F2>
	struct Distributive2<expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>, F1, F2> {
		static constexpr auto distributive2(const expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>& expr) {
			return Distributive1<Expr1>::distributive1(expr.lhs_m) - Distributive1<Expr2>::distributive1(expr.rhs_m);
		}
	};
	template <class Expr1, class Expr2>
	struct Distributive2<expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>, false, false> {
		static constexpr auto distributive2(const expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>& expr) {
			return Distributive1<Expr1>::distributive1(expr.lhs_m) * Distributive1<Expr2>::distributive1(expr.rhs_m);
		}
	};
	template <class Expr1, class Expr2>
	struct Distributive2<expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>, true, false> {
		static constexpr auto distributive2(const expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>& expr) {
			//exprの左には孫が存在することを用いて戻り値型の補助を作成
			using lhs_lhs_type = decltype(expr.lhs_m.lhs_m);
			using lhs_rhs_type = decltype(expr.lhs_m.rhs_m);
			auto temp1 = Distributive1<Expr2>::distributive1(expr.rhs_m);
			auto temp2 = Distributive1<lhs_lhs_type>::distributive1(expr.lhs_m.lhs_m);
			auto temp3 = Distributive1<lhs_rhs_type>::distributive1(expr.lhs_m.rhs_m);

			auto temp4 = Distributive1<expr_wrapper<mul_tag, type_tuple<decltype(temp2), decltype(temp1)>>>::distributive1(
				make_expr_wrapper<mul_tag>(temp2, temp1)
			);
			auto temp5 = Distributive1<expr_wrapper<mul_tag, type_tuple<decltype(temp3), decltype(temp1)>>>::distributive1(
				make_expr_wrapper<mul_tag>(temp3, temp1)
			);
			return make_expr_wrapper<typename expr_traits<Expr1>::tag>(temp4, temp5);
		}
	};
	template <class Expr1, class Expr2>
	struct Distributive2<expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>, false, true> {
		static constexpr auto distributive2(const expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>& expr) {
			//exprの右には孫が存在することを用いて戻り値型の補助を作成
			using rhs_lhs_type = decltype(expr.rhs_m.lhs_m);
			using rhs_rhs_type = decltype(expr.rhs_m.rhs_m);
			auto temp1 = Distributive1<Expr1>::distributive1(expr.lhs_m);
			auto temp2 = Distributive1<rhs_lhs_type>::distributive1(expr.rhs_m.lhs_m);
			auto temp3 = Distributive1<rhs_rhs_type>::distributive1(expr.rhs_m.rhs_m);

			auto temp4 = Distributive1<expr_wrapper<mul_tag, type_tuple<decltype(temp1), decltype(temp2)>>>::distributive1(
				make_expr_wrapper<mul_tag>(temp1, temp2)
			);
			auto temp5 = Distributive1<expr_wrapper<mul_tag, type_tuple<decltype(temp1), decltype(temp3)>>>::distributive1(
				make_expr_wrapper<mul_tag>(temp1, temp3)
			);
			return make_expr_wrapper<typename expr_traits<Expr2>::tag>(temp4, temp5);
		}
	};
	template <class Expr1, class Expr2>
	struct Distributive2<expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>, true, true> {
		static constexpr auto distributive2(const expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>& expr) {
			//exprの左右には孫が存在することを用いて戻り値型の補助を作成
			using lhs_lhs_type = decltype(expr.lhs_m.lhs_m);
			using lhs_rhs_type = decltype(expr.lhs_m.rhs_m);
			using rhs_lhs_type = decltype(expr.rhs_m.lhs_m);
			using rhs_rhs_type = decltype(expr.rhs_m.rhs_m);
			auto temp1 = Distributive1<lhs_lhs_type>::distributive1(expr.lhs_m.lhs_m);
			auto temp2 = Distributive1<lhs_rhs_type>::distributive1(expr.lhs_m.rhs_m);
			auto temp3 = Distributive1<rhs_lhs_type>::distributive1(expr.rhs_m.lhs_m);
			auto temp4 = Distributive1<rhs_rhs_type>::distributive1(expr.rhs_m.rhs_m);
			//左辺を優先的に展開
			auto temp5 = Distributive1<expr_wrapper<mul_tag, type_tuple<decltype(temp1), decltype(temp3)>>>::distributive1(
				make_expr_wrapper<mul_tag>(temp1, temp3)
			);
			auto temp6 = Distributive1<expr_wrapper<mul_tag, type_tuple<decltype(temp1), decltype(temp4)>>>::distributive1(
				make_expr_wrapper<mul_tag>(temp1, temp4)
			);
			auto temp7 = Distributive1<expr_wrapper<mul_tag, type_tuple<decltype(temp2), decltype(temp3)>>>::distributive1(
				make_expr_wrapper<mul_tag>(temp2, temp3)
			);
			auto temp8 = Distributive1<expr_wrapper<mul_tag, type_tuple<decltype(temp2), decltype(temp4)>>>::distributive1(
				make_expr_wrapper<mul_tag>(temp2, temp4)
			);
			auto temp9 = make_expr_wrapper<typename expr_traits<Expr2>::tag>(temp5, temp6);
			auto temp10 = make_expr_wrapper<typename expr_traits<Expr2>::tag>(temp7, temp8);
			return make_expr_wrapper<typename expr_traits<Expr1>::tag>(temp9, temp10);
		}
	};
	template <class Expr1, class Expr2>
	struct Distributive2<expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>, false, false> {
		static constexpr auto distributive2(const expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>& expr) {
			return Distributive1<Expr1>::distributive1(expr.lhs_m) / Distributive1<Expr2>::distributive1(expr.rhs_m);
		}
	};
	template <class Expr1, class Expr2>
	struct Distributive2<expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>, true, false> {
		static constexpr auto distributive2(const expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>& expr) {
			//exprの左には孫が存在することを用いて戻り値型の補助を作成
			using lhs_lhs_type = decltype(expr.lhs_m.lhs_m);
			using lhs_rhs_type = decltype(expr.lhs_m.rhs_m);
			auto temp1 = Distributive1<Expr2>::distributive1(expr.rhs_m);
			auto temp2 = Distributive1<lhs_lhs_type>::distributive1(expr.lhs_m.lhs_m);
			auto temp3 = Distributive1<lhs_rhs_type>::distributive1(expr.lhs_m.rhs_m);

			auto temp4 = Distributive1<expr_wrapper<div_tag, type_tuple<decltype(temp2), decltype(temp1)>>>::distributive1(
				make_expr_wrapper<div_tag>(temp2, temp1)
			);
			auto temp5 = Distributive1<expr_wrapper<div_tag, type_tuple<decltype(temp3), decltype(temp1)>>>::distributive1(
				make_expr_wrapper<div_tag>(temp3, temp1)
			);
			return make_expr_wrapper<typename expr_traits<Expr1>::tag>(temp4, temp5);
		}
	};
	template <class Expr1, class Expr2>
	struct Distributive2<expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>, false, true> {
		static constexpr auto distributive2(const expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>& expr) {
			//a/(b+c)に対して分配則を適用することは不可
			return Distributive1<Expr1>::distributive1(expr.lhs_m) / Distributive1<Expr2>::distributive1(expr.rhs_m);
		}
	};
	template <class Expr1, class Expr2>
	struct Distributive2<expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>, true, true> {
		static constexpr auto distributive2(const expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>& expr) {
			//exprの左には孫が存在することを用いて戻り値型の補助を作成
			using lhs_lhs_type = decltype(expr.lhs_m.lhs_m);
			using lhs_rhs_type = decltype(expr.lhs_m.rhs_m);
			auto temp1 = Distributive1<Expr2>::distributive1(expr.rhs_m);
			auto temp2 = Distributive1<lhs_lhs_type>::distributive1(expr.lhs_m.lhs_m);
			auto temp3 = Distributive1<lhs_rhs_type>::distributive1(expr.lhs_m.rhs_m);

			auto temp4 = Distributive1<expr_wrapper<div_tag, type_tuple<decltype(temp2), decltype(temp1)>>>::distributive1(
				make_expr_wrapper<div_tag>(temp2, temp1)
			);
			auto temp5 = Distributive1<expr_wrapper<div_tag, type_tuple<decltype(temp3), decltype(temp1)>>>::distributive1(
				make_expr_wrapper<div_tag>(temp3, temp1)
			);
			return make_expr_wrapper<typename expr_traits<Expr1>::tag>(temp4, temp5);
		}
	};
	template <class Expr1, class Expr2, bool F1, bool F2>
	struct Distributive2<expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>, F1, F2> {
		static constexpr auto distributive2(const expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>& expr) {
			return Distributive1<Expr1>::distributive1(expr.x_m)[Distributive1<Expr2>::distributive1(expr.index_m)];
		}
	};
	//下の階層から評価するためのもの(要は上の階層は遅延評価するということ)
	template <class T>
	struct Distributive1 {
		static constexpr auto distributive1(const T& expr) {
			return Distributive2<T>::distributive2(expr);
		}
	};
	template <class T>
	struct Distributive1<expr_wrapper<none_tag, type_tuple<T>>> {
		static constexpr auto distributive1(const expr_wrapper<none_tag, type_tuple<T>>& expr) {
			return Distributive2<expr_wrapper<none_tag, type_tuple<T>>>::distributive2(expr);
		}
	};
	template <class Expr>
	struct Distributive1<expr_wrapper<add_tag, type_tuple<Expr>>> {
		static constexpr auto distributive1(const expr_wrapper<add_tag, type_tuple<Expr>>& expr) {
			auto temp = Distributive1<Expr>::distributive1(expr.x_m);
			return Distributive2<expr_wrapper<add_tag, type_tuple<decltype(temp)>>>::distributive2(temp);
		}
	};
	template <class Expr>
	struct Distributive1<expr_wrapper<sub_tag, type_tuple<Expr>>> {
		static constexpr auto distributive1(const expr_wrapper<sub_tag, type_tuple<Expr>>& expr) {
			auto temp = Distributive1<Expr>::distributive1(expr.x_m);
			return Distributive2<expr_wrapper<sub_tag, type_tuple<decltype(temp)>>>::distributive2(temp);
		}
	};
	template <class Expr1, class Expr2>
	struct Distributive1<expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>> {
		static constexpr auto distributive1(const expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>& expr) {
			auto temp1 = Distributive1<Expr1>::distributive1(expr.lhs_m);
			auto temp2 = Distributive1<Expr2>::distributive1(expr.rhs_m);
			return Distributive2<expr_wrapper<add_tag, type_tuple<decltype(temp1), decltype(temp2)>>>::distributive2(
				make_expr_wrapper<add_tag>(temp1, temp2)
			);
		}
	};
	template <class Expr1, class Expr2>
	struct Distributive1<expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>> {
		static constexpr auto distributive1(const expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>& expr) {
			auto temp1 = Distributive1<Expr1>::distributive1(expr.lhs_m);
			auto temp2 = Distributive1<Expr2>::distributive1(expr.rhs_m);
			return Distributive2<expr_wrapper<sub_tag, type_tuple<decltype(temp1), decltype(temp2)>>>::distributive2(
				make_expr_wrapper<sub_tag>(temp1, temp2)
			);
		}
	};
	template <class Expr1, class Expr2>
	struct Distributive1<expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>> {
		static constexpr auto distributive1(const expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>& expr) {
			auto temp1 = Distributive1<Expr1>::distributive1(expr.lhs_m);
			auto temp2 = Distributive1<Expr2>::distributive1(expr.rhs_m);
			return Distributive2<expr_wrapper<mul_tag, type_tuple<decltype(temp1), decltype(temp2)>>>::distributive2(
				make_expr_wrapper<mul_tag>(temp1, temp2)
			);
		}
	};
	template <class Expr1, class Expr2>
	struct Distributive1<expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>> {
		static constexpr auto distributive1(const expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>& expr) {
			auto temp1 = Distributive1<Expr1>::distributive1(expr.lhs_m);
			auto temp2 = Distributive1<Expr2>::distributive1(expr.rhs_m);
			return Distributive2<expr_wrapper<div_tag, type_tuple<decltype(temp1), decltype(temp2)>>>::distributive2(
				make_expr_wrapper<div_tag>(temp1, temp2)
			);
		}
	};
	template <class Expr1, class Expr2>
	struct Distributive1<expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>> {
		static constexpr auto distributive1(const expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>& expr) {
			auto temp1 = Distributive1<Expr1>::distributive1(expr.x_m);
			auto temp2 = Distributive1<Expr2>::distributive1(expr.index_m);
			return Distributive2<expr_wrapper<subscript_tag, type_tuple<decltype(temp1), decltype(temp2)>>>::distributive2(
				make_expr_wrapper<subscript_tag>(temp1, temp2)
			);
		}
	};
	template <class T>
	constexpr auto distributive(const T& expr) { return Distributive1<T>::distributive1(expr); }
```

すごく長いけど割とやっていることは単純。冗長で無駄な部分はあるが、今回はソースの記述の統一性を重視した。
単項演算子の`+`とか`-`についても分配法則を適用してもいいが、今回は簡易化のために積と商の分配法則の適用のみを実装した。`bool`値についての分部特殊化すれば他の演算に対する分配法則を実装することができる。
こんな感じで使うことができる。

```C++
int main() {
	auto x = iml::expr_variable(10., 'x');
	auto y = (x + 10) * (x - 5) / (x + 1);

	//x*x/(x+1)-x*5/(x+1)+10*x/(x+1)-50/(x+1)
	std::cout << iml::distributive(y) << std::endl;

	return 0;
}
```

## 数式微分
これを一番最後にやる。ただし、式の整理と比べると非常に簡単。なぜならベースは自動微分だから。というわけで

- [n階微分可能な自動微分を実装する](https://qiita.com/168irohairoha/items/ff1d104685e95f00709f)

の理論を参照しよう。今回は1階微分だけで充分である。
これも本質的に`eval`と同じ実装である。

```C++
	//数式微分
	template <class T>
	struct Differential {
		static constexpr auto differential(const T& expr) {
			return 0;
		}
	};
	template <class T>
	struct Differential<expr_wrapper<none_tag, type_tuple<T>>> {
		static constexpr auto differential(const expr_wrapper<none_tag, type_tuple<T>>& expr) {
			return 1;
		}
	};
	template <class Expr>
	struct Differential<expr_wrapper<add_tag, type_tuple<Expr>>> {
		static constexpr auto differential(const expr_wrapper<add_tag, type_tuple<Expr>>& expr) {
			return Differential<Expr>::differential(expr.x_m);
		}
	};
	template <class Expr>
	struct Differential<expr_wrapper<sub_tag, type_tuple<Expr>>> {
		static constexpr auto differential(const expr_wrapper<sub_tag, type_tuple<Expr>>& expr) {
			return -Differential<Expr>::differential(expr.x_m);
		}
	};
	template <class Expr1, class Expr2>
	struct Differential<expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>> {
		static constexpr auto differential(const expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>& expr) {
			return Differential<Expr1>::differential(expr.lhs_m) + Differential<Expr2>::differential(expr.rhs_m);
		}
	};
	template <class Expr1, class Expr2>
	struct Differential<expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>> {
		static constexpr auto differential(const expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>& expr) {
			return Differential<Expr1>::differential(expr.lhs_m) - Differential<Expr2>::differential(expr.rhs_m);
		}
	};
	template <class Expr1, class Expr2>
	struct Differential<expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>> {
		static constexpr auto differential(const expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>& expr) {
			return Differential<Expr1>::differential(expr.lhs_m) * expr.rhs_m
				+ expr.lhs_m * Differential<Expr2>::differential(expr.rhs_m);
		}
	};
	template <class Expr1, class Expr2>
	struct Differential<expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>> {
		static constexpr auto differential(const expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>& expr) {
			return (Differential<Expr1>::differential(expr.lhs_m) * expr.rhs_m
				- expr.lhs_m * Differential<Expr2>::differential(expr.rhs_m)) / (expr.rhs_m * expr.rhs_m);
		}
	};
	template <class Expr1, class Expr2>
	struct Differential<expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>> {
		static constexpr auto differential(const expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>& expr) {
			return Differential<Expr1>::differential(expr.x_m)[expr.index_m];
		}
	};
	template <class T>
	constexpr auto differential(const T& expr) { return Differential<T>::differential(expr); }
```

このような安直な方法でもいいのだが、

```C++
int main() {
    auto x = iml::expr_variable(10., 'x');
    auto y = (x + 10) * (x - 5);

    //1*(x-5)+(x+10)*1
    //実際の型は(1+0)*(x-5)+(x+10)*(1+0)として扱っている
    std::cout << iml::differential(y) << std::endl;

    return 0;
}
```

となり、中々に煩雑というか酷い。
というわけで、$0$と$1$をリテラルとして扱うためのプレースホルダを定義する。

```C++
		//任意の型の零元
		template <class T>
		struct zero {
			//零元を保持
			static constexpr T value = multiplicative<T>::absorbing_element();
		};
		//任意の型の単位元
		template <class T>
		struct one {
			//単位元の保持
			static constexpr T value = multiplicative<T>::identity_element();
		};

		template <class T>
		struct is_zero : false_type {};
		template <class T>
		struct is_zero<zero<T>> : true_type {};
		template <class T>
		struct is_one : false_type {};
		template <class T>
		struct is_one<one<T>> : true_type {};
```

そして、これらを扱うための2項演算を定義したいところであるが、これ以上の2項演算のオーバーロードの汚染は不味い。2項演算にもスコープの概念はあるが、もはやその体を成していない。というか上手い使い方が思いつかない。
というわけで、新しく`diff_op`という名前空間を作成し、**関数として**2項演算を定義する。`zero`や`one`の名前空間の汚染防止のためにそこに定義する。
そして、`Differential`内の演算を`diff_op`内の2項演算を用いたコードに変換する。
というわけでコード全体。少し長い。

```C++
	namespace diff_op {

		//任意の型の零元
		template <class T>
		struct zero {
			//零元を保持
			static constexpr T value = multiplicative<T>::absorbing_element();
		};
		//任意の型の単位元
		template <class T>
		struct one {
			//単位元の保持
			static constexpr T value = multiplicative<T>::identity_element();
		};

		template <class T>
		struct is_zero : false_type {};
		template <class T>
		struct is_zero<zero<T>> : true_type {};
		template <class T>
		struct is_one : false_type {};
		template <class T>
		struct is_one<one<T>> : true_type {};


		//加算
		template <class Expr1, class Expr2, class = void>
		struct Add {
			static constexpr auto add(const Expr1& lhs, const Expr2& rhs) -> expr_wrapper<add_tag, type_tuple<Expr1, Expr2>> {
				return expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>(lhs, rhs);
			}
		};
		template <class Expr, class T>
		struct Add<Expr, zero<T>> {
			static constexpr auto add(const Expr& lhs, const zero<T>&) -> Expr { return lhs; }
		};
		template <class T, class Expr>
		struct Add<zero<T>, Expr, typename enable_if<!is_zero<Expr>::value>::type> {
			static constexpr auto add(const zero<T>&, const Expr& rhs) -> Expr { return rhs; }
		};
		template <class Expr, class T>
		struct Add<Expr, one<T>, typename enable_if<!is_zero<Expr>::value>::type> {
			static constexpr auto add(const Expr& lhs, const one<T>&) -> expr_wrapper<add_tag, type_tuple<Expr, T>> {
				return expr_wrapper<add_tag, type_tuple<Expr, T>>(lhs, one<T>::value);
			}
		};
		template <class T, class Expr>
		struct Add<one<T>, Expr, typename enable_if<!is_one<Expr>::value && !is_zero<Expr>::value>::type> {
			static constexpr auto add(const one<T>&, const Expr& rhs) -> expr_wrapper<add_tag, type_tuple<T, Expr>> {
				return expr_wrapper<add_tag, type_tuple<T, Expr>>(one<T>::value, rhs);
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
		template <class Expr, class T>
		struct Sub<Expr, zero<T>> {
			static constexpr auto sub(const Expr& lhs, const zero<T>&) -> Expr { return lhs; }
		};
		template <class T, class Expr>
		struct Sub<zero<T>, Expr, typename enable_if<!is_zero<Expr>::value>::type> {
			static constexpr auto sub(const zero<T>&, const Expr& rhs) -> expr_wrapper<sub_tag, type_tuple<Expr>> {
				return expr_wrapper<sub_tag, type_tuple<Expr>>(rhs);
			}
		};
		template <class Expr, class T>
		struct Sub<Expr, one<T>, typename enable_if<!is_zero<Expr>::value>::type> {
			static constexpr auto sub(const Expr& lhs, const one<T>&) -> expr_wrapper<sub_tag, type_tuple<Expr, T>> {
				return expr_wrapper<sub_tag, type_tuple<Expr, T>>(lhs, one<T>::value);
			}
		};
		template <class T, class Expr>
		struct Sub<one<T>, Expr, typename enable_if<!is_one<Expr>::value && !is_zero<Expr>::value>::type> {
			static constexpr auto sub(const one<T>&, const Expr& rhs) -> expr_wrapper<sub_tag, type_tuple<T, Expr>> {
				return expr_wrapper<sub_tag, type_tuple<T, Expr>>(one<T>::value, rhs);
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
		template <class Expr, class T>
		struct Mul<Expr, zero<T>> {
			static constexpr auto mul(const Expr& lhs, const zero<T>&) -> zero<T> { return zero<T>(); }
		};
		template <class T, class Expr>
		struct Mul<zero<T>, Expr, typename enable_if<!is_zero<Expr>::value>::type> {
			static constexpr auto mul(const zero<T>&, const Expr& rhs) -> zero<T> { return zero<T>(); }
		};
		template <class Expr, class T>
		struct Mul<Expr, one<T>, typename enable_if<!is_zero<Expr>::value>::type> {
			static constexpr auto mul(const Expr& lhs, const one<T>&) -> Expr { return lhs; }
		};
		template <class T, class Expr>
		struct Mul<one<T>, Expr, typename enable_if<!is_one<Expr>::value && !is_zero<Expr>::value>::type> {
			static constexpr auto mul(const one<T>&, const Expr& rhs) -> Expr { return rhs; }
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
		template <class Expr, class T>
		struct Div<Expr, zero<T>> {
			//呼び出されないはず
		};
		template <class T, class Expr>
		struct Div<zero<T>, Expr, typename enable_if<!is_zero<Expr>::value>::type> {
			static constexpr auto div(const zero<T>&, const Expr& rhs) -> zero<T> { return zero<T>; }
		};
		template <class Expr, class T>
		struct Div<Expr, one<T>, typename enable_if<!is_zero<Expr>::value>::type> {
			//呼び出されないはず
		};
		template <class T, class Expr>
		struct Div<one<T>, Expr, typename enable_if<!is_one<Expr>::value && !is_zero<Expr>::value>::type> {
			static constexpr auto div(const one<T>&, const Expr& rhs) -> expr_wrapper<div_tag, type_tuple<T, Expr>> {
				return expr_wrapper<div_tag, type_tuple<Expr, T>>(one<T>::value, rhs);
			}
		};
		template <class Expr1, class Expr2>
		constexpr auto div(const Expr1& lhs, const Expr2& rhs) { return Div<Expr1, Expr2>::div(lhs, rhs); }
	}

	//数式微分
	template <class T>
	struct Differential {
		static constexpr auto differential(const T& expr) {
			return diff_op::zero<T>();
		}
	};
	template <class T>
	struct Differential<expr_wrapper<none_tag, type_tuple<T>>> {
		static constexpr auto differential(const expr_wrapper<none_tag, type_tuple<T>>& expr) {
			return diff_op::one<T>();
		}
	};
	template <class Expr>
	struct Differential<expr_wrapper<add_tag, type_tuple<Expr>>> {
		static constexpr auto differential(const expr_wrapper<add_tag, type_tuple<Expr>>& expr) {
			return Differential<Expr>::differential(expr.x_m);
		}
	};
	template <class Expr>
	struct Differential<expr_wrapper<sub_tag, type_tuple<Expr>>> {
		static constexpr auto differential(const expr_wrapper<sub_tag, type_tuple<Expr>>& expr) {
			return -Differential<Expr>::differential(expr.x_m);
		}
	};
	template <class Expr1, class Expr2>
	struct Differential<expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>> {
		static constexpr auto differential(const expr_wrapper<add_tag, type_tuple<Expr1, Expr2>>& expr) {
			return diff_op::add(Differential<Expr1>::differential(expr.lhs_m), Differential<Expr2>::differential(expr.rhs_m));
		}
	};
	template <class Expr1, class Expr2>
	struct Differential<expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>> {
		static constexpr auto differential(const expr_wrapper<sub_tag, type_tuple<Expr1, Expr2>>& expr) {
			return diff_op::sub(Differential<Expr1>::differential(expr.lhs_m), Differential<Expr2>::differential(expr.rhs_m));
		}
	};
	template <class Expr1, class Expr2>
	struct Differential<expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>> {
		static constexpr auto differential(const expr_wrapper<mul_tag, type_tuple<Expr1, Expr2>>& expr) {
			return diff_op::add(diff_op::mul(Differential<Expr1>::differential(expr.lhs_m), expr.rhs_m)
				, diff_op::mul(expr.lhs_m, Differential<Expr2>::differential(expr.rhs_m)));
		}
	};
	template <class Expr1, class Expr2>
	struct Differential<expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>> {
		static constexpr auto differential(const expr_wrapper<div_tag, type_tuple<Expr1, Expr2>>& expr) {
			return diff_op::div(diff_op::sub(diff_op::mul(Differential<Expr1>::differential(expr.lhs_m), expr.rhs_m)
				, diff_op::mul(expr.lhs_m, Differential<Expr2>::differential(expr.rhs_m)))
				, diff_op::mul(expr.rhs_m, expr.rhs_m));
		}
	};
	template <class Expr1, class Expr2>
	struct Differential<expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>> {
		static constexpr auto differential(const expr_wrapper<subscript_tag, type_tuple<Expr1, Expr2>>& expr) {
			return Differential<Expr1>::differential(expr.x_m)[expr.index_m];
		}
	};
	template <class T>
	constexpr auto differential(const T& expr) { return Differential<T>::differential(expr); }
```

こんな風に使うことができる。

```C++
int main() {
    auto x = iml::expr_variable(10., 'x');
    //(3*x^4+5*x)/2
    auto y = (3 * x * x * x * x + 5 * x) / 2;
    //実際は6*x^3+5/2
    //((((3*x+3*x)*x+3*x*x)*x+3*x*x*x+5)*2)/(2*2)
    std::cout << iml::differential(y) << std::endl;

    return 0;
}
```

このように導関数が計算できていることと同時に、式の整理の重要性が理解できる。
というわけで完成。

# 終わりに
多分、実用上はテンプレートの深度が大きくなって使い物にならないような気もする。そもそも今回のテーマ自体が実用的じゃない。まぁC++の可能性を垣間見るための何かと思っていただければ。
あと、数式処理は門外漢の素人だからアルゴリズムに対して詳しいことはわからない。もしかしたらもっといい方法があるのかもしれない。プログラムも数学も所詮趣味だからそこまで深くやるつもりはないが。多分、それだけで一生終わる。
気が向いたら続きやるかも。やるとしたら、Expression templateで数学関数を扱うためのものとその場合の数式微分をやると思う。
といっても、今回のやつを少し拡張するだけでそこまで難しいことではないためやらないかもしれない。あくまでも気が向いたら程度。
