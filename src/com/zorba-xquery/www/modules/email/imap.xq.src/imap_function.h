/*
 * Copyright 2006-2008 The FLWOR Foundation.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ZORBA_EMAILMODULE_IMAPFUNCTION_H
#define ZORBA_EMAILMODULE_IMAPFUNCTION_H

#include <string>

#include <zorba/function.h>
#include <zorba/error.h>

#include <unicode/utypes.h>


namespace zorba { namespace emailmodule {

class ImapModule;
class EmailException;

class ImapFunction : public ContextualExternalFunction
{
  protected:
    const ImapModule* theModule;
    static const char* SCHEMA_NAMESPACE;  

    void
    raiseImapError(
      EmailException& e) const;

    void
    raiseImapError(
      const std::string& qName,
      const std::string& message) const;

    void
    getHostUserPassword(
      const ExternalFunction::Arguments_t& aArgs,
      int aPos,
      std::string& aHost,
      std::string& aUserName,
      std::string& aPassword) const;

    String
    getOneStringArg(
      const ExternalFunction::Arguments_t& args,
      int pos) const;

    std::string
    getMessageNumbers(
      const ExternalFunction::Arguments_t& args,
      int pos) const;

    unsigned long
    getOneMessageNumber(
      const ExternalFunction::Arguments_t& args,
      int pos) const;

    bool
    getOneBoolArg(
      const ExternalFunction::Arguments_t& args,
      int pos) const;

    /*
      * Converts a dateTime string as returned by the c-client (e.g. Tue, 24
      * Aug 2010 16:26:10 +0200'DD) into a xs:dateTime format.
      */
    std::string
    getDateTime(const std::string& aCClientDateTime) const;

    std::string
    getContentType(const unsigned short aType, const char* aSubtype) const;

    std::string
    getEncoding(const unsigned short aEncoding) const;

    void
    createFlagsNode(
      Item& aParent,
      Item& aFlags,
      std::vector<int>& aFlagsVector,
      const bool aQualified) const;

    /*
      * Creates a simple named  node containing a text node.
      */
    void
    createInnerNodeWithText(
      Item& aParent,
      const std::string& aNamespace,
      const std::string& aPrefix,
      const std::string& aName,
      const std::string& aTypeNamespace,
      const std::string& aType,
      const std::string& aContent) const;

    void
    createContentNode(
      Item& aParent,
      const std::string& aContent,
      const std::string& contentType,
      const std::string& charset,
      const std::string& contentTransferEncoding,
      const std::string& aContentDisposition,
      const std::string& aContentDispositionFilename,
      const std::string& aContentDispositionModificationDate,
      const std::string& aContentId) const;

    /*
      * Creates a simple email address node as defined in email.xsd
      */
    void
    createEmailAddressNode(
      Item& aParent,
      const std::string& aName,
      const char * aPersonal,
      const char* aMailbox,
      const char* aHost) const;

    void
    createRecipentNode(
      Item& aParent,
      const std::string& aName,
      const char* aPersonal,
      const char* aMailbox,
      const char* aHost) const;

    void
    createContentTypeAttributes(
      Item& aParent,
      const std::string& aValue,
      const std::string& aCharset,
      const std::string& aTransferEncoding,
      const std::string& aContentDisposition,
      const std::string& aContentDispositionFilename,
      const std::string& aContentDispositionModificationDate) const;

    void
    getMessage(
      Item& aParent, 
      const std::string& aHostName, 
      const std::string& aUserName, 
      const std::string& aPassword, 
      const std::string& aMailbox, 
      const unsigned long aMessageNumber, 
      const bool aUid, 
      const  bool aOnlyEnvelope) const;

    void
    toUtf8(
      const std::string& value,
      const char* fromCharset,
      std::string& result) const;

    void
    checkStatus(UErrorCode aStatus) const;

    void
    decodeHeader(
      const std::string& value,
      std::string& result) const;

    // This function is defined in imap.cpp because if defined in imap_function.cpp
    // some header file conflict are raised and it does not compile
    void
    decodeEncodedWords(
      const std::string& value,
      std::string& result) const;

  public:
    ImapFunction(const ImapModule* module);
    ~ImapFunction();

    virtual String
    getURI() const;

};

} /* namespace emailmodule */
} /* namespace zorba */

#endif /* ZORBA_EMAILMODULE_IMAPFUNCTION_H */
