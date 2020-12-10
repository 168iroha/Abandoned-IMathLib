#ifndef _BITSET_HPP
#define _BITSET_HPP

#include "IMathLib/IMathLib_config.hpp"
#include "IMathLib/string/string.hpp"
#include "IMathLib/math/math.hpp"

namespace iml {

	//ビットセットクラス
	template <size_t N>
	class bitset {
		template <size_t> friend class bitset;

		static constexpr size_t	array_size = ((N - 1) >> 3) + 1;
		unsigned char				x[array_size];

		//最上位バイトに対してビットマスクを作用させてN桁分以外は0クリア
		bitset& byte_check() {
			//8の倍数のときは除外
			if ((N & 7) != 0) x[array_size - 1] &= (1 << (N & 7)) - 1;
			return *this;
		}
	public:
		constexpr bitset() :x{} {}
		bitset(const bitset& b) {
			for (size_t i = 0; i < array_size; ++i) this->x[i] = b.x[i];
		}
		template <size_t N2>
		bitset(const bitset<N2>& b) : x{} {
			for (size_t i = 0, n = (min)(this->array_size, b.array_size); i < n; ++i) this->x[i] = b.x[i];
		}
		bitset(size_t n) :x{} {
			//全てを場合分けして定義
			switch (array_size) {
			case 1:
				x[0] = static_cast<unsigned char>(n & 0xFF);
				break;
			case 2:
				x[0] = static_cast<unsigned char>(n & 0xFF);
				x[1] = static_cast<unsigned char>((n & (0xFF << 8)) >> 8);
				break;
			case 3:
				x[0] = static_cast<unsigned char>(n & 0xFF);
				x[1] = static_cast<unsigned char>((n & (0xFF << 8)) >> 8);
				x[2] = static_cast<unsigned char>((n & (0xFF << 16)) >> 16);
				break;
				//64bitの場合はさらに場合分けを与える
#if defined _IMATH_INT_64BIT_
			case 4:
				x[0] = static_cast<unsigned char>(n & 0xFF);
				x[1] = static_cast<unsigned char>((n & (0xFF << 8)) >> 8);
				x[2] = static_cast<unsigned char>((n & (0xFF << 16)) >> 16);
				x[3] = static_cast<unsigned char>((n & (0xFF << 24)) >> 24);
				break;
			case 5:
				x[0] = static_cast<unsigned char>(n & 0xFF);
				x[1] = static_cast<unsigned char>((n & (0xFF << 8)) >> 8);
				x[2] = static_cast<unsigned char>((n & (0xFF << 16)) >> 16);
				x[3] = static_cast<unsigned char>((n & (0xFF << 24)) >> 24);
				x[4] = static_cast<unsigned char>((n & (0xFF << 32)) >> 32);
				break;
			case 6:
				x[0] = static_cast<unsigned char>(n & 0xFF);
				x[1] = static_cast<unsigned char>((n & (0xFF << 8)) >> 8);
				x[2] = static_cast<unsigned char>((n & (0xFF << 16)) >> 16);
				x[3] = static_cast<unsigned char>((n & (0xFF << 24)) >> 24);
				x[4] = static_cast<unsigned char>((n & (0xFF << 32)) >> 32);
				x[5] = static_cast<unsigned char>((n & (0xFF << 40)) >> 40);
				break;
			case 7:
				x[0] = static_cast<unsigned char>(n & 0xFF);
				x[1] = static_cast<unsigned char>((n & (0xFF << 8)) >> 8);
				x[2] = static_cast<unsigned char>((n & (0xFF << 16)) >> 16);
				x[3] = static_cast<unsigned char>((n & (0xFF << 24)) >> 24);
				x[4] = static_cast<unsigned char>((n & (0xFF << 32)) >> 32);
				x[5] = static_cast<unsigned char>((n & (0xFF << 40)) >> 40);
				x[6] = static_cast<unsigned char>((n & (0xFF << 48)) >> 48);
				break;
#endif
			default:
				x[0] = static_cast<unsigned char>(n & 0xFF);
				x[1] = static_cast<unsigned char>((n & (0xFF << 8)) >> 8);
				x[2] = static_cast<unsigned char>((n & (0xFF << 16)) >> 16);
				x[3] = static_cast<unsigned char>((n & (0xFF << 24)) >> 24);
#if defined _IMATH_INT_64BIT_
				x[4] = static_cast<unsigned char>((n & (0xFF << 32)) >> 32);
				x[5] = static_cast<unsigned char>((n & (0xFF << 40)) >> 40);
				x[6] = static_cast<unsigned char>((n & (0xFF << 48)) >> 48);
				x[7] = static_cast<unsigned char>((n & (0xFF << 56)) >> 56);
#endif
			}
			byte_check();
		}
		template <class CharT, class Predicate, class Allocator>
		explicit bitset(const string<CharT, Predicate, Allocator>& str) :x{} {
			for (size_t i = 0; i < array_size; ++i) {
				for (size_t j = 0; j < 8; ++j) {
					if (str.size() - 2 - ((i << 3) + j) < 0) x[i] &= ((1 << j) - 1);
					else if (str[str.size() - 2 - ((i << 3) + j)] == '1') x[i] |= (1 << j);
					else if (str[str.size() - 2 - ((i << 3) + j)] == '0') x[i] &= ~(1 << j);
				}
			}
			byte_check();
		}
		~bitset() {}

		//単項演算子
		bitset operator~() const {
			bitset<N> temp;
			for (size_t i = 0; i < array_size; ++i) temp.x[i] = ~this->x[i];
			return temp.byte_check();
		}
		//代入演算子
		bitset& operator=(const bitset& b) {
			for (size_t i = 0; i < array_size; ++i) this->x[i] = b.x[i];
		}
		bitset& operator&=(const bitset& b) {
			for (size_t i = 0; i < array_size; ++i) this->x[i] &= b.x[i];
		}
		bitset& operator|=(const bitset& b) {
			for (size_t i = 0; i < array_size; ++i) this->x[i] |= b.x[i];
		}
		bitset& operator^=(const bitset& b) {
			for (size_t i = 0; i < array_size; ++i) this->x[i] ^= b.x[i];
		}
		bitset& operator<<=(size_t pos) {
			size_t surplus = pos & 7;		//余るサイズ
			size_t shift = pos >> 3;		//シフトで飛び越える配列の量
			//surplus分のシフト
			for (size_t i = 1; i <= array_size; ++i) {
				x[array_size - i] <<= surplus;
				//下の配列から持ってくる
				if (array_size != i) x[array_size - i] |= (x[array_size - 1 - i] >> (8 - surplus));
			}
			//shift分コピー
			for (size_t i = 1; array_size >= i + shift; ++i) x[array_size - i] = x[array_size - i - shift];
			//0クリア
			for (size_t i = 0; i < shift; ++i) x[i] = 0;
			return byte_check();
		}
		bitset& operator>>=(size_t pos) {
			size_t surplus = pos & 7;		//余るサイズ
			size_t shift = pos >> 3;		//シフトで飛び越える配列の量
			//surplus分のシフト
			for (size_t i = 0; i < array_size; ++i) {
				x[i] >>= surplus;
				//上の配列から持ってくる
				if (i + 1 != array_size) x[i] |= (x[i + 1] << (8 - surplus));
			}
			//shift分コピー
			for (size_t i = 0; i + shift < array_size; ++i) x[i] = x[i + shift];
			//全て消える配列
			for (size_t i = 1; i <= shift; ++i) x[array_size - i] = 0;
			return *this;
		}
		//比較演算子
		bool operator==(const bitset& b) const {
			for (size_t i = 0; i < array_size; ++i) if (this->x[i] != b.x[i]) return false;
			return true;
		}
		bool operator!=(const bitset& b) const {
			return (*this == b);
		}

		constexpr size_t size() const noexcept { return N; }
		//先頭から任意バイト目を取得
		unsigned char byte(size_t n) const { return x[n]; }
		//先頭から任意ビット目を取得
		bool operator[](size_t pos) const { return (x[pos >> 3] & (1 << (pos & 7))) > 0; }
		bool bit(size_t pos) const { return (x[pos >> 3] & (1 << (pos & 7))) > 0; }

		//リセット
		bitset& reset() {
			for (size_t i = 0; i < array_size; ++i) x[i] = 0;
			return *this;
		}
		//ビットのセット
		bitset& set() {
			for (size_t i = 0; i < array_size; ++i) x[i] = 0xFF;
			return byte_check();
		}
		bitset& set(size_t pos, bool flag = true) {
			if (pos >= N) return *this;
			if(flag) x[pos >> 3] |= (1 << (pos & 7));
			else x[pos >> 3] &= ~(1 << (pos & 7));
			return *this;
		}
		//ビットの反転
		bitset& flip() {
			for (size_t i = 0; i < array_size; ++i) this->x[i] = ~this->x[i];
			return byte_check();
		}
		//ビットの反転
		bitset& flip(size_t pos) {
			if (pos >= N) return *this;
			x[pos >> 3] ^= (1 << (pos & 7));
			return *this;
		}

		//整数への変換
		size_t to_uint() const {
			size_t result = 0;
			for (size_t i = 0, n = (min<size_t>)(4, array_size); i < n; ++i) result |= x[i] << (8 * i);
			return result;
		}
		//整数への変換
		unsigned long to_ulong() const {
			size_t result = 0;
			for (size_t i = 0, n = (min<size_t>)(8, array_size); i < n; ++i) result |= x[i] << (8 * i);
			return result;
		}
		//文字列への変換
		template <class CharT, class Predicate = type_comparison<CharT>, class Allocator = allocator<CharT, array_iterator<CharT>>>
		string<CharT, Predicate, Allocator> to_string() const {
			string<CharT, Predicate, Allocator> result;
			result.reserve(N + 1);
			//最初はN桁分の確保によるビットのズレの分
			{
				unsigned char temp = x[array_size - 1];
				//Nが8の倍数とで分岐
				if ((N & 7) != 0) {
					for (size_t j = 9 - (N & 7); j <= 8; ++j)
						result.push_back(((temp & (1 << (8 - j))) > 0) ? '1' : '0');
				}
				else {
					for (size_t j = 1; j <= 8; ++j)
						result.push_back(((temp & (1 << (8 - j))) > 0) ? '1' : '0');
				}
			}
			for (size_t i = 2; i <= array_size; ++i) {
				unsigned char temp = x[array_size - i];
				for (size_t j = 1; j <= 8; ++j)
					result.push_back(((temp & (1 << (8 - j))) > 0) ? '1' : '0');
			}
			return result;
		}
		//1の数のカウント
		size_t count() const {
			size_t  result = 0;
			for (size_t i = 0; i < array_size; ++i) {
				unsigned char temp = x[i];
				for (size_t j = 0; j < 8; ++j)
					result += (temp & (1 << j)) > 0;
			}
			return result;
		}
	};

	//二項演算子
	template <size_t N>
	inline bitset<N> operator&(const bitset<N>& b1, const bitset<N>& b2) {
		bitset<N> temp(b1);
		return temp &= b2;
	}
	template <size_t N>
	inline bitset<N> operator|(const bitset<N>& b1, const bitset<N>& b2) {
		bitset<N> temp(b1);
		return temp |= b2;
	}
	template <size_t N>
	inline bitset<N> operator^(const bitset<N>& b1, const bitset<N>& b2) {
		bitset<N> temp(b1);
		return temp ^= b2;
	}
	template <size_t N>
	inline bitset<N> operator<<(const bitset<N>& b, size_t n) {
		bitset<N> temp(b);
		return temp <<= n;
	}
	template <size_t N>
	inline bitset<N> operator>>(const bitset<N>& b, size_t n) {
		bitset<N> temp(b);
		return temp >>= n;
	}
}


namespace iml {

	//1ビット半加算器
	struct half_adder {
		bool c;				//桁上がり
		bool s;				//加算結果

		constexpr half_adder() : c(false), s(false) {}
		constexpr half_adder(bool a, bool b) : c(a&b), s(a^b) {}

		void operator()(bool a, bool b) { c = a & b; s = a ^ b; }
	};
	//1ビット全加算器
	struct full_adder {
		bool c0;			//桁上がり
		bool s;				//加算結果

		constexpr full_adder() : c0(false), s(false) {}
		//c:下位からの桁上がり
		constexpr full_adder(bool a, bool b, bool c) : c0(a&c | b & c | a & b), s(a^b^c) {}

		void operator()(bool a, bool b, bool c) { c0 = a & c | b & c | a & b; s = a ^ b ^ c; }
	};
	//nビット全加算器
	template <size_t N>
	struct n_full_adder {
		bool		c0;			//桁上がり
		bitset<N>	s;			//加算結果

		constexpr n_full_adder() : c0(false) {}
		//c:下位からの桁上がり
		n_full_adder(const bitset<N>& a, const bitset<N>& b) {
			bool c = false;			//桁上がり
			//リップルキャリーによる実装
			for (size_t i = 0; i < N; ++i) {
				full_adder temp(a[i], b[i], c);
				s.set(i, temp.s);
				c = temp.c0;
			}
			c0 = c;
		}

		void operator()(const bitset<N>& a, const bitset<N>& b) {
			bool c = false;			//桁上がり
			//リップルキャリーによる実装
			for (size_t i = 0; i < N; ++i) {
				full_adder temp(a[i], b[i], c);
				s.set(i, temp.s);
				c = temp.c0;
			}
			c0 = c;
		}
	};


	//2の補数の取得(ビット反転して1を足す)
	template <size_t N>
	inline bitset<N> twos_complement(const bitset<N>& b) {
		bitset<N> temp1(~b);
		half_adder temp2;
		temp2.c = true;
		for (size_t i = 0; (i < N) && temp2.c; ++i) {
			temp2(temp1[i], temp2.c);
			temp1.set(i, temp2.s);
		}
		if (temp2.c) temp1.set(N - 1, temp2.c);

		return temp1;
	}

	//nビット減算器
	template <size_t N>
	struct n_subtractor {
		bool		c0;			//桁上がり(負数の判定)
		bitset<N>	s;			//減算結果

		constexpr n_subtractor() : c0(false) {}
		//c:下位からの桁上がり
		n_subtractor(const bitset<N>& a, const bitset<N>& b) {
			bool c = true;			//桁上がり
			//リップルキャリーによる実装
			for (size_t i = 0; i < N; ++i) {
				full_adder temp(a[i], !b[i], c);
				s.set(i, temp.s);
				c = temp.c0;
			}
			c0 = c;
		}

		void operator()(const bitset<N>& a, const bitset<N>& b) {
			bool c = true;			//桁上がり
			//リップルキャリーによる実装
			for (size_t i = 0; i < N; ++i) {
				full_adder temp(a[i], !b[i], c);
				s.set(i, temp.s);
				c = temp.c0;
			}
			c0 = c;
		}
	};

	//HL指定式SR-FF(true:H,false:L)
	template <bool HL>
	struct SR_FF {
		bool	q, nq;			//出力

		constexpr SR_FF() : q(false), nq(true) {}
		SR_FF(bool ini_q, bool s, bool r, bool clock) : q(ini_q), nq(!ini_q) {
			//クロックのnot処理
			clock = (HL) ? clock : !clock;
			bool ns = !(s & clock), nr = !(r & clock);
			//2周分計算して定常状態にする
			q = !(ns & nq);
			nq = !(nr & q);
			q = !(ns & nq);
			nq = !(nr & q);
		}

		void operator()(bool s, bool r, bool clock) {
			//クロックのnot処理
			clock = (HL) ? clock : !clock;
			bool ns = !(s & clock), nr = !(r & clock);
			//2周分計算して定常状態にする
			q = !(ns & nq);
			nq = !(nr & q);
			q = !(ns & nq);
			nq = !(nr & q);
		}

		//clear信号
		void clear() { q = false; nq = true; }
	};

	//HL指定式D-FF(true:H,false:L)
	template <bool HL>
	struct D_FF {
		bool	q, nq;			//出力

		constexpr D_FF() : q(false), nq(true) {}
		D_FF(bool ini_q, bool d, bool clock) : q(ini_q), nq(!ini_q) {
			//クロックのnot処理
			clock = (HL) ? clock : !clock;
			bool ns = !(d & clock), nr = !(!d & clock);
			//2周分計算して定常状態にする
			q = !(ns & nq);
			nq = !(nr & q);
			q = !(ns & nq);
			nq = !(nr & q);
		}

		void operator()(bool d, bool clock) {
			//クロックのnot処理
			clock = (HL) ? clock : !clock;
			bool ns = !(d & clock), nr = !(!d & clock);
			//2周分計算して定常状態にする
			q = !(ns & nq);
			nq = !(nr & q);
			q = !(ns & nq);
			nq = !(nr & q);
		}

		//clear信号
		void clear() { q = false; nq = true; }
	};

	//HL指定式JK-FF(true:H,false:L)
	template <bool HL>
	struct JK_FF {
		bool	q, nq;			//出力

		constexpr JK_FF() : q(false), nq(true) {}
		JK_FF(bool ini_q, bool j, bool k, bool clock) : q(ini_q), nq(!ini_q) {
			//クロックのnot処理
			clock = (HL) ? clock : !clock;
			bool ns = !(j & clock & nq), nr = !(k & clock & q);
			//2周分計算して定常状態にする
			q = !(ns & nq);
			nq = !(nr & q);
			q = !(ns & nq);
			nq = !(nr & q);
		}

		void operator()(bool j, bool k, bool clock) {
			//クロックのnot処理
			clock = (HL) ? clock : !clock;
			bool ns = !(j & clock & nq), nr = !(k & clock & q);
			//2周分計算して定常状態にする
			q = !(ns & nq);
			nq = !(nr & q);
			q = !(ns & nq);
			nq = !(nr & q);
		}

		//clear信号
		void clear() { q = false; nq = true; }
	};

	//HL指定式T-FF(true:H,false:L)
	template <bool HL>
	struct T_FF {
		bool	q, nq;			//出力

		constexpr T_FF() : q(false), nq(true) {}
		T_FF(bool ini_q, bool t, bool clock) : q(ini_q), nq(!ini_q) {
			//クロックのnot処理
			clock = (HL) ? clock : !clock;
			bool ns = !(t & clock), nr = !(t & clock);
			//2周分計算して定常状態にする
			q = !(ns & nq);
			nq = !(nr & q);
			q = !(ns & nq);
			nq = !(nr & q);
		}

		void operator()(bool t, bool clock) {
			//クロックのnot処理
			clock = (HL) ? clock : !clock;
			bool ns = !(t & clock & nq), nr = !(t & clock & q);
			//2周分計算して定常状態にする
			q = !(ns & nq);
			nq = !(nr & q);
			q = !(ns & nq);
			nq = !(nr & q);
		}

		//clear信号
		void clear() { q = false; nq = true; }
	};
}

#endif