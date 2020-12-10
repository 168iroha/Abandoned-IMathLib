#ifndef IMATHLIB_CONTAINER_MAP_HPP
#define IMATHLIB_CONTAINER_MAP_HPP

#include "IMathLib/container/allocator.hpp"
#include "IMathLib/utility/algorithm.hpp"
#include "IMathLib/utility/tuple.hpp"


//  二分木による連想配列
namespace iml {

	//  二分木による連想配列のイテレータ(T = pair<Key, U>となる)
	template <class T>
	class tree_map_iterator {
		template <class>
		friend class tree_map_iterator;

	public:
		using key_type = remove_cv_t<typename T::first_type>;			//  キーの型
		using mapped_type = typename T::second_type;					//  要素の型

		//  赤黒木用のノード
		struct node {
			T			value;
			node		*left = nullptr, *right = nullptr;
			node		*parent;
			bool		flag = true;			//  状態フラグ(true:赤,false:黒)
			node(const key_type& key, const mapped_type& v, node* parent) :value(key, v), parent(parent) {}
			node(const node& n) { *this = n; }

			node& operator=(const node& n) {
				const_cast<key_type&>(this->value.first) = n.value.first;
				this->value.second = n.value.second;
				this->left = n.left;
				this->right = n.right;
				this->parent = n.parent;
				this->flag = n.flag;
				return *this;
			}
			node& operator=(node&& n) {
				const_cast<key_type&>(this->value.first) = n.value.first;
				this->value.second = n.value.second;
				this->left = n.left;
				this->right = n.right;
				this->parent = n.parent;
				this->flag = n.flag;
				return *this;
			}
		};
	private:
		node*		current;
		node*		root;			//  ルートノード(ないといろいろ困る)
		bool		flag;			//  状態フラグ(右に進むかのフラグ)
	public:
		//  直接データの取得
		node* _Ptr() { return current; }
		node* _Ptr() const { return current; }

		using iterator_category = bidirectional_iterator_tag;
		using value_type = T;
		using difference_type = ptrdiff_t;
		using pointer = T*;
		using reference = T&;

		template<class Other>
		struct rebind {
			using other = tree_map_iterator<Other>;
		};
		template <class Other>
		using rebind_t = tree_map_iterator<Other>;

		constexpr tree_map_iterator() :current(nullptr), root(nullptr), flag(true) {}
		constexpr tree_map_iterator(node* itr, node* root = nullptr, bool flag = true) : current(itr), root(root), flag(flag) {}
		template <class S>
		constexpr tree_map_iterator(const tree_map_iterator<S>& itr)
			: current(reinterpret_cast<node*>(itr.current))
			, root(reinterpret_cast<node*>(itr.current)), flag(itr.flag) {}

		reference operator*() const { return current->value; }
		pointer operator->() const { return &current->value; }
		tree_map_iterator& operator++() {
			//  初めに戻り循環する
			if (current == nullptr) {
				current = root;
				while (current->left != nullptr) current = current->left;
				return *this;
			}

			if (!flag) {
				//  左に子が存在しないとき
				if (current->left == nullptr) {
					//  右に子が存在すれば一つ右に行き左の一番下の子に行く
					if (current->right != nullptr) {
						current = current->right;
						while (current->left != nullptr) current = current->left;
					}
					//  葉であるとき
					else {
						//  右の子から上に上がるとき(ここではparentは存在する)
						if (current->parent->right == current) {
							node* temp = current;
							current = current->parent;
							while (temp == current->right) {
								temp = current;
								current = current->parent;
								//  rootの上に行ってしまったときendイテレータとなる
								if (current == nullptr) return *this;
							}
							//  rootであるとき右下に行く
							if (current->parent == nullptr) flag = true;
							//  右上に進むためのフラグ
							else if ((current->parent->left == current) && (current->right == nullptr)) flag = true;
							//  右下に進むためのフラグ
							else if (current->right != nullptr) flag = true;
						}
						//  左の子から上に上がるとき
						else {
							current = current->parent;
							//  右上に進むためのフラグ
							if (current->parent->left == current) flag = true;
							//  右下に進むためのフラグ
							else if (current->right != nullptr) flag = true;
						}
					}
				}
				//  左に子が存在するとき
				else {
					//  フラグが立たずにrootならばendイテレータとなる
					if (current->parent == nullptr) current = nullptr;
					//  右下に存在する子ならば一気に上に行く
					else if (current->parent->right == current) {
						node* temp = current;
						current = current->parent;
						while (temp == current->right) {
							temp = current;
							current = current->parent;
							//  rootの上に行ってしまったときendイテレータとなる
							if (current == nullptr) return *this;
						}
						//  rootであるとき右下に行く
						if (current->parent == nullptr) flag = true;
						//  右上に進むためのフラグ
						else if ((current->parent->left == current) && (current->right == nullptr)) flag = true;
						//  右下に進むためのフラグ
						else if (current->right != nullptr) flag = true;
					}
					//  左の一番下の子に行く
					else while (current->left != nullptr) current = current->left;
				}
			}
			//  次に右に進むべきとき
			else {
				//  右に子が存在すれば一つ右に行き左の一番下の子に行く
				if (current->right != nullptr) {
					current = current->right;
					while (current->left != nullptr) current = current->left;
					flag = false;		//  右に行く必要はないためフラグを折る
				}
				//  なければ一つ上に行く
				else current = current->parent;
			}
			return *this;
		}
		tree_map_iterator operator++(int) { tree_map_iterator temp = *this; ++*this; return temp; }
		tree_map_iterator& operator--() {
			//  インクリメントのときの左右の子の扱いが逆になっただけ(フラグの扱いも逆になる)

			if (current == nullptr) {
				current = root;
				while (current->right != nullptr) current = current->right;
				return *this;
			}

			if (flag) {
				if (current->right == nullptr) {
					if (current->left != nullptr) {
						current = current->left;
						while (current->right != nullptr) current = current->right;
					}
					else {
						if (current->parent->left == current) {
							node* temp = current;
							current = current->parent;
							while (temp == current->left) {
								temp = current;
								current = current->parent;
								if (current == nullptr) return *this;
							}
							if (current->parent == nullptr) flag = false;
							else if ((current->parent->right == current) && (current->left == nullptr)) flag = false;
							else if (current->left != nullptr) flag = false;
						}
						else {
							current = current->parent;
							if (current->parent->right == current) flag = false;
							else if (current->left != nullptr) flag = false;
						}
					}
				}
				else {
					if (current->parent == nullptr) current = nullptr;
					else if (current->parent->left == current) {
						node* temp = current;
						current = current->parent;
						while (temp == current->left) {
							temp = current;
							current = current->parent;
							if (current == nullptr) return *this;
						}
						if (current->parent == nullptr) flag = false;
						else if ((current->parent->right == current) && (current->left == nullptr)) flag = false;
						else if (current->left != nullptr) flag = false;
					}
					else while (current->right != nullptr) current = current->right;
				}
			}
			else {
				if (current->left != nullptr) {
					current = current->left;
					while (current->right != nullptr) current = current->right;
					flag = true;
				}
				else current = current->parent;
			}
			return *this;
		}
		tree_map_iterator operator--(int) { tree_map_iterator temp = *this; --*this; return temp; }

		reference operator[](difference_type n) const { return *next(*this, n); }

		bool operator==(const tree_map_iterator& itr) const { return current == itr.current; }
		bool operator!=(const tree_map_iterator& itr) const { return !(*this == itr); }

	};

	//  二分木による連想配列のコンテナ
	template <class T, class Compare, class Allocator>
	class tree_map_container {
	public:
		using value_type = T;
		using reference = T & ;
		using const_reference = const T &;
		using iterator = tree_map_iterator<T>;
		using const_iterator = typename iterator::rebind_t<const T>;
		using allocator_type = Allocator;
		using size_type = typename allocator_traits<Allocator>::size_type;
	public:
		using key_type = typename remove_cv<typename T::first_type>::type;			//  キーの型
		using mapped_type = typename T::second_type;								//  要素の型
	protected:
		using _Node = typename tree_map_iterator<T>::node;

		Allocator	_allo;				//  アロケータ
		size_type	_size = 0;
		_Node*		root = nullptr;

		//  ノードの右回転(nを右上に持っていく)
		static void _Rot_right(_Node* n) {
			//  予め部分木nが接続されているノードを取得
			_Node* temp = n->parent->parent;

			n->parent->parent = n;
			n->parent->left = n->right;
			if (n->right != nullptr) n->right->parent = n->parent;
			n->right = n->parent;
			n->parent = temp;

			//  rootでなければ付け替え処理を行う
			if (temp != nullptr) ((temp->right == n->right) ? temp->right : temp->left) = n;
		}
		//  ノードの左回転(nを左上に持っていく)
		static void _Rot_left(_Node* n) {
			//  予め部分木nが接続されているノードを取得
			_Node* temp = n->parent->parent;

			n->parent->parent = n;
			n->parent->right = n->left;
			if (n->left != nullptr) n->left->parent = n->parent;
			n->left = n->parent;
			n->parent = temp;

			//  rootでなければ付け替え処理を行う
			if (temp != nullptr) ((temp->right == n->left) ? temp->right : temp->left) = n;
		}

		//  ノードのクリア
		static void _Clear(_Node* n, Allocator& _allo) {
			if (n->left != nullptr) {
				_Clear(n->left, _allo);
				allocator_traits<Allocator>::destroy(_allo, n->left);
				_allo.deallocate(n->left, 1);
			}
			if (n->right != nullptr) {
				_Clear(n->right, _allo);
				allocator_traits<Allocator>::destroy(_allo, n->right);
				_allo.deallocate(n->right, 1);
			}
		}
		//  ノードの探索
		static _Node* _Find(const key_type& key, _Node* n) {

			if (n == nullptr) return nullptr;

			if (key == n->value.first) return n;
			if (key < n->value.first) return _Find(key, n->left);
			if (n->value.first < key) return _Find(key, n->right);

			return nullptr;
		}
		//  ノードの挿入(key == n->value.firstのときの代入はサイズのカウントが不可のためなし)
		static _Node* _Insert(const key_type& key, const mapped_type& v, _Node* n, Allocator& _allo) {

			if (key < n->value.first) {
				if (n->left == nullptr) {
					n->left = _allo.allocate(1);
					allocator_traits<Allocator>::construct(_allo, n->left, key, v, n);
					return n->left;
				}
				return _Insert(key, v, n->left, _allo);
			}
			if (n->value.first < key) {
				if (n->right == nullptr) {
					n->right = _allo.allocate(1);
					allocator_traits<Allocator>::construct(_allo, n->right, key, v, n);
					return n->right;
				}
				return _Insert(key, v, n->right, _allo);
			}

			return nullptr;
		}
		//  赤ノードの判定
		static constexpr bool _Red(_Node* n) {
			return (n == nullptr) ? false : n->flag;
		}
		//  挿入の赤黒の制御(root以外のnullは黒として扱う)
		static _Node* _Insert_support(_Node* n) {
			_Node* result;		//  戻り値のroot用の変数

			//  n->parentがrootならば黒にする
			if (n->parent == nullptr) n->flag = false;
			//  n->parentが黒ならばnは赤にする
			else if (!n->parent->flag) n->flag = true;
			//  n->parentが赤のとき必ずn->parent->parentが存在し、それは黒である
			else {
				//  nが木の左側に存在するとき
				if (n->parent->parent->left == n->parent) {
					//  n->parent->parent->rightが赤のとき
					if (_Red(n->parent->parent->right)) {
						//  n->parent->parentを赤にし、その子を黒にする
						n->parent->parent->flag = true;
						n->parent->parent->right->flag = false;
						n->parent->flag = false;
						return _Insert_support(n->parent->parent);
					}
					//  nが木の内側として存在するとき
					else if (n->parent->right == n) {
						//  elseの状態にする
						_Rot_left(n);
						return _Insert_support(n->left);
					}
					else {
						n->parent->flag = false;
						n->parent->parent->flag = true;
						_Rot_right(n->parent);
					}
				}
				//  nが木の右側に存在するとき
				else if (n->parent->parent->right == n->parent) {
					//  n->parent->parent->leftが赤のとき
					if (_Red(n->parent->parent->left)) {
						//  n->parent->parentを赤にし、その子を黒にする
						n->parent->parent->flag = true;
						n->parent->parent->left->flag = false;
						n->parent->flag = false;
						return _Insert_support(n->parent->parent);
					}
					//  nが木の内側として存在するとき
					else if (n->parent->left == n) {
						//  elseの状態にする
						_Rot_right(n);
						return _Insert_support(n->right);
					}
					else {
						n->parent->flag = false;
						n->parent->parent->flag = true;
						_Rot_left(n->parent);
					}
				}
			}

			//  rootの取得
			result = n;
			while (result->parent != nullptr) result = result->parent;
			return result;
		}
		//  ノードの削除
		static _Node* _Erase(_Node* n) {
			_Node* temp = nullptr;			//  削除によって置き換えられた子

			if (n == nullptr) return nullptr;

			//  子がなければ単純に除去
			if ((n->left == nullptr) && (n->right == nullptr)) {
				//  rootでないときはn->parentの子の情報を書き換える
				if (n->parent != nullptr) ((n->parent->right == n) ? n->parent->right : n->parent->left) = nullptr;
			}
			//  片方のみ子があるとき部分木で置き換える
			else if ((n->left == nullptr) && (n->right != nullptr)) {
				n->right->parent = n->parent;
				//  rootでないときはn->parentの子の情報を書き換える
				if (n->parent != nullptr) ((n->parent->right == n) ? n->parent->right : n->parent->left) = n->right;
				temp = n->right;
			}
			else if ((n->left != nullptr) && (n->right == nullptr)) {
				n->left->parent = n->parent;
				//  rootでないときはn->parentの子の情報を書き換える
				if (n->parent != nullptr) ((n->parent->right == n) ? n->parent->right : n->parent->left) = n->left;
				temp = n->left;
			}
			//  両方に子を持つとき
			else {
				_Node* temp2 = n->left;
				//  keyより小さい値の内、最大値を探索
				while (temp2->right != nullptr) temp2 = temp2->right;
				//  値の代入
				const_cast<key_type&>(n->value.first) = temp2->value.first;
				n->value.second = temp2->value.second;
				return _Erase(temp2);
			}

			//  木を調整する
			if (temp != nullptr) _Erase_support(temp, n->flag);
			else {
				//  n->parentが赤であるとき
				if (_Red(n->parent)) {
					n->parent->flag = false;
					((n->parent->right == nullptr) ? n->parent->left : n->parent->right)->flag = true;
				}
			}

			return n;
		}
		//  ノードの削除の赤黒の制御(n:削除により置き換えられたノード,flag:削除されたノードの色)
		static void _Erase_support(_Node* n, bool flag) {
			//  赤ノードならば終了
			if (flag) return;
			//  flagが黒でnが赤のときnを黒にして終了
			if (!flag && _Red(n)) { n->flag = false; return; }

			//  以下nは必ず黒

			//  rootなら黒にする
			if (n->parent == nullptr) n->flag = false;
			//  nが木の左側に存在するとき(平衡であるならn->parentの子は両方存在し、反対側の子にはさらに子が存在する)
			else if (n->parent->left == n) {
				//  n->parent->rightが赤のとき(暗黙的にn->parentが黒である)
				if (_Red(n->parent->right)) {
					//  色を変えてから回転
					n->parent->right->flag = false;
					n->parent->flag = true;
					_Rot_left(n->parent->right);
					//  再帰しなくてもできるが、else ifで分岐させるためにしておく
					_Erase_support(n, flag);
				}
				//  反対側の子の子がそれぞれ黒であるとき
				else if (!_Red(n->parent->right->left) && !_Red(n->parent->right->right)) {
					//  親が黒であるとき反対側の子を赤にする
					if (!_Red(n->parent)) {
						n->parent->right->flag = true;
						_Erase_support(n->parent, flag);
					}
					else {
						//  色変換
						n->parent->flag = false;
						n->parent->right->flag = true;
					}
				}
				//  反対側の子の内側の子のみが赤であるとき
				else if (_Red(n->parent->right->left) && !_Red(n->parent->right->right)) {
					//  色を変えてから回転
					n->parent->right->flag = true;
					n->parent->right->left->flag = false;
					_Rot_right(n->parent->right->left);
					//  再帰しなくてもできるが、else ifで分岐させるためにしておく
					_Erase_support(n, flag);
				}
				//  上の条件とは逆であるとき
				else if (!_Red(n->parent->right->left) && _Red(n->parent->right->right)) {
					//  色を変えてから回転
					n->parent->right->right->flag = false;
					_Rot_left(n->parent->right);
				}
			}
			//  nが木の右側に存在するとき
			else if (n->parent->right == n) {
				//  n->parent->leftが赤のとき(暗黙的にn->parentが黒である)
				if (_Red(n->parent->left)) {
					//  色を変えてから回転
					n->parent->left->flag = false;
					n->parent->flag = true;
					_Rot_right(n->parent->left);
					//  再帰しなくてもできるが、else ifで分岐させるためにしておく
					_Erase_support(n, flag);
				}
				//  反対側の子の子がそれぞれ黒であるとき
				else if (!_Red(n->parent->left->left) && !_Red(n->parent->left->right)) {
					//  親が黒であるとき反対側の子を赤にする
					if (!_Red(n->parent)) {
						n->parent->left->flag = true;
						_Erase_support(n->parent, flag);
					}
					else {
						//  色変換
						n->parent->flag = false;
						n->parent->left->flag = true;
					}
				}
				//  反対側の子の内側の子のみが赤であるとき
				else if (!_Red(n->parent->left->left) && _Red(n->parent->left->right)) {
					//  色を変えてから回転
					n->parent->left->flag = true;
					n->parent->left->left->flag = false;
					_Rot_left(n->parent->left->right);
					//  再帰しなくてもできるが、else ifで分岐させるためにしておく
					_Erase_support(n, flag);
				}
				//  上の条件とは逆であるとき
				else if (_Red(n->parent->left->left) && !_Red(n->parent->left->right)) {
					//  色を変えてから回転
					n->parent->left->left->flag = false;
					_Rot_right(n->parent->left);
				}
			}
		}
		//  内部イテレータ
		template <class F>
		static F _For_each(_Node* n, F f) {
			if (n->left != nullptr) _For_each(n->left, f);
			f(n->value);
			if (n->right != nullptr) _For_each(n->right, f);
			return f;
		}
	public:
		tree_map_container() {}
		virtual ~tree_map_container() = 0 {}

		virtual iterator begin() noexcept = 0;
		virtual const_iterator begin() const noexcept = 0;
		virtual iterator end() noexcept = 0;
		virtual const_iterator end() const noexcept = 0;

		void clear() {
			if (root == nullptr) return;
			_Clear(root, _allo);
			_size = 0;
			allocator_traits<Allocator>::destroy(_allo, root);
			_allo.deallocate(root, 1);
		}

		_Node* insert(const key_type& key, const mapped_type& v = mapped_type()) {
			_Node* temp;
			//  ルートは黒にして追加
			if (root == nullptr) {
				temp = _allo.allocate(1);
				allocator_traits<Allocator>::construct(_allo, temp, key, v, nullptr);
				//  rootならば無条件に黒
				temp->flag = false;
				++_size;
				return root = temp;
			}
			else {
				//  既に存在するときは値を代入して終了
				if ((temp = find(key)) != nullptr) {
					temp->value.second = v;
					return temp;
				}
				temp = _Insert(key, v, root, _allo);
			}
			++_size;
			root = _Insert_support(temp);
			return temp;
		}
		void erase(const key_type& key) {
			_Node* temp = _Erase(find(key));
			if (temp != nullptr) {
				allocator_traits<Allocator>::destroy(_allo, temp);
				_allo.deallocate(temp, 1);
				--_size;
			}
			return;
		}

		_Node* find(const key_type& key) { return _Find(key, root); }

		virtual bool empty() const noexcept = 0;

		size_type size() const { return _size; }

		template <class F>
		F for_each(F f) const { return _For_each(root, f); }
	};

	//  二分木による連想配列
	template <class Key, class T, class Compare = type_comparison<Key>, class Allocator = allocator<typename tree_map_iterator<pair<const Key, T>>::node>>
	class tree_map : public tree_map_container<pair<const Key, T>, Compare, Allocator> {
	public:
		using typename tree_map_container<pair<const Key, T>, Compare, Allocator>::value_type;
		using typename tree_map_container<pair<const Key, T>, Compare, Allocator>::iterator;
		using typename tree_map_container<pair<const Key, T>, Compare, Allocator>::const_iterator;
		using typename tree_map_container<pair<const Key, T>, Compare, Allocator>::allocator_type;
		using typename tree_map_container<pair<const Key, T>, Compare, Allocator>::size_type;

		using typename tree_map_container<pair<const Key, T>, Compare, Allocator>::_Node;
	private:
		//  代入の補助関数
		void substitution_support(_Node* n1, _Node* n2) {
			if (n2 == nullptr) return;
			n1 = _allo.allocate(1);
			allocator_traits<Allocator>::construct(_allo, n1, *n2);
			if (n->left != nullptr) substitution_support(n1->left, n2->left);
			if (n->right != nullptr) substitution_support(n1->right, n2->right);
		}
	public:
		constexpr tree_map() {}
		~tree_map() { clear(); }

		//  イテレータ位置の取得
		iterator begin() noexcept {
			_Node* temp = root;
			if (temp == nullptr) return iterator(nullptr, root);
			while (temp->left != nullptr) temp = temp->left;
			return iterator(temp, root, false);
		}
		const_iterator begin() const noexcept {
			_Node* temp = root;
			if (temp == nullptr) return to_const_iterator(iterator(nullptr, root));
			while (temp->left != nullptr) temp = temp->left;
			return to_const_iterator(iterator(temp ,root,false));
		}
		iterator end() noexcept { return iterator(nullptr, root); }
		const_iterator end() const noexcept { return to_const_iterator(iterator(nullptr, root)); }

		bool empty() const noexcept { return _size == 0; }

		//  代入
		tree_map& operator=(const tree_map& s) {
			if (s._p == this->_p) return *this;
			this->_allo = s._allo;
			this->_size = s._size;
			this->clear();
			substitution_support(this->root, s.root);
			return *this;
		}
		tree_map& operator=(tree_map&& s) {
			if (s._p == this->_p) return *this;
			this->_allo = s._allo;
			this->_size = s._size;
			this->root = s.root;
			return *this;
		}

		T& operator[](const Key& key) {
			_Node* temp;
			if ((temp = find(key)) != nullptr) return temp->value.second;
			return insert(key, T())->value.second;
		}
	};

}


#endif