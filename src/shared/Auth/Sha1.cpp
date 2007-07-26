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

#include "Auth/Sha1.h"
#include <stdarg.h>

Sha1Hash::Sha1Hash()
{
	SHA1_Init(&mC);
}

Sha1Hash::~Sha1Hash()
{
	SHA1_Init(&mC);
}

void Sha1Hash::UpdateData(const uint8 *dta, int len)
{
	SHA1_Update(&mC, dta, len);
}

void Sha1Hash::UpdateData(const std::string &str)
{
	UpdateData((uint8 *)str.c_str(), str.length());
}

void Sha1Hash::UpdateBigNumbers(BigNumber *bn0, ...)
{
	va_list v;
	BigNumber *bn;

	va_start(v, bn0);
	bn = bn0;
	while (bn)
	{
		UpdateData(bn->AsByteArray(), bn->GetNumBytes());
		bn = va_arg(v, BigNumber *);
	}
	va_end(v);
}

void Sha1Hash::Initialize()
{
	SHA1_Init(&mC);
}

void Sha1Hash::Finalize(void)
{
	SHA1_Final(mDigest, &mC);
}
