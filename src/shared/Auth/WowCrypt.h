/****************************************************************************
 *
 * General Object Type File
 * Copyright (c) 2007 Antrix Team
 *
 * This file may be distributed under the terms of the Q Public License
 * as defined by Trolltech ASA of Norway and appearing in the file
 * COPYING included in the packaging of this file.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#ifndef _WOWCRYPT_H
#define _WOWCRYPT_H

#include <Common.h>
#include <vector>

class WowCrypt {
public:
	WowCrypt();
	~WowCrypt();

	const static size_t CRYPTED_SEND_LEN = 4;
	const static size_t CRYPTED_RECV_LEN = 6;

	void Init();

	void SetKey(uint8 *, size_t);

	void DecryptRecv(uint8 *, size_t);
	void EncryptSend(uint8 *, size_t);

	bool IsInitialized() { return _initialized; }

private:
	std::vector<uint8> _key;
	uint8 _send_i, _send_j, _recv_i, _recv_j;
	bool _initialized;
};

#endif
