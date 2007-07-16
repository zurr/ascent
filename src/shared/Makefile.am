INCLUDES += -I$(srcdir) -I$(srcdir)/../../dep/include

noinst_LIBRARIES = libshared.a libauth.a libconfig.a libdatabase.a libnetwork.a

libshared_a_SOURCES = \
    Log.cpp \
    MemoryLeaks.cpp \
    MersenneTwister.cpp \
    MersenneTwister.h \
    PreallocatedQueue.h \
    ByteBuffer.h \
    Common.h \
    Errors.h \
    Log.h \
    MemoryLeaks.h \
    Singleton.h \
    Threading.h \
    Timer.h \
    UnixThreading.h \
    Util.h \
    Util.cpp \
    WinThreading.h \
    WorldPacket.h \
    crc32.cpp \
    crc32.h \
    TextLogger.h \
    TextLogger.cpp \
    Threading/Mutex.cpp \
    Storage.h \
    antrix_getopt.h \
    antrix_getopt.cpp

libauth_a_SOURCES = \
    Auth/BigNumber.cpp \
    Auth/BigNumber.h \
    Auth/md5.c \
    Auth/md5.h \
    Auth/Sha1.cpp \
    Auth/Sha1.h \
    Auth/WowCrypt.cpp \
    Auth/WowCrypt.h


libconfig_a_SOURCES = \
    Config/Config.cpp \
    Config/Config.h \
    Config/ConfigEnv.h

libdatabase_a_SOURCES = \
    Database/DBC.cpp \
    Database/Database.cpp \
    Database/DBC.h \
    Database/Database.h \
    Database/DBCStores.cpp \
    Database/DatabaseEnv.h \
    Database/DBCStores.h \
    Database/Field.h \
    Database/DataStore.h \
    Database/dbcfile.cpp \
    Database/dbcfile.h \
    Database/impl/MySQLDatabase.h \
    Database/impl/MySQLDatabase.cpp \
    Database/impl/PostgreDatabase.h \
    Database/impl/PostgreDatabase.cpp

libnetwork_a_SOURCES = \
	Network/BaseBuffer.h \
	Network/BaseSocket.h \
	Network/CircularBuffer.h \
	Network/ListenSocket.h \
	Network/Network.h \
	Network/SocketEngine.cpp \
	Network/SocketEngine.h \
	Network/SocketEngine_epoll.cpp \
	Network/SocketEngine_epoll.h \
	Network/SocketEngine_iocp.cpp \
	Network/SocketEngine_iocp.h \
	Network/SocketEngine_kqueue.cpp \
	Network/SocketEngine_kqueue.h \
	Network/SocketEngine_poll.cpp \
	Network/SocketEngine_poll.h \
	Network/SocketEngine_select.cpp \
	Network/SocketEngine_select.h \
	Network/StraightBuffer.h \
	Network/TcpSocket.cpp \
	Network/TcpSocket.h
    
