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

#ifndef _AUTH_SHA1_H
#define _AUTH_SHA1_H

#include "Common.h"
#include <openssl/sha.h>
#include "Auth/BigNumber.h"

class Sha1Hash
{
	public:
		Sha1Hash();
		~Sha1Hash();

		void UpdateFinalizeBigNumbers(BigNumber *bn0, ...);
		void UpdateBigNumbers(BigNumber *bn0, ...);

		void UpdateData(const uint8 *dta, int len);
		void UpdateData(const std::string &str);

		void Initialize();
		void Finalize();

		uint8 *GetDigest(void) { return mDigest; };
		int GetLength(void) { return SHA_DIGEST_LENGTH; };

		BigNumber GetBigNumber();

	private:
		SHA_CTX mC;
		uint8 mDigest[SHA_DIGEST_LENGTH];
};

#endif
