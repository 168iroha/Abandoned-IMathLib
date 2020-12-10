#ifndef IMATHLIB_INTERFACE_LOAD_HPP
#define IMATHLIB_INTERFACE_LOAD_HPP

//ファイルのロードを中継する(文字コードの問題の回避とソースの簡易化のため)

#include "IMathLib/math_traits.hpp"
#include <filesystem>

namespace iml {
	namespace ml {

		//最も基本的なローダー
		template <class T, class... Args>
		inline T* load(Args&&... args) {
			return new T(std::forward<Args>(args)...);
		}

		//全てのバイナリデータのロードのためのクラス
		template <class CharT>
		class binary_load {
			unsigned char* data = nullptr;
		public:
			binary_load(const CharT* binary_file) {
				std::ifstream ifs(binary_file, std::ios::in | std::ios::binary);
				int len;
				ifs.seekg(0, std::ios::end);
				len = ifs.tellg();
				ifs.seekg(0, std::ios::beg);
				data = new unsigned char[len];
				ifs.read((char*)data, len);
			}
			~binary_load() { delete[] data; }

			operator const unsigned char*() const { return data; }
		};
		//全てのテキストデータのロードのためのクラス
		template <class CharT>
		class text_load {
			std::string str;
		public:
			//flag:コメント除去フラグ(C++準拠)
			text_load(const CharT* text_file, bool flag = false) {
				//ファイル読み込み
				std::fstream ifs(text_file, std::ios::in);
				if (ifs.fail()) {
					IMATHLIB_LOG(iml::ml::logger::error, text_file) << "The file could not be read.";
					str = nullptr;
					return;
				}
				str = std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());


			}
			~text_load() {}

			operator const char*() const { return str.c_str(); }
		};

		//1行づつテキストをロードするためのクラス
		template <class CharT>
		class text_line_load {
			//std::ifstream	ifs;
			//std::string		str;
			FILE			*fp;
			char			str[256];
			//std::filesystem::path		path;
			std::string		path;
		public:
			text_line_load(const CharT* text_file) : path(text_file) {
				errno_t error;
				if ((error = fopen_s(&fp, text_file, "rt")) != 0) {
					fp = nullptr;
					strerror_s(str, 256, error);
					IMATHLIB_LOG(iml::ml::logger::error, u8"fopen_s") << str;
				}
			}
			~text_line_load() { fclose(fp); }

			//1行取得
			const char* get_line() {
				//if (!std::getline(ifs, str)) return nullptr;
				//return str.c_str();
				if (fp == nullptr) return nullptr;
				if (fgets(str, 256, fp) == nullptr) return nullptr;
				return str;
			}
			//ディレクトリパスの取得
			std::string dir_path() const {
				//return path.parent_path().string() + '/';
				return std::filesystem::path(this->path).parent_path().string() + '/';
			}
		};
	}
}

#endif