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

#pragma once

#include <list>
#include <vector>
#include <map>
#include <sstream>
#include <string>
//#include <fstream>

#include "../shared/Common.h"
#include "../shared/MersenneTwister.h"
#include "../shared/WorldPacket.h"
#include "../shared/Log.h"
#include "../shared/NGLog.h"
#include "../shared/ByteBuffer.h"
#include "../shared/StackBuffer.h"
#include "../shared/Config/ConfigEnv.h"
#include "../shared/crc32.h"
#include "../shared/LocationVector.h"

#include <zlib/zlib.h>

#include "../shared/Database/DatabaseEnv.h"
#include "../shared/Database/DBCStores.h"
#include "../shared/Database/dbcfile.h"

#include <Network/Network.h>

#include "../shared/Auth/md5.h"
#include "../shared/Auth/BigNumber.h"
#include "../shared/Auth/Sha1.h"
#include "../shared/Auth/WowCrypt.h"
#include "../shared/CrashHandler.h"

#include "../game/Opcodes.h"
#include "../shared/CallBack.h"

#include "Master.h"
#include "WorkerOpcodes.h"
#include "WorkerServerSocket.h"
#include "Structures.h"
#include "WorldSocket.h"
#include "Session.h"
#include "WorkerServer.h"
#include "ClusterManager.h"
#include "ClientManager.h"
#include "LogonCommClient.h"
#include "LogonCommHandler.h"
#include "../shared/Storage.h"
#include "ObjectStorage.h"
