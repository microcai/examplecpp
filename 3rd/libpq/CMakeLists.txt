add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE -D_WINSOCK_DEPRECATED_NO_WARNINGS -DHAVE_INET_ATON -DFRONTEND)
include_directories(include)
add_library(libpq STATIC


include/arpa
include/arpa/inet.h
include/c.h
include/common/config_info.h
include/common/controldata_utils.h
include/common/fe_memutils.h
include/common/int128.h
include/common/keywords.h
include/common/pg_lzcompress.h
include/common/relpath.h
include/common/restricted_token.h
include/common/string.h
include/common/username.h
include/dirent.h
include/fe-auth.h
include/getaddrinfo.h
include/libpq/auth.h
include/libpq/be-fsstubs.h
include/libpq/crypt.h
include/libpq/hba.h
include/libpq/ip.h
include/libpq/libpq-be.h
include/libpq/libpq-fs.h
include/libpq/libpq.h
include/libpq/md5.h
include/libpq/pqcomm.h
include/libpq/pqformat.h
include/libpq/pqmq.h
include/libpq/pqsignal.h
include/libpq-events.h
include/libpq-fe.h
include/libpq-int.h
include/mb/pg_wchar.h
include/netdb.h
include/netinet
include/netinet/in.h
include/pg_config.h
include/pg_config_ext.h
include/pg_config_manual.h
include/pg_config_os.h
include/pg_config_paths.h
include/port.h
include/postgres_ext.h
include/postgres_fe.h
include/pqexpbuffer.h
include/pthread-win32.h
include/pwd.h
include/sys/file.h
include/sys/param.h
include/sys/socket.h
include/sys/time.h
include/sys/wait.h
include/unistd.h
include/win32.h


src/chklocale.c
src/encnames.c
src/fe-auth.c
src/fe-connect.c
src/fe-exec.c
src/fe-lobj.c
src/fe-misc.c
src/fe-print.c
src/fe-protocol2.c
src/fe-protocol3.c
src/fe-secure.c
src/getaddrinfo.c
src/inet_net_ntop.c
src/libpq-events.c
src/md5.c
src/noblock.c
src/pgstrcasecmp.c
src/pqexpbuffer.c
src/pqsignal.c
src/pthread-win32.c
src/strlcpy.c
src/thread.c
src/wchar.c
src/win32.c
src/snprintf.c
src/dirmod.c
src/ip.c
src/win32error.c
src/inet_aton.c
src/win32setlocale.c
src/pgsleep.c
)

set_target_properties(libpq PROPERTIES FOLDER "third_party")

target_link_libraries(libpq Secur32)



