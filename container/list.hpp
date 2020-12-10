#ifndef IMATHLIB_CONTAINER_LIST_HPP
#define IMATHLIB_CONTAINER_LIST_HPP

#include "IMathLib/container/array.hpp"


//  双方向連結リスト
namespace iml {

	//  連結リストのノード
	template <class T>
	struct list_node {
		list_node*	next_m;			//  次のノード
		list_node*	prev_m;			//  前のノード
		T			value_m;

		constexpr list_node(const T& v) : value_m(v), next_m(nullptr), prev_m(nullptr) {}
		constexpr list_node(const T& v, list_node* prev, list_node* next) : prev_m(prev), next_m(next), value_m(v) {}
		list_node(list_node&& node) : next_m(node.next_m), prev_m(node.prev_m), value_m(node.value_m) { node.next_m = node.prev_m = node.next_m = nullptr; }
	};


	//  双方向連結リストのイテレータ
	template <class T>
	struct list_iterator {
		list_node<T>*	current_m;
		list_node<T>*	prev_m;

		using iterator_category = bidirectional_iterator_tag;
		using value_type = T;
		using difference_type = ptrdiff_t;
		using pointer = T*;
		using reference = T&;

		template<class Other>
		struct rebind {
			using other = list_iterator<Other>;
		};
		template <class Other>
		using rebind_t = list_iterator<Other>;

		constexpr list_iterator() : current_m(nullptr) {}
		template <class U>
		list_iterator(list_node<U>* prev, list_node<U>* current) : prev_m(prev), current_m(current) {}
		template <class U>
		list_iterator(nullptr_t, list_node<U>* current) : prev_m(nullptr), current_m(current) {}
		template <class U>
		list_iterator(list_node<U>* prev, nullptr_t) : prev_m(prev), current_m(nullptr) {}
		list_iterator(nullptr_t, nullptr_t) : prev_m(nullptr), current_m(nullptr) {}
		template <class U>
		constexpr list_iterator(const list_iterator<U>& itr) : prev_m(reinterpret_cast<list_node<T>*>(itr.prev_m)), current_m(reinterpret_cast<list_node<T>*>(itr.current_m)) {}
		
		reference operator*() const { return current_m->value_m; }
		pointer operator->() const { return &current_m->value_m; }
		list_iterator& operator++() { prev_m = current_m; current_m = current_m->next_m; return *this; }
		list_iterator operator++(int) { list_iterator temp = *this; ++*this; return temp; }
		list_iterator& operator--() { current_m = prev_m; prev_m = current_m->prev_m; return *this; }
		list_iterator operator--(int) { list_iterator temp = *this; --*this; return temp; }

		reference operator[](difference_type n) const { return *next(*this, n); }
		
		bool operator==(const list_iterator& itr) const { return current_m == itr.current_m; }
		bool operator!=(const list_iterator& itr) const { return !(*this == itr); }
	};


	//  双方向連結リスト
	template <class T, class Allocator = allocator<list_node<T>>>
	class list {
	public:
		using value_type = T;
		using reference = T & ;
		using const_reference = const T &;
		using iterator = list_iterator<T>;
		using const_iterator = list_iterator<const T>;
		using allocator_type = Allocator;
		using size_type = typename allocator_traits<Allocator>::size_type;
	private:
		Allocator		alloc_m;			//  アロケータ
		list_node<T>*	first_m;			//  一番最初の要素
		list_node<T>*	last_m;				//  一番最後の要素
		size_type		size_m;				//  要素数
	public:
		constexpr list() : first_m(nullptr), last_m(nullptr), size_m(0), alloc_m() {}
		explicit list(const Allocator& alloc) : first_m(nullptr), last_m(nullptr), size_m(0)
			, alloc_m(allocator_traits<Allocator>::select_on_container_copy_construction(alloc)) {}
		template <class InputIterator>
		list(InputIterator first, InputIterator last) {
			static_assert(is_iterator_v<InputIterator, input_iterator_tag>, "The type of iterator is different.");

			
		}
		list(const list& l) : first_m(nullptr), last_m(nullptr), size_m(0), alloc_m() { *this = l; }
		list(list&& l) : first_m(l.first_m), last(l.last_m), size_m(l.size_m), alloc_m(l.alloc_m) { first_m = last_m = nullptr; }
		~list() { clear(); }

		iterator begin() noexcept { return iterator(nullptr, first_m); }
		const_iterator begin() const noexcept { return const_iterator(nullptr, first_m); }
		iterator end() noexcept { return iterator(last_m, nullptr); }
		const_iterator end() const noexcept { return const_iterator(last_m, nullptr); }

		void clear() {
			for (list_node<T>* ptr = first_m; ptr != nullptr;) {
				list_node<T>* temp = ptr; ptr = ptr->next_m;
				allocator_traits<Allocator>::destroy(alloc_m, temp);
				alloc_m.deallocate(temp, 1);
			}
			size_m = 0;
			first_m = last_m = nullptr;
		}
		bool empty() const noexcept { return size_m == 0; }
		size_type size() const { return size_m; }
		void swap(list& l) {
			iml::swap(alloc_m, l.alloc_m);
			iml::swap(first_m, l.first_m);
			iml::swap(last_m, l.last_m);
			iml::swap(size_m, l.size_m);
		}
		// 内部イテレータ
		template <class F>
		F for_each(F f) const { return iml::for_each(begin(), end(), f); }


		//  要素の挿入(emptyなときはiteratorが無効であるため挿入不可)
		void insert(const_iterator p, const T& v) {
			list_node<T>* temp = alloc_m.allocate(1);
			//  pの位置にtempを挿入するためにpの前とpで前後を結合する
			allocator_traits<Allocator>::construct(alloc_m, temp, v, to_iterator(p).current_m->prev_m, to_iterator(p).current_m);
			//  tempの前後の情報をtempで書き換える
			temp->prev_m->next_m = temp;
			temp->next_m->prev_m = temp;
			++size_m;
		}
		//  [first, last)
		template <class InputIterator>
		void insert(const_iterator p, InputIterator first, InputIterator last) {
			static_assert(is_iterator_v<InputIterator, input_iterator_tag>, "The type of iterator is different.");

			list_node<T>* temp = to_iterator(p).current_m->prev_m;
			for (InputIterator itr = first; itr != last; ++itr, temp = temp->next_m, ++size_m) {
				temp->next_m = alloc_m.allocate(1);
				allocator_traits<Allocator>::construct(alloc_m, temp->next_m, *itr, temp, nullptr);
			}
			temp->next_m = to_iterator(p).current_m;
			temp->prev_m->next_m = temp;
			temp->next_m->prev_m = temp;
		}
		//  要素の削除(emptyなときはiteratorが無効であるため削除不可)
		void erase(const_iterator p) {
			//  pを除去するためにpの前後を結合する
			//  pが最初の要素のとき
			if (p.current_m->prev_m == nullptr) {
				first_m = to_iterator(p).current_m->next_m;
				if (first_m != nullptr) first_m->prev_m = nullptr;
			}
			else to_iterator(p).current_m->prev_m->next_m = to_iterator(p).current_m->next_m;
			//  pが最後の要素のとき
			if (p.current_m->next_m == nullptr) {
				last_m = to_iterator(p).current_m->prev_m;
				if (last_m != nullptr) last_m->next_m = nullptr;
			}
			else to_iterator(p).current_m->next_m->prev_m = to_iterator(p).current_m->prev_m;
			
			allocator_traits<Allocator>::destroy(alloc_m, to_iterator(p).current_m);
			alloc_m.deallocate(to_iterator(p).current_m, 1);
			--size_m;
		}
		//  [first, last)
		void erase(const_iterator first, const_iterator last) {
			//  firstが最初の要素のとき
			if (first.current_m->prev_m == nullptr) {
				first_m = to_iterator(last).current_m;
				if (first_m != nullptr) first_m->prev_m = nullptr;
			}
			else to_iterator(first).current_m->prev_m->next_m = to_iterator(last).current_m;
			//  lastが終端を示す要素のとき
			if (last.current_m == nullptr) {
				last_m = to_iterator(first).current_m->prev_m;
				if (last_m != nullptr) last_m->next_m = nullptr;
			}
			else to_iterator(last).current_m->prev_m = to_iterator(first).current_m->prev_m;
			
			//  cleanのコピペ
			for (list_node<T>* ptr = to_iterator(first).current_m, last_ptr = to_iterator(last).current_m; ptr != last_ptr; --size_m) {
				list_node<T>* temp = ptr; ptr = ptr->next_m;
				allocator_traits<Allocator>::destroy(alloc_m, temp);
				alloc_m.deallocate(temp, 1);
			}
		}


		// 前後への挿入
		list& push_front(const T& v) {
			if (size_m == 0) {
				first_m = last_m = alloc_m.allocate(1);
				allocator_traits<Allocator>::construct(alloc_m, first_m, v, nullptr, nullptr);
			}
			else {
				first_m->prev_m = alloc_m.allocate(1);
				allocator_traits<Allocator>::construct(alloc_m, first_m->prev_m, v, nullptr, first_m);
				first_m = first_m->prev_m;
			}
			++size_m;
			return *this;
		}
		list& push_back(const T& v) {
			if (size_m == 0) {
				first_m = last_m = alloc_m.allocate(1);
				allocator_traits<Allocator>::construct(alloc_m, first_m, v, nullptr, nullptr);
			}
			else {
				last_m->next_m = alloc_m.allocate(1);
				allocator_traits<Allocator>::construct(alloc_m, last_m->next_m, v, last_m, nullptr);
				last_m = last_m->next_m;
			}
			++size_m;
			return *this;
		}
		list& pop_front() {
			if (size_m == 0) return *this;

			list_node<T>* temp = first_m->next_m;
			allocator_traits<Allocator>::destroy(alloc_m, first_m);
			alloc_m.deallocate(first_m, 1);
			first_m = temp;
			//  要素数が1でなかったときは終端設定をする必要がある
			if (first_m != nullptr) first_m->prev_m = nullptr;
			--size_m;
			return *this;
		}
		list& pop_back() {
			if (size_m == 0) return *this;

			list_node<T>* temp = last_m->prev_m;
			allocator_traits<Allocator>::destroy(alloc_m, last_m);
			alloc_m.deallocate(last_m, 1);
			last_m = temp;
			if (last_m != nullptr) last_m->next_m = nullptr;
			--size_m;
			return *this;
		}

		//  代入
		list& operator=(const list& l) {
			if (first_m == l.first_m) return *this;
			if (size_m > l.size_m) {
				if (l.size_m == 0) clear();
				else {
					//  [l.first_m, l.last_m]を代入
					list_node<T>* temp = first_m;
					for (list_node<T>* ptr = l.first_m; ptr != nullptr; temp = temp->next_m, ptr = ptr->next_m) temp->value_m = ptr->value_m;
					//  一番最後の位置の決定(仮定からtemp->prev_mは存在する)
					last_m = temp->prev_m;
					last_m->next_m = nullptr;
					//  [temp, last_m]を削除
					while (temp != nullptr) {
						list_node<T>* temp2 = temp; temp = temp->next_m;
						allocator_traits<Allocator>::destroy(alloc_m, temp2);
						alloc_m.deallocate(temp2, 1);
					}
				}
			}
			else if (size_m == l.size_m) {
				for (list_node<T>* ptr1 = first_m, *ptr2 = l.first_m; ptr2 != nullptr; ptr1 = ptr1->next_m, ptr2 = ptr2->next_m)
					ptr1->value_m = ptr2->value_m;
			}
			else if (size_m == 0) {
				//  size_m < l.size_mかつsize_m == 0となる特殊な場合
				first_m = last_m = alloc_m.allocate(1);
				allocator_traits<Allocator>::construct(alloc_m, first_m, l.first_m->value_m, nullptr, nullptr);
				for (list_node<T>* ptr = l.first_m->next_m; ptr != nullptr; ptr = ptr->next_m, last_m = last_m->next_m) {
					last_m->next_m = alloc_m.allocate(1);
					allocator_traits<Allocator>::construct(alloc_m, last_m->next_m, ptr->value_m, last_m, nullptr);
				}
			}
			else if (size_m < l.size_m) {
				list_node<T>* temp = l.first_m;
				//  [first_m,last_m]に対して代入
				for (list_node<T>* ptr = first_m; ptr != nullptr; ptr = ptr->next_m, temp = temp->next_m) ptr->value_m = temp->value_m;
				//  一番最後の位置に対してメモリ確保しながら代入
				for (; temp != nullptr; last_m = last_m->next_m, temp = temp->next_m) {
					last_m->next_m = alloc_m.allocate(1);
					allocator_traits<Allocator>::construct(alloc_m, last_m->next_m, temp->value_m, last_m, nullptr);
				}
			}
			size_m = l.size_m;
			return *this;
		}
		list& operator=(list&& l) {
			if (first_m == l.first_m) return *this;
			first_m = l.first_m; last_m = l.last_m;
			alloc_m = l.alloc_m;
			size_m = l.size_m;
			l.first_m = l.last_m = nullptr;
			return *this;
		}

		T& operator[](size_type index) noexcept {
			if (index < size_m / 2) {
				list_node<T>* ptr = first_m;
				for (size_type i = 0; i < index; ++i, ptr = ptr->next_m);
				return ptr->value_m;
			}
			else {
				list_node<T>* ptr = last_m;
				for (size_type i = 0, n = size_m - index - 1; i < n; ++i, ptr = ptr->prev_m);
				return ptr->value_m;
			}
		}
		const T& operator[](size_type index) const noexcept {
			if (index < size_m / 2) {
				list_node<T>* ptr = first_m;
				for (size_type i = 0; i < index; ++i, ptr = ptr->next_m);
				return ptr->value_m;
			}
			else {
				list_node<T>* ptr = last_m;
				for (size_type i = 0, n = size_m - index - 1; i < n; ++i, ptr = ptr->prev_m);
				return ptr->value_m;
			}
		}

		// 前後へのアクセス
		T& front() { return first_m->value_m; }
		const T& front() const { return first_m->value_m; }
		T& back() { return last_m->value_m; }
		const T& back() const { return last_m->value_m; }
	};

}


#endif