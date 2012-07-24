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

# CClient library part of the UW IMAP toolkit required by the email
# functionality
IF (ZORBA_SUPPRESS_CCLIENT)
  MESSAGE(STATUS "ZORBA_SUPPRESS_CCLIENT is true - not searching for C-Client")
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
      cclient/imap_client.cpp
      cclient/email_exception.cpp
    )
    SET_TARGET_PROPERTIES (imap_commons PROPERTIES
      FOLDER "Modules"
    )

    IF(UNIX AND NOT WIN32)
      ADD_DEFINITIONS(-fPIC)
    ENDIF(UNIX AND NOT WIN32)

    TARGET_LINK_LIBRARIES(imap_commons ${SMTP_LINK_LIBRARIES})
    INCLUDE_DIRECTORIES("cclient")
  
  
    DECLARE_ZORBA_SCHEMA (FILE email.xsd
      URI "http://www.zorba-xquery.com/modules/email")
    DECLARE_ZORBA_MODULE (URI "http://www.zorba-xquery.com/modules/email/smtp" VERSION 1.0 FILE "smtp.xq" LINK_LIBRARIES "imap_commons;${SMTP_LINK_LIBRARIES}")
    DECLARE_ZORBA_MODULE (URI "http://www.zorba-xquery.com/modules/email/imap" VERSION 1.0 FILE "imap.xq" LINK_LIBRARIES "imap_commons;${SMTP_LINK_LIBRARIES}")
  
  ELSE(CCLIENT_FOUND)
  
    MESSAGE(STATUS "CClient library not found -- if you want to use Email functionality please set CCLIENT_INCLUDE_DIRS and CCLIENT_LIBRARIES parameters.")
  
  ENDIF(CCLIENT_FOUND)
ENDIF(ZORBA_SUPPRESS_CCLIENT)
MESSAGE(STATUS "")
