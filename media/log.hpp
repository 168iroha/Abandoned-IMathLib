#ifndef IMATHLIB_H_MEDIA_LOG_HPP
#define IMATHLIB_H_MEDIA_LOG_HPP

#include "IMathLib/math_traits.hpp"
#include "IMathLib/preprocessor.hpp"
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <chrono>


// ログを出力する
namespace iml {
	namespace ml {

		// ログ出力のためのクラス
		class logger {
			static inline std::string			file_name_m;			// 出力ファイル名
			static inline int_t					log_level_m;			// ログの出力レベル
			static inline int_t					prev_log_level_m;		// 前回のログの出力レベル
			static inline std::stringstream		str_stream_m;			// 出力対象を一時的に保持しておくためのストリーム

			// ログレベルの文字列テーブルから取得
			static const char* log_level_str(size_t n) {
				static const char* const table[] = {
					// デバッグ用のログ出力
					"Debug"
					// 以下はエラー
					,"Warn"
					,"Error"
					,"Fatal"
				};
				return table[n];
			}

			struct date {
				size_t	ms;			// ミリ秒
				size_t	sec;		// 秒
				size_t	mini;		// 分
				size_t	hour;		// 時
				size_t	day;		// 日
				size_t	month;		// 月
				size_t	year;		// 年
			};
			static void get_date(date& d) {
				auto chrono_time = std::chrono::system_clock::now();
				auto time_t_time = std::chrono::system_clock::to_time_t(chrono_time);
				std::tm t;
#if defined(__STDC__) && defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)		// C11
				localtime_s(&time_t_time, &t);
#elif defined(_MSC_VER)				// MSVC
				localtime_s(&t, &time_t_time);
#elif defined(__GNUC__)				// gcc
				localtime_r(&time_t_time, &t);
#endif
				d.sec = t.tm_sec;
				d.mini = t.tm_min;
				d.hour = t.tm_hour;
				d.day = t.tm_mday;
				d.month = t.tm_mon + 1;
				d.year = t.tm_year + 1900;
				long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(chrono_time.time_since_epoch()).count();
				d.ms = ms % 1000;
			}
		public:
			// ログレベルの識別定数
			static constexpr size_t debug = 0;
			static constexpr size_t warn = 1;
			static constexpr size_t error = 2;
			static constexpr size_t fatal = 3;

			static void init(size_t level = logger::debug) {
				namespace sys = std::filesystem;
				sys::path p("log.txt");
				// ログの出力ファイル名の決定(log[1-9]+\.txt)
				if (sys::exists(p)) {
					for (size_t i = 2; i < size_t(-1); ++i) {
						p = "log" + std::to_string(i) + ".txt";
						if (!sys::exists(p)) break;
					}
				}
				logger::file_name_m = p.string();
				logger::log_level_m = level;
				logger::prev_log_level_m = -1;
			}

			// 時間等のログ本文以外の出力要素の書き込み
			static logger wrrite_info(int_t level, const char* file_name, int line, const char* function) {
				logger::prev_log_level_m = level;
				if (level >= logger::log_level_m) {
					// logger::str_stream_mが空でなければ改行を挿入
					if (!logger::str_stream_m.str().empty()) logger::str_stream_m << "\n";
					// 現在時刻をミリ秒で取得
					date da;
					get_date(da);

					logger::str_stream_m << da.year << "/" << da.month << "/" << da.day << " " << da.hour << ":" << da.mini << ":" << da.sec << "." << da.ms
						// [ファイル名 at 行番号:関数]
						<< " [" << file_name << " at " << line << ":" << function << "] "
						<< log_level_str(level) << "\n";
				}
				return logger();
			}
			// エラー等の概要付き時間等のログ本文以外の出力要素の書き込み
			static logger wrrite_info(int_t level, const char* file_name, int line, const char* function, const char* error_str) {
				logger::prev_log_level_m = level;
				if (level >= logger::log_level_m) {
					// logger::str_stream_mが空でなければ改行を挿入
					if (!logger::str_stream_m.str().empty()) logger::str_stream_m << "\n";
					// 現在時刻をミリ秒で取得
					date da;
					get_date(da);

					logger::str_stream_m << da.year << "/" << da.month << "/" << da.day << " " << da.hour << ":" << da.mini << ":" << da.sec << "." << da.ms
						// [ファイル名 at 行番号:関数]
						<< " [" << file_name << " at " << line << ":" << function << "] "
						<< log_level_str(level) << ":" << error_str << "\n";
				}
				return logger();
			}
			// ストリームに蓄積された文字列を出力する
			static void wrrite() {
				if (!logger::str_stream_m.str().empty()) {
					// 文字列ストリームの中身を全て出力したら中身を空にする
					std::fstream ofs(logger::file_name_m, std::ios::app | std::ios::out);
					ofs << logger::str_stream_m.str() << "\n";
					logger::str_stream_m.str("");
					logger::str_stream_m.clear(std::stringstream::goodbit);
				}
			}

			template <class T>
			friend logger operator<<(logger os, const T& x) {
				// 出力可能ならば出力する
				if (logger::prev_log_level_m >= logger::log_level_m)
					logger::str_stream_m << x;
				return os;
			}
		};

		// ログレベルのみを指定
#define IMATHLIB_LOG1(LOG_LEVEL)				::iml::ml::logger::wrrite_info(LOG_LEVEL, __FILE__, __LINE__, __func__)
		// エラー等の概要付き
#define IMATHLIB_LOG2(LOG_LEVEL, SUMMARY)		::iml::ml::logger::wrrite_info(LOG_LEVEL, __FILE__, __LINE__, __func__, SUMMARY)

		// ログ出力時に呼び出すマクロ
#define IMATHLIB_LOG(...)						IMATHLIB_PP_EXPAND(IMATHLIB_PP_OVERLOAD(IMATHLIB_LOG, __VA_ARGS__)(__VA_ARGS__))
	}
}


#endif