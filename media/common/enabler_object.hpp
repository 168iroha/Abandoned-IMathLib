#ifndef IMATHLIB_H_MEDIA_COMMON_ENABLER_OBJECT_HPP
#define IMATHLIB_H_MEDIA_COMMON_ENABLER_OBJECT_HPP

#include <memory>
#include <stack>

namespace iml {

	// enabler objectおよびdisabler objectのインスタンスの管理
	template <class T, class Container>
	class ENABLER_OBJECT_CONTROL : public singleton<ENABLER_OBJECT_CONTROL<T, Container>> {
		friend class singleton<ENABLER_OBJECT_CONTROL<T, Container>>;

		std::stack<T*, Container>	instances_m;

		ENABLER_OBJECT_CONTROL() {

		}
		// 標準で適用される制御対象を指定するとき
		ENABLER_OBJECT_CONTROL(T* default_obj) {
			this->instances_m.push(default_obj);
		}
	public:
		void push(T* obj) { this->instances_m.push(obj); }
		void pop() { this->instances_m.pop(); }
		T* top() { return this->instances_m.top(); }

		size_t size() { return this->instances_m.size(); }
	};

	// 初期化を示すタグ
	struct enabler_object_init_tag {};


	// enabler objectの基底
	template <class Object, class EnablerObject, class StackContainer = c_deque_tag>
	struct ENABLER_OBJECT_BASE {
		template <class, class, class>
		friend struct ENABLER_OBJECT_BASE;

		std::shared_ptr<Object>		obj_m;			// Tという状態に関するオブジェクト

		// 状態を更新するメソッド
		virtual void push_state() = 0;
		// 状態を外すメソッド
		virtual void pop_state() = 0;
		// 状態を設定するメソッド
		virtual void set_state() = 0;

	protected:
		// ENABLER_OBJECT_CONTROLに用いるスタックのコンテナ
		using stack_container_t = container_tag_t<StackContainer, ENABLER_OBJECT_BASE*>;
		// ENABLER_OBJECT_CONTROLのエイリアス
		using enabler_object_control_t = ENABLER_OBJECT_CONTROL<ENABLER_OBJECT_BASE, stack_container_t>;

		// 初期状態のための派生クラスで呼び出すENABLER_OBJECT_BASEのコンストラクタ
		void init_state_construct() {
			enabler_object_control_t::construct(this);
			if (enabler_object_control_t::inst()->size() > 0) this->set_state();
		}
		// 派生クラスで呼び出すENABLER_OBJECT_BASEのコンストラクタ
		void construct() {
			enabler_object_control_t::inst()->push(this);
			this->push_state();
		}
		// 派生クラスで呼び出すENABLER_OBJECT_BASEのデストラクタ
		void destroy() {
			if (enabler_object_control_t::is_exist_instance()) {
				enabler_object_control_t::inst()->pop();
				this->pop_state();
				if (enabler_object_control_t::inst()->size() > 0) enabler_object_control_t::inst()->top()->set_state();
			}
		}

		// 標準状態のインスタンス
		static inline ENABLER_OBJECT_BASE* default_m = nullptr;
	public:
		// 初期化(デフォルトの状態の構築)
		static void init() {
			if (ENABLER_OBJECT_BASE::default_m != nullptr) ENABLER_OBJECT_BASE::quit();
			ENABLER_OBJECT_BASE::default_m = new EnablerObject(enabler_object_init_tag());
		}
		template <class SubObject, class = std::enable_if_t<std::is_base_of_v<Object, SubObject> || std::is_same_v<Object, SubObject>>>
		static void init(const SubObject& obj) {
			if (ENABLER_OBJECT_BASE::default_m != nullptr) ENABLER_OBJECT_BASE::quit();
			ENABLER_OBJECT_BASE::default_m = new EnablerObject(enabler_object_init_tag(), obj);
		}
		// 終了
		static void quit() {
			delete ENABLER_OBJECT_BASE::default_m;
			ENABLER_OBJECT_BASE::default_m = nullptr;
			enabler_object_control_t::destroy();
		}

		ENABLER_OBJECT_BASE() : obj_m(new Object()) {}
		template <class SubObject, class = std::enable_if_t<std::is_base_of_v<Object, SubObject> || std::is_same_v<Object, SubObject>>>
		ENABLER_OBJECT_BASE(const SubObject& obj) : obj_m(static_cast<Object*>(new SubObject(obj))) {}
		// 状態の共有はsetでのみ行う
		ENABLER_OBJECT_BASE(std::shared_ptr<Object>&& obj) noexcept : obj_m(obj) {}
		virtual ~ENABLER_OBJECT_BASE() {}
	private:
		// 状態を設定
		void set_impl() {
			// スタックのtopと同じインスタンスなら状態の設定をする
			if (enabler_object_control_t::inst()->top() == this) this->set_state();
		}
	public:
		// 状態を共通化
		void set(const ENABLER_OBJECT_BASE& obj) {
			this->obj_m = obj.obj_m;
			this->set_impl();
		}
		// 状態に対する制御対象の変更
		template <class SubObject, class = std::enable_if_t<std::is_base_of_v<Object, SubObject> || std::is_same_v<Object, SubObject>>>
		void set(const SubObject& obj) {
			*(this->obj_m) = static_cast<Object>(obj);
			this->set_impl();
		}
	};
}

// enabler objectのためのマクロ
#define IMATHLIB_ENABLER_OBJECT(PROCESS) if (PROCESS; true)


#endif