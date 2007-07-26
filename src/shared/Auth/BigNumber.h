/****************************************************************************
 *
 * General Object Type File
 * Copyright (c) 2007 Team Ascent
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0
 * License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to Creative Commons,
 * 543 Howard Street, 5th Floor, San Francisco, California, 94105, USA.
 *
 * EXCEPT TO THE EXTENT REQUIRED BY APPLICABLE LAW, IN NO EVENT WILL LICENSOR BE LIABLE TO YOU
 * ON ANY LEGAL THEORY FOR ANY SPECIAL, INCIDENTAL, CONSEQUENTIAL, PUNITIVE OR EXEMPLARY DAMAGES
 * ARISING OUT OF THIS LICENSE OR THE USE OF THE WORK, EVEN IF LICENSOR HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 *
 */

#ifndef _AUTH_BIGNUMBER_H
#define _AUTH_BIGNUMBER_H

#include "Common.h"
#include "ByteBuffer.h"

//#include "openssl/bn.h"
struct bignum_st;

class BigNumber
{
	public:
		BigNumber();
		BigNumber(const BigNumber &bn);
		BigNumber(uint32);
		~BigNumber();

		void SetDword(uint32);
		void SetQword(uint64);
		void SetBinary(const uint8 *bytes, int len);
		void SetHexStr(const char *str);

		void SetRand(int numbits);

		BigNumber operator=(const BigNumber &bn);
//	  BigNumber operator=(Sha1Hash &hash);

		BigNumber operator+=(const BigNumber &bn);
		BigNumber operator+(const BigNumber &bn) {
			BigNumber t(*this);
			return t += bn;
		}
		BigNumber operator-=(const BigNumber &bn);
		BigNumber operator-(const BigNumber &bn) {
			BigNumber t(*this);
			return t -= bn;
		}
		BigNumber operator*=(const BigNumber &bn);
		BigNumber operator*(const BigNumber &bn) {
			BigNumber t(*this);
			return t *= bn;
		}
		BigNumber operator/=(const BigNumber &bn);
		BigNumber operator/(const BigNumber &bn) {
			BigNumber t(*this);
			return t /= bn;
		}
		BigNumber operator%=(const BigNumber &bn);
		BigNumber operator%(const BigNumber &bn) {
			BigNumber t(*this);
			return t %= bn;
		}

		BigNumber ModExp(const BigNumber &bn1, const BigNumber &bn2);
		BigNumber Exp(const BigNumber &);

		int GetNumBytes(void);

		struct bignum_st *BN() { return _bn; }

		uint32 AsDword();
		uint8* AsByteArray();
		ByteBuffer AsByteBuffer();
		std::vector<uint8> AsByteVector();

		const char *AsHexStr();
		const char *AsDecStr();

	private:
		struct bignum_st *_bn;
		uint8 *_array;
};

#endif
