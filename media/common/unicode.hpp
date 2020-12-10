#ifndef IMATHLIB_H_MEDIA_COMMON_UNICODE_HPP
#define IMATHLIB_H_MEDIA_COMMON_UNICODE_HPP

#include <iterator>
#include <string>

namespace iml {

	// UTF-8のバイト数の取得(0は不正)
	inline size_t utf8_byte(char c) {
		if ((c & 0b10000000) == 0b00000000) return 1;
		else if ((c & 0b11100000) == 0b11000000) return 2;
		else if ((c & 0b11110000) == 0b11100000) return 3;
		else if ((c & 0b11111000) == 0b11110000) return 4;
		return 0;
	}


	// UTF-8の始端バイトでないことの判定
	inline bool is_not_utf8_beginning_byte(char c) {
		// バイトの上位2桁が0b10
		return (c & 0b11000000) == 0b10000000;
	}


	// ハイサロゲートかの判定
	inline bool is_high_surrogate(char16_t c) {
		// バイトの上位6桁が0b110110
		return (c & 0b1111110000000000) == 0b1101100000000000;
	}
	inline bool is_high_surrogate(wchar_t c) {
		return (c & 0b1111110000000000) == 0b1101100000000000;
	}
	// ローサロゲートかの判定
	inline bool is_low_surrogate(char16_t c) {
		// バイトの上位6桁が0b110111
		return (c & 0b1111110000000000) == 0b1101110000000000;
	}
	inline bool is_low_surrogate(wchar_t c) {
		return (c & 0b1111110000000000) == 0b1101110000000000;
	}


	// 文字列の先頭のUnicodeの取得
	// UTF-32
	char32_t unicode(const char32_t* c) {
		return c[0];
	}
	// UTF-16
	char32_t unicode(const char16_t* c) {
		if (is_high_surrogate(c[0])) {
			if (is_low_surrogate(c[1])) return 0x10000 + (char32_t(c[0]) - 0xD800) * 0x400 + (char32_t(c[1]) - 0xDC00);
			else return char32_t(0x110000);
		}
		else if (is_low_surrogate(c[0])) return char32_t(0x110000);
		else return char32_t(c[0]);
	}
	char32_t unicode(const wchar_t* c) {
		if (is_high_surrogate(c[0])) {
			if (is_low_surrogate(c[1])) return 0x10000 + (char32_t(c[0]) - 0xD800) * 0x400 + (char32_t(c[1]) - 0xDC00);
			else return char32_t(0x110000);
		}
		else if (is_low_surrogate(c[0])) return char32_t(0x110000);
		else return char32_t(c[0]);
	}
	// UTF-8
	char32_t unicode(const char* c) {
		char32_t result = char32_t(0x110000);;
		switch (utf8_byte(c[0])) {
		case 1:
			result = char32_t(c[0]);
			break;
		case 2:
			// バイトの有効性のチェック
			if (is_not_utf8_beginning_byte(c[1])) break;
			if (!((c[0] & 0b00011111) >= 0b00000010)) break;
			result = char32_t(c[0] & 0b00011111) << 6;
			result |= char32_t(c[1] & 0b00111111);
			break;
		case 3:
			// バイトの有効性のチェック
			if (is_not_utf8_beginning_byte(c[1]) && is_not_utf8_beginning_byte(c[2])) break;
			if ((c[1] & 0b00111111) >= 0b00100000) break;
			result = char32_t(c[0] & 0b00001111) << 12;
			result |= char32_t(c[1] & 0b00111111) << 6;
			result |= char32_t(c[2] & 0b00111111);
			break;
		case 4:
			// バイトの有効性のチェック
			if (is_not_utf8_beginning_byte(c[1]) && is_not_utf8_beginning_byte(c[2]) && is_not_utf8_beginning_byte(c[3])) break;
			if ((c[1] & 0b00111111) >= 0b00010000) break;
			result = char32_t(c[0] & 0b00000111) << 18;
			result |= char32_t(c[1] & 0b00111111) << 12;
			result |= char32_t(c[2] & 0b00111111) << 6;
			result |= char32_t(c[3] & 0b00111111);
			break;
		}
		return result;
	}


	// Unicodeから符号化したものをstrに接続(戻り値は正常に終了したか)
	// UTF-32
	template <class Traits, class Allocator>
	inline bool cat_unicode(std::basic_string<char32_t, Traits, Allocator>& str, char32_t c) {
		if (0x10FFFF < c) return false;
		str += c;
		return true;
	}
	// UTF-16
	template <class Traits, class Allocator>
	inline bool cat_unicode(std::basic_string<char16_t, Traits, Allocator>& str, char32_t c) {
		if (0x10FFFF < c) return false;
		// サロゲートが必要ない場合
		if (c < 0x10000) str += char16_t(c);
		// サロゲートが必要な場合
		else {
			str += char16_t((c - 0x10000) / 0x400 + 0xD800);
			str += char16_t((c - 0x10000) % 0x400 + 0xDC00);
		}
		return true;
	}
	template <class Traits, class Allocator>
	inline bool cat_unicode(std::basic_string<wchar_t, Traits, Allocator>& str, char32_t c) {
		if (0x10FFFF < c) return false;
		// サロゲートが必要ない場合
		if (c < 0x10000) str += char16_t(c);
		// サロゲートが必要な場合
		else {
			str += char16_t((c - 0x10000) / 0x400 + 0xD800);
			str += char16_t((c - 0x10000) % 0x400 + 0xDC00);
		}
		return true;
	}
	// UTF-8
	template <class Traits, class Allocator>
	inline bool cat_unicode(std::basic_string<char, Traits, Allocator>& str, char32_t c) {
		if (0x10FFFF < c) return false;
		// 7ビットで表現(1バイト)
		if (c < 128) {
			str += char(c);
		}
		// 8~11ビットで表現(2バイト)
		else if (c < 2048) {
			str += 0xC0 | char(c >> 6);
			str += 0x80 | (char(c) & 0x3F);
		}
		// 12~16ビットで表現(3バイト)
		else if (c < 65536) {
			str += 0xE0 | char(c >> 12);
			str += 0x80 | (char(c >> 6) & 0b00111111);
			str += 0x80 | (char(c) & 0b00111111);
		}
		// 17~21ビットで表現(4バイト)
		else {
			str += 0xF0 | char(c >> 18);
			str += 0x80 | (char(c >> 12) & 0b00111111);
			str += 0x80 | (char(c >> 6) & 0b00111111);
			str += 0x80 | (char(c) & 0b00111111);
		}
		return true;
	}


	// 次の文字の取得
	// UTF-16
	inline const char16_t* next_char(const char16_t* str, typename std::iterator_traits<const char*>::difference_type n = 1) {
		if (str == nullptr) return nullptr;
		for (typename std::iterator_traits<const char*>::difference_type i = 0; (i < n) && (*str != 0); ++i) {
			if (is_high_surrogate(*str)) str += 2;
			else ++str;
		}
		return str;
	}
	inline char16_t* next_char(char16_t* str, typename std::iterator_traits<const char*>::difference_type n = 1) {
		if (str == nullptr) return nullptr;
		for (typename std::iterator_traits<const char*>::difference_type i = 0; (i < n) && (*str != 0); ++i) {
			if (is_high_surrogate(*str)) str += 2;
			else ++str;
		}
		return str;
	}
	inline const wchar_t* next_char(const wchar_t* str, typename std::iterator_traits<const char*>::difference_type n = 1) {
		if (str == nullptr) return nullptr;
		for (typename std::iterator_traits<const char*>::difference_type i = 0; (i < n) && (*str != 0); ++i) {
			if (is_high_surrogate(*str)) str += 2;
			else ++str;
		}
		return str;
	}
	inline wchar_t* next_char(wchar_t* str, typename std::iterator_traits<const char*>::difference_type n = 1) {
		if (str == nullptr) return nullptr;
		for (typename std::iterator_traits<const char*>::difference_type i = 0; (i < n) && (*str != 0); ++i) {
			if (is_high_surrogate(*str)) str += 2;
			else ++str;
		}
		return str;
	}
	// UTF-8
	inline const char* next_char(const char* str, typename std::iterator_traits<const char*>::difference_type n = 1) {
		if (str == nullptr) return nullptr;
		for (typename std::iterator_traits<const char*>::difference_type i = 0; (i < n) && (*str != 0); ++i) {
			while (*str != 0) {
				++str;
				if (!is_not_utf8_beginning_byte(*str)) break;
			}
		}
		return str;
	}
	inline char* next_char(char* str, typename std::iterator_traits<char*>::difference_type n = 1) {
		if (str == nullptr) return nullptr;
		for (typename std::iterator_traits<const char*>::difference_type i = 0; (i < n) && (*str != 0); ++i) {
			while (*str != 0) {
				++str;
				if (!is_not_utf8_beginning_byte(*str)) break;
			}
		}
		return str;
	}
	// 前の文字の取得
	// UTF-16
	inline const char16_t* prev_char(const char16_t* str, typename std::iterator_traits<const char*>::difference_type n = 1) {
		if (str == nullptr) return nullptr;
		for (typename std::iterator_traits<const char*>::difference_type i = 0; (i < n) && (*str != 0); ++i) {
			--str;
			if (is_low_surrogate(*str)) --str;
		}
		return str;
	}
	inline char16_t* prev_char(char16_t* str, typename std::iterator_traits<const char*>::difference_type n = 1) {
		if (str == nullptr) return nullptr;
		for (typename std::iterator_traits<const char*>::difference_type i = 0; (i < n) && (*str != 0); ++i) {
			--str;
			if (is_low_surrogate(*str)) --str;
		}
		return str;
	}
	inline const wchar_t* prev_char(const wchar_t* str, typename std::iterator_traits<const char*>::difference_type n = 1) {
		if (str == nullptr) return nullptr;
		for (typename std::iterator_traits<const char*>::difference_type i = 0; (i < n) && (*str != 0); ++i) {
			--str;
			if (is_low_surrogate(*str)) --str;
		}
		return str;
	}
	inline wchar_t* prev_char(wchar_t* str, typename std::iterator_traits<const char*>::difference_type n = 1) {
		if (str == nullptr) return nullptr;
		for (typename std::iterator_traits<const char*>::difference_type i = 0; (i < n) && (*str != 0); ++i) {
			--str;
			if (is_low_surrogate(*str)) --str;
		}
		return str;
	}
	// UTF-8
	inline const char* prev_char(const char* str, typename std::iterator_traits<const char*>::difference_type n = 1) {
		if (str == nullptr) return nullptr;
		for (typename std::iterator_traits<const char*>::difference_type i = 0; (i < n) && (*str != 0); ++i) {
			while (*str != 0) {
				--str;
				if (!is_not_utf8_beginning_byte(*str)) break;
			}
		}
		return str;
	}
	inline char* prev_char(char* str, typename std::iterator_traits<const char*>::difference_type n = 1) {
		if (str == nullptr) return nullptr;
		for (typename std::iterator_traits<const char*>::difference_type i = 0; (i < n) && (*str != 0); ++i) {
			while (*str != 0) {
				--str;
				if (!is_not_utf8_beginning_byte(*str)) break;
			}
		}
		return str;
	}


	// Unicode間の変換(str1:出力先文字列,str2:入力文字列)
	template <class CharT1, class Traits1, class Allocator1, class CharT2, class Traits2, class Allocator2>
	inline bool convert_unicode(std::basic_string<CharT1, Traits1, Allocator1>& str1, const std::basic_string<CharT2, Traits2, Allocator2>& str2) {
		const char16_t* p = str2.c_str();
		if (p == nullptr) return true;
		while (*p != 0) {
			if (!cat_unicode(str1, unicode(p))) return false;
			p = next_char(p);
		}
		return true;
	}
	template <class CharT1, class Traits, class Allocator, class CharT2>
	inline bool convert_unicode(std::basic_string<CharT1, Traits, Allocator>& str1, const CharT2* str2) {
		if (str2 == nullptr) return true;
		while (*str2 != 0) {
			if (!cat_unicode(str1, unicode(str2))) return false;
			str2 = next_char(str2);
		}
		return true;
	}

}


#endif