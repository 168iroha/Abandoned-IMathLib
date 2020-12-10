#ifndef IMATHLIB_H_UTILITY_SMART_PTR_HPP
#define IMATHLIB_H_UTILITY_SMART_PTR_HPP

#include "IMathLib/container/allocator.hpp"


// インスタンスの所有権が唯一なスマートポインタ
namespace iml {

	template <class T>
	class unique_ptr {
		template <class>
		friend class shared_ptr;
		template <class>
		friend class unique_ptr;

		T*					p_m;			// 保持しているインスタンス
		deallocator_base*	dealloc_m;		// ディアロケータ
	public:
		constexpr unique_ptr() noexcept : p_m(nullptr), dealloc_m(nullptr) {}
		unique_ptr(unique_ptr&& u) noexcept : p_m(u.p_m), dealloc_m(u.dealloc_m) { u.p_m = nullptr; u.dealloc_m = nullptr; }
		template <class U>
		unique_ptr(unique_ptr<U>&& u) noexcept : p_m(static_cast<T*>(u.p_m)), dealloc_m(u.dealloc_m) { u.p_m = nullptr; u.dealloc_m = nullptr; }
		template <class U>
		explicit unique_ptr(U* p) : p_m(static_cast<T*>(p)), dealloc_m(deallocator<T, dealloc::variable>::get(p)) {}
		template <class U, size_t N, class... Types>
		explicit unique_ptr(U* p, dealloc_ident<N>, Types&&... args) : p_m(static_cast<T*>(p)), dealloc_m(deallocator<T, N>::get(p, forward<Types>(args)...)) {}
		~unique_ptr() { dealloc_m->destroy(); }

		void swap(unique_ptr& x) noexcept {
			iml::swap(p_m, x.p_m); iml::swap(dealloc_m, x.dealloc_m);
		}
		// インスタンスの所有権を破棄(解放はしない)
		T* release() noexcept {
			T* temp = p_m;
			if (!dealloc_m) {
				dealloc_m->release();
				dealloc_m = nullptr;
			}
			p_m = nullptr;
			return temp;
		}
		// インスタンスの破棄
		void reset() { unique_ptr().swap(*this); }
		// インスタンスを破棄して再設定
		template <class U>
		void reset(U* p) { unique_ptr(p).swap(*this); }
		template <class U, size_t N, class... Types>
		void reset(U* p, dealloc_ident<N> id = dealloc_ident<N>(), Types&&... args) {
			unique_ptr(p, id, forward<Types>(args)...).swap(*this);
		}

		unique_ptr& operator=(const unique_ptr&) = delete;
		unique_ptr& operator=(unique_ptr&& u) noexcept {
			if (this != &u) {
				p_m = u.p; u.p_m = nullptr;
				dealloc_m = u.dealloc_m; u.dealloc_m = nullptr;
			}
			return *this;
		}


		// 各種参照
		T* get() const { return p_m; }
		T& operator*() const { return *get(); }
		T* operator->() const { return get(); }

		// リソースを所持しているかの判定
		operator bool() const { return !!p_m; }
		bool operator!() const { return !p_m; }

		const T& operator[](size_t index) const { return p_m[index]; }
		T& operator[](size_t index) { return p_m[index]; }
	};
}



// シェアポインタ
namespace iml {

	// 参照カウンタ
	struct shared_count_impl {
		size_t					shared_cnt_m;		// シェアカウンタ
		size_t					weak_cnt_m;			// ウィークカウンタ
		deallocator_base*		dealloc_m;			// リソースを参照してその破棄の中継をする

		// p:保持しているリソースへのポインタ
		// N:deallocatorの種類を示す定数
		template <class T, size_t N, class... Types>
		explicit shared_count_impl(T* p, dealloc_ident<N>, Types&&... args) : shared_cnt_m(1), weak_cnt_m(0), dealloc_m(deallocator<T, N>::get(p, forward<Types>(args)...)) {}
		template <class T>
		explicit shared_count_impl(T* p, deallocator_base* dealloc) : shared_cnt_m(1), weak_cnt_m(0), dealloc_m(dealloc) {}
		shared_count_impl(const shared_count_impl& c)  noexcept : shared_cnt_m(c.shared_cnt_m), weak_cnt_m(c.weak_cnt_m), dealloc_m(c.dealloc_m) {}
		shared_count_impl(shared_count_impl&& c)  noexcept : shared_cnt_m(c.shared_cnt_m), weak_cnt_m(c.weak_cnt_m), dealloc_m(c.dealloc_m) { c.dealloc_m = nullptr; }
		~shared_count_impl() { dealloc_m->destroy(); }

		size_t inc() { return ++shared_cnt_m; }
		size_t dec() { return --shared_cnt_m; }					// 仕様上，負数になることはない
		size_t weak_inc() { return ++weak_cnt_m; }
		size_t weak_dec() { return --weak_cnt_m; }
		size_t shared_count() { return shared_cnt_m; }
		size_t weak_count() { return weak_cnt_m; }

		shared_count_impl& operator=(const shared_count_impl&) = delete;
		shared_count_impl& operator=(shared_count_impl&& c) noexcept {
			this->shared_cnt_m = c.shared_cnt_m;
			this->weak_cnt_m = c.weak_cnt_m;
			this->dealloc_m = c.dealloc_m;
			c.dealloc_m = nullptr;
			return *this;
		}
	};
	class shared_count {
		friend class weak_count;

		shared_count_impl*	cnt_m;
	public:
		constexpr shared_count() : cnt_m(nullptr) {}
		shared_count(const shared_count& s)  noexcept : cnt_m(s.cnt_m) { if (cnt_m) cnt_m->inc(); }
		shared_count(shared_count&& s)  noexcept : cnt_m(s.cnt_m) { s.cnt_m = nullptr; }
		shared_count(const weak_count& w) noexcept;
		template <class T>
		explicit shared_count(T* p) : cnt_m(new shared_count_impl(p, dealloc_ident<dealloc::variable>())) {}
		template <class T, size_t N, class... Types>
		explicit shared_count(T* p, dealloc_ident<N> id, Types&&... args) : cnt_m(new shared_count_impl(p, id, forward<Types>(args)...)) {}
		// deallocは実態をもたなければならない
		template <class T>
		explicit shared_count(T* p, deallocator_base& dealloc) : cnt_m(new shared_count_impl(p, addressof(dealloc))) {}
		~shared_count() { release(); }

		// 弱参照とシェアカウンタが0(他にリソースを参照しているものが存在しない)ならば解放
		void release() {
			if (cnt_m != nullptr)
				if (cnt_m && !cnt_m->dec() && !cnt_m->weak_count()) {
					cnt_m->dealloc_m->dispose();
					delete cnt_m;
					cnt_m = nullptr;
				}
		}
		shared_count& operator=(const shared_count& r) {
			shared_count_impl* temp = r.cnt_m;
			// シェアカウンタが同一のものでないとき代入
			if (temp != cnt_m) {
				if (temp != nullptr) temp->inc();
				release();
				cnt_m = temp;
			}
			return *this;
		}
		shared_count& operator=(shared_count&& r) {
			// シェアカウンタが同一のものでないときインスタンスの移動
			if (r.cnt_m != cnt_m) {
				release();
				cnt_m = r.cnt_m;
				r.cnt_m = nullptr;
			}
			return *this;
		}
		// スワップ
		void swap(shared_count& r) { iml::swap(cnt_m, r.cnt_m); }
		// シェアしている数
		size_t use_count() const { return (cnt_m != nullptr) ? cnt_m->shared_count() : 0; }
		// シェアしているインスタンスが唯一か
		bool unique() const { return use_count() == 1; }
	};


	// 所有権をシェアするスマートポインタ
	template <class T>
	class shared_ptr {
		template <class>
		friend class shared_ptr;
		template <class>
		friend class weak_ptr;
		template <class>
		friend class unique_ptr;

		T*				p_m;		// 保持しているインスタンス
		shared_count	sc_m;		// シェアカウンタ
	public:
		constexpr shared_ptr() noexcept : p_m(nullptr), sc_m() {}
		shared_ptr(const shared_ptr& s) noexcept : p_m(s.p_m), sc_m(s.sc_m) {}
		shared_ptr(shared_ptr&& s) noexcept : p_m(s.p_m), sc_m(s.sc_m) { s.p_m = nullptr; }
		template<class U>
		shared_ptr(const shared_ptr<U>& s) noexcept : p_m(const_cast<T*>(static_cast<const T*>(s.p_m))), sc_m(s.sc_m) {}
		template<class U>
		shared_ptr(shared_ptr<U>&& s) noexcept : p_m(static_cast<T*>(s.p_m)), sc_m(s.sc_m) { s.p_m = nullptr; }
		template <class U>
		shared_ptr(const weak_ptr<U>& w);
		template<class U>
		shared_ptr(unique_ptr<U>&& u) noexcept : p_m(static_cast<T*>(u.p_m)), sc_m(u.p_m, *u.dealloc_m) { u.p_m = u.dealloc_m = nullptr; }
		template<class U>
		explicit shared_ptr(U* p) : p_m(static_cast<T*>(p)), sc_m(p, dealloc_ident<dealloc::variable>()) {}
		template<class U, size_t N, class... Types>
		explicit shared_ptr(U* p, dealloc_ident<N> id, Types&&... args) : p_m(static_cast<T*>(p)), sc_m(p, id, forward<Types>(args)...) {}
		~shared_ptr() {}

		//スワップ
		void swap(shared_ptr& other) noexcept { iml::swap(p_m, other.p_m); sc_m.swap(other.sc_m); }

		// 所有権を破棄して新しい所有権を得る
		void reset() { shared_ptr().swap(*this); }
		void reset(shared_ptr& s) { shared_ptr(s).swap(*this); }
		template <class U>
		void reset(U* p) { shared_ptr(p).swap(*this); }
		template <class U, size_t N, class... Types>
		void reset(U* p, dealloc_ident<N> id, Types&&... args) {
			shared_ptr(p, id, forward<Types>(args)...).swap(*this);
		}

		// インスタンスが唯一かの判定
		bool unique() const { return sc_m.unique(); }
		// カウントの取得
		size_t use_count() const { return sc_m.use_count(); }

		// 代入演算
		shared_ptr& operator=(const shared_ptr& s) {
			sc_m = s.sc_m; p_m = s.p_m;
			return *this;
		}
		shared_ptr& operator=(shared_ptr&& s) noexcept {
			sc_m = s.sc_m; p_m = s.p_m;
			p_m = nullptr;
			return *this;
		}
		template <class U>
		shared_ptr& operator=(const shared_ptr<U>& s) {
			sc_m = s.sc_m; p_m = s.p_m;
			return *this;
		}
		template <class U>
		shared_ptr& operator=(shared_ptr<U>&& s) noexcept {
			sc_m = s.sc_m; p_m = s.p_m;
			s.p_m = nullptr;
			return *this;
		}
		template <class U>
		shared_ptr& operator=(unique_ptr<U>&& u) noexcept {
			shared_count(u.p_m, u.dealloc_m).swap(sc_m);
			p_m = u.p_m;
			u.p_m = u.dealloc_m = nullptr;
			return *this;
		}


		// 各種参照
		T* get() const { return p_m; }
		T& operator*() const { return *get(); }
		T* operator->() const { return get(); }

		// リソースを所持しているかの判定
		operator bool() const { return !!p_m; }
		bool operator!() const { return !p_m; }

		const T& operator[](size_t index) const { return p_m[index]; }
		T& operator[](size_t index) { return p_m[index]; }
	};
}

// shared_ptrの弱参照を保持するスマートポインタ(shared_ptrのオブサーバー)
namespace iml {

	// ウィークカウンタ
	class weak_count {
		friend class shared_count;

		shared_count_impl* cnt_m;
	public:
		constexpr weak_count() : cnt_m(nullptr) {}
		weak_count(const shared_count& s) : cnt_m(s.cnt_m) { if (cnt_m) cnt_m->weak_inc(); }
		weak_count(const weak_count& w) : cnt_m(w.cnt_m) { if (cnt_m) cnt_m->weak_inc(); }
		weak_count(weak_count&& w) : cnt_m(w.cnt_m) { w.cnt_m = nullptr; }
		~weak_count() { release(); }

		// 弱参照とシェアカウントが0ならば解放
		void release() {
			if (cnt_m && !cnt_m->weak_dec() && !cnt_m->shared_count()) {
				cnt_m->dealloc_m->dispose();
				delete cnt_m;
				cnt_m = nullptr;
			}
		}

		weak_count& operator=(const shared_count& s) {
			shared_count_impl* temp = s.cnt_m;
			if (temp != cnt_m) {
				if (temp != nullptr) temp->weak_inc();
				release();
				cnt_m = temp;
			}
			return *this;
		}
		weak_count& operator=(const weak_count& w) {
			shared_count_impl* temp = w.cnt_m;
			if (temp != cnt_m) {
				if (temp) temp->weak_inc();
				release();
				cnt_m = temp;
			}
			return *this;
		}
		weak_count& operator=(weak_count&& w) {
			if (w.cnt_m != cnt_m) {
				cnt_m = w.cnt_m;
				w.cnt_m = nullptr;
			}
			return *this;
		}
		//スワップ
		void swap(weak_count& w) { iml::swap(cnt_m, w.cnt_m); }
		// 弱参照しているインスタンスのシェア数
		size_t use_count() const { return cnt_m ? cnt_m->shared_count() : 0; }
		// 弱参照しているインスタンスのシェアが唯一か
		bool unique() const { return use_count() == 1; }
	};
	// シャアカウンタのコンストラクタの記述
	inline shared_count::shared_count(const weak_count& w) noexcept : cnt_m(w.cnt_m) { if (cnt_m) cnt_m->inc(); }

	//shared_ptrの弱参照を保持して循環参照を解決するスマートポインタ
	template <class T>
	class weak_ptr {
		template <class>
		friend class weak_ptr;
		template <class>
		friend class shared_ptr;

		T*			p_m;		// 保持しているインスタンス
		weak_count	wc_m;		// ウィークカウンタ
	public:
		constexpr weak_ptr() noexcept : p_m(nullptr), wc_m() {}
		weak_ptr(const weak_ptr& w) noexcept : p_m(w.p_m), wc_m(w.wc_m) {}
		weak_ptr(const weak_ptr&& w) noexcept : p_m(w.p_m), wc_m(w.wc_m) { w.p_m = nullptr; }
		template <class U>
		weak_ptr(const weak_ptr<U>& w) noexcept : p_m(const_cast<T*>(static_cast<const T*>(w.p_m))), wc_m(w.wc_m) {}
		template <class U>
		weak_ptr(const weak_ptr<U>&& w) noexcept : p_m(const_cast<T*>(static_cast<const T*>(w.p_m))), wc_m(w.wc_m) { w.p_m = nullptr; }
		template <class U>
		weak_ptr(const shared_ptr<U>& s) noexcept : p_m(const_cast<T*>(static_cast<const T*>(s.p_m))), wc_m(s.sc_m) {}
		~weak_ptr() {}

		// スワップ
		void swap(weak_ptr& w) noexcept {
			iml::swap(p_m, w.p_m); wc_m.swap(w.wc_m);
		}
		// 所有権の破棄
		void reset() { weak_ptr().swap(*this); }
		void reset(const weak_ptr& w) { weak_ptr(w).swap(*this); }
		// 監視しているリソースのインスタンスの取得
		shared_ptr<T> lock() const { return shared_ptr<T>(*this); }

		// 弱参照しているインスタンスのシェア数
		size_t use_count() const { return wc_m.use_count(); }
		// 弱参照しているインスタンスのシェアが唯一か
		bool unique() const { return wc_m.unique(); }

		//代入演算
		template <class U>
		weak_ptr& operator=(const shared_ptr<U>& s) {
			p_m = s.p_m; wc_m = s.sc_m;
			return *this;
		}
		weak_ptr& operator=(const weak_ptr& w) {
			p_m = w.p_m; wc_m = w.wc_m;
			return *this;
		}
		weak_ptr& operator=(weak_ptr&& w) {
			p_m = w.p_m; wc_m = w.wc_m;
			w.p_m = nullptr;
			return *this;
		}
		template <class U>
		weak_ptr& operator=(const weak_ptr<U>& w) {
			p_m = w.p_m; wc_m = w.wc_m;
			return *this;
		}
		template <class U>
		weak_ptr& operator=(weak_ptr<U>&& w) {
			p_m = w.p_m; wc_m = w.wc_m;
			w.p_m = nullptr;
			return *this;
		}


		// 各種参照
		T* get() const { return p_m; }
		T& operator*() const { return *get(); }
		T* operator->() const { return get(); }

		// リソースを所持しているかの判定
		operator bool() const { return !!p_m; }
		bool operator!() const { return !p_m; }

		const T& operator[](size_t index) const { return p_m[index]; }
		T& operator[](size_t index) { return p_m[index]; }
	};
	template <class T>
	template <class U>
	inline shared_ptr<T>::shared_ptr(const weak_ptr<U>& w) : p_m(const_cast<T*>(static_cast<const T*>(w.p_m))), sc_m(w.wc_m) {}
}

#endif