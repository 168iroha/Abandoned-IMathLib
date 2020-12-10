#ifndef _QUEUE_HPP
#define _QUEUE_HPP

#include "IMathLib/container/list.hpp"

// キュー

namespace iml {

	// キュー
	template <class T, class Container = list<T>>
	class queue {
	protected:
		Container	cont;
	public:
		using container_type = Container;
		using value_type = T;

		constexpr queue() {}
		template <class InputIterator>
		queue(InputIterator first, InputIterator last) :cont(first, last) {}
		~queue() {}

		void enqueue(const T& v) { cont.push_back(v); }
		void dequeue() { cont.pop_front(); }

		T& front() { return *cont.begin(); }
		const T& front() const { return *cont.begin(); }

		size_t size() const { return cont.size(); }

		void swap(queue& s) { cont.swap(s.cont); }
	};
}


#endif