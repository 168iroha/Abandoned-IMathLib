#ifndef IMATHLIB_H_MEDIA_3D_METASEQUOIA_HPP
#define IMATHLIB_H_MEDIA_3D_METASEQUOIA_HPP

#include "IMathLib/media/3d/model.hpp"

// メタセコイアモデルのロード

namespace iml {

	//文字列の長さの取得
	template <class CharT>
	inline size_t string_length(const CharT* str) {
		size_t temp = 0;
		for (; *str != CharT(0); ++str, ++temp);
		return temp;
	}
	//文字列の探索(ないときはnullptrを返す)
	template <class CharT>
	inline const CharT* string_find(const CharT* str1, size_t len1, const CharT* str2, size_t len2) {
		if (len1 < len2) return nullptr;
		/*// 探索文字列の文字数が1のときは単純なfind
		const CharT* result = (len2 == 1) ? find(str1, str1 + len1, *str2) : find(str1, str1 + len1, str2, str2 + len2);
		// ないときはnullptrを返す
		return (result == str1 + len1) ? nullptr : result;*/
		std::string temp(str1, len1);
		auto result = temp.find(str2, 0, len2);
		return (result == std::basic_string<CharT>::npos) ? nullptr : str1 + result;
	}
	template <class CharT>
	inline const CharT* string_find(const CharT* str1, const CharT* str2) {
		return string_find(str1, string_length(str1), str2, string_length(str2));
	}
	template <class CharT>
	inline const CharT* string_find(const CharT* str1, size_t len1, const CharT* str2) {
		return string_find(str1, len1, str2, string_length(str2));
	}
	template <class CharT>
	inline const CharT* string_find(const CharT* str1, const CharT* str2, size_t len2) {
		return string_find(str1, string_length(str1), str2, len2);
	}
	//後方から文字列の探索
	/*template <class CharT>
	inline const CharT* string_rfind(const CharT* str1, size_t len1, const CharT* str2, size_t len2) {
		if (len1 < len2) return nullptr;
		//探索文字列の文字数が1のときは単純なrfind
		const CharT* result = (len2 == 1) ? rfind(str1, str1 + len1, *str2) : rfind(str1, str1 + len1, str2, str2 + len2);
		return (result == str1 + len1) ? nullptr : result;
	}
	template <class CharT>
	inline const CharT* string_rfind(const CharT* str1, const CharT* str2) {
		return string_rfind(str1, string_length(str1), str2, string_length(str2));
	}
	template <class CharT>
	inline const CharT* string_rfind(const CharT* str1, size_t len1, const CharT* str2) {
		return string_rfind(str1, len1, str2, string_length(str2));
	}
	template <class CharT>
	inline const CharT* string_rfind(const CharT* str1, const CharT* str2, size_t len2) {
		return string_rfind(str1, string_length(str1), str2, len2);
	}*/

	namespace m3d {

		// 頂点情報から頂点法線の計算
		template <class T>
		inline void vertex_normal(object_data<T>* obj) {
			// 面の法線を面積の加重平均で頂点に足しこむ
			for (auto itr = obj->face_info.begin(); itr != obj->face_info.end(); ++itr) {
				vector3<T> f_normal = surface_normal(
					obj->vertex_info.first[itr->v[0]].point
					, obj->vertex_info.first[itr->v[1]].point
					, obj->vertex_info.first[itr->v[2]].point
				);
				for (size_t j = 0; j < 3; ++j) obj->vertex_info.first[itr->v[j]].normal += f_normal;
			}
			// 法線ベクトルを正規化
			for (size_t i = 0; i < obj->vertex_info.second; ++i) {
				T	temp = abs(obj->vertex_info.first[i].normal);
				// 面になっていない頂点では計算しない
				if (temp != 0) obj->vertex_info.first[i].normal /= temp;
			}
		}

		// 頂点配列の作成
		// (mat:マテリアル情報,mat_pos:材質番号,f:面情報,vtx:頂点情報,mat_col:マテリアルカラー)
		template <class T>
		inline void make_vertex_array(material_info<T>* mat, dword mat_pos
			, std::vector<face<T>>& f, vertex_array_data<T>& vtx) {
			size_t	d_pos = 0;

			for (auto itr = f.begin(); itr != f.end(); ++itr) {
				if (itr->m != mat_pos) continue;
				for (size_t j = 0; j < 3; ++j, ++d_pos) {
					mat->uv_point[d_pos] = itr->uv[j];
					mat->vtx_index.first[d_pos] = itr->v[j];
				}
			}
		}

		// オブジェクトデータからポリゴンモデルの作成(内部オブジェクトに対して)
		template <class T>
		inline void _Make_polygon_data(inner_object<T>* model, object_data<T>* obj, mat_array_data& mat) {
			material_info<T>	*material;
			color_rgba			mat_col;
			dword				*mat_vtx_n;
			dword				temp = 0;

			// オブジェクトデータをコピー
			//copy_order(model->name, obj->name, 256);
			std::copy(obj->name, obj->name + 256, model->name);
			model->visible = obj->visible;
			model->shading = obj->shading;

			mat_vtx_n = new dword[mat.second]{};

			// マテリアルに対する頂点数をカウントする
			for (auto itr = obj->face_info.begin(); itr != obj->face_info.end(); ++itr) {
				if (itr->m < 0) continue;		// マテリアルが設定されていない

				mat_vtx_n[itr->m] += 3;
				// 最大値を得る
				if (temp < itr->m + 1) temp = itr->m + 1;
			}

			// マテリアル別に頂点配列を作成する
			model->mat.resize(temp);

			for (size_t i = 0; i < model->mat.size(); ++i) {
				material = &model->mat[i];

				if (mat_vtx_n[i] < 0) continue;
				// マテリアル情報の設定
				material->texture_id = mat.first[i].tex_id;
				material->use_tex = material->texture_id != 0;
				material->mat_index = i;

				// メモリ確保
				material->vtx_index.second = mat_vtx_n[i];
				material->vtx_index.first.reset(new word[material->vtx_index.second]);
				material->uv_point.resize(mat_vtx_n[i]);
				// 頂点情報の構築
				make_vertex_array(material, i, obj->face_info, obj->vertex_info);

			}

			delete[] mat_vtx_n;
		}
		// オブジェクトデータからポリゴンモデルの作成
		template <class T>
		inline void make_polygon_data(model_object<T>* model, std::list<object_data<T>>& obj, mat_array_data& mat) {
			model->object.resize(obj.size());			// 予めメモリを確保
			model->mat = mat;							// マテリアル情報の取得

			auto itr = obj.begin();
			for (size_t i = 0; i < obj.size(); ++i, ++itr) {
				// オブジェクトごとの頂点データを渡す
				model->object[i].vtx = itr->vertex_info;
				_Make_polygon_data(&model->object[i], &(*itr), mat);
			}
		}

		// メタセコイアモデルのロードのための構造体
		template <class T>
		struct mqo_model :model_object<T> {
		private:
			// マテリアルデータの読み込み
			template <class L>
			void read_material(L& line_obj, mat_array_data& mat) {
				const char		*str;
				const char		*p;
				float			c[4];
				float			dif, amb, emi, spc, power;

				// "}"が出るまではマテリアルデータが存在する
				for (auto itr = mat.first.get(); str = line_obj.get_line(), string_find(str, "}", 1) == nullptr; ++itr) {
					// 材質名はスキップ
					p = string_find(str, "col(", 4);
					sscanf_s(p, "col(%f %f %f %f) dif(%f) amb(%f) emi(%f) spc(%f) power(%f)",
						&c[0], &c[1], &c[2], &c[3], &dif, &amb, &emi, &spc, &power);

					itr->col.r = c[0] * 255;
					itr->col.g = c[1] * 255;
					itr->col.b = c[2] * 255;
					itr->col.a = c[3] * 255;

					itr->dif = itr->col*dif;
					itr->amb = itr->col*amb;
					itr->emi = itr->col*emi;
					itr->spe = itr->col*spc;
					itr->power = power * 255 / 100;

					// テクスチャマッピング情報
					if ((p = string_find(p, "tex(", 4)) != nullptr) {
						const char	*end = string_find(p, ")", 1) - 1;		// ダブルクオーテーションのスキップ
						//copy_order(itr->tex_name, p + 5, end);
						std::copy(p + 5, end, itr->tex_name);
					}
				}
			}

			// 頂点情報の読み込み
			template <class L>
			static void read_vertex_info(L& line_obj, vertex_array_data<T>& v, const T& scale) {
				const char	*str;
				for (size_t i = 0; str = line_obj.get_line(), string_find(str, "}", 1) == nullptr; ++i) {
					sscanf_s(str, "%f %f %f", &v.first[i].point[0], &v.first[i].point[1], &v.first[i].point[2]);
					v.first[i].point *= scale;
				}
			}

			// 面情報の読み込み
			template <class L>
			static void read_face_info(L& line_obj, std::vector<face<T>>& f) {
				const char	*str;
				const char	*p;
				size_t	face_4_n = 0;			// 面が4つの頂点で構成されている数

				for (auto itr = f.begin(); str = line_obj.get_line(), string_find(str, "}", 1) == nullptr;) {
					auto itr2 = itr;
					dword temp_n;
					sscanf_s(str, "%d", &temp_n);
					// temp_nが4ならば2つイテレータを進める
					++itr;
					if (temp_n == 4) ++itr;

					if ((p = string_find(str, "V(", 2)) != nullptr) {
						dword temp_v[4];
						// 3か4のみをサポート(メタセコイアはこれが右回りとなっているが、OpenGLでは左回りであるためglFrontFaceを用いる)
						switch (temp_n) {
						case 3:
							// sscanf_s(p, "V(%d %d %d)", &itr->v[0], &itr->v[1], &itr->v[2]);
							sscanf_s(p, "V(%d %d %d)", &temp_v[2], &temp_v[1], &temp_v[0]);
							itr2[0].v[0] = temp_v[0]; itr2[0].v[1] = temp_v[1]; itr2[0].v[2] = temp_v[2];
							break;
						case 4:
							++face_4_n;

							// sscanf_s(p, "V(%d %d %d %d)", &itr->v[0], &itr->v[1], &itr->v[2], &itr->v[3]);
							sscanf_s(p, "V(%d %d %d %d)", &temp_v[3], &temp_v[2], &temp_v[1], &temp_v[0]);
							itr2[0].v[0] = temp_v[0]; itr2[0].v[1] = temp_v[1]; itr2[0].v[2] = temp_v[2];
							itr2[1].v[0] = temp_v[0]; itr2[1].v[1] = temp_v[2]; itr2[1].v[2] = temp_v[3];
							break;
						}
					}
					// マテリアル情報
					if ((p = string_find(str, "M(", 2)) != nullptr) sscanf_s(p, "M(%d)", &itr2[0].m);
					else itr2[0].m = -1;
					// UVマップ
					if ((p = string_find(str, "UV(", 3)) != nullptr) {
						vector2<float> temp_uv[4];
						switch (temp_n) {
						case 3:
							sscanf_s(p, "UV(%f %f %f %f %f %f)",
								&temp_uv[0][0], &temp_uv[0][1],
								&temp_uv[1][0], &temp_uv[1][1],
								&temp_uv[2][0], &temp_uv[2][1]
							);
							itr2[0].uv[0] = temp_uv[0];
							itr2[0].uv[1] = temp_uv[1];
							itr2[0].uv[2] = temp_uv[2];
							break;
						case 4:
							// マテリアル情報を代入しておく
							itr2[1].m = itr2[0].m;

							sscanf_s(p, "UV(%f %f %f %f %f %f %f %f)",
								&temp_uv[0][0], &temp_uv[0][1],
								&temp_uv[1][0], &temp_uv[1][1],
								&temp_uv[2][0], &temp_uv[2][1],
								&temp_uv[3][0], &temp_uv[3][1]
							);
							/*itr2[0].uv[0] = temp_uv[2];
							itr2[0].uv[1] = temp_uv[1];
							itr2[0].uv[2] = temp_uv[0];
							itr2[1].uv[0] = temp_uv[3];
							itr2[1].uv[1] = temp_uv[2];
							itr2[1].uv[2] = temp_uv[0];*/
							itr2[0].uv[0] = temp_uv[0];
							itr2[0].uv[1] = temp_uv[1];
							itr2[0].uv[2] = temp_uv[2];
							itr2[1].uv[0] = temp_uv[0];
							itr2[1].uv[1] = temp_uv[2];
							itr2[1].uv[2] = temp_uv[3];
							break;
						}
					}
				}
				// 余分に確保したメモリを考慮してメモリ使用量の再設定
				f.resize(face_4_n + f.size() / 2);
			}

			// オブジェクトデータの読み込み
			template <class L>
			void read_object(L& line_obj, object_data<T>* obj, const T& scale) {
				const char *str;
				// '}'が出るまで(スペースはタブ対策)
				while (str = line_obj.get_line(), string_find(str, "}", 1) == nullptr) {
					size_t str_length = string_length(str);
					dword temp;
					// visible
					if (string_find(str, str_length, "visible ", 8)) {
						sscanf_s(str, " visible %d", &obj->visible);
					}
					// shading
					else if (string_find(str, str_length, "shading ", 8)) {
						sscanf_s(str, " shading %d", &obj->shading);
					}
					// facet
					/*else if (string_find(str, str_length, "facet ", 6)) {
						static T _pi = pi_v<T>();
						sscanf_s(str, " facet %f", &obj->facet);
						// ラジアンに変換
						obj->facet *= _pi / 180;
					}*/
					// vertex
					else if (string_find(str, str_length, "vertex ", 7)) {
						sscanf_s(str, " vertex %d", &obj->vertex_info.second);
						obj->vertex_info.first.reset(new vertex_data<T>[obj->vertex_info.second]);
						read_vertex_info(line_obj, obj->vertex_info, scale);
					}
					// BVertexは無し
					// face
					else if (string_find(str, str_length, "face ", 5)) {
						sscanf_s(str, " face %d", &temp);
						obj->face_info.resize(temp * 2);			// 精々2倍あればいい(頂点数は4までだから)
						read_face_info(line_obj, obj->face_info);
					}
				}
			}

			// version1.0
			template <class L>
			void mqo_load_1_0(L& line_obj, const T& scale) {
				std::list<object_data<T>>	obj;
				mat_array_data			mat;
				const char				*str;

				for (auto itr =(obj.push_back(object_data<T>()),obj.begin()); (str = line_obj.get_line()) != nullptr;) {
					size_t temp = string_length(str);

					// Object
					if (string_find(str, temp, "Object", 6)) {
						sscanf_s(str, "Object %s", itr->name, 256);
						read_object(line_obj, &(*itr), scale);
						// 法線データの計算
						vertex_normal(&*itr);
						// 次のデータの追加
						obj.push_back(object_data<T>());
						++itr;
					}
					// Material(1回のみ)
					else if (string_find(str, temp, "Material", 8)) {
						sscanf_s(str, "Material %d", &mat.second);			// 実際は必要ない
						mat.first.reset(new mat_data[mat.second]);
						read_material(line_obj, mat);
					}
				}
				// 余分なデータの削除
				obj.pop_back();

				// テクスチャの登録
				for (size_t i = 0; i != mat.second; ++i) {
					if (mat.first[i].tex_name[0] != 0) {
						// 絶対パスならばそのままでそうでなければディレクトリパスと連結する
						std::string temp = (string_find(mat.first[i].tex_name, ":", 1) == nullptr)
							? mat.first[i].tex_name : line_obj.dir_path() + mat.first[i].tex_name;
						// テクスチャを読み込む
						auto img = iml::img::load_image(iml::ml::binary_load<char>(temp.c_str()));
						auto tex = ml::load<ml::texture>(img);
						mat.first[i].tex_id = tex->id();
						delete img;
					}
				}

				make_polygon_data(this, obj, mat);
			}
			template <class L>
			void mqo_load_1_1(L&, const T&) {

			}
		public:
			// Lは1行づつテキストが取得可能なオブジェクト
			template <class L>
			mqo_model(L& line_obj, const T& scale = 1) {
				line_obj.get_line();
				// バージョン番号の取得
				const char *str = line_obj.get_line();
				str += 16;
				// version1.0
				if ((str[0] == '1') && (str[1] == '.') && (str[2] == '0')) mqo_load_1_0(line_obj, scale);
				// version1.1
				else if ((str[0] == '1') && (str[1] == '.') && (str[2] == '1')) mqo_load_1_1(line_obj, scale);
			}
		};
	}
}


#endif