#ifndef IMATH_CONTAINER_ALLOCATOR_HPP
#define IMATH_CONTAINER_ALLOCATOR_HPP

#include "IMathLib/utility/iterator.hpp"
#include "IMathLib/utility/functional.hpp"
#include "IMathLib/math/math/numeric_traits.hpp"


// コンテナ等で用いるためのアロケータの実装
namespace iml {

	//  allocator_traitsのための補助
	namespace alloc {
		//  allocatorにpointerが存在
		template <class, class = void>
		struct is_exist_pointer : false_type {};
		template <class T>
		struct is_exist_pointer<T, void_t<typename T::pointer>> : true_type {};
		template <class Allocator, class T, bool = is_exist_pointer<Allocator>::value>
		struct pointer {
			using type = typename Allocator::pointer;
		};
		template <class Allocator, class T>
		struct pointer<Allocator, T, false> {
			using type = T;
		};

		//  allocatorにdifference_typeが存在
		template <class, class = void>
		struct is_exist_difference_type : false_type {};
		template <class T>
		struct is_exist_difference_type<T, void_t<typename T::difference_type>> : true_type {};
		template <class Allocator, class T, bool = is_exist_difference_type<Allocator>::value>
		struct difference_type {
			using type = typename Allocator::difference_type;
		};
		template <class Allocator, class T>
		struct difference_type<Allocator, T, false> {
			using type = T;
		};

		//  allocatorにsize_typeが存在
		template <class, class = void>
		struct is_exist_size_type : false_type {};
		template <class T>
		struct is_exist_size_type<T, void_t<typename T::size_type>> : true_type {};
		template <class Allocator, class T, bool = is_exist_size_type<Allocator>::value>
		struct size_type {
			using type = typename Allocator::size_type;
		};
		template <class Allocator, class T>
		struct size_type<Allocator, T, false> {
			using type = T;
		};

		//  max_size()が存在するならそれを呼び出す
		template <class Allocator>
		struct is_exist_max_size {
		private:
			template <class UAlloc> static auto tester(UAlloc*) ->decltype(declval<UAlloc>().max_size(), true_type());
			template <class UAlloc> static false_type tester(...);
		public:
			static constexpr bool value = decltype(tester<Allocator>(nullptr))::value;
		};
		template <class Allocator, class SizeType, class ValueType, bool = is_exist_max_size<Allocator>::value>
		struct Max_size {
			static constexpr SizeType max_size(const Allocator& a) { return a.max_size(); }
		};
		template <class Allocator, class SizeType, class ValueType>
		struct Max_size<Allocator, SizeType, ValueType, false> {
			static constexpr SizeType max_size(const Allocator&) { return (numeric_traits<SizeType>::max)() / sizeof(ValueType); }
		};

		//  construct()が存在するならそれを呼び出す
		template <class, class, class>
		struct is_exist_construct {};
		template <class Allocator, class T, class... Types>
		struct is_exist_construct<Allocator, T, type_tuple<Types...>> {
		private:
			template <class UAlloc> static auto tester(UAlloc*) ->decltype(declval<UAlloc>().construct(declval<T*>(), declval<Types>()...), true_type());
			template <class UAlloc> static false_type tester(...);
		public:
			static constexpr bool value = decltype(tester<Allocator>(nullptr))::value;
		};
		template <class Allocator, class T, class Types, bool = is_exist_construct<Allocator, T, Types>::value>
		struct Construct {};
		template <class Allocator, class T, class... Types>
		struct Construct<Allocator, T, type_tuple<Types...>, true> {
			static constexpr void construct(Allocator& a, T* p, Types&&... args) { a.construct(p, forward<Types>(args)...); }
		};
		template <class Allocator, class T, class... Types>
		struct Construct<Allocator, T, type_tuple<Types...>, false> {
			static constexpr void construct(Allocator& a, T* p, Types&&... args) { ::new (static_cast<void*>(p)) T(forward<Types>(args)...); }
		};

		//  destroy()が存在するならそれを呼び出す
		template <class Allocator, class T>
		struct is_exist_destroy {
		private:
			template <class UAlloc> static auto tester(UAlloc*) ->decltype(declval<UAlloc>().destroy(declval<T*>()), true_type());
			template <class UAlloc> static false_type tester(...);
		public:
			static constexpr bool value = decltype(tester<Allocator>(nullptr))::value;
		};
		template <class Allocator, class T, bool = is_exist_destroy<Allocator, T>::value>
		struct Destroy {
			static void destroy(Allocator& a, T* p) { a.destroy(p); }
		};
		template <class Allocator, class T>
		struct Destroy<Allocator, T, false> {
			static void destroy(Allocator&, T* p) { p->~T(); }
		};

		//  select_on_container_copy_construction()が存在するならそれを呼び出す
		template <class Allocator>
		struct is_exist_select_on_container_copy_construction {
		private:
			template <class UAlloc> static auto tester(UAlloc*) ->decltype(declval<UAlloc>().select_on_container_copy_construction(), true_type());
			template <class UAlloc> static false_type tester(...);
		public:
			static constexpr bool value = decltype(tester<Allocator>(nullptr))::value;
		};
		template <class Allocator, bool = is_exist_select_on_container_copy_construction<Allocator>::value>
		struct Select_on_container_copy_construction {
			static constexpr Allocator select_on_container_copy_construction(const Allocator& a) { return a.select_on_container_copy_construction(); }
		};
		template <class Allocator>
		struct Select_on_container_copy_construction<Allocator, false> {
			static constexpr Allocator select_on_container_copy_construction(const Allocator&) { return Allocator(); }
		};
	}


	template <class Allocator>
	struct allocator_traits {
		using allocator_type = Allocator;
		using value_type = typename Allocator::value_type;
		using pointer = typename alloc::pointer<Allocator, value_type*>::type;
		using difference_type = typename alloc::difference_type<Allocator, typename pointer_traits<pointer>::difference_type>::type;
		using size_type = typename alloc::size_type<Allocator, make_unsigned_t<difference_type>>::type;


		template <class Other>
		using rebind = typename Allocator::template rebind<Other>;
		template <class Other>
		using rebind_t = typename Allocator::template rebind_t<Other>;


		//  メモリのアロケート
		[[nodiscard]] static constexpr pointer allocate(allocator_type& a, size_t n) { return a.allocate(n); }
		//  メモリのディアロケート
		static constexpr void deallocate(allocator_type& a, pointer p, size_t n) { return a.deallocate(p, n); }
		//  一度に確保可能なメモリの数
		static constexpr size_type max_size(const allocator_type& a) noexcept{
			return alloc::Max_size<allocator_type, size_type, value_type>::max_size(a);
		}
		//  コンストラクタの適用
		template <class T, class... Types>
		static constexpr void construct(allocator_type& a, T* p, Types&&... args) {
			alloc::Construct<allocator_type, T, type_tuple<Types...>>::construct(a, p, forward<Types>(args)...);
		}
		template <class InputIterator, class... Types>
		static constexpr void construct_all(allocator_type& a, InputIterator first, InputIterator last, Types&&... args) {
			//  入力イテレータでなければならない
			static_assert(is_iterator_v<InputIterator, input_iterator_tag>, "The type of iterator is different.");

			while (first != last) construct(a, to_address(first++), forward<Types>(args)...);
		}
		template <class InputIterator1, class InputIterator2>
		static constexpr void copy_construct(allocator_type& a, InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2) {
			//  入力イテレータでなければならない
			static_assert(is_iterator_v<InputIterator1, input_iterator_tag> && is_iterator_v<InputIterator2, input_iterator_tag>, "The type of iterator is different.");

			while (first1 != last1) {
				if (first2 == last2) break;
				construct(a, to_address(first1++), *to_address(first2++));
			}
			//  足りない部分はデフォルトコンストラクタを適用する
			while (first1 != last1) construct(a, to_address(first1++));
		}
		template <class InputIterator1, class InputIterator2>
		static constexpr void move_construct(allocator_type& a, InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2) {
			//  入力イテレータでなければならない
			static_assert(is_iterator_v<InputIterator1, input_iterator_tag> && is_iterator_v<InputIterator2, input_iterator_tag>, "The type of iterator is different.");

			while (first1 != last1) {
				if (first2 == last2) break;
				construct(a, to_address(first1++), move(*to_address(first2++)));
			}
			while (first1 != last1) construct(a, to_address(first1++));
		}
		//  デストラクタの適用
		template <class T>
		static void destroy(allocator_type& a, T* p) {
			alloc::Destroy<allocator_type, T>::destroy(a, p);
		}
		template <class OutputIterator>
		static void destroy(allocator_type& a, OutputIterator first, OutputIterator last) {
			//  出力イテレータでなければならない
			static_assert(is_iterator_v<OutputIterator, output_iterator_tag>, "The type of iterator is different.");

			while (first != last) destroy(a, to_address(first++));
		}
		//  コンテナのコピー構築に使用するアロケータオブジェクトを取得
		static constexpr allocator_type select_on_container_copy_construction(const allocator_type& a) {
			return alloc::Select_on_container_copy_construction<allocator_type>::select_on_container_copy_construction(a);
		}
	};


	//  アロケータ
	template <class T>
	class allocator {
	public:
		constexpr allocator() noexcept {}
		constexpr allocator(allocator&) noexcept {}
		template <class U>
		constexpr allocator(allocator<U>&) noexcept {}
		~allocator() {}

		using value_type = T;
		using pointer = T * ;

		template <class Other>
		struct rebind {
			using other = allocator<Other>;
		};
		template <class Other>
		using rebind_t = allocator<Other>;

		//  メモリ確保(new[]で確保しないためdeleteで解放できる)
		[[nodiscard]] pointer allocate(size_t n) { return static_cast<pointer>(::operator new(n * sizeof(value_type))); }
		//  メモリ解放
		void deallocate(pointer p, size_t) { ::operator delete(static_cast<void*>(p)); }
	};
	template <class T, class U>
	bool operator==(const allocator<T>&, const allocator<U>&) { return true; }
	template <class T, class U, class Iterator1, class Iterator2>
	bool operator!=(const allocator<T>&, const allocator<U>&) { return false; }


	//  リソースの破棄条件をdeallocator_baseを通して共通化する
	namespace dealloc {
		static constexpr size_t variable = 0;				//  newで確保されたインスタンスに対してdeleteをする
		static constexpr size_t array = 1;					//  new[]で確保されたインスタンスに対してdelete[]をする
		static constexpr size_t allocator = 2;				//  アロケータによって確保されたインスタンス
		static constexpr size_t function = 3;				//  void(T*)な関数を用いてインスタンスを破棄する
	}
	//  ディアロケータ用のプレースホルダ
	template <size_t N>
	using dealloc_ident = index_tuple<size_t, N>;
	class deallocator_base {
	protected:
		void*	p_m;

		constexpr deallocator_base(void* p) : p_m(p) {}
	public:
		virtual ~deallocator_base() = 0 {}

		//  ホールドしている変数の解放
		virtual void dispose(void* p = nullptr) = 0;
		//  インスタンスと自身のリソースを解放
		virtual void destroy() = 0;
		//  インスタンスの監視を止め，自身のリソースを解放
		virtual void release() = 0;
	};
	template <class, size_t>
	class deallocator;
	//  newで確保されたインスタンスについて
	template <class T>
	class deallocator<T, dealloc::variable> : public deallocator_base {
		template <class U>
		constexpr deallocator(U* p) : deallocator_base(static_cast<void*>(p)) {}
	public:
		~deallocator() {}
		void dispose(void* p) { delete static_cast<T*>(p_m); p_m = static_cast<void*>(p); }
		void destroy() { delete static_cast<T*>(p_m); delete this; }
		void release() { delete this; }

		using value_type = T;

		template <class U>
		static deallocator* get(U* p) { return new deallocator(p); }
	};
	//  new[]で確保されたインスタンスについて
	template <class T>
	class deallocator<T, dealloc::array> : public deallocator_base {
		template <class U>
		constexpr deallocator(U* p) : deallocator_base(static_cast<void*>(p)) {}
	public:
		~deallocator() {}
		void dispose(void* p) { delete[] static_cast<T*>(p_m); p_m = static_cast<void*>(p); }
		void destroy() { delete[] static_cast<T*>(p_m); delete this; }
		void release() { delete this; }

		using value_type = T;

		template <class U>
		static deallocator* get(U* p) { return new deallocator(p); }
	};
	//  アロケータにより確保されたインスタンスについて
	template <class Allocator>
	class deallocator<Allocator, dealloc::allocator> : public deallocator_base {
	public:
		using value_type = typename Allocator::value_type;
	private:
		Allocator*	alloc_m;
		size_t		size_m;					//  アロケータによってアロケートされている要素数

		template <class U>
		constexpr deallocator(U* p, Allocator& alloc, size_t n) : deallocator_base(static_cast<void*>(p)), alloc_m(addressof(alloc)), size_m(n) {}
	public:
		~deallocator() {}
		void dispose(void* p) {
			allocator_traits<Allocator>::destroy(*alloc_m, static_cast<value_type*>(p_m));
			alloc_m->deallocate(static_cast<value_type*>(p_m), size_m);
			p_m = static_cast<void*>(p);
		}
		void destroy() {
			allocator_traits<Allocator>::destroy(*alloc_m, static_cast<value_type*>(p_m));
			alloc_m->deallocate(static_cast<value_type*>(p_m), size_m);
			delete this;
		}
		void release() { delete this; }

		template <class U>
		static deallocator* get(U* p, Allocator& alloc, size_t n) { return new deallocator(p, alloc, n); }
	};
	//  特定の関数を用いて破棄するインスタンスについて
	template <class T>
	class deallocator<T, dealloc::function> : public deallocator_base {
		function<void(T*)>	f_m;

		template <class U, class F>
		constexpr deallocator(U* p, F f) : deallocator_base(static_cast<void*>(p)), f_m(f) {}
	public:
		~deallocator() {}
		void dispose(void* p) { f_m(static_cast<T*>(p_m)); p_m = static_cast<void*>(p); }
		void destroy() { f_m(static_cast<T*>(p_m)); delete this; }
		void release() { delete this; }

		using value_type = T;

		template <class U, class F>
		static deallocator* get(U* p, F f) { return new deallocator(p, f); }
	};

}


#endif