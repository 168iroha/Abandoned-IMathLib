#ifndef IMATHLIB_H_MEDIA_CAMERA_HPP
#define IMATHLIB_H_MEDIA_CAMERA_HPP

#include "IMathLib/math/liner_algebra.hpp"
#include "IMathLib/media/common/enabler_object.hpp"

#undef near
#undef far


// デフォルトのnear
#ifndef IMATHLIB_F_CAMERA_DEFAULT_NEAR
#define IMATHLIB_F_CAMERA_DEFAULT_NEAR			20
#endif
// デフォルトのfar
#ifndef IMATHLIB_F_CAMERA_DEFAULT_FAR
#define IMATHLIB_F_CAMERA_DEFAULT_FAR			600
#endif
// デフォルトのカメラからの距離
#ifndef IMATHLIB_F_CAMERA_DEFAULT_LENGTH
#define IMATHLIB_F_CAMERA_DEFAULT_LENGTH		100
#endif


namespace iml {
	namespace ml {

		// カメラの定数についての名前空間
		namespace cam {
			// カメラの投影法を示す定数
			inline constexpr size_t ortho_projection = 0;				// 垂直投影
			inline constexpr size_t frustum_projection = 1;			// 透視投影
		}

		// glLoadMatrixのラップ
		inline void glLoadMatrix(const matrix4<GLfloat>& m) { ::glLoadMatrixf(m[0]); }
		inline void glLoadMatrix(const matrix4<GLdouble>& m) { ::glLoadMatrixd(m[0]); }
		inline void glLoadTransposeMatrix(const matrix4<GLfloat>& m) { ::glLoadTransposeMatrixf(m[0]); }
		inline void glLoadTransposeMatrix(const matrix4<GLdouble>& m) { ::glLoadTransposeMatrixd(m[0]); }

		// glGetFloatvおよびglGetDoublevのラップ
		inline void glGetMatrix(GLenum name, matrix4<GLfloat>& m) { ::glGetFloatv(name, m[0]); }
		inline void glGetMatrix(GLenum name, matrix4<GLdouble>& m) { ::glGetDoublev(name, m[0]); }


		// カメラ制御系
		// OpenGLでは各行列を転置して扱わなければならない
		template <class T>
		class camera {
			template <class, size_t>
			friend class light;
			// ビュー行列に関するパラメータ
			vector3<T>			target_pos_m;			// 注視点座標
			vector3<T>			coordinate_m;			// 注視点からの相対座標により定義されるカメラの座標(angle_h,angle_v,length)
			vector3<T>			front_direction_m;		// カメラの座標系
			vector3<T>			right_direction_m;		// *
			vector3<T>			up_direction_m;			// *
			// プロジェクション行列に関するパラメータ
			T					near_m, far_m;			// 3D空間の遠近
			rect<T>				rect_m;					// カメラのnear面の領域(投影先の面の領域)
			size_t				projection_m;			// 投影法
			// カメラの移動制御に関するパラメータ
			vector3<T>			trans_descartes_m;		// 線型でカメラの移動の持越し(デカルト座標)
			vector3<T>			trans_polar_m;			// 線型でカメラの移動の持越し(極座標)
			vector3<T>			trans_target_m;			// 線型で注視点の移動の持越し(デカルト座標)


			// 極座標系をデカルト座標系に変換(y-up/z-front系)
			static vector3<T> polar_convert(const T& angle_h, const T& angle_v) {
				return vector3<T>(cos(angle_v)*sin(angle_h), sin(angle_v), cos(angle_v)*cos(angle_h));
			}
			// 射影行列のセット
			/*void set_projection_matrix() {
				// 射影行列の設定GL_TRANSPOSE_PROJECTION_MATRIX
				glMatrixMode(GL_PROJECTION);
				switch (projection_m) {
				case cam::ortho_projection:
					glLoadMatrix(transpose_ortho_projection_matrix(rect_m.left, rect_m.right, rect_m.bottom, rect_m.top, near_m, far_m));
					//gluOrtho2D(rect_m.left, rect_m.right, rect_m.bottom, rect_m.top, near_m, far_m)
					break;
				case cam::frustum_projection:
					glLoadMatrix(transpose_frustum_projection_matrix(rect_m.left, rect_m.right, rect_m.bottom, rect_m.top, near_m, far_m));
					//glFrustum(rect_m.left, rect_m.right, rect_m.bottom, rect_m.top, near_m, far_m)
					break;
				}
			}
			// ビュー行列のセット
			void set_view_matrix() {
				glMatrixMode(GL_MODELVIEW);
				// y-up系だからこの順序
				glLoadMatrix(transpose_view_matrix(camera_pos(), right_direction_m, up_direction_m, front_direction_m));
				//gluLookAt(
				//	camera_pos[0], camera_pos[1], camera_pos[2],			// 視点の位置x,y,z;
				//	target_pos_m[0], target_pos_m[1], target_pos_m[2],			// 視界の中心位置の参照点座標x,y,z
				//	up_direction_m[0], up_direction_m[1], up_direction_m[2]);		// 視界の上方向のベクトルx,y,z
			}*/
			// coordinate_mからカメラの座標系の設定
			void set_camera() {
				// 値の補正
				while (this->coordinate_m[1] > pi_v<T>) this->coordinate_m[1] -= 2 * pi_v<T>;
				while (this->coordinate_m[1] < -pi_v<T>) this->coordinate_m[1] += 2 * pi_v<T>;
				while (this->coordinate_m[0] > 2 * pi_v<T>) this->coordinate_m[0] -= 2 * pi_v<T>;
				while (this->coordinate_m[0] < -2 * pi_v<T>) this->coordinate_m[0] += 2 * pi_v<T>;
				if (this->coordinate_m[2] < near_m) this->coordinate_m[2] = near_m;
				if (this->coordinate_m[2] > far_m) this->coordinate_m[2] = far_m;
				// カメラ座標系の変換
				this->front_direction_m = camera::polar_convert(this->coordinate_m[0], this->coordinate_m[1]);
				// 水平角度によって右は変わる
				if (abs(this->coordinate_m[1]) > pi_v<T> / 2) this->right_direction_m = unit(vector3<T>(0, -1, 0) % this->front_direction_m);
				else this->right_direction_m = unit(vector3<T>(0, 1, 0) % this->front_direction_m);
				this->up_direction_m = this->front_direction_m% this->right_direction_m;
			}
		public:
			constexpr camera() : near_m(IMATHLIB_F_CAMERA_DEFAULT_NEAR), far_m(IMATHLIB_F_CAMERA_DEFAULT_FAR)
				, coordinate_m(0, 0, IMATHLIB_F_CAMERA_DEFAULT_LENGTH)
				, front_direction_m(0, 0, 1), right_direction_m(1, 0, 0), up_direction_m(0, 1, 0)
				, projection_m(cam::frustum_projection) {
			}
			~camera() {}
			// カメラモードの設定
			camera& camera_mode(size_t mode) { projection_m = mode; return *this; }

			// 極座標でカメラの移動(vector3<T>(angle_h, angle_v, length))
			camera& move_camera_polar(const vector3<T>& v, const T& trans = 0) {
				vector3<T> temp = v + this->trans_polar_m;
				this->coordinate_m += temp;
				this->set_camera();

				// 次回への持ち越し量
				this->trans_polar_m = temp * trans;
				return *this;
			}
			camera& set_camera_polar(const vector3<T>& v) {
				this->coordinate_m = v;
				this->set_camera();

				// 持ち越しのリセット
				this->trans_polar_m[0] = this->trans_polar_m[1] = this->trans_polar_m[2] = 0;
				return *this;
			}
			// デカルト座標でカメラの位置の移動(flag:abs(angle_v)がpi/2を超えるかのフラグ)
			camera& move_camera_descartes(const vector3<T>& v, const T& trans = 0) {
				vector3<T> temp = v + this->trans_descartes_m;
				vector3<T> camera_pos = this->coordinate_m[2] * camera::polar_convert(this->coordinate_m[0], this->coordinate_m[1]) + temp;
				// 直交座標系から極座標系への変換
				this->coordinate_m[2] = abs(camera_pos);
				// up_direction_mが-y方向であるとき
				if (abs(this->coordinate_m[1]) > pi_v<T> / 2) {
					this->coordinate_m[1] = pi_v<T> - asin(camera_pos[1] / this->coordinate_m[2]);
					if ((camera_pos[0] == 0) && (camera_pos[2] == 0)) this->coordinate_m[0] = pi_v<T>;
					else {
						// 演算誤差を考慮する必要がある
						auto temp2 = camera_pos[2] / sqrt(camera_pos[0] * camera_pos[0] + camera_pos[2] * camera_pos[2]);
						if (temp2 <= -1) this->coordinate_m[0] = pi_v<T> * (1 + sgn(camera_pos[0]));
						else if (1 <= temp2) this->coordinate_m[0] = pi_v<T>;
						else this->coordinate_m[0] = pi_v<T> + sgn(camera_pos[0])*acos(temp2);
					}
				}
				else {
					// 演算誤差を考慮する
					auto temp3 = camera_pos[1] / this->coordinate_m[2];
					if (temp3 <= -1) this->coordinate_m[1] = -pi_v<T> / 2;
					else if (temp3 >= 1) this->coordinate_m[1] = pi_v<T> / 2;
					else this->coordinate_m[1] = asin(camera_pos[1] / this->coordinate_m[2]);
					if ((camera_pos[0] == 0) && (camera_pos[2] == 0)) this->coordinate_m[0] = 0;
					else {
						// 演算誤差を考慮する
						auto temp2 = camera_pos[2] / sqrt(camera_pos[0] * camera_pos[0] + camera_pos[2] * camera_pos[2]);
						if (temp2 <= -1) this->coordinate_m[0] = sgn(camera_pos[0]) * pi_v<T>;
						else if (1 <= temp2) this->coordinate_m[0] = 0;
						else this->coordinate_m[0] = sgn(camera_pos[0])*acos(temp2);
					}
				}
				this->set_camera();

				// 次回への持ち越し量
				this->trans_descartes_m = temp * trans;
				return *this;
			}
			camera& set_camera_descartes(const vector3<T>& v) {
				// 直交座標系から極座標系への変換
				this->coordinate_m[2] = abs(v);
				// up_direction_mが-y方向であるとき
				if (abs(this->coordinate_m[1]) > pi_v<T> / 2) {
					this->coordinate_m[1] = pi_v<T> -asin(v[1] / this->coordinate_m[2]);
					if ((v[0] == 0) && (v[2] == 0)) this->coordinate_m[0] = pi_v<T>;
					else {
						// 演算誤差を考慮する必要がある
						auto temp2 = v[2] / sqrt(v[0] * v[0] + v[2] * v[2]);
						if (temp2 <= -1) this->coordinate_m[0] = pi_v<T> * (1 + sgn(v[0]));
						else if (1 <= temp2) this->coordinate_m[0] = pi_v<T>;
						else this->coordinate_m[0] = pi_v<T> + sgn(v[0])*acos(temp2);
					}
				}
				else {
					this->coordinate_m[1] = asin(v[1] / this->coordinate_m[2]);
					if ((v[0] == 0) && (v[2] == 0)) this->coordinate_m[0] = 0;
					else {
						// 演算誤差を考慮する必要がある
						auto temp2 = v[2] / sqrt(v[0] * v[0] + v[2] * v[2]);
						if (temp2 <= -1) this->coordinate_m[0] = sgn(v[0]) * pi_v<T>;
						else if (1 <= temp2) this->coordinate_m[0] = 0;
						else this->coordinate_m[0] = sgn(v[0])*acos(temp2);
					}
				}
				this->set_camera();

				// 持ち越しのリセット
				this->trans_descartes_m[0] = this->trans_descartes_m[1] = this->trans_descartes_m[2] = 0;
				return *this;
			}
			// カメラのターゲットの平行移動(カメラの極座標系をも連動する)
			camera& move_target(const vector3<T>& v, const T& trans = 0) {
				vector3<T> temp = v + this->trans_target_m;
				this->target_pos_m += temp;

				// 次回への持ち越し量
				this->trans_target_m = temp * trans;
				return *this;
			}
			// カメラのターゲットのビューポート面に対する平行移動
			camera& move_target(const vector2<T>& v, const T& trans = 0) {
				vector3<T> temp = (v[0] * this->right_direction_m + v[1] * this->up_direction_m) + this->trans_target_m;
				this->target_pos_m += temp;

				// 次回への持ち越し量
				this->trans_target_m = temp * trans;
				return *this;
			}
			camera& set_target(const vector3<T>& v) {
				this->target_pos_m = v;

				// 持ち越しのリセット
				this->trans_target_m[0] = this->trans_target_m[1] = this->trans_target_m[2] = 0;
				return *this;
			}

			// 視野角からnear面の領域の構築
			camera& viewing_angle(const T& angle_x, const T& angle_y) {
				this->rect_m.right = this->near_m*tan(angle_x / 2);
				this->rect_m.left = -this->rect_m.right;
				this->rect_m.top = this->near_m*tan(angle_y / 2);
				this->rect_m.bottom = -this->rect_m.top;
				return *this;
			}
			// x方向の視野角とアスペクト比からnear面の領域の構築
			camera& viewing_angle_magnific(const T& angle_x, const T& ratio) {
				this->rect_m.right = this->near_m*tan(angle_x / 2);
				this->rect_m.left = -this->rect_m.right;
				this->rect_m.top = this->near_m*tan(angle_x*ratio / 2);
				this->rect_m.bottom = -this->rect_m.top;
				return *this;
			}
			// 直接領域を構築
			camera& surface_rect(const rect<T>& rect) { this->rect_m = rect; return *this; }

			// カメラの遠近の設定
			camera& near(const T& n) { this->near_m = n; return *this; }
			camera& far(const T& f) { this->far_m = f; return *this; }

			// 各種カメラの情報の取得
			const T& near() const { return this->near_m; }
			const T& far() const { return this->far_m; }
			const vector3<T>& target_pos() const { return this->target_pos_m; }
			const T& angle_h() const { return this->coordinate_m[0]; }
			const T& angle_v() const { return this->coordinate_m[1]; }
			const T& length() const { return this->coordinate_m[2]; }
			vector3<T> camera_pos() const { return this->target_pos_m + this->coordinate_m[2] * camera::polar_convert(this->coordinate_m[0], this->coordinate_m[1]); }

			// 射影行列の取得
			matrix4<T> projection_matrix() const {
				switch (this->projection_m) {
				case cam::ortho_projection:
					return iml::ortho_projection_matrix(this->rect_m.left, this->rect_m.right, this->rect_m.bottom, this->rect_m.top, this->near_m, this->far_m);
				case cam::frustum_projection:
					return iml::frustum_projection_matrix(this->rect_m.left, this->rect_m.right, this->rect_m.bottom, this->rect_m.top, this->near_m, this->far_m);
				default:
					return matrix4<T>();
				}
			}
			// ビュー行列の取得
			matrix4<T> view_matrix() const {
				return iml::view_matrix(this->camera_pos(), this->right_direction_m, this->up_direction_m, this->front_direction_m);
			}
		};


		// カメラ(ビュー行列及びプロジェクション行列)の設定
		template <class T>
		struct CAMERA_OBJECT : ENABLER_OBJECT_BASE<pair<matrix4<T>, matrix4<T>>, CAMERA_OBJECT<T>> {
			// pairの第一成分は射影行列，第二成分はビュー行列
			using base_class = ENABLER_OBJECT_BASE<pair<matrix4<T>, matrix4<T>>, CAMERA_OBJECT>;
			friend base_class;

		private:
			// 初期状態のためのコンストラクタ
			CAMERA_OBJECT(enabler_object_init_tag) : base_class() { base_class::init_state_construct(); }
			CAMERA_OBJECT(enabler_object_init_tag, const camera<T>& c) : base_class(pair<matrix4<T>, matrix4<T>>(c.projection_matrix(), c.view_matrix())) {
				base_class::init_state_construct();
			}
		public:
			// 通常のコンストラクタ
			CAMERA_OBJECT() : base_class() { base_class::construct(); }
			// 状態は共有しない
			CAMERA_OBJECT(const CAMERA_OBJECT& c) : base_class(pair<matrix4<T>, matrix4<T>>(c.projection_matrix(), c.view_matrix())) {
				base_class::construct();
			}
			CAMERA_OBJECT(CAMERA_OBJECT&& c) noexcept : base_class(std::move(c.obj_m)) {}
			CAMERA_OBJECT(const camera<T>& c) : base_class(pair<matrix4<T>, matrix4<T>>(c.projection_matrix(), c.view_matrix())) {
				base_class::construct();
			}
			CAMERA_OBJECT(const matrix4<T>& projection, const matrix4<T>& view) : base_class(pair<matrix4<T>, matrix4<T>>(projection, view)) {
				base_class::construct();
			}
			~CAMERA_OBJECT() {
				// 状態を持つならば処理する
				if (this->obj_m.get() != nullptr) base_class::destroy();
			}

			// ENABLER_OBJECT_CONTROLのエイリアス
			using base_class::enabler_object_control_t;

			CAMERA_OBJECT& operator=(const CAMERA_OBJECT& c) {
				this->obj_m = c.obj_m;
				return *this;
			}
			CAMERA_OBJECT& operator=(CAMERA_OBJECT&& c) {
				this->obj_m = std::move(c.obj_m);
				return *this;
			}

			// 状態を更新するメソッド
			void push_state() {
				glMatrixMode(GL_PROJECTION);
				glPushMatrix();
				glLoadTransposeMatrix(this->projection_matrix());
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadTransposeMatrix(this->view_matrix());
			}
			// 状態を外すメソッド
			void pop_state() {
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
			}
			// 状態を設定するメソッド
			void set_state() {
				glMatrixMode(GL_PROJECTION);
				glLoadTransposeMatrix(this->obj_m->first);
				glMatrixMode(GL_MODELVIEW);
				glLoadTransposeMatrix(this->obj_m->second);
			}

			// 各種行列の取得
			const matrix4<T>& projection_matrix() const { return this->obj_m->first; }
			const matrix4<T>& view_matrix() const { return this->obj_m->second; }

			// 標準状態のカメラの取得
			static CAMERA_OBJECT<T>& default_camera() { return *static_cast<CAMERA_OBJECT<T>*>(base_class::default_m); }
		};
		template <class T>
		inline CAMERA_OBJECT<T> CREATE_CAMERA_OBJECT(const camera<T>& c) {
			return CAMERA_OBJECT<T>(c);
		}
		// 現在のカメラの状態をcと共通化
		template <class T>
		inline void set_camera(const CAMERA_OBJECT<T>& c) {
			using enabler_object_control_t = typename CAMERA_OBJECT<T>::enabler_object_control_t;
			enabler_object_control_t::inst()->top()->set(c);
		}
		// 現在のカメラの状態をobjにより変更
		template <class T>
		inline void set_camera(const camera<T>& c) {
			using enabler_object_control_t = typename CAMERA_OBJECT<T>::enabler_object_control_t;
			enabler_object_control_t::inst()->top()->set(pair<matrix4<T>, matrix4<T>>(c.projection_matrix(), c.view_matrix()));
		}
		// 現在のビュー行列およびプロジェクション行列の取得
		template <class T>
		inline matrix4<T> projection_matrix() {
			using enabler_object_control_t = typename CAMERA_OBJECT<T>::enabler_object_control_t;
			return static_cast<CAMERA_OBJECT<T>*>(enabler_object_control_t::inst()->top())->projection_matrix();
		}
		template <class T>
		inline matrix4<T> view_matrix() {
			using enabler_object_control_t = typename CAMERA_OBJECT<T>::enabler_object_control_t;
			return static_cast<CAMERA_OBJECT<T>*>(enabler_object_control_t::inst()->top())->view_matrix();
		}
		// 標準状態のカメラの取得
		template <class T>
		inline CAMERA_OBJECT<T>& default_camera() { return CAMERA_OBJECT<T>::default_camera(); }
		// カメラの初期化(これは厳密にカメラ利用前に実行しなければならないように定義)
		template <class T>
		inline void init_camera() {
			CAMERA_OBJECT<T>::init();
		}
		template <class T>
		inline void init_camera(const camera<T>& c) {
			CAMERA_OBJECT<T>::init(pair<matrix4<T>, matrix4<T>>(c.projection_matrix(), c.view_matrix()));
		}
		// カメラの終了
		template <class T>
		inline void quit_camera() {
			CAMERA_OBJECT<T>::quit();
		}
#define IMATHLIB_CAMERA(NAME, CAMERA)	IMATHLIB_ENABLER_OBJECT(auto NAME = ::iml::ml::CREATE_CAMERA_OBJECT(CAMERA))

		// ビューポートの設定
		struct VIEWPORT_OBJECT : ENABLER_OBJECT_BASE<rect<int_t>, VIEWPORT_OBJECT> {
			using base_class = ENABLER_OBJECT_BASE<rect<int_t>, VIEWPORT_OBJECT>;
			friend base_class;

		private:
			// スケーリングが行われたかを示すbool値
			bool is_scaling_m = false;

			// 初期状態のためのコンストラクタ
			VIEWPORT_OBJECT(enabler_object_init_tag) : base_class() { base_class::init_state_construct(); }
			VIEWPORT_OBJECT(enabler_object_init_tag, const rect<int_t>& r) : base_class(r) { base_class::init_state_construct(); }
		public:
			// 通常のコンストラクタ
			VIEWPORT_OBJECT() : base_class() { base_class::construct(); }
			// 状態は共有しない
			VIEWPORT_OBJECT(const VIEWPORT_OBJECT& v) : base_class(v.viewport_rect()) { base_class::construct(); }
			VIEWPORT_OBJECT(VIEWPORT_OBJECT&& v) noexcept : base_class(std::move(v.obj_m)) {}
			VIEWPORT_OBJECT(const rect<int_t>& r) : base_class(r) { base_class::construct(); }
			~VIEWPORT_OBJECT() {
				// 状態を持つならば処理する
				if (this->obj_m.get() != nullptr) base_class::destroy();
				/*if (this->is_scaling_m) {
					glMatrixMode(GL_MODELVIEW);
					glPopMatrix();
				}*/
			}

			// ENABLER_OBJECT_CONTROLのエイリアス
			using base_class::enabler_object_control_t;

			VIEWPORT_OBJECT& operator=(const VIEWPORT_OBJECT& v) {
				this->obj_m = v.obj_m;
				return *this;
			}
			VIEWPORT_OBJECT& operator=(VIEWPORT_OBJECT&& v) noexcept {
				this->obj_m = std::move(v.obj_m);
				return *this;
			}

			// 状態を更新するメソッド
			void push_state() {
				glViewport(*(this->obj_m));
			}
			// 状態を外すメソッド
			void pop_state() {}
			// 状態を設定するメソッド
			void set_state() {
				glViewport(*(this->obj_m));
			}

			// スクリーンに対してビューポート領域が正規化されるようにビュー行列をスケーリング
			void unit_scaling() {
				/*if (!this->is_scaling_m && (this->camera_m != nullptr)) {
					this->is_scaling_m = true;
					glMatrixMode(GL_MODELVIEW);
					glPushMatrix();
					glLoadTransposeMatrix(this->camera_m->view_matrix());
					glScalef(float(screen_rect::base_width()) / (this->obj_m->right - this->obj_m->left), float(screen_rect::base_height()) / (this->obj_m->bottom - this->obj_m->top), 1);
				}*/
			}

			// ビューポート行列の取得
			template <class T>
			matrix4<T> viewport_matrix() const {
				return iml::viewport_matrix<T>(this->obj_m->left, this->obj_m->top, this->obj_m->right - this->obj_m->left, this->obj_m->bottom - this->obj_m->top, 0, 1);
			}
			// ビューポート領域の取得
			const rect<int_t>& viewport_rect() const {
				return *(this->obj_m);
			}
		};
		inline VIEWPORT_OBJECT CREATE_VIEWPORT_OBJECT(const rect<int_t>& r) {
			return VIEWPORT_OBJECT(r);
		}
		// 現在のビューポートの状態をvと共通化
		inline void set_viewport(const VIEWPORT_OBJECT& v) {
			using enabler_object_control_t = typename VIEWPORT_OBJECT::enabler_object_control_t;
			enabler_object_control_t::inst()->top()->set(v);
		}
		// 現在のビューポートの状態をrにより変更
		inline void set_viewport(const rect<int_t>& r) {
			using enabler_object_control_t = typename VIEWPORT_OBJECT::enabler_object_control_t;
			enabler_object_control_t::inst()->top()->set(r);
		}
		inline void set_viewport(int_t left, int_t right, int_t bottom, int_t top) {
			using enabler_object_control_t = typename VIEWPORT_OBJECT::enabler_object_control_t;
			enabler_object_control_t::inst()->top()->set(rect<int_t>(left, right, bottom, top));
		}
		// 現在のビューポート行列の取得
		template <class T>
		inline matrix4<T> viewport_matrix() {
			using enabler_object_control_t = typename VIEWPORT_OBJECT::enabler_object_control_t;
			return static_cast<VIEWPORT_OBJECT*>(enabler_object_control_t::inst()->top())->viewport_matrix<T>();
		}
		// 現在のビューポート領域の取得
		template <class T>
		inline const rect<int_t>& viewport_rect() {
			using enabler_object_control_t = typename VIEWPORT_OBJECT::enabler_object_control_t;
			return static_cast<VIEWPORT_OBJECT*>(enabler_object_control_t::inst()->top())->viewport_rect();
		}
		// 現在のワールド座標から現在のスクリーン座標の取得
		template <class T>
		vector3<T> world_to_screen(const vector3<T>& v) {
			// ワールド座標系からの投影は左下を(0, 0)として扱っているため、その補正(まだしてない)
			auto temp = iml::ml::viewport_matrix<T>() * iml::ml::projection_matrix<T>() * iml::ml::view_matrix<T>()
				* vector4<T>(v[0], v[1], v[2], 1);
			return vector3<T>(temp[0] / temp[3], temp[1] / temp[3], temp[2] / temp[3]);
		}
		template <class T>
		vector3<T> world_to_screen(const vector3<T>& v, const CAMERA_OBJECT<T>& ca, const VIEWPORT_OBJECT& vp) {
			// ワールド座標系からの投影は左下を(0, 0)として扱っているため、その補正(まだしてない)
			auto temp = vp.viewport_matrix<T>() * ca.projection_matrix() * ca.view_matrix()
				* vector4<T>(v[0], v[1], v[2], 1);
			return vector3<T>(temp[0] / temp[3], temp[1] / temp[3], temp[2] / temp[3]);
		}
		// 現在のスクリーン座標から現在のワールド座標の取得
		template <class T>
		vector3<T> screen_to_world(const vector3<T>& v) {
			// ワールド座標系からの投影は左下を(0, 0)として扱っているため、その補正(まだしてない)
			auto temp = inverse_matrix(iml::ml::viewport_matrix<T>() * iml::ml::projection_matrix<T>() * iml::ml::view_matrix<T>())
				* vector4<T>(v[0], v[1], v[2], 1);
			return vector3<T>(temp[0] / temp[3], temp[1] / temp[3], temp[2] / temp[3]);
		}
		template <class T>
		vector3<T> screen_to_world(const vector3<T>& v, const CAMERA_OBJECT<T>& ca, const VIEWPORT_OBJECT& vp) {
			// ワールド座標系からの投影は左下を(0, 0)として扱っているため、その補正(まだしてない)
			auto temp = inverse_matrix(vp.viewport_matrix<T>() * ca.projection_matrix() * ca.view_matrix())
				* vector4<T>(v[0], v[1], v[2], 1);
			return vector3<T>(temp[0] / temp[3], temp[1] / temp[3], temp[2] / temp[3]);
		}
		// ビューポートの初期化
		inline void init_viewport() {
			// rootウィンドウのウィンドウサイズで初期化
			VIEWPORT_OBJECT::init();
		}
		inline void init_viewport(const rect<int_t>& r) {
			VIEWPORT_OBJECT::init(r);
		}
		inline void init_viewport(int_t left, int_t right, int_t bottom, int_t top) {
			VIEWPORT_OBJECT::init(rect<int_t>(left, right, bottom, top));
		}
		// ビューポートの終了
		inline void quit_viewport() {
			VIEWPORT_OBJECT::quit();
		}
#define IMATHLIB_VIEWPORT(NAME, RECT)	IMATHLIB_ENABLER_OBJECT(auto NAME = ::iml::ml::CREATE_VIEWPORT_OBJECT(iml::ml::rect<iml::int_t>##RECT))

		// スクリーンモード
		struct SCREEN_MODE_OBJECT {
		private:
			VIEWPORT_OBJECT		viewport_m;
		public:
			SCREEN_MODE_OBJECT(const VIEWPORT_OBJECT& v) : SCREEN_MODE_OBJECT(v.viewport_rect()) {}
			SCREEN_MODE_OBJECT(const rect<int_t>& r) : viewport_m(r) {
				// スクリーン領域で平行投影にする
				glMatrixMode(GL_PROJECTION);
				glPushMatrix();
				glLoadTransposeMatrix(iml::ortho_projection_matrix<float>(0, r.right - r.left, r.bottom - r.top, 0, -1, 1));
				//gluOrtho2D(0, r.right - r.left, r.bottom - r.top, 0);
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadIdentity();
			}
			~SCREEN_MODE_OBJECT() {
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
			}

			// enabler objectの参照の取得
			VIEWPORT_OBJECT& viewport_object() { return this->viewport_m; }
			const VIEWPORT_OBJECT& viewport_object() const { return this->viewport_m; }
		};
		inline SCREEN_MODE_OBJECT CREATE_SCREEN_MODE_OBJECT(const rect<int_t>& r) {
			return SCREEN_MODE_OBJECT(r);
		}
#define IMATHLIB_SCREEN_MODE(NAME, RECT)				IMATHLIB_ENABLER_OBJECT(auto NAME = ::iml::ml::CREATE_SCREEN_MODE_OBJECT(iml::ml::rect<iml::int_t>##RECT))

	}
}

#endif