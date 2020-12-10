#define IMATHLIB_F_MEDIA_LOG_OUTPUT

#define IMATHLIB_F_MEDIA_TTF
#define IMATHLIB_F_MEDIA_INPUT
#define IMATHLIB_F_GLEW

#define IMATHLIB_F_MEDIA_UI

#include <iostream>

#include <IMathLib/IMathLib.hpp>
#include <IMathLib/IMediaLib.hpp>
#include <IMathLib/preprocessor.hpp>

#include "../resource.h"

//ボーンモデルのテスト(xyz:基底,pos:絶対位置,length:ボーンの長さ)
template <class T>
inline void draw_bone(const iml::vector3<T>& x, const iml::vector3<T>& y, const iml::vector3<T>& z, const iml::vector3<T>& pos, const T& length) {
	//基底情報から頂点配列を作成
	static const iml::vector3<T>	bone_vtx[6] = { {0,0,0},{0,1,0},{-0.1f,0.2f,0.1f},{0.1f,0.2f,0.1f},{0.1f,0.2f,-0.1f},{-0.1f,0.2f,-0.1f} };
	iml::vector3<T> vtx[6] = { bone_vtx[0] ,bone_vtx[1] ,bone_vtx[2] ,bone_vtx[3] ,bone_vtx[4] ,bone_vtx[5] };
	unsigned int index[] = { 2,3,1,2,0,3,4,5,1,4,0,5,2 };
	iml::matrix3<T>	tm;
	for (size_t i = 0; i < 3; ++i) {
		tm[i][0] = x[i];
		tm[i][1] = y[i];
		tm[i][2] = z[i];
	}
	tm *= length;
	vtx[0] = tm*vtx[0];
	vtx[1] = tm*vtx[1];
	vtx[2] = tm*vtx[2];
	vtx[3] = tm*vtx[3];
	vtx[4] = tm*vtx[4];
	vtx[5] = tm*vtx[5];

	glPushMatrix();

	glEnableClientState(GL_VERTEX_ARRAY);
	//アルファブレンドをする
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glVertexPointer(3, GL_FLOAT, sizeof(iml::vector3<T>), &vtx[0][0]);

	glTranslatef(pos[0], pos[1], pos[2]);
	//glRotatef(angle, axis_x, axis_y, asix_z);
	glDrawElements(GL_LINE_LOOP, 13, GL_UNSIGNED_INT, index);

	glDisable(GL_BLEND);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
}

namespace imf = iml::ml;


// 棒人間のためのボーン
template <class Float>
struct stick_figure_bone {
	Float				angle_h_m;					// 現在の水平角
	Float				angle_v_m;					// 現在の垂直角
	Float				angle_s_m;					// 現在の捩れ角
	Float				length_m;					// ボーンの長さ
	Float				angle_h_limit_m[2];			// 水平角の制限
	Float				angle_v_limit_m[2];			// 垂直角の制限
	Float				angle_s_limit_m[2];			// 捩れ角の制限
	iml::vector3<Float>	default_axes_m[3];			// デフォルトの座標軸(x,y,z)

	constexpr stick_figure_bone() : angle_h_m(0), angle_v_m(0), angle_s_m(0), length_m(0)
		, angle_h_limit_m{ 0,0 }, angle_v_limit_m{ 0,0 }, angle_s_limit_m{ 0,0 }, default_axes_m{} {}
	constexpr stick_figure_bone(const Float& length) : angle_h_m(0), angle_v_m(0), angle_s_m(0), length_m(length)
		, angle_h_limit_m{ 0,0 }, angle_v_limit_m{ 0,0 }, angle_s_limit_m{ 0,0 }, default_axes_m{} {}
	constexpr stick_figure_bone(const Float& angle_h, const Float& angle_v, const Float& angle_s, const Float& length) : angle_h_m(angle_h), angle_v_m(angle_v), angle_s_m(angle_s), length_m(length)
		, angle_h_limit_m{ 0,0 }, angle_v_limit_m{ 0,0 }, angle_s_limit_m{ 0,0 }, default_axes_m{} {}

	// 各々の角度制限の設定
	stick_figure_bone& angle_degree_h_limit(const Float& mi, const Float& ma) {
		this->angle_h_limit_m[0] = (iml::max)(mi, -90)*iml::pi_v<Float> / 180; this->angle_h_limit_m[1] = (iml::min)(ma, 90)*iml::pi_v<Float> / 180;
		return *this;
	}
	stick_figure_bone& angle_radian_h_limit(const Float& mi, const Float& ma) {
		this->angle_h_limit_m[0] = (iml::max)(mi, -iml::pi_v<Float> / 2); this->angle_h_limit_m[1] = (iml::min)(ma, iml::pi_v<Float> / 2);
		return *this;
	}
	stick_figure_bone& angle_degree_v_limit(const Float& mi, const Float& ma) {
		this->angle_v_limit_m[0] = (iml::max)(mi, -180)*iml::pi_v<Float> / 180; this->angle_v_limit_m[1] = (iml::min)(ma, 180)*iml::pi_v<Float> / 180;
		return *this;
	}
	stick_figure_bone& angle_radian_v_limit(const Float& mi, const Float& ma) {
		this->angle_v_limit_m[0] = (iml::max)(mi, -iml::pi_v<Float>); this->angle_v_limit_m[1] = (iml::min)(ma, iml::pi_v<Float>);
		return *this;
	}
	stick_figure_bone& angle_degree_s_limit(const Float& mi, const Float& ma) {
		this->angle_s_limit_m[0] = (iml::max)(mi, -180)*iml::pi_v<Float> / 180; this->angle_s_limit_m[1] = (iml::min)(ma, 180)*iml::pi_v<Float> / 180;
		return *this;
	}
	stick_figure_bone& angle_radian_s_limit(const Float& mi, const Float& ma) {
		this->angle_s_limit_m[0] = (iml::max)(mi, -iml::pi_v<Float>); this->angle_s_limit_m[1] = (iml::min)(ma, iml::pi_v<Float>);
		return *this;
	}
	// 角度制限を適用した角度の取得
	Float angle_degree_h_limit(const Float& angle) {
		return (iml::min)((iml::max)(this->angle_h_limit_m[0] * iml::pi_v<Float> / 180, angle), this->angle_h_limit_m[1]) * 180 / iml::pi_v<Float>;
	}
	Float angle_radian_h_limit(const Float& angle) {
		return (iml::min)((iml::max)(this->angle_h_limit_m[0], angle), this->angle_h_limit_m[1]);
	}
	Float angle_degree_v_limit(const Float& angle) {
		return (iml::min)((iml::max)(this->angle_v_limit_m[0] * iml::pi_v<Float> / 180, angle), this->angle_v_limit_m[1]) * 180 / iml::pi_v<Float>;
	}
	Float angle_radian_v_limit(const Float& angle) {
		return (iml::min)((iml::max)(this->angle_v_limit_m[0], angle), this->angle_v_limit_m[1]);
	}
	Float angle_degree_s_limit(const Float& angle) {
		return (iml::min)((iml::max)(this->angle_s_limit_m[0] * iml::pi_v<Float> / 180, angle), this->angle_s_limit_m[1]) * 180 / iml::pi_v<Float>;
	}
	Float angle_radian_s_limit(const Float& angle) {
		return (iml::min)((iml::max)(this->angle_s_limit_m[0], angle), this->angle_s_limit_m[1]);
	}
	// ボーンの座標系の設定(yはボーンの標準方向)
	stick_figure_bone& default_axes(const iml::vector3<Float>& x, const iml::vector3<Float>& y, const iml::vector3<Float>& z) {
		this->default_axes_m[0] = iml::unit(x); this->default_axes_m[1] = iml::unit(y); this->default_axes_m[2] = iml::unit(z);
		return *this;
	}
	stick_figure_bone& default_axes(const iml::vector3<Float>& bone_dir, const iml::vector3<Float>& bend_dir) {
		this->default_axes_m[1] = iml::unit(bone_dir); this->default_axes_m[2] = iml::unit(bend_dir);
		// x方向はy方向とz方向との外積により与えられる
		this->default_axes_m[0] = this->default_axes_m[1] % this->default_axes_m[2];
		return *this;
	}

	// 現在の姿勢における回転行列の取得
	iml::matrix3<Float> rotation_matrix() const {
		// iml::rotation_vector3(this->default_axes_m[2], this->default_axes_m[1], this->angle_h_m) (z軸を水平角だけ回転)
		// this->default_axes_m[1] % (垂直角のための回転軸の計算)
		// iml::rotation_matrix3(～, this->angle_v_m) (垂直角の回転行列の取得)
		// *iml::rotation_matrix3(this->default_axes_m[1], this->angle_s_m) (捩れ角の回転行列をかける)
		return iml::rotation_matrix3(this->default_axes_m[1] % iml::rotation_vector3(this->default_axes_m[2], this->default_axes_m[1], this->angle_h_m), this->angle_v_m)*iml::rotation_matrix3(this->default_axes_m[1], this->angle_s_m);
	}
	iml::matrix3<Float> rotation_matrix_hv() const {
		return iml::rotation_matrix3(this->default_axes_m[1] % iml::rotation_vector3(this->default_axes_m[2], this->default_axes_m[1], this->angle_h_m), this->angle_v_m);
	}
	iml::matrix3<Float> rotation_matrix(const Float& angle_s) const {
		return iml::rotation_matrix3(this->default_axes_m[1] % iml::rotation_vector3(this->default_axes_m[2], this->default_axes_m[1], this->angle_h_m), this->angle_v_m)*iml::rotation_matrix3(this->default_axes_m[1], angle_s);
	}
	iml::matrix3<Float> rotation_matrix(const Float& angle_h, const Float& angle_v, const Float& angle_s) const {
		return iml::rotation_matrix3(this->default_axes_m[1] % iml::rotation_vector3(this->default_axes_m[2], this->default_axes_m[1], angle_h), angle_v)*iml::rotation_matrix3(this->default_axes_m[1], angle_s);
	}
	iml::matrix3<Float> rotation_matrix_hv(const Float& angle_h, const Float& angle_v) const {
		return iml::rotation_matrix3(this->default_axes_m[1] % iml::rotation_vector3(this->default_axes_m[2], this->default_axes_m[1], angle_h), angle_v);
	}
};

// 棒人間のためのIK(NはIKの対象となるボーンの数)
template <class Float, size_t N>
class stick_figure_IK_engine {
	stick_figure_bone<Float>*		bone_list_m[N];			// ボーンの参照
	Float							angle_h_m[N];			// 水平角度の計算結果
	Float							angle_v_m[N];			// 垂直角度の計算結果
public:
	// ボーン情報の参照の取得
	template <class... Types>
	stick_figure_IK_engine(Types&... args) : bone_list_m{ std::addressof(args)... }, angle_h_m{}, angle_v_m{} {}

	// IKの計算
	void calculation(const iml::vector3<Float>& target, const std::array<Float, N>& angle_s, const iml::vector3<Float>& trans = iml::vector3<Float>(), const iml::matrix3<Float>& r = iml::identity_matrix<Float, 3>(), const Float& epsilon2 = 1) {
		// ボーンの各々の絶対座標のテーブル(ボーン単体は(position_table[i + 1] - position_table[i])で表現可能)
		iml::vector3<Float> position_table[N + 1];
		position_table[0] = trans;
		iml::matrix3<Float> r2(r);
		for (size_t i = 0; i < N; ++i) {
			r2 *=  bone_list_m[i]->rotation_matrix(angle_s[i]);
			position_table[i + 1] = position_table[i] + r2 * (bone_list_m[i]->length_m * bone_list_m[i]->default_axes_m[1]);
			// 初期角度の構築
			angle_h_m[i] = bone_list_m[i]->angle_h_m;
			angle_v_m[i] = bone_list_m[i]->angle_v_m;
		}

		// IKによる収束のためのベクトル(初期状態はボーンの始点からみたターゲット)
		iml::vector3<Float> prev_vec = target - trans;

		// ボーンの終端とターゲットの自己内積が大きいかつ変化量が大きいときは再計算
		while ((epsilon2 < iml::abs2(position_table[N] - target)) && (epsilon2 < iml::abs2(prev_vec - (position_table[N] - position_table[0])))) {
			prev_vec = position_table[N] - position_table[0];
			r2 = r;
			for (size_t i = 0; i < N; ++i) {
				// 座標軸を計算(元の座標軸がボーンの基準座標のものであるため)
				iml::vector3<Float> e1(r2*bone_list_m[i]->default_axes_m[0]), e2(r2*bone_list_m[i]->default_axes_m[1]), e3(r2*bone_list_m[i]->default_axes_m[2]);
				// 各々の相対ベクトルの取得
				iml::vector3<Float> relative_target = target - position_table[i];
				iml::vector3<Float> relative_boneend = position_table[N] - position_table[i];
				Float temp = iml::unit(relative_target) * iml::unit(relative_boneend);
				// 2つの相対ベクトルが垂直かつターゲットよりもボーンの末端の方が遠くに存在するとき
				if (temp == 1) {
					if (iml::abs2(relative_target) < iml::abs2(relative_boneend)) {
						// 垂直角度が正の方向に動かすことが可能なとき
						if (bone_list_m[i]->angle_v_limit_m[1] - angle_v_m[i] > 0)
							angle_v_m[i] = bone_list_m[i]->angle_radian_v_limit(angle_v_m[i] + iml::pi_v<Float> / 2);
						// 垂直角度が負の方向に動かすことが可能なとき
						else if (bone_list_m[i]->angle_v_limit_m[0] - angle_v_m[i] < 0)
							angle_v_m[i] = bone_list_m[i]->angle_radian_v_limit(angle_v_m[i] - iml::pi_v<Float> / 2);
						// 水平角度は動かすことが可能であるとき
						else if (angle_v_m[i] != 0) {
							// 水平角度が正の方向に動かすことが可能なとき
							if (bone_list_m[i]->angle_h_limit_m[1] - angle_h_m[i] > 0)
								angle_h_m[i] = bone_list_m[i]->angle_radian_h_limit(angle_h_m[i] + iml::pi_v<Float> / 2);
							// 水平角度が負の方向に動かすことが可能なとき
							else if (bone_list_m[i]->angle_h_limit_m[0] - angle_h_m[i] < 0)
								angle_h_m[i] = bone_list_m[i]->angle_radian_h_limit(angle_h_m[i] - iml::pi_v<Float> / 2);
							// 角度を動かすことができないとき
							else {
								r2 *= bone_list_m[i]->rotation_matrix_hv(angle_h_m[i], angle_v_m[i]);
								continue;
							}
						}
						// 角度を動かすことができないとき
						else {
							r2 *= bone_list_m[i]->rotation_matrix_hv(angle_h_m[i], angle_v_m[i]);
							continue;
						}

						// 以下角度制限が実行された場合とほぼ同じコード

						// 新しい回転行列により変換されたベクトル
						iml::vector3<Float> x = iml::rotation_matrix3(e2 % iml::rotation_vector3(e3, e2, angle_h_m[i]), angle_v_m[i])*(bone_list_m[i]->length_m * e2);
						relative_target = iml::unit(x);
						relative_boneend = iml::unit(position_table[i + 1] - position_table[i]);
						// 演算誤差を考慮して回転角の計算
						temp = relative_target * relative_boneend;
						if (temp >= 1) temp = 0;
						else if (temp <= -1) temp = iml::pi_v<Float>;
						else temp = std::acos(temp);
						// このステップでボーンを変形する回転行列
						iml::matrix3<Float> rotation = iml::rotation_matrix3(iml::unit(relative_boneend%relative_target), temp);

						// 逐次ボーンを回転させる
						iml::vector3<Float> temp_vec(position_table[i + 1]);
						position_table[i + 1] = position_table[i] + x;
						for (size_t j = i + 1; j < N; ++j) {
							iml::vector3<Float> temp_vec2(position_table[j + 1]);
							position_table[j + 1] = position_table[j] + rotation * (position_table[j + 1] - temp_vec);
							temp_vec = temp_vec2;
						}
					}
				}
				else {
					// 演算誤差を考慮して回転角の計算
					if (temp >= 1) temp = 0;
					else if (temp <= -1) temp = iml::pi_v<Float>;
					else temp = std::acos(temp);
					// このステップでボーンを変形する回転行列
					iml::matrix3<Float> rotation = iml::rotation_matrix3(iml::unit(relative_boneend%relative_target), temp);
					// 現在操作対象のボーンを変形してみて変形に必要な角度の算出
					iml::vector3<Float> x = rotation * (position_table[i + 1] - position_table[i]);
					Float phi = std::atan((x*e1) / (x*e3));
					Float theta = std::atan2(x*iml::rotation_vector3(e3, e2, phi), x*e2);
					// 角度制限の適用
					angle_h_m[i] = bone_list_m[i]->angle_radian_h_limit(phi);
					angle_v_m[i] = bone_list_m[i]->angle_radian_v_limit(theta);
					// 角度制限が実行されたなら回転行列を更新
					if ((phi != angle_h_m[i]) || (theta != angle_v_m[i])) {
						// 新しい回転行列により変換されたベクトル
						x = iml::rotation_matrix3(e2 % iml::rotation_vector3(e3, e2, angle_h_m[i]), angle_v_m[i])*(bone_list_m[i]->length_m * e2);
						relative_target = iml::unit(x);
						relative_boneend = iml::unit(position_table[i + 1] - position_table[i]);
						// 演算誤差を考慮して回転角の計算
						temp = relative_target * relative_boneend;
						if (temp >= 1) temp = 0;
						else if (temp <= -1) temp = iml::pi_v<Float>;
						else temp = std::acos(temp);
						// このステップでボーンを変形する回転行列
						rotation = iml::rotation_matrix3(iml::unit(relative_boneend%relative_target), temp);

						// 逐次ボーンを回転させる
						iml::vector3<Float> temp_vec(position_table[i + 1]);
						position_table[i + 1] = position_table[i] + x;
						for (size_t j = i + 1; j < N; ++j) {
							iml::vector3<Float> temp_vec2(position_table[j + 1]);
							position_table[j + 1] = position_table[j] + rotation * (position_table[j + 1] - temp_vec);
							temp_vec = temp_vec2;
						}
					}
					else {
						// 現在操作対象のボーンを基準として回転させる
						position_table[i + 1] = position_table[i] + x;
						for (size_t j = i + 1; j < N; ++j) {
							position_table[j + 1] = position_table[i] + rotation * (position_table[j + 1] - position_table[i]);
						}
					}
				}
				r2 *= bone_list_m[i]->rotation_matrix_hv(angle_h_m[i], angle_v_m[i]);
			}
		}
	}

	// 各種計算結果の取得
	Float angle_h(size_t n) const { return angle_h_m[n]; }
	Float angle_v(size_t n) const { return angle_v_m[n]; }
};

// 左足のためのボーン
template <class Float>
class stick_figure_bone_lleg {
	template <class>
	friend class stick_figure_bone_spine;

	stick_figure_bone<Float>			bone_m[4];			// 腸骨,大腱骨,脛骨,足骨
	stick_figure_IK_engine<Float, 4>	engine_m;			// IKエンジン

	// 現在の姿勢の描画
	void draw(const iml::vector3<Float>& trans, iml::matrix3<Float> r) {
		iml::vector3<Float> vertex[5];
		// 頸椎
		vertex[0] = trans;
		// 腸骨-大腱骨
		r *= bone_m[0].rotation_matrix();
		vertex[1] = vertex[0] + r * (bone_m[0].length_m * bone_m[0].default_axes_m[1]);
		// 大腱骨-脛骨
		r *= bone_m[1].rotation_matrix();
		vertex[2] = vertex[1] + r * (bone_m[1].length_m * bone_m[1].default_axes_m[1]);
		// 脛骨-足骨
		r *= bone_m[2].rotation_matrix();
		vertex[3] = vertex[2] + r * (bone_m[2].length_m * bone_m[2].default_axes_m[1]);
		// 足骨
		r *= bone_m[3].rotation_matrix();
		vertex[4] = vertex[3] + r * (bone_m[3].length_m * bone_m[3].default_axes_m[1]);

		iml::ml::glVertexPointer(3, sizeof(iml::vector3<Float>), &vertex[0][0]);
		glEnableClientState(GL_VERTEX_ARRAY);
		glDrawArrays(GL_LINE_STRIP, 0, 5);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
public:
	// 全体で長さは腸骨と足骨を除いて57 / 100 程度(腸骨は鎖骨よりも短い程度,足骨は手骨と同サイズ)
	stick_figure_bone_lleg() : bone_m{ stick_figure_bone<Float>(8),stick_figure_bone<Float>(27),stick_figure_bone<Float>(30),stick_figure_bone<Float>(9) }, engine_m(bone_m[0], bone_m[1], bone_m[2], bone_m[3]) {
		// 腰椎-腸骨(頸椎-胸椎-鎖骨と同方向)
		bone_m[0].default_axes(iml::vector3<Float>(7, 1, 0), iml::vector3<Float>(-1, 7, 0));
		// 腸骨-大腱骨
		bone_m[1].angle_degree_s_limit(-90, 90).angle_degree_h_limit(-90, 90).angle_degree_v_limit(-15, 125);
		bone_m[1].default_axes(iml::vector3<Float>(0, -1, 0), iml::vector3<Float>(0, 0, 1));
		// 大腱骨-脛骨
		bone_m[2].angle_degree_v_limit(0, 140);
		bone_m[2].default_axes(iml::vector3<Float>(0, -1, 0), iml::vector3<Float>(0, 0, -1));
		// 脛骨-足骨
		bone_m[3].angle_degree_v_limit(0, 90);
		bone_m[3].default_axes(iml::vector3<Float>(0, -1, 0), iml::vector3<Float>(0, 0, 1));

		engine_m.calculation(iml::vector3<Float>(0, -30, 20), { 0,0,0,0 });

		bone_m[0].angle_v_m = engine_m.angle_v(0);
		bone_m[0].angle_h_m = engine_m.angle_h(0);
		bone_m[1].angle_v_m = engine_m.angle_v(1);
		bone_m[1].angle_h_m = engine_m.angle_h(1);
		bone_m[2].angle_v_m = engine_m.angle_v(2);
		bone_m[2].angle_h_m = engine_m.angle_h(2);
		bone_m[3].angle_v_m = engine_m.angle_v(3);
		bone_m[3].angle_h_m = engine_m.angle_h(3);
	}
};
// 右足のためのボーン
template <class Float>
class stick_figure_bone_rleg {
	template <class>
	friend class stick_figure_bone_spine;

	stick_figure_bone<Float>	bone_m[4];			// 腸骨,大腱骨,脛骨,足骨


	// 現在の姿勢の描画
	void draw(const iml::vector3<Float>& trans, iml::matrix3<Float> r) {
		iml::vector3<Float> vertex[5];
		// 頸椎
		vertex[0] = trans;
		// 腸骨-大腱骨
		r *= bone_m[0].rotation_matrix();
		vertex[1] = vertex[0] + r * (bone_m[0].length_m * bone_m[0].default_axes_m[1]);
		// 大腱骨-脛骨
		r *= bone_m[1].rotation_matrix();
		vertex[2] = vertex[1] + r * (bone_m[1].length_m * bone_m[1].default_axes_m[1]);
		// 脛骨-足骨
		r *= bone_m[2].rotation_matrix();
		vertex[3] = vertex[2] + r * (bone_m[2].length_m * bone_m[2].default_axes_m[1]);
		// 足骨
		r *= bone_m[3].rotation_matrix();
		vertex[4] = vertex[3] + r * (bone_m[3].length_m * bone_m[3].default_axes_m[1]);

		iml::ml::glVertexPointer(3, sizeof(iml::vector3<Float>), &vertex[0][0]);
		glEnableClientState(GL_VERTEX_ARRAY);
		glDrawArrays(GL_LINE_STRIP, 0, 5);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
public:
	// 全体で長さは腸骨と足骨を除いて57 / 100 程度(腸骨は鎖骨よりも短い程度,足骨は手骨と同サイズ)
	stick_figure_bone_rleg() : bone_m{ stick_figure_bone<Float>(8),stick_figure_bone<Float>(27),stick_figure_bone<Float>(30),stick_figure_bone<Float>(9) } {
		// 腰椎-腸骨(頸椎-胸椎-鎖骨と同方向)
		bone_m[0].default_axes(iml::vector3<Float>(-7, 1, 0), iml::vector3<Float>(1, 7, 0));
		// 腸骨-大腱骨
		bone_m[1].angle_degree_s_limit(-90, 90).angle_degree_h_limit(-90, 90).angle_degree_v_limit(-15, 125);
		bone_m[1].default_axes(iml::vector3<Float>(0, -1, 0), iml::vector3<Float>(0, 0, 1));
		// 大腱骨-脛骨
		bone_m[2].angle_degree_v_limit(0, 140);
		bone_m[2].default_axes(iml::vector3<Float>(0, -1, 0), iml::vector3<Float>(0, 0, -1));
		// 脛骨-足骨
		bone_m[3].angle_degree_v_limit(0, 90);
		bone_m[3].default_axes(iml::vector3<Float>(0, -1, 0), iml::vector3<Float>(0, 0, 1));
	}
};
// 左腕のためのボーン
template <class Float>
class stick_figure_bone_larm {
	template <class>
	friend class stick_figure_bone_spine;

	stick_figure_bone<Float>	bone_m[4];			// 鎖骨,上腕骨,前腕骨,手骨


	// 現在の姿勢の描画
	void draw(const iml::vector3<Float>& trans, iml::matrix3<Float> r) {
		iml::vector3<Float> vertex[5];
		// 鎖骨
		vertex[0] = trans;
		// 鎖骨-上腕骨
		r *= bone_m[0].rotation_matrix();
		vertex[1] = vertex[0] +  r * (bone_m[0].length_m * bone_m[0].default_axes_m[1]);
		// 上腕骨-前腕骨
		r *= bone_m[1].rotation_matrix();
		vertex[2] = vertex[1] + r * (bone_m[1].length_m * bone_m[1].default_axes_m[1]);
		// 前腕骨-手骨
		r *= bone_m[2].rotation_matrix();
		vertex[3] = vertex[2] + r * (bone_m[2].length_m * bone_m[2].default_axes_m[1]);
		// 手骨
		r *= bone_m[3].rotation_matrix();
		vertex[4] = vertex[3] + r * (bone_m[3].length_m * bone_m[3].default_axes_m[1]);

		iml::ml::glVertexPointer(3, sizeof(iml::vector3<Float>), &vertex[0][0]);
		glEnableClientState(GL_VERTEX_ARRAY);
		glDrawArrays(GL_LINE_STRIP, 0, 5);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
public:
	// 全体で長さは鎖骨を除いて40 / 100 程度(両鎖骨+両腕で大体100になる)
	stick_figure_bone_larm() : bone_m{ stick_figure_bone<Float>(11),stick_figure_bone<Float>(16),stick_figure_bone<Float>(15),stick_figure_bone<Float>(9) } {
		// 頸椎-胸椎-鎖骨
		bone_m[0].angle_degree_h_limit(-90, 90).angle_degree_v_limit(-10, 40);
		bone_m[0].default_axes(iml::vector3<Float>(7, 1, 0), iml::vector3<Float>(-1, 7, 0));
		// 鎖骨-上腕骨
		bone_m[1].angle_degree_s_limit(-90, 90).angle_degree_h_limit(-90, 90).angle_degree_v_limit(-50, 140);
		bone_m[1].default_axes(iml::vector3<Float>(1, 0, 0), iml::vector3<Float>(0, -1, 0));
		// 上腕骨-前腕骨
		bone_m[2].angle_degree_s_limit(-90, 90).angle_degree_v_limit(0, 140);
		bone_m[2].default_axes(iml::vector3<Float>(1, 0, 0), iml::vector3<Float>(0, 0, 1));
		// 前腕骨-手骨
		bone_m[3].angle_degree_v_limit(-70, 90);
		bone_m[3].default_axes(iml::vector3<Float>(1, 0, 0), iml::vector3<Float>(0, -1, 0));


		/*bone_m[1].angle_v_m = iml::pi_v<Float> / 2;
		bone_m[2].angle_v_m = iml::pi_v<Float> / 2;
		bone_m[3].angle_v_m = iml::pi_v<Float> / 2;*/
	}
};
// 右腕のためのボーン(左腕と違って各座標系のx-z平面のx方向が反転するため水平角と捩れ角を逆転させる)
template <class Float>
class stick_figure_bone_rarm {
	template <class>
	friend class stick_figure_bone_spine;

	stick_figure_bone<Float>	bone_m[4];			// 鎖骨,上腕骨,前腕骨,手骨


	// 現在の姿勢の描画
	void draw(const iml::vector3<Float>& trans, iml::matrix3<Float> r) {
		iml::vector3<Float> vertex[5];
		// 鎖骨
		vertex[0] = trans;
		// 鎖骨-上腕骨
		r *= bone_m[0].rotation_matrix();
		vertex[1] = vertex[0] + r * (bone_m[0].length_m * bone_m[0].default_axes_m[1]);
		// 上腕骨-前腕骨
		r *= bone_m[1].rotation_matrix();
		vertex[2] = vertex[1] + r * (bone_m[1].length_m * bone_m[1].default_axes_m[1]);
		// 前腕骨-手骨
		r *= bone_m[2].rotation_matrix();
		vertex[3] = vertex[2] + r * (bone_m[2].length_m * bone_m[2].default_axes_m[1]);
		// 手骨
		r *= bone_m[3].rotation_matrix();
		vertex[4] = vertex[3] + r * (bone_m[3].length_m * bone_m[3].default_axes_m[1]);

		iml::ml::glVertexPointer(3, sizeof(iml::vector3<Float>), &vertex[0][0]);
		glEnableClientState(GL_VERTEX_ARRAY);
		glDrawArrays(GL_LINE_STRIP, 0, 5);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
public:
	// 全体で長さは鎖骨を除いて40 / 100 程度(両鎖骨+両腕で大体100になる)
	stick_figure_bone_rarm() : bone_m{ stick_figure_bone<Float>(11),stick_figure_bone<Float>(16),stick_figure_bone<Float>(15),stick_figure_bone<Float>(9) } {
		// 頸椎-胸椎-鎖骨
		bone_m[0].angle_degree_h_limit(-90, 90).angle_degree_v_limit(-10, 40);
		bone_m[0].default_axes(iml::vector3<Float>(-7, 1, 0), iml::vector3<Float>(1, 7, 0));
		// 鎖骨-上腕骨
		bone_m[1].angle_degree_s_limit(-90, 90).angle_degree_h_limit(-90, 90).angle_degree_v_limit(-50, 140);
		bone_m[1].default_axes(iml::vector3<Float>(-1, 0, 0), iml::vector3<Float>(0, -1, 0));
		// 上腕骨-前腕骨
		bone_m[2].angle_degree_s_limit(-90, 90).angle_degree_v_limit(0, 140);
		bone_m[2].default_axes(iml::vector3<Float>(-1, 0, 0), iml::vector3<Float>(0, 0, 1));
		// 前腕骨-手骨
		bone_m[3].angle_degree_v_limit(-70, 90);
		bone_m[3].default_axes(iml::vector3<Float>(-1, 0, 0), iml::vector3<Float>(0, -1, 0));
	}
};
// 背骨のためのボーン
template <class Float>
class stick_figure_bone_spine {
	template <class>
	friend class stick_figure;

	stick_figure_bone<Float>	bone_m[3];			// 頸椎,胸椎,腰椎


	// 現在の姿勢の描画
	void draw(const iml::vector3<Float>& trans, iml::matrix3<Float> r, stick_figure_bone_larm<Float>& larm, stick_figure_bone_rarm<Float>& rarm, stick_figure_bone_lleg<Float>& lleg, stick_figure_bone_rleg<Float>& rleg) {
		iml::vector3<Float> vertex[4];
		// 頸椎の頂点
		vertex[0] = trans;
		// 頸椎-胸椎
		r *= bone_m[0].rotation_matrix();
		vertex[1] = vertex[0] +  r * (bone_m[0].length_m * bone_m[0].default_axes_m[1]);

		// 腕の描画に遷移
		larm.draw(vertex[1], r);
		rarm.draw(vertex[1], r);

		// 胸椎-腰椎
		r *= bone_m[1].rotation_matrix();
		vertex[2] = vertex[1] +  r * (bone_m[1].length_m * bone_m[1].default_axes_m[1]);
		// 腰椎の頂点
		r *= bone_m[2].rotation_matrix();
		vertex[3] = vertex[2] + r * (bone_m[2].length_m * bone_m[2].default_axes_m[1]);

		iml::ml::glVertexPointer(3, sizeof(iml::vector3<Float>), &vertex[0][0]);
		glEnableClientState(GL_VERTEX_ARRAY);
		glDrawArrays(GL_LINE_STRIP, 0, 4);
		glDisableClientState(GL_VERTEX_ARRAY);

		// 足の描画
		lleg.draw(vertex[3], r);
		rleg.draw(vertex[3], r);
	}
public:
	// 全体で長さは29 / 100 程度
	stick_figure_bone_spine() : bone_m{ stick_figure_bone<Float>(5),stick_figure_bone<Float>(15),stick_figure_bone<Float>(9) } {
		// 頭-頸椎
		bone_m[0].default_axes(iml::vector3<Float>(0, -1, 0), iml::vector3<Float>(0, 0, 1));
		// 頸椎-胸椎
		bone_m[1].angle_degree_s_limit(-70, 70).angle_degree_h_limit(-90, 90).angle_degree_v_limit(-50, 50);
		bone_m[1].default_axes(iml::vector3<Float>(0, -1, 0), iml::vector3<Float>(0, 0, 1));
		// 胸椎-腰椎
		bone_m[2].angle_degree_h_limit(-90, 90).angle_degree_v_limit(-5, 5);
		bone_m[2].default_axes(iml::vector3<Float>(0, -1, 0), iml::vector3<Float>(0, 0, 1));
	}
};
// 棒人間
template <class Float>
class stick_figure {
	iml::ml::drawer_for_figure2<Float>		head_m;			// 頭(円を描画するためのもの)
	stick_figure_bone_spine<Float>			spine_m;		// 背骨
	stick_figure_bone_larm<Float>			larm_m;			// 左腕
	stick_figure_bone_rarm<Float>			rarm_m;			// 右腕
	stick_figure_bone_lleg<Float>			lleg_m;			// 左足
	stick_figure_bone_rleg<Float>			rleg_m;			// 右足
public:
	stick_figure() {
		// 頭のサイズは14 / 100 (大体7等身)
		// y方向に86の半径7の円
		head_m = iml::ml::circle(iml::vector2<Float>(0, 0), Float(7)).shift(0, 93);
	}

	// 現在の姿勢の描画
	void draw() {
		// 変換行列(現状棒人間が回転したりするわけではないため意味は持たない)
		iml::matrix3<Float> r(iml::identity_matrix<Float, 3>());
		// 描画開始の絶対位置
		iml::vector3<Float> trans(0, 86, 0);
		// 頭の描画
		head_m.color(0, 0, 0).draw();
		// 頭以下の描画
		glLineWidth(10);
		spine_m.draw(trans, r, this->larm_m, this->rarm_m, this->lleg_m, this->rleg_m);
	}
};



int main(int argc, char* argv[]) {

	// このスコープ内でライブラリの利用
	IMATHLIB_MEDIA(medialib(u8"test", 800, 600, 0)) {
		
		//カメラの設定
		iml::ml::camera<float> camera;
		camera.viewing_angle_magnific(iml::pi_v<float> / 4, 900 / 1200.);
		//ウィンドウの生成
		iml::ml::mlwindow& wnd = iml::ml::medialib().window();
		//メインウィンドウのクローズイベントのコールバックの設定
		wnd.close_ev(iml::ml::send_quit_event);

		//フォント
		auto font = iml::ml::medialib().font_path("C:/Windows/Fonts/HGRGE.TTC").font(16);

		//ウィンドウの表示
		wnd.show();

		iml::ml::shadow_mapping shadow_map(2048, 2048, "shadow.vert", "shadow.frag");
		shadow_map.camera_handle().set_target({ 0, 50, 0 }).set_camera_descartes({ 50, 100, 100 });
		// 透視投影用の設定
		//shadow_map.camera().viewing_angle_magnific(iml::pi_v<float> / 4, shadow_map.height() / float(shadow_map.width()));
		// 垂直投影用の設定
		shadow_map.camera_handle().camera_mode(iml::ml::cam::ortho_projection).surface_rect(iml::ml::rect<float>(-250, 250, -250, 250));
		shadow_map.set_shadow_ambient(0.2f);

		GLint tex_unit;
		glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &tex_unit);
		std::cout << "頂点シェーダで利用可能なテクスチャ数:" << tex_unit << std::endl;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &tex_unit);
		std::cout << "フラグメントシェーダで利用可能なテクスチャ数:" << tex_unit << std::endl;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &tex_unit);
		std::cout << "利用可能なテクスチャ数:" << tex_unit << std::endl;
		std::cout << "OpenGLのバージョン:" << glGetString(GL_VERSION) << std::endl;
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &tex_unit);
		std::cout << "利用可能なカラーバッファ数:" << tex_unit << std::endl;

		//座標平面軸用の配列データ
		iml::vector3<float> plane_axis_vtx[84];
		{
			size_t cnt = 0;
			for (int i = -10; i < 11; ++i, cnt += 2) {
				plane_axis_vtx[cnt][0] = -100.f; plane_axis_vtx[cnt][1] = 0.f; plane_axis_vtx[cnt][2] = 10.f * i;
				plane_axis_vtx[cnt + 1][0] = 100.f; plane_axis_vtx[cnt + 1][1] = 0.f; plane_axis_vtx[cnt + 1][2] = 10.f * i;
			}
			for (int i = -10; i < 11; ++i, cnt += 2) {
				plane_axis_vtx[cnt][0] = 10.f * i; plane_axis_vtx[cnt][1] = 0.f; plane_axis_vtx[cnt][2] = -100.f;
				plane_axis_vtx[cnt + 1][0] = 10.f * i; plane_axis_vtx[cnt + 1][1] = 0.f; plane_axis_vtx[cnt + 1][2] = 100.f;
			}
		}
		iml::vector3<float> plane_vtx[4] = { {-100,-0.1f,-100},{ -100,-0.1f,100 },{ 100,-0.1f,100 },{ 100,-0.1f,-100 } };

		//ライト
		auto light = iml::ml::light<float, 0>::inst();
		light->set_position(iml::vector4<float>(0, 300, 100, 1));
		light->set_direction(iml::vector3<float>(0, -1, -1));
		light->set_ambient({ 0.3f,0.3f,0.3f });
		light->set_specular({ 1,1,1 });
		light->set_diffuse({ 1,1,1 });

		// 適当な画像を読みこむ
		iml::img::image* im_buf = iml::img::load_bmp(iml::ml::binary_load<char>("org_back.bmp"));
		iml::img::image* im_buf_ = iml::img::load_png(iml::ml::binary_load<char>("org_back.png"));
		// 画像データからテクスチャを作成
		auto image = iml::ml::load<iml::ml::texture>(im_buf);
		auto image_ = iml::ml::load<iml::ml::texture>(im_buf_);
		// Metasequoiaの3Dモデルを読み込む(Formatのバージョンは1.0のみ)
		auto model_ = iml::ml::load<iml::m3d::mqo_model<float>>(iml::ml::text_line_load<char>("iroha.mqo"), .1);

		std::cout << "プログラムループ" << std::endl;

		stick_figure<float> stick_figure_obj;

		iml::ml::ui::gui_window gui;
		iml::ml::ui::gui_text	gui_text(iml::ml::medialib().font(16), std::addressof(gui));
		gui_text.add(u8"あいうえお");
		std::cout << gui_text.width() << "," << gui_text.height() << std::endl;
		gui.add(gui_text);

		iml::ml::frame_buffer_object	fbo;
		auto tex0 = fbo.attach(iml::ml::texture(500, 500, iml::ml::txc::rgb_texture)).second;
		auto tex1 = fbo.attach(iml::ml::texture(500, 500, iml::ml::txc::rgb_texture)).second;


		SDL_Rect srcrect;
		srcrect.x = 0;
		srcrect.y = 0;
		srcrect.w = 200;
		srcrect.h = 500;
		SDL_SetTextInputRect(&srcrect);
		SDL_StartTextInput();
		while (iml::ml::event()) {
			//ESCSAPを押したら終了イベントを送信
			if (iml::ml::keyboard::key_escape.trigger()) iml::ml::send_quit_event();

			//マウスの操作によってカメラの視点を切り替える
			{
				auto relative = iml::ml::mouse::mov();
				iml::vector3<float> rot;
				iml::vector3<float> trans;
				iml::vector2<float> view_trans_target;
				iml::vector3<float> trans_target;
				//右クリックのみならば回転
				if (iml::ml::mouse::r.push() && !iml::ml::mouse::l.push()) rot = iml::vector3<float>(-relative[0], relative[1], 0) * 0.01f;
				//左クリックのみならば平行移動
				if (!iml::ml::mouse::r.push() && iml::ml::mouse::l.push()) trans = iml::vector3<float>(1, 1, 1);
				//中央クリックならばビューボート面に対する平行移動(ターゲットに近いほど移動距離は減少)
				if (iml::ml::mouse::m.push()) view_trans_target = iml::vector2<float>(-relative[0], relative[1]) * camera.length() * 0.001f;
				//左右両方クリックしているとき注視点からの距離を調整
				if (iml::ml::mouse::r.push() && iml::ml::mouse::l.push()) rot[2] = relative[0] * 2;

				camera.move_camera_polar(rot, 0.6f);
				camera.move_camera_descartes(trans, 0.6f);
				camera.move_target(view_trans_target, 0.6f);
			}
			IMATHLIB_CAMERA(ca_, camera) {
				ca_.projection_matrix();
				// ビューポートの設定(left, right, bottom, top)
				IMATHLIB_VIEWPORT(vp, (0, iml::ml::medialib().window().width(), iml::ml::medialib().window().height(), 0)) {
					//シャドウマップのレンダリング
					static auto draw_f = [&]() {
						iml::ml::draw_model(model_);
						//iml::ml::draw_wire_frame(model_, iml::normal_color<float>(1, 1, 0));
					};
					shadow_map.create_shadow(draw_f);
					//シャドウを適応させる空間の描画
					//glEnable(GL_DEPTH_TEST);
					shadow_map.start();
					draw_f();
					glNormal3d(0, 1, 0);
					iml::ml::glColor(iml::normal_color<float>(1, 1, 1));
					iml::ml::draw_figure<GL_QUADS, iml::index_tuple<iml::size_t, GL_VERTEX_ARRAY>, sizeof(iml::vector3<float>)>(3, &plane_vtx[0][0], 0, 4);
					shadow_map.end();
					//draw_f();
					//iml::ml::draw_texture2(iml::ml::vertex2<double>(0, 0, 200, 200), 3);
					//glDisable(GL_DEPTH_TEST);

					//light->disable();

					stick_figure_obj.draw();

					glEnable(GL_DEPTH_TEST);
					//ワールド座標軸
					iml::ml::glColor(iml::normal_color<float>(1, 0, 0));
					iml::ml::draw_line(iml::vector3<float>(0, 0, 0), iml::vector3<float>(100, 0, 0));
					iml::ml::glColor(iml::normal_color<float>(0, 1, 0));
					iml::ml::draw_line(iml::vector3<float>(0, 0, 0), iml::vector3<float>(0, 100, 0));
					iml::ml::glColor(iml::normal_color<float>(0, 0, 1));
					iml::ml::draw_line(iml::vector3<float>(0, 0, 0), iml::vector3<float>(0, 0, 100));
					iml::ml::glColor(iml::normal_color<float>(0.8, 0.8, 0.8));
					iml::ml::draw_figure<GL_LINES, iml::index_tuple<iml::size_t, GL_VERTEX_ARRAY>, sizeof(iml::vector3<float>)>(3, &plane_axis_vtx[0][0], 0, 84);
					glDisable(GL_DEPTH_TEST);

					auto start_sc = iml::ml::screen_to_world(iml::vector3<float>(iml::ml::mouse::pos()[0], iml::ml::mouse::pos()[1], 0));
					auto end_sc = iml::ml::screen_to_world(iml::vector3<float>(iml::ml::mouse::pos()[0], iml::ml::mouse::pos()[1], 1));
					start_sc = iml::ml::screen_to_world(iml::vector3<float>(iml::ml::mouse::pos()[0], iml::ml::mouse::pos()[1], 0));
					end_sc = iml::ml::screen_to_world(iml::vector3<float>(iml::ml::mouse::pos()[0], iml::ml::mouse::pos()[1], 1));

					//スクリーンモード
					//y=0平面の取得(y-up系だから)
					IMATHLIB_SCREEN_MODE(ca, (0, vp.viewport_rect().right, vp.viewport_rect().bottom, 0), ) {
						iml::ml::glColor(iml::normal_color<float>(1, 1, 1));
						iml::ml::draw_texture<GL_QUADS, iml::index_tuple<iml::size_t, GL_VERTEX_ARRAY, GL_TEXTURE_COORD_ARRAY>, 0, 0>(
							2, image_, iml::ml::vertex2<float>(0, 0, 300, 300).vtx, iml::ml::vertex2<float>(0, 0, 1, 1).vtx, 0, 4);
						iml::ml::draw_texture<GL_QUADS, iml::index_tuple<iml::size_t, GL_VERTEX_ARRAY, GL_TEXTURE_COORD_ARRAY>, 0, 0>(
							2, image, iml::ml::vertex2<float>(300, 0, 500, 200).vtx, iml::ml::vertex2<float>(0, 0, 1, 1).vtx, 0, 4);
						iml::ml::draw_string_texture2(10, 500, *font, (L"FPS\n" + std::to_wstring(iml::ml::medialib().fps().fps())).c_str(), { 0,0,0 });
						iml::ml::draw_string_texture2(10, 540
							, iml::ml::string_texture(*font, L"時間", { 0,0,0 }).ln().underline(true).add(*font, std::to_wstring(iml::ml::medialib().fps().time()), { 0,0,0 })
						);
						IMATHLIB_VIEWPORT(vp_, (0, 500, 500, 0)) {
							vp_.unit_scaling();
							iml::ml::draw_string_texture2(10, 10, *font
								, u8"あああああああaaaaaあああaaaaaああああああああ魑魅魍魎あああああああああああああああああああああああああああああああああああああああああああああああああああああああ"
								, { 0,0,0 }, 130);
						}

						auto rr = iml::ml::rounded_rectangle(iml::vector2<double>(400, 300), iml::vector2<double>(600, 400), 10.);
						rr.draw();
						rr = iml::ml::upper_rounded_rectangle(iml::vector2<double>(100, 300), iml::vector2<double>(300, 400), 10.);
						rr.draw();

						gui.draw();
						gui_text.draw();

						iml::ml::draw_texture2(iml::ml::vertex2<double>(100, 300, 300, 500), shadow_map.get_fbo().depth()->tex_handle().get());
					}

					iml::ml::glColor(iml::normal_color<float>(0, 0, 0));
					iml::ml::draw_line(start_sc, end_sc);
					iml::ml::draw_line(iml::vector3<float>(), start_sc, 10);
					iml::ml::draw_line(iml::vector3<float>(), end_sc);
				}

				IMATHLIB_VIEWPORT(vp, (0, tex1->tex_handle()->width(), tex1->tex_handle()->height(), 0)) {
					IMATHLIB_SCREEN_MODE(ca, (0, tex1->tex_handle()->width(), tex1->tex_handle()->height(), 0)) {
						iml::ml::glColor(iml::normal_color<float>(1, 1, 1));
						tex1->clear_color({ 0.3f, 1.f, 0.7f });
						// フレームバッファを描画する
						iml::ml::set_draw_framebuffer_object(std::addressof(fbo));
						auto _rr = iml::ml::upper_rounded_rectangle(iml::vector2<double>(100, 100), iml::vector2<double>(400, 400), 10.);
						_rr.color(iml::normal_color<double>(1, 0, 0)).draw();
						iml::ml::set_draw_framebuffer_object();
						iml::ml::glColor(iml::normal_color<float>(1, 1, 1));
						iml::ml::draw_texture2(iml::ml::vertex2<double>(0, 0, 300, 300), tex1->tex_handle().get());
					}
				}
			}

			//メインウィンドウの更新とバッファのクリア
			wnd.update();
			wnd.clear();
		}
		SDL_StopTextInput();



		std::cout << "ライブラリ終了チェック" << std::endl;
	}

	return 0;

}
//#endif