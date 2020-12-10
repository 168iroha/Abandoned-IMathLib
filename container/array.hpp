#ifndef IMATHLIB_CONTAINER_ARRAY_HPP
#define IMATHLIB_CONTAINER_ARRAY_HPP

#include "IMathLib/utility/algorithm.hpp"
#include "IMathLib/container/allocator.hpp"
#include "IMathLib/utility/smart_ptr.hpp"


//  配列リスト
namespace iml {

	//  配列リストのイテレータ
	template <class T>
	struct array_iterator {
		T*	current_m;

		using iterator_category = random_access_iterator_tag;
		using value_type = T;
		using difference_type = ptrdiff_t;
		using pointer = T*;
		using reference = T&;

		template <class Other>
		struct rebind {
			using other = array_iterator<Other>;
		};
		template <class Other>
		using rebind_t = array_iterator<Other>;

		constexpr array_iterator() : current_m(nullptr) {}
		template <class U>
		constexpr array_iterator(U* itr) : current_m(static_cast<T*>(itr)) {}
		constexpr array_iterator(const array_iterator& itr) : current_m(itr.current_m) {}
		template <class U>
		constexpr array_iterator(const array_iterator<U>& itr) : current_m(reinterpret_cast<T*>(itr.current_m)) {}

		constexpr reference operator*() const { return *current_m; }
		constexpr pointer operator->() const { return current_m; }
		constexpr array_iterator& operator++() { ++current_m; return *this; }
		constexpr array_iterator operator++(int) { array_iterator temp = *this; ++*this; return temp; }
		constexpr array_iterator& operator--() { --current_m; return *this; }
		constexpr array_iterator operator--(int) { array_iterator temp = *this; --*this; return temp; }

		constexpr array_iterator operator+(difference_type n) const { return array_iterator(current_m + n); }
		constexpr array_iterator operator-(difference_type n) const { return array_iterator(current_m - n); }
		constexpr difference_type operator-(const array_iterator& itr) const { return current_m - itr.current_m; }

		array_iterator& operator+=(difference_type n) { current_m += n; return *this; }
		array_iterator& operator-=(difference_type n) { current_m -= n; return *this; }

		constexpr reference operator[](difference_type n) const { return *next(*this, n); }

		constexpr bool operator==(const array_iterator& itr) const { return current_m == itr.current_m; }
		constexpr bool operator!=(const array_iterator& itr) const { return current_m != itr.current_m; }
		constexpr bool operator<(const array_iterator& itr) const { return current_m < itr.current_m; }
		constexpr bool operator>(const array_iterator& itr) const { return current_m < itr.current_m; }
		constexpr bool operator<=(const array_iterator& itr) const { return current_m <= itr.current_m; }
		constexpr bool operator>=(const array_iterator& itr) const { return current_m >= itr.current_m; }
	};


	//  静的配列リスト
	template <class T, size_t N>
	struct static_array {
		T elements[N];

		//  集成体による初期化をする

		using value_type = T;
		using reference = T & ;
		using const_reference = const T & ;
		using iterator = array_iterator<T>;
		using const_iterator = typename iterator::rebind_t<const T>;

		//  イテレータの取得
		constexpr iterator begin() noexcept { return iterator(elements); }
		constexpr const_iterator begin() const noexcept { return const_iterator(elements); }
		constexpr iterator end() noexcept { return iterator(elements + N); }
		constexpr const_iterator end() const noexcept { return const_iterator(elements + N); }

		//  同一要素で埋める
		void fill(const T& n) { iml::fill(begin(), end(), n); }
		//  使用中のサイズの取得
		constexpr size_t size() const { return N; }
		//  コンテナが空かの判定
		[[nodiscard]] constexpr bool empty() const noexcept { return N == 0; }
		// 内部イテレータ
		template <class F>
		constexpr F for_each(F f) const { return iml::for_each(begin(), end(), f); }

		static_array& operator=(const static_array& s) {
			copy_order(begin(), s.begin(), s.end());
			return *this;
		}

		//  コンテナの中身の入れ替え
		constexpr void swap(static_array& c) {
			for (auto itr1 = begin(), itr2 = c.begin(); itr1 != end(); ++itr1, ++itr2) iml::swap(*itr1, *itr2);
		}

		constexpr reference operator[](size_t index) noexcept { return elements[index]; }
		constexpr const_reference operator[](size_t index) const noexcept { return elements[index]; }
	};
	template <class T, class U, size_t N>
	constexpr bool operator==(const static_array<T, N>& a1, const static_array<U, N>& a2) {
		for (size_t i = 0; i < N; ++i) if (a1[i] != a1[i]) return false;
		return true;
	}
	template <class T, class U, size_t N>
	constexpr bool operator!=(const static_array<T, N>& a1, const static_array<U, N>& a2) {
		return !(a1 == a2);
	}

	//  動的配列リスト(厳密にはアロケータによる配列リスト)
	template <class T, class Allocator = allocator<T>>
	class dynamic_array {
	public:
		using value_type = T;
		using reference = T & ;
		using const_reference = const T &;
		using iterator = array_iterator<T>;
		using const_iterator = array_iterator<const T>;
		using allocator_type = Allocator;
		using size_type = typename allocator_traits<Allocator>::size_type;
	private:
		T*			p_m;
		size_type	size_m;				//  確保したメモリサイズ
		size_type	use_size_m;			//  確保したメモリのうち使用中のサイズ
		Allocator	alloc_m;
	public:
		constexpr dynamic_array() : p_m(nullptr), size_m(0), use_size_m(0), alloc_m() {}
		explicit dynamic_array(const Allocator& alloc) : p_m(nullptr), size_m(0), use_size_m(0)
			, alloc_m(allocator_traits<Allocator>::select_on_container_copy_construction(alloc)) {}
		dynamic_array(size_type size, const Allocator& alloc = Allocator()) : p_m(nullptr), size_m(0), use_size_m(size)
			, alloc_m(allocator_traits<Allocator>::select_on_container_copy_construction(alloc)) {
			p_m = alloc_m.allocate(size_m);
		}
		template <class InputIterator>
		dynamic_array(InputIterator first, InputIterator last) : alloc_m() {
			static_assert(is_iterator_v<InputIterator, input_iterator_tag>, "The type of iterator is different.");

			size_type dist = distance(first, last);
			if (max_size() < dist) throw std::length_error("size exceeds max_size().");
			size_m = (iml::min)(max_size(), dist * 2);				//  基本は領域設定量の2倍分だけ確保
			use_size_m = dist;
			p_m = alloc_m.allocate(size_m);
			allocator_traits<Allocator>::copy_construct(alloc_m, p_m, p_m + use_size_m, first, last);
		}
		dynamic_array(const dynamic_array& a) : p_m(nullptr), size_m(a.size_m), use_size_m(a.use_size_m)
			, alloc_m(allocator_traits<Allocator>::select_on_container_copy_construction(a.alloc_m)) {
			p_m = alloc_m.allocate(size_m);
			allocator_traits<Allocator>::copy_construct(alloc_m, p_m, p_m + use_size_m, a.p_m, a.p_m + use_size_m);
		}
		dynamic_array(dynamic_array&& a) : p_m(a.p_m), size_m(a.size_m), use_size_m(a.use_size_m), alloc_m(a.alloc_m) { a.p_m = nullptr; a.use_size_m = a.size_m = 0; }
		~dynamic_array() {
			allocator_traits<Allocator>::destroy(alloc_m, p_m, p_m + use_size_m);		//  デストラクタを作用
			alloc_m.deallocate(p_m, size_m);											//  メモリ解放
		}

		iterator begin() noexcept { return iterator(p_m); }
		const_iterator begin() const noexcept { return const_iterator(p_m); }
		iterator end() noexcept { return iterator(p_m + use_size_m); }
		const_iterator end() const noexcept { return const_iterator(p_m + use_size_m); }

		//  空かの判定
		[[nodiscard]] bool empty() const noexcept { return use_size_m == 0; }
		//  データの破棄
		void clear() {
			allocator_traits<Allocator>::destroy(alloc_m, p_m, p_m + use_size_m);
			alloc_m.deallocate(p_m, size_m);
			size_m = use_size_m = 0;
			p_m = nullptr;
		}
		//  同一要素で埋める
		void fill(const T& n) { iml::fill(p_m, p_m + use_size_m, n); }
		//  使用中のサイズの取得
		size_type size() const { return use_size_m; }
		//  容量
		size_type capacity() const { return size_m; }
		//  コンテナに格納可能な最大サイズ
		constexpr size_type max_size() const { return allocator_traits<Allocator>::max_size(alloc_m); }


		//  要素数の再設定
		void resize(size_type size) {
			if (size == 0) { clear(); return; }

			//  確保されているメモリが十分に大きいときは余剰分にデフォルトコンストラクタかデストラクタを作用
			if (use_size_m >= size) {
				allocator_traits<Allocator>::destroy(alloc_m, p_m + size, p_m + use_size_m);
				use_size_m = size;
				return;
			}
			else if (size_m >= size) {
				allocator_traits<Allocator>::construct_all(alloc_m, p_m + use_size_m, p_m + size);
				use_size_m = size;
				return;
			}
			if (max_size() < size) throw std::length_error("size exceeds max_size().");
			//  メモリを再確保して構築
			auto temp = alloc_m.allocate(size);
			allocator_traits<Allocator>::move_construct(alloc_m, temp, temp + size, p_m, p_m + use_size_m);
			allocator_traits<Allocator>::destroy(alloc_m, p_m, p_m + use_size_m);
			alloc_m.deallocate(p_m, size_m);
			size_m = use_size_m = size;
			p_m = temp;
		}
		void resize(size_type size, const T& c) {
			//  確保されているメモリが十分に大きいときは余剰分にコピーコンストラクタかデストラクタを作用
			if (use_size_m >= size) {
				allocator_traits<Allocator>::destroy(alloc_m, p_m + size, p_m + use_size_m);
				use_szie_m = size;
				return;
			}
			else if (size_m >= size) {
				allocator_traits<Allocator>::construct_all(alloc_m, p_m + use_size_m, p_m + size, c);
				use_size_m = size;
				return;
			}
			if (max_size() < size) throw std::length_error("size exceeds max_size().");
			//  メモリを再確保して構築
			auto temp = alloc_m.allocate(size);
			allocator_traits<Allocator>::move_construct(alloc_m, temp, temp + use_size_m, p_m, p_m + use_size_m);
			allocator_traits<Allocator>::construct_all(alloc_m, temp + use_size_m, temp + size, c);
			allocator_traits<Allocator>::destroy(alloc_m, p_m, p_m + use_size_m);
			alloc_m.deallocate(p_m, size_m);
			size_m = use_size_m = size;
			p_m = temp;
		}
		//  capacityよりもsizeが大きいときメモリの再確保
		void reserve(size_type size) {
			if (size < size_m) return;
			if (max_size() < size) throw std::length_error("size exceeds max_size().");
			auto temp = alloc_m.allocate(size);
			allocator_traits<Allocator>::move_construct(alloc_m, temp, temp + use_size_m, p_m, p_m + use_size_m);
			allocator_traits<Allocator>::destroy(alloc_m, p_m, p_m + use_size_m);
			alloc_m.deallocate(p_m, size_m);
			size_m = size;
			p_m = temp;
		}


		//  要素の挿入(emptyなときはiteratorが無効であるため挿入不可)
		//  itrに対してc
		void insert(const_iterator itr, const T& c) {
			if (!((begin() <= to_iterator(itr)) && (to_iterator(itr) <= end()))) return;		// イテレータの範囲外
			//  メモリを再確保する必要がある場合
			if (size_m == use_size_m) {
				if (max_size() == size_m) throw std::length_error("size exceeds max_size().");
				size_type alloc_size = (iml::min)(max_size(), (size_m + 1) * 2);
				auto temp = alloc_m.allocate(size_m);
				auto itr_pos = distance(begin(), to_iterator(itr));
				allocator_traits<Allocator>::move_construct(alloc_m, temp, temp + itr_pos, begin(), to_iterator(itr));
				allocator_traits<Allocator>::construct(alloc_m, temp + itr_pos, c);
				allocator_traits<Allocator>::move_construct(alloc_m, temp + itr_pos + 1, temp + use_size_m + 1, to_iterator(itr), end());
				allocator_traits<Allocator>::destroy(alloc_m, p_m, p_m + use_size_m);
				alloc_m.deallocate(p_m, size_m);
				size_m = alloc_size;
				p_m = temp;
			}
			else {
				allocator_traits<Allocator>::construct(alloc_m, p_m + use_size_m);
				move_reverse_order(to_iterator(itr) + 1, to_iterator(itr), end());
				*to_iterator(itr) = c;
			}
			++use_size_m;
		}
		//  itrに対して[first,last)
		template <class InputIterator>
		void insert(const_iterator itr, InputIterator first, InputIterator last) {
			static_assert(is_iterator_v<InputIterator, input_iterator_tag>, "The type of iterator is different.");
			
			if (!((begin() <= to_iterator(itr)) && (to_iterator(itr) <= end()))) return;		// イテレータの範囲外
			size_type dist = distance(first, last);
			size_type alloc_size = dist + use_size_m;			//  必要なメモリ領域
			//  メモリを再確保する必要がある場合
			if (size_m < alloc_size) {
				if (max_size()  < alloc_size) throw std::length_error("size exceeds max_size().");
				alloc_size = (iml::min)(max_size(), alloc_size * 2);
				auto temp = alloc_m.allocate(alloc_size);
				auto itr_pos = distance(begin(), to_iterator(itr));
				allocator_traits<Allocator>::move_construct(alloc_m, temp, temp + itr_pos, begin(), to_iterator(itr));
				allocator_traits<Allocator>::copy_construct(alloc_m, temp + itr_pos, temp + itr_pos + dist, first, last);
				allocator_traits<Allocator>::move_construct(alloc_m, temp + itr_pos + dist, temp + use_size_m + dist, to_iterator(itr), end());
				allocator_traits<Allocator>::destroy(alloc_m, p_m, p_m + use_size_m);
				alloc_m.deallocate(p_m, size_m);
				size_m = alloc_size;
				p_m = temp;
			}
			else {
				allocator_traits<Allocator>::construct_all(alloc_m, p_m + use_size_m, p_m + use_size_m + dist);
				move_reverse_order(to_iterator(itr) + dist, to_iterator(itr), end());
				copy_order(to_iterator(itr), first, last);
			}
			use_size_m += dist;
		}
		//  要素の削除(emptyなときはiteratorが無効であるため削除不可)
		//  itrの位置
		void erase(const_iterator itr) {
			//  itrの分だけ前に詰めて後ろを除去する
			move_order(to_iterator(itr), to_iterator(itr + 1), end());
			allocator_traits<Allocator>::destroy(alloc_m, p_m + use_size_m - 1);
			--use_size_m;
		}
		//  [first,last)の範囲
		void erase(const_iterator first, const_iterator last) {
			move_order(to_iterator(first), to_iterator(last), end());
			size_type dist = distance(first, last);
			allocator_traits<Allocator>::destroy(alloc_m, p_m + use_size_m - dist, p_m + use_size_m);
			use_size_m -= dist;
		}


		// 後方にデータの挿入
		dynamic_array& push_back(const T& v) {
			//  領域が足りないときはメモリ再確保
			if (size_m == use_size_m) {
				if (max_size() == size_m) throw std::length_error("size exceeds max_size().");
				reserve((iml::min)(max_size(), (size_m + 1) * 2));
			}
			allocator_traits<Allocator>::construct(alloc_m, p_m + use_size_m, v);
			++use_size_m;
			return *this;
		}
		//  後方のデータの削除
		dynamic_array& pop_back() {
			if (use_size_m == 0) return *this;
			allocator_traits<Allocator>::destroy(alloc_m, p_m  + use_size_m - 1);
			--use_size_m;
			return *this;
		}


		//  メモリを使用サイズにフィットさせる
		void shrink_to_fit() {
			if (size_m == use_size_m) return;
			auto temp = alloc_m.allocate(use_size_m);
			allocator_traits<Allocator>::move_construct(alloc_m, temp, temp + use_size_m, p_m, p_m + use_size_m);
			allocator_traits<Allocator>::destroy(alloc_m, p_m, p_m + use_size_m);
			alloc_m.deallocate(p_m, size_m);
			size_m = use_size_m;
			p_m = temp;
		}


		//  コンテナの中身の入れ替え
		void swap(dynamic_array& a) {
			iml::swap(p_m, a.p_m);
			iml::swap(size_m, a.size_m);
			iml::swap(use_size_m, a.use_size_m);
			iml::swap(alloc_m, a.alloc_m);
		}

		//  代入
		dynamic_array& operator=(const dynamic_array& a) {
			if (a.p_m == p_m) return *this;
			allocator_traits<Allocator>::destroy(alloc_m, p_m, p_m + use_size_m);
			//  メモリを確保する必要があるならば確保する
			if (size_m < a.use_size_m) {
				alloc_m.deallocate(p_m, size_m);
				alloc_m = allocator_traits<Allocator>::select_on_container_copy_construction(a.alloc_m);
				size_m = a.size_m;
				p_m = alloc_m.allocate(size_m);
			}
			else {
				alloc_m = allocator_traits<Allocator>::select_on_container_copy_construction(a.alloc_m);
			}
			use_size_m = a.use_size_m;
			allocator_traits<Allocator>::copy_construct(alloc_m, p_m, p_m + use_size_m, a.p_m, a.p_m + use_size_m);
			return *this;
		}
		dynamic_array& operator=(dynamic_array&& a) {
			if (a.p_m == p_m) return *this;
			alloc_m = a.alloc_m;
			size_m = a.size_m;
			use_size_m = a.use_size_m;
			p_m = a.p_m;
			a.p_m = nullptr;
			a.use_size_m = a.size_m = 0;
			return *this;
		}

		reference operator[](size_type index) noexcept { return p_m[index]; }
		const_reference operator[](size_type index) const noexcept { return p_m[index]; }

		//  内部イテレータ
		template <class F>
		F for_each(F f) const { return iml::for_each(p_m, p_m + use_size_m, f); }
	};
	template <class T, class U, class TAllocator, class UAllocator>
	constexpr bool operator==(const dynamic_array<T, TAllocator>& a1, const dynamic_array<U, UAllocator>& a2) {
		if (a1.size() != a2.size()) return false;
		auto itr1 = a1.begin();
		auto itr2 = a2.begin();
		for (; itr1 != a1.end(); ++itr1, ++itr2) if (*itr1 != *itr2) return false;
		return true;
	}
	template <class T, class U, class TAllocator, class UAllocator>
	constexpr bool operator!=(const dynamic_array<T, TAllocator>& a1, const dynamic_array<U, UAllocator>& a2) {
		return !(a1 == a2);
	}


	//  配列の判定の登録
	template<class T, size_t N>
	struct is_array<static_array<T, N>> : true_type {};
	template<class T, class Allocator>
	struct is_array<dynamic_array<T, Allocator>> : true_type {};

}

#endif