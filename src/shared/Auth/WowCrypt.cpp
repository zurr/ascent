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

#include "WowCrypt.h"

#include <algorithm>

WowCrypt::WowCrypt()
{
	_initialized = false;
}

void WowCrypt::Init()
{
	_send_i = _send_j = _recv_i = _recv_j = 0;
	_initialized = true;
}

void WowCrypt::DecryptRecv(uint8 *data, size_t len)
{
	if (!_initialized) return;
	if (len < CRYPTED_RECV_LEN) return;
	uint8 x;

	for (size_t t = 0; t < CRYPTED_RECV_LEN; t++) {
		_recv_i %= _key.size();
		x = (data[t] - _recv_j) ^ _key[_recv_i];
		++_recv_i;
		_recv_j = data[t];
		data[t] = x;
	}
}

void WowCrypt::EncryptSend(uint8 *data, size_t len)
{
	if (!_initialized) return;
	if (len < CRYPTED_SEND_LEN) return;
	uint8 x;

	for (size_t t = 0; t < CRYPTED_SEND_LEN; t++) {
		_send_i %= _key.size();
		x = (data[t] ^ _key[_send_i]) + _send_j;
		++_send_i;
		data[t] = _send_j = x;
	}
}

void WowCrypt::SetKey(uint8 *key, size_t len)
{
	_key.resize(len);
	std::copy(key, key + len, _key.begin());
}

WowCrypt::~WowCrypt()
{
}
