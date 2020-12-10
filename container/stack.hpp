#ifndef IMATHLIB_CONTAINER_STACK_HPP
#define IMATHLIB_CONTAINER_STACK_HPP

#include "IMathLib/container/list.hpp"

// スタック

namespace iml {

	// スタック
	template <class T, class Container = list<T>>
	class stack {
	protected:
		Container	cont;
	public:
		using container_type = Container;
		using value_type = T;

		constexpr stack() {}
		template <class InputIterator>
		stack(InputIterator first, InputIterator last) :cont(first, last) {}
		~stack() {}

		void push(const T& v) { cont.push_back(v); }
		void pop() { cont.pop_back(); }

		void dep() { push(peek()); }

		T& peek() { return *--cont.end(); }
		const T& peek() const { return *--cont.end(); }

		size_t size() const { return cont.size(); }

		void swap(stack& s) { cont.swap(s.cont); }
	};
}


#endif