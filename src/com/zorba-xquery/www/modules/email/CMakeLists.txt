# Copyright 2006-2008 The FLWOR Foundation.
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

##############################
## only for smtp library set -Wno-write-strings in order to remove
## the warning "deprecated conversion from string constant to char*".
## we need to set it to both GNUCC and GNUCXX because of linkage.c
## and c-client.h. QQQ this is a crappy solution - move it somehow to
## the smtp module itself
#IF(${MODULE_NAME} STREQUAL "smtp")
#  MESSAGE (FATAL_ERROR "This happened----------------------------")
#  IF (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
#    FOREACH(SMTP_SRC ${SRC_FILES})
#      SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/smtp.xq.src/${SMTP_SRC} PROPERTIES COMPILE_FLAGS "-Wno-write-strings")
#    ENDFOREACH(SMTP_SRC)
#  ENDIF (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
#ENDIF(${MODULE_NAME} STREQUAL "smtp")
##############################

# CClient library part of the UW IMAP toolkit required by the email functionality
IF (ZORBA_SUPPRESS_CCLIENT)
  MESSAGE(STATUS "ZORBA_SUPPRESS_CCLIENT is true - not searching for C-Client")
ELSE(ZORBA_SUPPRESS_CCLIENT)
  
  IF(WIN32)
    INCLUDE (${PROJECT_SOURCE_DIR}/cmake_modules/Windows/FindCClient.cmake)
  ELSE(WIN32)
    FIND_PACKAGE (CClient)
  ENDIF(WIN32)

  IF (NOT WIN32 AND NOT CYGWIN)
    FIND_PACKAGE(OpenSSL)
    FIND_PACKAGE(PAM)
  ENDIF (NOT WIN32 AND NOT CYGWIN)

  IF (APPLE)
    FIND_PACKAGE(Kerberos)
  ENDIF (APPLE)
ENDIF(ZORBA_SUPPRESS_CCLIENT)

IF(CCLIENT_FOUND)
  SET(SMTP_LINK_LIBRARIES ${CCLIENT_LIBRARIES})
  SET(BUILD_MODULE 1)

  IF (OPENSSL_FOUND)
    LIST(APPEND SMTP_LINK_LIBRARIES ${OPENSSL_LIBRARIES})
    LIST(APPEND SMTP_LINK_LIBRARIES ${OPENSSL_CRYPTO_LIBRARIES})
  ELSE (OPENSSL_FOUND)
    MESSAGE(STATUS "CClient library found but OpenSSL not found. This may cause runtime problems on certain platforms where CClient is linked against OpenSSL by default, including MacOS X and Ubuntu Linux (and probably others). See this module's README.txt for more information.")
  ENDIF (OPENSSL_FOUND)

  # Mac Ports compiles CCLient with kerberos support by default.
  IF (APPLE)
    IF(KERBEROS_FOUND)
      LIST(APPEND SMTP_LINK_LIBRARIES ${KERBEROS_LIBRARY})
    ELSE (KERBEROS_FOUND)
      MESSAGE(STATUS "CClient library found but Kerberos not found -- if you want to use Email functionality please set KERBEROS_LIBRARIES parameter.")
      SET (BUILD_MODULE 0)
    ENDIF (KERBEROS_FOUND)
  ENDIF (APPLE)
  
  INCLUDE_DIRECTORIES(${CCLIENT_INCLUDE_DIRS})
  IF (NOT WIN32 AND NOT CYGWIN)
    LIST(APPEND SMTP_LINK_LIBRARIES ${PAM_LIBRARIES})
  ELSE (NOT WIN32 AND NOT CYGWIN)
    LIST(APPEND SMTP_LINK_LIBRARIES secur32.lib crypt32.lib winmm.lib ws2_32.lib)
  ENDIF (NOT WIN32 AND NOT CYGWIN)
 
  # including shared c-client library
  ADD_LIBRARY(imap_commons STATIC cclient.xq.src/imap_client.cpp)
  TARGET_LINK_LIBRARIES(imap_commons ${SMTP_LINK_LIBRARIES})
  INCLUDE_DIRECTORIES("cclient.xq.src")


  IF (BUILD_MODULE)
    DECLARE_ZORBA_SCHEMA (FILE email.xsd
      URI "http://www.zorba-xquery.com/modules/email/email")
    DECLARE_ZORBA_SCHEMA (FILE imap.xsd
      URI "http://www.zorba-xquery.com/modules/email/imap")
    DECLARE_ZORBA_MODULE (URI "http://www.zorba-xquery.com/modules/email/smtp" VERSION 1.0 FILE "smtp.xq" LINK_LIBRARIES "imap_commons;${SMTP_LINK_LIBRARIES}")
    DECLARE_ZORBA_MODULE (URI "http://www.zorba-xquery.com/modules/email/imap" VERSION 1.0 FILE "imap.xq" LINK_LIBRARIES "imap_commons;${SMTP_LINK_LIBRARIES}")
  ENDIF (BUILD_MODULE)

ELSE(CCLIENT_FOUND)

  MESSAGE(STATUS "CClient library not found -- if you want to use Email functionality please set CCLIENT_INCLUDE_DIRS and CCLIENT_LIBRARIES parameters.")

ENDIF(CCLIENT_FOUND)
