#ifndef IMATHLIB_H_UTILITY_TEMPLATE_ALGORITHM_HPP
#define IMATHLIB_H_UTILITY_TEMPLATE_ALGORITHM_HPP


#include "IMathLib/utility/utility.hpp"
#include "IMathLib/utility/tuple.hpp"

//テンプレート用のアルゴリズム


namespace iml {

	//テンプレート用の比較
	template <class T, T M, T N, bool = (M == N)>
	struct template_eq : std::true_type {};
	template <class T, T M, T N>
	struct template_eq<T, M, N, false> : std::false_type {};
	template <class T, T M, T N, bool = (M < N)>
	struct template_lt : std::true_type {};
	template <class T, T M, T N>
	struct template_lt<T, M, N, false> : std::false_type {};
	template <class T, T M, T N, bool = (M <= N)>
	struct template_lt_eq : std::true_type {};
	template <class T, T M, T N>
	struct template_lt_eq<T, M, N, false> : std::false_type {};
	template <class T, T M, T N, bool = (M > N)>
	struct template_gt : std::true_type {};
	template <class T, T M, T N>
	struct template_gt<T, M, N, false> : std::false_type {};
	template <class T, T M, T N, bool = (M >= N)>
	struct template_gt_eq : std::true_type {};
	template <class T, T M, T N>
	struct template_gt_eq<T, M, N, false> : std::false_type {};


	//テンプレート引数から最大値を得る
	template <class>
	struct template_max;
	template <class T, T First>
	struct template_max<index_tuple<T, First>> {
		static constexpr T value = First;
	};
	template <class T, T First, T Second, T... Indices>
	struct template_max<index_tuple<T, First, Second, Indices...>>
		: template_max<index_tuple<T, ((First < Second) ? Second : First), Indices...>> {};


	//テンプレート引数から最小値を得る
	template <class>
	struct template_min;
	template <class T, T First>
	struct template_min<index_tuple<T, First>> {
		static constexpr T value = First;
	};
	template <class T, T First, T Second, T... Indices>
	struct template_min<index_tuple<T, First, Second, Indices...>>
		: template_min<index_tuple<T, ((First > Second) ? Second : First), Indices...>> {};


	//条件を満たす要素を抽出してindex_tupleの構築
	template <bool, class T, class, class, template<T> class>
	struct template_find_if_impl2;
	template <class T, T First, T Second, T... Indices1, T... Indices2, template<T> class Predicate>
	struct template_find_if_impl2<true, T, index_tuple<T, First, Second, Indices1...>, index_tuple<T, Indices2...>, Predicate>
		: template_find_if_impl2<Predicate<Second>::value, T, index_tuple<T, Second, Indices1...>, index_tuple<T, Indices2..., First>, Predicate> {};
	template <class T, T First, T Second, T... Indices1, T... Indices2, template <T> class Predicate>
	struct template_find_if_impl2<false, T, index_tuple<T, First, Second, Indices1...>, index_tuple<T, Indices2...>, Predicate>
		: template_find_if_impl2<Predicate<Second>::value, T, index_tuple<T, Second, Indices1...>, index_tuple<T, Indices2...>, Predicate> {};
	template <class T, T First, T... Indices, template <T> class Predicate>
	struct template_find_if_impl2<true, T, index_tuple<T, First>, index_tuple<T, Indices...>, Predicate> {
		using type = index_tuple<T, Indices..., First>;
	};
	template <class T, T First, T... Indices, template <T> class Predicate>
	struct template_find_if_impl2<false, T, index_tuple<T, First>, index_tuple<T, Indices...>, Predicate> {
		using type = index_tuple<T, Indices...>;
	};
	template <class T, class, template <T> class>
	struct template_find_if_impl1;
	template <class T, T First, T... Indices, template <T> class Predicate>
	struct template_find_if_impl1<T, index_tuple<T, First, Indices...>, Predicate>
		: template_find_if_impl2<Predicate<First>::value, T, index_tuple<T, First, Indices...>, index_tuple<T>, Predicate> {};
	template <class T, template <T> class Predicate>
	struct template_find_if_impl1<T, index_tuple<T>, Predicate> {
		using type = index_tuple<T>;
	};
	template <class IndexTuple, template <typename IndexTuple::type> class Predicate>
	using template_find_if = template_find_if_impl1<typename IndexTuple::type, IndexTuple, Predicate>;


	//テンプレートのクイックソート
	template <bool, class>
	struct template_sort_impl;
	template <class T, T First, T... Indices>
	struct template_sort_impl<true, index_tuple<T, First, Indices...>> {
	private:
		template <T N>
		using le_pivot_wrapper = template_lt_eq<T, N, First>;
		template <T N>
		using gt_pivot_wrapper = template_gt<T, N, First>;

		using rest = index_tuple<T, Indices...>;
		using le_part = typename template_find_if<rest, le_pivot_wrapper>::type;
		using gt_part = typename template_find_if<rest, gt_pivot_wrapper>::type;
		using sorted_le_part = typename template_sort_impl<(le_part::value >= 2), le_part>::type;
		using sorted_gt_part = typename template_sort_impl<(gt_part::value >= 2), gt_part>::type;
	public:
		//ピボットを間に挟んで構築
		using type = cat_index_tuple_t<sorted_le_part, index_tuple<T, First>, sorted_gt_part>;
	};
	template <class T, T... Indices>
	struct template_sort_impl <false, index_tuple<T, Indices...>> {
		using type = index_tuple<T, Indices...>;
	};
	template <class>
	struct template_sort;
	template <class T, T... Indices>
	struct template_sort<index_tuple<T, Indices...>>
		: template_sort_impl<(sizeof...(Indices) >= 2), index_tuple<T, Indices...>> {};

}


#endif