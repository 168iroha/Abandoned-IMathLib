#ifndef IMATHLIB_H_MATH_LINER_ALGEBRA_MATRIX_HPP
#define IMATHLIB_H_MATH_LINER_ALGEBRA_MATRIX_HPP

#include "IMathLib/math/liner_algebra/vector.hpp"


namespace iml {

	template <class, size_t, size_t>
	class matrix;


	//行列型のパラメータ
	template <class T, size_t M, size_t N, class... Types>
	using matrix_parameter = type_parameter<matrix<T, M, N>, type_tuple<Types...>>;
	template <class T, size_t M, size_t N, class... Params>
	struct type_parameter<matrix<T, M, N>, type_tuple<type_parameter<T, Params>...>> {
		static_assert(M * N == type_tuple<type_parameter<T, Params>...>::value, "There must be M*N parameters.");

		using type = matrix<T, M, N>;
		static constexpr type value = type(type_parameter<T, Params>::value...);

		template <class = std::enable_if_t<is_exist_additive_inverse_v<T>>>
		auto operator-() const { return matrix_parameter<T, M, N, decltype(-type_parameter<T, Params>())...>(); }
		auto operator+() const { return *this; }

		template <class T, T Val, class = std::enable_if_t<(Val >= 0) && (Val < M)>>
		auto operator[](int_parameter<T, Val>) const {
			using array_type = array_parameter<T, index_tuple<size_t, M, N>, type_parameter<T, Params>...>;
			return array_type()[int_parameter<T, Val>];
		}
	};


	// 行列におけるスカラー演算と標準演算の十分条件の定義
	// 乗算
	template <class T1, class T2, size_t M, size_t N>
	struct is_lscalar_operation_impl2<is_multipliable, T1, matrix<T2, M, N>> : std::bool_constant<is_multipliable_v<T1, T2>> {};
	template <class T1, size_t M, size_t N, class T2>
	struct is_rscalar_operation_impl2<is_multipliable, matrix<T1, M, N>, T2> : std::bool_constant<is_multipliable_v<T1, T2>> {};
	template <class T1, class T2, size_t M, size_t N, size_t L>
	struct is_standard_operation_impl2<is_multipliable, matrix<T1, M, L>, matrix<T2, L, N>> : std::bool_constant<is_magma<mul_result_t<T1, T2>>::add_value> {};
	template <class T1, class T2, size_t M, size_t N>
	struct is_standard_operation_impl2<is_multipliable, matrix<T1, M, N>, vector<T2, N>> : std::bool_constant<is_magma<mul_result_t<T1, T2>>::add_value> {};
	// 乗算代入
	template <class T1, size_t M, size_t N, class T2>
	struct is_rscalar_operation_impl2<is_mul_assignable, matrix<T1, M, N>, T2> : std::bool_constant<is_high_rank_math_type_v<mul_result_t<T1, T2>, T1>> {};
	template <class T1, size_t M, size_t N, class T2>
	struct is_standard_operation_impl2<is_mul_assignable, matrix<T1, M, N>, matrix<T2, M, N>> : std::bool_constant<(M == N) && is_high_rank_math_type_v<mul_result_t<T1, T2>, T1> && is_magma<mul_result_t<T1, T2>>::add_value> {};


	template <class, size_t M, size_t N, class = index_range_t<size_t, 0, (M * N)>>
	class matrix_base;
	template <class T, size_t M, size_t N, size_t... Indices>
	class matrix_base<T, M, N, index_tuple<size_t, Indices...>> {
		template <class, size_t, size_t> friend class matrix;
		template <class, size_t, size_t, class> friend class matrix_base;
	protected:
		T x_m[M][N];
	public:
		constexpr matrix_base() : x_m{} {}
		template <class... UTypes, class = std::enable_if_t<(sizeof...(UTypes) == M * N) && is_high_rank_math_type_v<common_math_type_t<UTypes...>, T>>>
		constexpr matrix_base(const UTypes&... args) : x_m{ T(args)... } {}
		template <class U, class = std::enable_if_t<is_high_rank_math_type_v<U, T>>>
		constexpr matrix_base(const matrix<U, M, N>& ma) : x_m{ ma.x_m[0][Indices]... } {}
		template <class U, class = std::enable_if_t<is_high_rank_math_type_v<U, T>>>
		constexpr matrix_base(matrix<U, M, N>&& ma) : x_m{ ma.x_m[0][Indices]... } {}

		//単項演算
		template <class = std::enable_if_t<is_exist_additive_inverse_v<T>>>
		constexpr matrix<T, M, N> operator-() const { return matrix<T, M, N>(-this->x_m[0][Indices]...); }
		constexpr matrix<T, M, N> operator+() const { return matrix<T, M, N>(*this); }

		//2項演算(多重定義防止にスカラー型でない方をTとして扱い，そうでないならT1 = Tとして扱う)
		template <class T2, class = std::enable_if_t<is_addable_v<T, T2>>>
		friend constexpr auto operator+(const matrix<T, M, N>& lhs, const matrix<T2, M, N>& rhs) {
			return matrix<add_result_t<T, T2>, M, N>((lhs[0][Indices] + rhs[0][Indices])...);
		}
		template <class T2, class... Types1, class... Types2, class = std::enable_if_t<is_addable_v<T, T2>>>
		friend auto operator+(matrix_parameter<T, M, N, Types1...>, matrix_parameter<T2, M, N, Types2...>) {
			return matrix_parameter<add_result_t<T, T2>, M, N, decltype(Types1() + Types2())...>();
		}
		template <class T2, class = std::enable_if_t<is_subtractable_v<T, T2>>>
		friend constexpr auto operator-(const matrix<T, M, N>& lhs, const matrix<T2, M, N>& rhs) {
			return matrix<sub_result_t<T, T2>, M, N>((lhs[0][Indices] - rhs[0][Indices])...);
		}
		template <class T2, class... Types1, class... Types2, class = std::enable_if_t<is_subtractable_v<T, T2>>>
		friend auto operator-(matrix_parameter<T, M, N, Types1...>, matrix_parameter<T2, M, N, Types2...>) {
			return matrix_parameter<sub_result_t<T, T2>, M, N, decltype(Types1() - Types2())...>();
		}
		template <class T2, class = std::enable_if_t<is_rscalar_operation_v<is_multipliable, matrix<T, M, N>, T2>>>
		friend constexpr auto operator*(const matrix<T, M, N>& lhs, const T2& rhs) {
			return matrix<mul_result_t<T, T2>, M, N>((lhs[0][Indices] * rhs)...);
		}
		template <class T2, class... Types, class Param, class = std::enable_if_t<is_rscalar_operation_v<is_multipliable, matrix<T, M, N>, T2>>>
		friend auto operator*(matrix_parameter<T, M, N, Types...>, type_parameter<T2, Param> rhs) {
			return matrix_parameter<mul_result_t<T, T2>, M, N, decltype(Types() * rhs)...>();
		}
		template <class T1, class = std::enable_if_t<is_lscalar_operation_v<is_multipliable, T1, matrix<T, M, N>>>>
		friend constexpr auto operator*(const T1& lhs, const matrix<T, M, N>& rhs) {
			return matrix<mul_result_t<T1, T>, M, N>((lhs * rhs[0][Indices])...);
		}
		template <class T1, class Param, class... Types, class = std::enable_if_t<is_lscalar_operation_v<is_multipliable, T1, matrix<T, M, N>>>>
		friend auto operator*(type_parameter<T1, Param> lhs, matrix_parameter<T, M, N, Types...>) {
			return matrix_parameter<mul_result_t<T1, T>, M, N, decltype(lhs * Types())...>();
		}
		template <class T2, class = std::enable_if_t<is_divisible_v<T, T2>>>
		friend constexpr auto operator/(const matrix<T, M, N>& lhs, const T2& rhs) {
			return matrix<div_result_t<T, T2>, M, N>((lhs[0][Indices] / rhs)...);
		}
		template <class T2, class... Types, class Param, class = std::enable_if_t<is_divisible_v<T, T2>>>
		friend auto operator/(matrix_parameter<T, M, N, Types...>, type_parameter<T2, Param> rhs) {
			return matrix_parameter<div_result_t<T, T2>, M, N, decltype(Types() / rhs)...>();
		}
	};

	//行列型
	template <class T, size_t M, size_t N>
	class matrix : public matrix_base<T, M, N> {
		//MとNは0より大きくなければならない
		static_assert(M > 0 && N > 0, "M and N must be greater than 0.");

		template <class, size_t, size_t> friend class matrix;
		template <class, size_t, size_t, class> friend class matrix_base;
	public:
		using matrix_base<T, M, N>::matrix_base;

		using basis_type = T;
		using iterator = linear_iterator<T>;
		using const_iterator = linear_iterator<const T>;

		template<class Other>
		struct rebind {
			using other = matrix<Other, M, N>;
		};

		constexpr iterator begin() noexcept { return iterator(x_m[0]); }
		constexpr const_iterator begin() const noexcept { return const_iterator(x_m[0]); }
		constexpr iterator end() noexcept { return iterator(x_m[0] + M * N); }
		constexpr const_iterator end() const noexcept { return const_iterator(x_m[0] + M * N); }

		//単項演算
		using matrix_base<T, M, N>::operator-;
		using matrix_base<T, M, N>::operator+;
		//代入演算
		matrix& operator=(const matrix& ma) {
			if (this != std::addressof(ma)) for (size_t i = 0; i < M * N; ++i) this->x_m[0][i] = ma.x_m[0][i];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_high_rank_math_type_v<U, T>>>
		matrix& operator=(const matrix<U, M, N>& ma) {
			for (size_t i = 0; i < M * N; ++i) this->x_m[0][i] = ma.x_m[0][i];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_operation<T, U, T>::add_value>>
		matrix& operator+=(const matrix<U, M, N>& ma) {
			for (size_t i = 0; i < M * N; ++i) this->x_m[0][i] += ma.x_m[0][i];
			return *this;
		}
		template <class U, class = std::enable_if_t<is_operation<T, U, T>::sub_value>>
		matrix& operator-=(const matrix<U, M, N>& ma) {
			for (size_t i = 0; i < M * N; ++i) this->x_m[0][i] -= ma.x_m[0][i];
			return *this;
		}
		//内積
		template <class U, class = std::enable_if_t<is_standard_operation_v<is_mul_assignable, matrix, matrix<U, M, N>>>>
		matrix& operator*=(const matrix<U, M, N>& ma) {
			matrix<T, M, M> temp{};
			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < M; ++j)
					for (size_t k = 0; k < M; ++k)
						temp[i][j] += this->x_m[i][k] * ma.x_m[k][j];
			return *this = temp;
		}
		template <class U, class = std::enable_if_t<is_rscalar_operation_v<is_mul_assignable, matrix, U>>>
		matrix& operator*=(const U& k) {
			for (size_t i = 0; i < M * N; ++i) this->x_m[0][i] *= k;
			return *this;
		}
		template <class U, class = std::enable_if_t<is_operation<T, U, T>::div_value>>
		matrix& operator/=(const U& k) {
			for (size_t i = 0; i < M * N; ++i) this->x_m[0][i] /= k;
			return *this;
		}

		//添え字演算
		const constexpr auto& operator[](size_t index) const { return this->x_m[index]; }
		constexpr auto& operator[](size_t index) { return this->x_m[index]; }

		//ストリーム出力
		friend std::ostream& operator<<(std::ostream& os, const matrix& ma) {
			//-1は改行対策
			for (size_t i = 0; i < M - 1; ++i) {
				for (size_t j = 0; j < N; ++j)
					os << ma.x_m[i][j] << ' ';
				os << std::endl;
			}
			for (size_t j = 0; j < N; ++j)
				os << ma.x_m[M - 1][j] << ' ';
			return os;
		}
		friend std::wostream& operator<<(std::wostream& os, const matrix& ma) {
			//-1は改行対策
			for (size_t i = 0; i < M - 1; ++i) {
				for (size_t j = 0; j < N; ++j)
					os << ma.x_m[i][j] << L' ';
				os << std::endl;
			}
			for (size_t j = 0; j < N; ++j)
				os << ma.x_m[M - 1][j] << L' ';
			return os;
		}

		template <class T>
		constexpr linear_input<iterator> operator<<(const T& value) {
			iterator itr = this->begin();
			*itr = value; ++itr;
			return linear_input<iterator>(itr, this->end());
		}
	};


	//内積
	template <class T1, class T2, size_t M, size_t N, size_t L, class = std::enable_if_t<is_standard_operation_v<is_multipliable, matrix<T1, M, L>, matrix<T2, L, N>>>>
	inline constexpr auto operator*(const matrix<T1, M, L>& lhs, const matrix<T2, L, N>& rhs) {
		matrix<mul_result_t<T1, T2>, M, N> temp{};
		for (size_t i = 0; i < M; ++i)
			for (size_t j = 0; j < N; ++j)
				for (size_t k = 0; k < L; ++k)
					temp[i][j] += lhs[i][k] * rhs[k][j];
		return temp;
	}
	namespace tp {
		//行列型のパラメータの乗算の計算
		template <size_t, class, class, class>
		struct matrix_parameter_mul_impl2;
		template <size_t RC, class T1, class T2, size_t M, size_t L, size_t N, class... Types1, class... Types2, size_t... Indices>
		struct matrix_parameter_mul_impl2<RC, matrix_parameter<T1, M, L, Types1...>, matrix_parameter<T2, L, N, Types2...>, index_tuple<size_t, Indices...>> {
			//メモリの配置が temp[0][0], temp[0][1], ・・・, temp[1][0], temp[1][1], ・・・となることから算出
			static constexpr size_t row = RC % M;		//行
			static constexpr size_t col = RC / M;		//列
			//内積をとる
			using type = decltype(tp::sum((matrix_parameter<T1, M, L, Types1...>()[int_parameter<size_t, row>()][int_parameter<size_t, Indices>()]
				* matrix_parameter<T2, L, N, Types2...>()[int_parameter<size_t, Indices>()][int_parameter<size_t, col>()])...));
		};
		template <class, class, class>
		struct matrix_parameter_mul_impl;
		template <class T1, class T2, size_t M, size_t L, size_t N, class... Types1, class... Types2, size_t... Indices>
		struct matrix_parameter_mul_impl<matrix_parameter<T1, M, L, Types1...>, matrix_parameter<T2, L, N, Types2...>, index_tuple<size_t, Indices...>> {
			//各成分づつ計算
			using type = matrix_parameter<mul_result_t<T1, T2>, M, N
				, typename matrix_parameter_mul_impl2<Indices, matrix_parameter<T1, M, L, Types1...>, matrix_parameter<T2, L, N, Types2...>, index_range_t<size_t, 0, L>>::type...>;
		};
		template <class, class>
		struct matrix_parameter_mul;
		template <class T1, class T2, size_t M, size_t L, size_t N, class... Types1, class... Types2>
		struct matrix_parameter_mul<matrix_parameter<T1, M, L, Types1...>, matrix_parameter<T2, L, N, Types2...>>
			//M*N行列の全要素を走査するためのインデックスの生成
			: matrix_parameter_mul_impl<matrix_parameter<T1, M, L, Types1...>, matrix_parameter<T2, L, N, Types2...>, index_range_t<size_t, 0, M * N>> {};
	}
	template <class T1, class T2, size_t M, size_t N, size_t L, class... Types1, class... Types2, class = std::enable_if_t<is_standard_operation_v<is_multipliable, matrix<T1, M, L>, matrix<T2, L, N>>>>
	inline auto operator*(matrix_parameter<T1, M, L, Types1...>, matrix_parameter<T2, L, N, Types2...>) {
		return typename tp::matrix_parameter_mul<matrix_parameter<T1, M, L, Types1...>, matrix_parameter<T2, L, N, Types2...>>::type()\
	}
	template <class T1, class T2, size_t M, size_t N, class = std::enable_if_t<is_standard_operation_v<is_multipliable, matrix<T1, M, N>, vector<T2, N>>>>
	inline constexpr auto operator*(const matrix<T1, M, N>& lhs, const vector<T2, N>& rhs) {
		vector<mul_result_t<T1, T2>, M> temp{};
		for (size_t i = 0; i < M; ++i) for (size_t j = 0; j < N; ++j) temp[i] += lhs[i][j] * rhs[j];
		return temp;
	}
	namespace tp {
		//内積の補助(Indices1 : ベクトルの各要素に対応するシーケンス, Indices2 : 内積をとるためのシーケンス)
		template <class, class, class, class>
		struct matrix_vector_parameter_mul_impl;
		template <class T1, class... Types1, class T2, class... Types2, size_t M, size_t N, size_t... Indices1, size_t... Indices2>
		struct matrix_vector_parameter_mul_impl<matrix_parameter<T1, M, N, Types1...>, vector_parameter<T2, N, Types2...>, index_tuple<size_t, Indices1...>, index_tuple<size_t, Indices2...>> {
			using type = vector_parameter<mul_result_t<T1, T2>, M
				, decltype(tp::sum(matrix_parameter<T1, M, N, Types1...>()[int_parameter<size_t, Indices1>()][int_parameter<size_t, Indices2>()]
					* vector_parameter<T2, N, Types2...>()[int_parameter<size_t, Indices2>()]...))...>;
		};
	}
	template <class T1, class... Types1, class T2, class... Types2, size_t M, size_t N, class = std::enable_if_t<is_standard_operation_v<is_multipliable, matrix<T1, M, N>, vector<T2, N>>>>
	inline auto operator*(matrix_parameter<T1, M, N, Types1...> lhs, vector_parameter<T2, N, Types2...> rhs) {
		return typename tp::matrix_vector_parameter_mul_impl<matrix_parameter<T1, M, N, Types1...>, vector_parameter<T2, N, Types2...>
			, index_range_t<size_t, 0, M>, index_range_t<size_t, 0, N>>::type();
	}


	//比較演算
	template <class T1, class T2, size_t M, size_t N, class = std::enable_if_t<is_comparable_v<T1, T2>>>
	inline constexpr bool operator==(const matrix<T1, M, N>& lhs, const matrix<T2, M, N>& rhs) {
		for (size_t i = 0; i < M * N; ++i) if (lhs[0][i] != rhs[0][i]) return false;
		return true;
	}
	template <class T1, class T2, size_t M, size_t N, class... Types1, class... Types2, class = std::enable_if_t<is_comparable_v<T1, T2>>>
	inline auto operator==(matrix_parameter<T1, M, N, Types1...>, matrix_parameter<T2, M, N, Types2...>) {
		constexpr bool temp = tp::sum((Types1() == Types2())...)::value == M * N;
		return int_parameter<bool, temp>();
	}
	template <class T1, class T2, size_t M, size_t N, class = std::enable_if_t<is_comparable_v<T1, T2>>>
	inline constexpr bool operator!=(const matrix<T1, M, N>& lhs, const matrix<T2, M, N>& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, size_t M, size_t N, class... Types1, class... Types2, class = std::enable_if_t<is_comparable_v<T1, T2>>>
	inline auto operator!=(matrix_parameter<T1, M, N, Types1...> rhs, matrix_parameter<T2, M, N, Types2...> lhs) { return !(lhs == rhs); }


	//行列の判定
	template <class T>
	struct is_matrix_impl : std::false_type {};
	template <class T, size_t M, size_t N>
	struct is_matrix_impl<matrix<T, M, N>> : std::true_type {};
	template <class T>
	struct is_matrix : is_matrix_impl<std::remove_cv_t<T>> {};
	template <class T>
	inline constexpr bool is_matrix_v = is_matrix<T>::value;


	//行列の除去
	template <class T>
	struct remove_matrix {
		using type = T;
	};
	template <class T, size_t M, size_t N>
	struct remove_matrix<matrix<T, M, N>> {
		using type = T;
	};
	template <class T>
	using remove_matrix_t = typename remove_matrix<T>::type;


	//各種行列の定義
	template <class T>
	using matrix2 = matrix<T, 2, 2>;
	template <class T>
	using matrix3 = matrix<T, 3, 3>;
	template <class T>
	using matrix4 = matrix<T, 4, 4>;


	template <class From, class To, size_t M, size_t N>
	struct is_high_rank_math_type<matrix<From, M, N>, matrix<To, M, N>> : is_high_rank_math_type<From, To> {};
	template <class From, class To, size_t N>
	struct is_high_rank_math_type<matrix<From, N, 1>, vector<To, N>> : is_high_rank_math_type<From, To> {};
	template <class From, class To, size_t N>
	struct is_high_rank_math_type<vector<From, N>, matrix<To, N, 1>> : is_high_rank_math_type<From, To> {};


	//加法の特性
	template <class T, size_t M, size_t N>
	struct addition_traits<matrix<T, M, N>> {
		//単位元(零行列)
		template <class = std::enable_if_t<is_exist_additive_identity_v<T>>>
		static constexpr matrix<T, M, N> identity_element() { return matrix<T, M, N>(); }
		//結合律
		static constexpr bool associative_value = addition_traits<T>::associative_value;
		//消約律
		static constexpr bool cancellative_value = addition_traits<T>::cancellative_value;
		//可換律
		static constexpr bool commutative_value = addition_traits<T>::commutative_value;
	};
	//乗法の特性
	template <class T, size_t M, size_t N>
	struct multiplication_traits<matrix<T, M, N>> {
		//単位元(単位行列)
		template <class = std::enable_if_t<is_exist_multiplicative_identity_v<T> && (M == N)>>
		static constexpr matrix<T, M, M> identity_element() {
			matrix<T, M, M> temp{};
			for (size_t i = 0; i < M; ++i) temp[i][i] = multiplication_traits<T>::identity_element();
			return temp;
		}
		//吸収元(零行列)
		template <class = std::enable_if_t<is_exist_absorbing_element_v<T> && (M == N)>>
		static constexpr matrix<T, M, M> absorbing_element() { return matrix<T, M, M>(); }
		//結合律
		static constexpr bool associative_value = multiplication_traits<T>::associative_value;
		//消約律
		static constexpr bool cancellative_value = false;
		//可換律
		static constexpr bool commutative_value = false;
		//分配律
		static constexpr bool distributive_value = multiplication_traits<T>::distributive_value;
	};


	//逆元が存在するならば逆元の取得(存在しない場合は例外を出す)
	template <class T, size_t M, size_t N>
	struct Inverse_element<matrix<T, M, N>> {
		template <size_t... Indices>
		static constexpr matrix<T, M, N> _additive_inverse_impl_(const matrix<T, M, N>& x, index_tuple<size_t, Indices...>) {
			return matrix<T, M, N>(additive_inverse(x[Indices])...);
		}
		static constexpr matrix<T, M, N> _additive_inverse_(const matrix<T, M, N>& x) {
			return _additive_inverse_impl_(x, index_range_t<size_t, 0, M * N>());
		}

		//Tに乗法逆元が存在する場合の乗法逆元
		static constexpr matrix<T, M, M> _multiplicative_inverse_impl_(matrix<T, M, M> x, std::true_type) {
			matrix<T, M, M> result(multiplication_traits<matrix<T, M, M>>::identity_element());

			//xを単位行列になるように操作する(ガウスの消去法)
			for (size_t i = 0; i < M; ++i) {
				//対角成分が0であれば現在走査中の行より下段の0でない行と交換する
				if (x[i][i] == 0) {
					size_t j = i + 1;
					for (; j < M; ++j) {
						//0でない項が存在するとき行を交換
						if (x[j][i] != 0) {
							//for (size_t k = i; k < M; ++k) swap(x[i][k], x[j][k]);
							//for (size_t k = 0; k < M; ++k) swap(result[i][k], result[j][k]);
							size_t k = 0;
							for (; k < i; ++k) swap(result[i][k], result[j][k]);
							for (; k < M; ++k) { swap(x[i][k], x[j][k]); swap(result[i][k], result[j][k]); }
							break;
						}
					}
					//交換できなかった時は正則ではない
					if (j == M) return matrix<T, M, M>();
				}
				//i行を対角成で正規化(無駄を省くために対角成分は正規化しない)
				//for (size_t j = i + 1; j < M; ++j) x[i][j] /= x[i][i];
				//for (size_t j = 0; j < M; ++j) result[i][j] /= x[i][i];
				size_t l = 0;
				for (; l < i + 1; ++l) result[i][l] /= x[i][i];
				for (; l < M; ++l) { x[i][l] /= x[i][i]; result[i][l] /= x[i][i]; }
				//第i列の対角成分より上を0にする
				for (size_t j = 0; j < i; ++j) {
					if (x[j][i] != 0) {
						//i列目から順に引いていく(i列より左は全て0であるため計算しない)
						//実際、i列の成分は以後0となるため計算する必要がない
						//for (size_t k = i + 1; k < M; ++k) x[j][k] -= x[i][k] * x[j][i];
						//for (size_t k = 0; k < M; ++k) result[j][k] -= result[i][k] * x[j][i];
						size_t k = 0;
						for (; k < i + 1; ++k) result[j][k] -= result[i][k] * x[j][i];
						for (; k < M; ++k) { x[j][k] -= x[i][k] * x[j][i]; result[j][k] -= result[i][k] * x[j][i]; }
					}
				}
				//第i列の対角成分より下を0にする
				for (size_t j = i + 1; j < M; ++j) {
					if (x[j][i] != 0) {
						//i列目から順に引いていく(i列より左は全て0であるため計算しない)
						//実際、i列の成分は以後0となるため計算する必要がない
						//for (size_t k = i + 1; k < M; ++k) x[j][k] -= x[i][k] * x[j][i];
						//for (size_t k = 0; k < M; ++k) result[j][k] -= result[i][k] * x[j][i];
						size_t k = 0;
						for (; k < i + 1; ++k) result[j][k] -= result[i][k] * x[j][i];
						for (; k < M; ++k) { x[j][k] -= x[i][k] * x[j][i]; result[j][k] -= result[i][k] * x[j][i]; }
					}
				}
			}
			return result;
		}
		//Tの乗法逆元が存在しない場合の乗法逆元
		static constexpr matrix<T, M, M> _multiplicative_inverse_impl_(matrix<T, M, M> x, std::false_type) {
			matrix<T, M, M> result(multiplication_traits<matrix<T, M, M>>::identity_element());

			//xを単位行列になるように操作する(ガウスの消去法)
			for (size_t i = 0; i < M; ++i) {
				//対角成分が0であれば現在走査中の行より下段の0でない行と交換する
				if (x[i][i] == 0) {
					size_t j = i + 1;
					for (; j < M; ++j) {
						//0でない項が存在するとき行を交換
						if (x[j][i] != 0) {
							//for (size_t k = i; k < M; ++k) swap(x[i][k], x[j][k]);
							//for (size_t k = 0; k < M; ++k) swap(result[i][k], result[j][k]);
							size_t k = 0;
							for (; k < i; ++k) swap(result[i][k], result[j][k]);
							for (; k < M; ++k) { swap(x[i][k], x[j][k]); swap(result[i][k], result[j][k]); }
							break;
						}
					}
					//交換できなかった時は正則ではない
					if (j == M) return matrix<T, M, M>();
				}
				//i行を対角成で正規化(無駄を省くために対角成分は正規化しない)
				T temp = multiplicative_inverse(x[i][i]);
				//for (size_t j = i + 1; j < M; ++j) x[i][j] *= temp;
				//for (size_t j = 0; j < M; ++j) result[i][j] *= temp;
				size_t l = 0;
				for (; l < i + 1; ++l) result[i][l] *= temp;
				for (; l < M; ++l) { x[i][l] *= temp; result[i][l] *= temp; }
				//第i列の対角成分より上を0にする
				for (size_t j = 0; j < i; ++j) {
					if (x[j][i] != 0) {
						//i列目から順に引いていく(i列より左は全て0であるため計算しない)
						//実際、i列の成分は以後0となるため計算する必要がない
						//for (size_t k = i + 1; k < M; ++k) x[j][k] -= x[i][k] * x[j][i];
						//for (size_t k = 0; k < M; ++k) result[j][k] -= result[i][k] * x[j][i];
						size_t k = 0;
						for (; k < i + 1; ++k) result[j][k] -= result[i][k] * x[j][i];
						for (; k < M; ++k) { x[j][k] -= x[i][k] * x[j][i]; result[j][k] -= result[i][k] * x[j][i]; }
					}
				}
				//第i列の対角成分より下を0にする
				for (size_t j = i + 1; j < M; ++j) {
					if (x[j][i] != 0) {
						//i列目から順に引いていく(i列より左は全て0であるため計算しない)
						//実際、i列の成分は以後0となるため計算する必要がない
						//for (size_t k = i + 1; k < M; ++k) x[j][k] -= x[i][k] * x[j][i];
						//for (size_t k = 0; k < M; ++k) result[j][k] -= result[i][k] * x[j][i];
						size_t k = 0;
						for (; k < i + 1; ++k) result[j][k] -= result[i][k] * x[j][i];
						for (; k < M; ++k) { x[j][k] -= x[i][k] * x[j][i]; result[j][k] -= result[i][k] * x[j][i]; }
					}
				}
			}
			return result;
		}
		template <class = std::enable_if_t<M == N>>
		static constexpr matrix<T, M, M> _multiplicative_inverse_(const matrix<T, M, M>& x) {
			return _multiplicative_inverse_impl_(x, std::bool_constant<is_exist_multiplicative_inverse_v<T>>());
		}
	};


	//誤差評価
	template <class T, size_t M, size_t N>
	struct Error_evaluation<matrix<T, M, N>> {
		static constexpr matrix<T, M, N> epsilon() {
			matrix<T, M, N> temp{};
			for (size_t i = 0; i < M * N; ++i) temp[0][i] = Error_evaluation<T>::epsilon();
			return temp;
		}
		static constexpr bool _error_evaluation_(const matrix<T, M, N>& x1, const matrix<T, M, N>& x2) {
			for (size_t i = 0; i < M * N; ++i) if (!error_evaluation(x1[0][i], x2[0][i])) return false;
			return true;
		}
	};
}

#endif