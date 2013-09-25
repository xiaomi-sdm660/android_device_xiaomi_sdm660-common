AM_CFLAGS = \
     $(QMIF_CFLAGS) \
     -I../utils \
     -I../platform_lib_abstractions \
     -I./ulp/inc \
     -I./libloc_api_50001 \
     -I./loc_api_v02 \
     -I./ \
     -fno-short-enums \
     -DFEATURE_GNSS_BIT_API

libloc_adapter_so_la_SOURCES = \
            libloc_api_50001/loc_eng_log.cpp \
            libloc_api_50001/LocApiAdapter.cpp \
            fake_property_service.c \
            fake_sched_policy.c

if USE_GLIB
libloc_adapter_so_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libloc_adapter_so_la_LDFLAGS = -lstdc++ -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
libloc_adapter_so_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libloc_adapter_so_la_CFLAGS = $(AM_CFLAGS)
libloc_adapter_so_la_LDFLAGS = -lpthread -shared -version-info 1:0:0
libloc_adapter_so_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif
libloc_adapter_so_la_LIBADD = -lstdc++ ../utils/libgps_utils_so.la


libloc_api_la_SOURCES = \
            loc_api_v02/LocApiV02Adapter.cpp \
            loc_api_v02/loc_api_v02_log.c \
            loc_api_v02/loc_api_v02_client.c \
            loc_api_v02/loc_api_sync_req.c \
            loc_api_v02/location_service_v02.c \
            loc_api_v02/LocApiV02Adapter.h \
            loc_api_v02/loc_util_log.h \
            loc_api_v02/location_service_v02.h \
            loc_api_v02/loc_api_sync_req.h \
            loc_api_v02/loc_api_v02_client.h \
            loc_api_v02/loc_api_v02_log.h

if USE_GLIB
libloc_api_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libloc_api_la_LDFLAGS = -lstdc++ -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
libloc_api_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libloc_api_la_CFLAGS = $(AM_CFLAGS)
libloc_api_la_LDFLAGS = -shared -version-info 1:0:0
libloc_api_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libloc_api_la_LIBADD = -lstdc++ $(QMIF_LIBS) ../utils/libgps_utils_so.la libloc_adapter_so.la

libloc_eng_so_la_SOURCES = \
    libloc_api_50001/loc_eng.cpp \
    libloc_api_50001/loc_eng_agps.cpp \
    libloc_api_50001/loc_eng_xtra.cpp \
    libloc_api_50001/loc_eng_ni.cpp \
    libloc_api_50001/loc_eng_log.cpp \
    libloc_api_50001/loc_eng_dmn_conn.cpp \
    libloc_api_50001/loc_eng_dmn_conn_handler.cpp \
    libloc_api_50001/loc_eng_dmn_conn_thread_helper.c \
    libloc_api_50001/loc_eng_dmn_conn_glue_msg.c \
    libloc_api_50001/loc_eng_dmn_conn_glue_pipe.c \
    fake_property_service.c


if USE_GLIB
libloc_eng_so_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libloc_eng_so_la_LDFLAGS = -lstdc++ -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
libloc_eng_so_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libloc_eng_so_la_CFLAGS = $(AM_CFLAGS)
libloc_eng_so_la_LDFLAGS = -lpthread -shared -version-info 1:0:0
libloc_eng_so_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libloc_eng_so_la_LIBADD = -lstdc++ -ldl ../utils/libgps_utils_so.la libloc_adapter_so.la libloc_api.la


libgps_default_so_la_SOURCES = \
      libloc_api_50001/loc.cpp \
      libloc_api_50001/gps.c \
      fake_property_service.c

if USE_GLIB
libgps_default_so_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libgps_default_so_la_LDFLAGS = -lstdc++ -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
libgps_default_so_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libgps_default_so_la_CFLAGS = $(AM_CFLAGS)
libgps_default_so_la_LDFLAGS = -lpthread -shared -version-info 1:0:0
libgps_default_so_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libgps_default_so_la_LIBADD = -lstdc++ ../utils/libgps_utils_so.la -ldl libloc_eng_so.la

library_include_HEADERS = \
   libloc_api_50001/LocApiAdapter.h \
   libloc_api_50001/loc.h \
   libloc_api_50001/loc_eng.h \
   libloc_api_50001/loc_eng_xtra.h \
   libloc_api_50001/loc_eng_ni.h \
   libloc_api_50001/loc_eng_agps.h \
   libloc_api_50001/loc_eng_msg.h \
   libloc_api_50001/loc_eng_msg_id.h \
   libloc_api_50001/loc_eng_log.h \
   loc_api_v02/LocApiV02Adapter.h \
   loc_api_v02/loc_util_log.h \
   loc_api_v02/location_service_v02.h \
   loc_api_v02/loc_api_sync_req.h \
   loc_api_v02/loc_api_v02_client.h \
   loc_api_v02/loc_api_v02_log.h \
   ulp/inc/ulp.h \
   fake_property_service.h

library_includedir = $(pkgincludedir)

#Create and Install libraries
lib_LTLIBRARIES = libloc_adapter_so.la libloc_api.la libloc_eng_so.la libgps_default_so.la
