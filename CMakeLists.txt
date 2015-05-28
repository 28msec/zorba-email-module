# Copyright 2006-2010 The FLWOR Foundation.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
# http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

MESSAGE(STATUS "")
MESSAGE(STATUS "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++")
MESSAGE(STATUS "Configuring external module <email>")
MESSAGE(STATUS "-------------------------------------------------------------")
MESSAGE(STATUS "")

CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

PROJECT (zorba_email_module)
ENABLE_TESTING ()
INCLUDE (CTest)

FIND_PACKAGE (Zorba REQUIRED HINTS "${ZORBA_BUILD_DIR}")
INCLUDE ("${Zorba_USE_FILE}")

SET_CMAKE_MODULE_PATH()

# CClient library part of the UW IMAP toolkit required by the email
# functionality
IF (ZORBA_SUPPRESS_CCLIENT)
  MESSAGE(STATUS "ZORBA_SUPPRESS_CCLIENT is true - not searching for C-Client")
  
  MESSAGE(STATUS "")
  MESSAGE(STATUS "-------------------------------------------------------------")  
  MESSAGE(STATUS "Configuration SUPPRESSED for external module <email>")
    
ELSE(ZORBA_SUPPRESS_CCLIENT)

  MESSAGE (STATUS "Looking for CClient")
  FIND_PACKAGE (CClient)

  IF (NOT WIN32 AND NOT CYGWIN)
    FIND_PACKAGE(OpenSSL)
    FIND_PACKAGE(PAM)
    FIND_PACKAGE(Kerberos)
  ENDIF (NOT WIN32 AND NOT CYGWIN)

  IF(CCLIENT_FOUND)
    SET(SMTP_LINK_LIBRARIES ${CCLIENT_LIBRARIES})
  
    IF (OPENSSL_FOUND)
      LIST(APPEND SMTP_LINK_LIBRARIES ${OPENSSL_LIBRARIES})
      LIST(APPEND SMTP_LINK_LIBRARIES ${OPENSSL_CRYPTO_LIBRARIES})
    ELSE (OPENSSL_FOUND)
      MESSAGE(STATUS "CClient library found but OpenSSL not found. This may cause runtime problems on certain platforms where CClient is linked against OpenSSL by default, including MacOS X and Ubuntu Linux (and probably others). See this module's README.txt for more information.")
    ENDIF (OPENSSL_FOUND)
  
    # Mac Ports and Ubuntu compiles CCLient with kerberos support by default.
    IF(KERBEROS_FOUND)
      LIST(APPEND SMTP_LINK_LIBRARIES ${KERBEROS_LIBRARY})
    ELSE (KERBEROS_FOUND)
      MESSAGE(STATUS "CClient library found but Kerberos not found. This may cause runtime problems on certain platforms where CClient is linked against Kerberos by default, including MacOS X and Ubuntu Linux (and probably others). See this module's README.txt for more information.")
    ENDIF (KERBEROS_FOUND)
    
    INCLUDE_DIRECTORIES(${CCLIENT_INCLUDE_DIRS})
    IF (NOT WIN32 AND NOT CYGWIN)
      LIST(APPEND SMTP_LINK_LIBRARIES ${PAM_LIBRARIES})
    ELSE (NOT WIN32 AND NOT CYGWIN)
      LIST(APPEND SMTP_LINK_LIBRARIES secur32.lib crypt32.lib winmm.lib ws2_32.lib)
    ENDIF (NOT WIN32 AND NOT CYGWIN)
   
    # including shared c-client library
    ADD_LIBRARY (imap_commons STATIC
      src/cclient/imap_client.cpp
      src/cclient/email_exception.cpp
    )
    SET_TARGET_PROPERTIES (imap_commons PROPERTIES
      FOLDER "Modules"
    )

    IF(UNIX AND NOT WIN32)
      ADD_DEFINITIONS(-fPIC)

      # option needed to make c-client compilable with clang
      IF (CLANG)
        SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fms-compatibility")
      ENDIF()

    ENDIF(UNIX AND NOT WIN32)

    TARGET_LINK_LIBRARIES(imap_commons ${SMTP_LINK_LIBRARIES})
    INCLUDE_DIRECTORIES("src/cclient")

    ADD_SUBDIRECTORY ("src")
    ADD_TEST_DIRECTORY ("${PROJECT_SOURCE_DIR}/test")
    
    MESSAGE(STATUS "")
    MESSAGE(STATUS "-------------------------------------------------------------")
    MESSAGE(STATUS "Configuration SUCCEEDED for external module <email>")
  
  ELSE(CCLIENT_FOUND)
    MESSAGE(STATUS "CClient library not found -- if you want to use Email functionality please set CCLIENT_INCLUDE_DIRS and CCLIENT_LIBRARIES parameters.")
    
    MESSAGE(STATUS "")
    MESSAGE(STATUS "-------------------------------------------------------------")  
    MESSAGE(STATUS "Configuration FAILED for external module <email>")
    
  ENDIF(CCLIENT_FOUND)
ENDIF(ZORBA_SUPPRESS_CCLIENT)

DONE_DECLARING_ZORBA_URIS ()
MESSAGE(STATUS "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++")
