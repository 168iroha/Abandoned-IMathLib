#ifndef IMATHLIB_H_MEDIA_COMMON_SINGLETON_HPP
#define IMATHLIB_H_MEDIA_COMMON_SINGLETON_HPP

#include "IMathLib/utility/utility.hpp"

namespace iml {

	// シングルトンを実装するための基底クラス
	template <class T>
	class singleton {
		static inline T* inst_m = nullptr;

		singleton(const singleton &) = delete;
		singleton(singleton &&) = delete;
		singleton& operator=(const singleton &) = delete;
		singleton& operator=(singleton &&) = delete;
	protected:
		constexpr singleton() {}
	public:
		virtual ~singleton() = 0 {}

		// インスタンスの生成
		template <class... Types>
		static T* construct(Types&&... args) {
			if (singleton::is_exist_instance()) delete singleton::inst_m;
			return singleton::inst_m = new T(std::forward<Types>(args)...);
		}
		// インスタンスの破棄
		static void destroy() {
			delete singleton::inst_m;
			singleton::inst_m = nullptr;
		}
		// インスタンスの取得
		static T* inst() {
			return (singleton::inst_m == nullptr) ? singleton::construct() : singleton::inst_m;
		}
		// インスタンスの有無
		static bool is_exist_instance() { return singleton::inst_m != nullptr; }
	};
}

#endif