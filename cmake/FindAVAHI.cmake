
find_file(AVAHILOCALCOMMON_INCLUDE_DIR avahi-common/defs.h
	/usr/local/include
)

find_file(AVAHILOCALCLIENT_INCLUDE_DIR avahi-client/client.h
	/usr/local/include
)

find_file(AVAHILOCALCOMMON_LIBRARY libavahi-common.so
	/usr/local/lib
)

find_file(AVAHILOCALCLIENT_LIBRARY libavahi-client.so
	/usr/local/lib
)

if (
	AVAHILOCALCOMMON_INCLUDE_DIR
		AND
	AVAHILOCALCLIENT_INCLUDE_DIR
		AND
	AVAHILOCALCOMMON_LIBRARY
		AND
	AVAHILOCALCLIENT_LIBRARY
)

	set(AVAHI_FOUND "yes")
	set(AVAHI_INCLUDES /usr/local/include)
	set(AVAHI_LIBRARIES ${AVAHILOCALCOMMON_LIBRARY} ${AVAHILOCALCLIENT_LIBRARY})    
	set(MESSAGE "STATUS:\n
         FOUND A LOCAL VERSION OF AVAHI: 
             LIBRARIES: ${AVAHILOCALCOMMON_LIBRARY}, ${AVAHILOCALCLIENT_LIBRARY}
             INCLUDES: /usr/local/include")

ELSE()


	find_file(AVAHICOMMON_INCLUDE_DIR avahi-common/defs.h
		/usr/include
	)

	find_file(AVAHICLIENT_INCLUDE_DIR avahi-client/client.h
		/usr/include
	)

	find_file(AVAHICOMMON_LIBRARY libavahi-common.so
		/usr/lib
	)

	find_file(AVAHICLIENT_LIBRARY libavahi-client.so
		/usr/lib
	)

	IF (
		AVAHICOMMON_INCLUDE_DIR
			AND
		AVAHICLIENT_INCLUDE_DIR
			AND
		AVAHICOMMON_LIBRARY
			AND
		AVAHICLIENT_LIBRARY
	)

	set(AVAHI_FOUND "yes")
	set(AVAHI_INCLUDES /usr/include)
	set(AVAHI_LIBRARIES ${AVAHICOMMON_LIBRARY} ${AVAHICLIENT_LIBRARY})    
	set(MESSAGE "STATUS:\n
         FOUND AVAHI: 
             LIBRARIES: ${AVAHICOMMON_LIBRARY}, ${AVAHICLIENT_LIBRARY}
             INCLUDES: /usr/include")

	ENDIF()

ENDIF()

    
    
IF (AVAHI_FOUND)

   IF (NOT AVAHI_FIND_QUIETLY)
      MESSAGE(${MESSAGE})
   ENDIF ()


ELSE ()
   IF (AVAHI_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Couldn't find avahi-common and avahi-client development files")
   ENDIF ()
ENDIF ()
