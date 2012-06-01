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

#include "imap_function.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>

#include <zorba/zorba.h>
#include <zorba/iterator.h>
#include <zorba/user_exception.h>
#include <zorba/diagnostic_list.h>

#include <unicode/ucnv.h>
#include <unicode/ustring.h>

#include "imap_module.h"
#include "imap_client.h"
#include "email_exception.h"

namespace zorba { namespace emailmodule {

const char* ImapFunction::SCHEMA_NAMESPACE = "http://www.zorba-xquery.com/modules/email";  

ImapFunction::ImapFunction(const ImapModule* aModule)
  : theModule(aModule)
{
}

ImapFunction::~ImapFunction()
{
}

void
ImapFunction::raiseImapError(
  EmailException& e) const
{
  std::string lCode;
  if (e.get_localname() == "CONNECTION_ERROR") {
    lCode = "IMAP0002";
  }
  else if (e.get_localname() == "WRONG_ID") {
    lCode = "IMAP0003";
  }
  else {
    lCode = "IMAP0001";
  }
  raiseImapError(lCode, e.get_message());
}

void
ImapFunction::raiseImapError(
      const std::string& qName,
      const std::string& message) const
{
  Item lQName = theModule->getItemFactory()->createQName(getURI(), "imap", qName);
  throw USER_EXCEPTION(lQName, message);
}

String
ImapFunction::getURI() const
{
  return theModule->getURI();
}

void 
ImapFunction::getHostUserPassword(
  const ExternalFunction::Arguments_t& aArgs,
  int aPos,
  std::string& aHost,
  std::string& aUserName,
  std::string& aPassword) const
{
  Item lNode;
  Iterator_t args_iter = aArgs[aPos]->getIterator();
  args_iter->open();
  args_iter->next(lNode);
  args_iter->close();
  Iterator_t lChildren = lNode.getChildren();
  lChildren->open();
  Item lChild;
  lChildren->next(lChild);
  aHost = lChild.getStringValue().c_str();
  lChildren->next(lChild);
  aUserName = lChild.getStringValue().c_str();
  lChildren->next(lChild);
  aPassword = lChild.getStringValue().c_str();
  lChildren->close();  
}

String
ImapFunction::getOneStringArg(
  const ExternalFunction::Arguments_t& aArgs,
  int aPos) const
{
  Item lItem;
  Iterator_t args_iter = aArgs[aPos]->getIterator();
  args_iter->open();
  args_iter->next(lItem);
  args_iter->close();
  return lItem.getStringValue();
}

std::string
ImapFunction::getMessageNumbers(
  const ExternalFunction::Arguments_t& aArgs,
  int aPos) const
{
  Item lItem;
  std::stringstream lMessageNumbers;
  Iterator_t args_iter = aArgs[aPos]->getIterator();
  args_iter->open();
  args_iter->next(lItem);
  lMessageNumbers << lItem.getStringValue().c_str();
  while(args_iter->next(lItem)) {
    lMessageNumbers << "," << lItem.getStringValue().c_str();
  }
  args_iter->close();
  return lMessageNumbers.str(); 
}

unsigned long
ImapFunction::getOneMessageNumber(
  const ExternalFunction::Arguments_t& aArgs,
  int aPos) const
{
  Item lItem;
  Iterator_t args_iter = aArgs[aPos]->getIterator();
  args_iter->open();
  args_iter->next(lItem);
  args_iter->close();
  return lItem.getLongValue();
}

bool
ImapFunction::getOneBoolArg(
  const ExternalFunction::Arguments_t& aArgs,
  int aPos) const
{
  Item lItem;
  Iterator_t args_iter = aArgs[aPos]->getIterator();
  args_iter->open();
  args_iter->next(lItem);
  args_iter->close();
  return lItem.getBooleanValue();
}

std::string
ImapFunction::getDateTime(const std::string& aCClientDateTime) const
{
  std::stringstream lResult;    
  std::stringstream lDateTimeStream(aCClientDateTime);
  std::string lBuffer;
  std::vector<std::string> lTokens;
  
  while (lDateTimeStream >> lBuffer) { 
    lTokens.push_back(lBuffer);
  }
  // YYYY-MM-DDThh:mm:ss, first push YYYY
  lResult << lTokens[3] << "-";
  // then push MM
  // build up map for Months
  
  std::string lMonths = "JanFebMarAprMayJunJulAugSepOctNovDec";
  size_t lMonthNumber = lMonths.find(lTokens[2]);
  size_t lTokensShift = 0;
  if (lMonthNumber == std::string::npos) {
    // if the month is not there, day of week is possibly missing, 
    // shift tokens by one and retry
    lTokensShift = -1;
    lMonthNumber = lMonths.find(lTokens[2 + lTokensShift]);
    if (lMonthNumber == std::string::npos) {
      // now, we are really in trouble, something is wrong
      Item lQName = theModule->getItemFactory()
                      ->createQName(SCHEMA_NAMESPACE, "XPTY0004");
      throw USER_EXCEPTION(
              lQName, 
              "Error while processing month in date of email message");
    }
  }

  lMonthNumber = lMonthNumber/3 + 1;
  // make sure its MM and not just <
  if (lMonthNumber < 10) {
    lResult << 0;
  }  
  lResult << lMonthNumber << "-";
  
  if (lTokens[1 + lTokensShift].size() == 1) {
    lResult << 0;
  }
  lResult << lTokens[1 + lTokensShift] << "T";
  // now hh:mm:ss
  lResult << lTokens[4 + lTokensShift].substr(0,2) << ":" 
    << lTokens[4 + lTokensShift].substr(3,2) << ":" 
    << lTokens[4 + lTokensShift].substr(6,2);


  return lResult.str();
  
}

std::string
ImapFunction::getContentType(
  const unsigned short aType,
  const char* aSubtype) const
{
  std::stringstream lType;
  switch (aType) {
    // Text
    case 0 : 
      lType << "text";
    break;
    case 1 : 
      lType << "multipart";
    break;
    case 2 : 
      lType << "message";
    break;
    case 3 : 
      lType << "application";
    break;
    case 4 :
      lType << "audio";
    break;
    case 5 :
      lType << "image";
    break;
    case 6 :
      lType << "video";
    break;
    case 7 : 
      lType << "model";
    break;
    case 8 :
      lType << "other";
    break;
  }  
  std::string lSubType(aSubtype); 
  std::transform(lSubType.begin(), lSubType.end(), lSubType.begin(), tolower);
  lType << "/" << lSubType; 
  return lType.str();

}

std::string
ImapFunction::getEncoding(const unsigned short aEncoding) const
{
  std::stringstream lEncoding;
  switch (aEncoding) {
    case 0 :
      lEncoding << "ENC7BIT";
    break;
    case 1 : 
      lEncoding << "ENC8BIT";
    break;
    case 2 :
      lEncoding << "ENCBINARY";
    break;
    case 3 : 
      lEncoding << "ENCBASE64";
    break;
    case 4 :
      lEncoding << "ENCQUOTEDPRINTABLE";
    break;
    case 5 :
      lEncoding << "ENCOTHER";
    break;
    default : lEncoding << "ENCOTHER"; 


  }
  return lEncoding.str();
}

void
ImapFunction::createFlagsNode(
  Item& aParent,
  Item& aFlags,
  std::vector<int>& aFlagsVector,
  const bool aQualified) const
{
  NsBindings ns_binding;
  ns_binding.push_back(std::pair<String, String>("email", SCHEMA_NAMESPACE));
  
  // if aParent is null, then we want to have the flags node qualified (so that it can be shema validated)
  Item lFlagsName;
  lFlagsName = theModule->getItemFactory()->createQName(SCHEMA_NAMESPACE, "email", "flags");
  Item lFlagsType = theModule->getItemFactory()->createQName(SCHEMA_NAMESPACE,  "flagsType");
  aFlags = theModule->getItemFactory()->createElementNode(aParent, lFlagsName, lFlagsType, false, false, ns_binding);

  std::string lFlagName;
  for (int i = 0; i < 5; ++i) {
    int lFlagNumber = aFlagsVector[i];
    if (lFlagNumber == 1) {
      switch (i) {
      case 0:
        lFlagName = "seen";
        break;
      case 1 :
        lFlagName = "deleted";
        break;
      case 2 :
        lFlagName = "flagged";
        break;
      case 3 :
        lFlagName = "answered";
        break;
      case 4 :
        lFlagName = "draft";
        break;
      }
      
      Item lOneFlagName = theModule->getItemFactory()->createQName(SCHEMA_NAMESPACE, "email", lFlagName);
      Item lOneFlagType = theModule->getItemFactory()->createQName(SCHEMA_NAMESPACE, "emptyType");
      Item lOneFlag = theModule->getItemFactory()->createElementNode(aFlags, lOneFlagName, lOneFlagType, false, true, ns_binding);
    }
  }
}

void
ImapFunction::createInnerNodeWithText(
  Item& aParent,
  const std::string& aNamespace,
  const std::string& aPrefix,
  const std::string& aName,
  const std::string& aTypeNamespace,
  const std::string& aType,
  const std::string& aContent) const
{
  NsBindings null_binding;
  Item lName = theModule->getItemFactory()->createQName(aNamespace, aPrefix, aName);
  Item lType = theModule->getItemFactory()->createQName(aTypeNamespace,  aType);
  Item lItem = theModule->getItemFactory()->createElementNode(aParent, lName, lType, false, false, null_binding);
  theModule->getItemFactory()->createTextNode(lItem, String(aContent));
}  

void
ImapFunction::createContentNode(
  Item& aParent,
  const std::string& aContent,
  const std::string& aContentType,
  const std::string& aCharset,
  const std::string& aContentTransferEncoding,
  const std::string& aContentDisposition,
  const std::string& aContentDispositionFilename,
  const std::string& aContentDispositionModificationDate,
  const std::string& aContentId) const
{  
  Item lNullItem;
   
  NsBindings null_binding;
  Item lName = theModule->getItemFactory()->createQName(SCHEMA_NAMESPACE, "email", "content");
  Item lType = theModule->getItemFactory()->createQName(SCHEMA_NAMESPACE, "contentType" );
  Item lItem = theModule->getItemFactory()->createElementNode(aParent, lName, lType, false, false, null_binding);
  
  createContentTypeAttributes(lItem, aContentType, aCharset, aContentTransferEncoding, aContentDisposition, aContentDispositionFilename, aContentDispositionModificationDate);

  if (aContentId.length() > 1) {
    Item lContentIdName = theModule->getItemFactory()->createQName("", "content-id");
    Item lContentIdType = theModule->getItemFactory()->createQName("http://www.w3.org/2001/XMLSchema", "string");
    Item lContentIdText = theModule->getItemFactory()->createTextNode(lNullItem, String(aContentId));
    theModule->getItemFactory()->createAttributeNode(lItem, lContentIdName, lContentIdType, lContentIdText);
  }  

  theModule->getItemFactory()->createTextNode(lItem, String(aContent));
}

void 
ImapFunction::createEmailAddressNode(
  Item& aParent,
  const std::string& aName,
  const char* aPersonal,
  const char* aMailbox,
  const char* aHost) const
{
  Item lType = theModule->getItemFactory()->createQName(SCHEMA_NAMESPACE, "emailAddress");
  Item lName = theModule->getItemFactory()->createQName(SCHEMA_NAMESPACE, "email",  aName);

  NsBindings ns_binding;
  ns_binding.push_back(std::pair<String, String>("email", SCHEMA_NAMESPACE));
 
  Item lItem = theModule->getItemFactory()->createElementNode(aParent, lName, lType, false, false, ns_binding);
  if (aPersonal) {
    createInnerNodeWithText(lItem, SCHEMA_NAMESPACE, "email", "name", "http://www.w3.org/2001/XMLSchema", "string", aPersonal);
  }
  if ((aMailbox) && (aHost)) {
    createInnerNodeWithText(lItem, SCHEMA_NAMESPACE, "email", "email", SCHEMA_NAMESPACE, "emailAddressType", std::string(aMailbox) + "@" + std::string(aHost));  
  }
}

void 
ImapFunction::createRecipentNode(
  Item& aParent,
  const std::string& aName,
  const char* aPersonal,
  const char* aMailbox,
  const char* aHost) const
{                    
  Item lType = theModule->getItemFactory()->createQName(SCHEMA_NAMESPACE, "recipientType");                                                                    
  Item lName = theModule->getItemFactory()->createQName(SCHEMA_NAMESPACE, "recipient");                           
  
  NsBindings ns_binding;
  ns_binding.push_back(std::pair<String, String>("email", SCHEMA_NAMESPACE));
 
  Item lItem = theModule->getItemFactory()->createElementNode(aParent, lName, lType, false, false, ns_binding); 
  createEmailAddressNode(lItem, aName, aPersonal, aMailbox, aHost);
}

void 
ImapFunction::createContentTypeAttributes(
  Item& aParent,
  const std::string& aContentType,
  const std::string& aCharset,
  const std::string& aContentTransferEncoding,
  const std::string& aContentDisposition,
  const std::string& aContentDispositionFilename,
  const std::string& aContentDispositionModificationDate) const
{
  Item lNullItem;
  /* build the value attribute */
  Item lContentTypeName = theModule->getItemFactory()->createQName("", "contentType");
  Item lContentTypeType = theModule->getItemFactory()->createQName(SCHEMA_NAMESPACE, "contentTypeValue");
  Item lContentTypeText = theModule->getItemFactory()->createTextNode(lNullItem, String(aContentType));
  theModule->getItemFactory()->createAttributeNode(aParent, lContentTypeName, lContentTypeType, lContentTypeText);

  /* build the charset attribute */
  Item lCharsetName = theModule->getItemFactory()->createQName("", "charset");
  Item lCharsetType = theModule->getItemFactory()->createQName(SCHEMA_NAMESPACE, "string");
  Item lCharsetText = theModule->getItemFactory()->createTextNode(lNullItem, String(aCharset));
  theModule->getItemFactory()->createAttributeNode(aParent, lCharsetName, lCharsetType, lCharsetText);

  /* build the contentTransferEncoding attribute */
  Item lContentTransferName = theModule->getItemFactory()->createQName("", "contentTransferEncoding");
  Item lContentTransferType = theModule->getItemFactory()->createQName(SCHEMA_NAMESPACE, "cteType");
  Item lContentTransferText = theModule->getItemFactory()->createTextNode(lNullItem, String(aContentTransferEncoding));
  theModule->getItemFactory()->createAttributeNode(aParent, lContentTransferName, lContentTransferType, lContentTransferText);
  /* build the contentDisposition attribute */
  
  if (aContentDisposition.length() > 2) {
  
    Item lContentDispositionName = theModule->getItemFactory()->createQName("", "contentDisposition");
    Item lContentDispositionType = theModule->getItemFactory()->createQName("http://www.w3.org/2001/XMLSchema", "string");
    Item lContentDispositionText = theModule->getItemFactory()->createTextNode(lNullItem, String(aContentDisposition));
    theModule->getItemFactory()->createAttributeNode(aParent, lContentDispositionName, lContentDispositionType, lContentDispositionText);
    if (aContentDispositionFilename.length() > 1) {
    /* build the contentDispositionFilename attribute */
      Item lContentDispositionFilenameName = theModule->getItemFactory()->createQName("", "contentDisposition-filename");
      Item lContentDispositionFilenameType = theModule->getItemFactory()->createQName(SCHEMA_NAMESPACE, "string");
      Item lContentDispositionFilenameText = theModule->getItemFactory()->createTextNode(lNullItem, String(aContentDispositionFilename));
      theModule->getItemFactory()->createAttributeNode(aParent, lContentDispositionFilenameName, lContentDispositionFilenameType, lContentDispositionFilenameText);
    }
    if (aContentDispositionModificationDate.length() > 2) { 
      /* build the contentDispositionModificationDate attribute */ 
      Item lContentDispositionModificationDateName = theModule->getItemFactory()->createQName("", "contentDisposition-modification-date");
      Item lContentDispositionModificationDateType = theModule->getItemFactory()->createQName("http://www.w3.org/2001/XMLSchema", "string");
      Item lContentDispositionModificationDateText = theModule->getItemFactory()->createTextNode(lNullItem, String(getDateTime(aContentDispositionModificationDate)));
      theModule->getItemFactory()->createAttributeNode(aParent, lContentDispositionModificationDateName, lContentDispositionModificationDateType, lContentDispositionModificationDateText);
    }
  }
}

void
ImapFunction::getMessage(
  Item& aParent,
  const std::string& aHostName, 
  const std::string& aUserName, 
  const std::string& aPassword, 
  const std::string& aMailbox, 
  const unsigned long aMessageNumber, 
  const bool aUid, 
  const bool aOnlyEnvelope) const
{
  std::vector<int> lFlags(6,0);
  ENVELOPE* lEnvelope;
  BODY* lBody;
  if (aOnlyEnvelope) {
    // only fetch envelope
    lEnvelope =  ImapClient::Instance().fetchEnvelope(aHostName, aUserName, aPassword, aMailbox, aMessageNumber, lFlags, aUid);
  } else {
    // the flags vector in the imap client will be filled by this call, so clear it
    lEnvelope = ImapClient::Instance().fetchStructure(aHostName, aUserName, aPassword, aMailbox, &lBody, aMessageNumber, aUid, lFlags);
  }
    
  NsBindings ns_binding;
  ns_binding.push_back(std::pair<String, String>("email", SCHEMA_NAMESPACE));
    
  Item lEnvelopeItem;
  std::string lErrorMessage = ImapClient::Instance().getError();
  if (lErrorMessage.size() != 0) {
    Item lQName = ImapModule::getItemFactory()->createQName(ImapModule::getURIString(), "imap", "IMAP0001");
    throw USER_EXCEPTION(lQName, lErrorMessage);
  }

  // First construct the envelope (depending on aOnlyEnvelope we may be finished then)
  // Important: if we only want the envelope, then the envelope MUST be qualified (being the root of the DOM)
  Item lEnvelopeName;
  if (aOnlyEnvelope) {
    lEnvelopeName = theModule->getItemFactory()->createQName(SCHEMA_NAMESPACE, "email", "envelope");
  } else {
    lEnvelopeName = theModule->getItemFactory()->createQName(SCHEMA_NAMESPACE, "email", "envelope");
  }  
  Item lEnvelopeType = theModule->getItemFactory()->createQName(SCHEMA_NAMESPACE, "email", "envelopeType");
    
  Item lNullItem;
  // if we only want the envelope, then create it with a null parent, else create the message and use it as parent
  if (aOnlyEnvelope) {
    lEnvelopeItem =  theModule->getItemFactory()->createElementNode(lNullItem, lEnvelopeName, lEnvelopeType, false, false, ns_binding);
  } else {
    Item lMessageName = theModule->getItemFactory()->createQName(SCHEMA_NAMESPACE, "email", "message");
    Item lMessageType = theModule->getItemFactory()->createQName(SCHEMA_NAMESPACE, "email", "messageType");
    aParent =  theModule->getItemFactory()->createElementNode(lNullItem, lMessageName, lMessageType, false, false, ns_binding);
    lEnvelopeItem =  theModule->getItemFactory()->createElementNode(aParent, lEnvelopeName, lEnvelopeType, false, false, ns_binding);
  }
    
  // create the remail node if needed
  if (lEnvelope->remail) {
    createInnerNodeWithText(lEnvelopeItem, SCHEMA_NAMESPACE, "email",  "remail", "http://www.w3.org/2001/XMLSchema", "string", lEnvelope->remail);
  }
  // create the date node if needed
  if (lEnvelope->date) {
    createInnerNodeWithText(
      lEnvelopeItem,
      SCHEMA_NAMESPACE,
      "email",
      "date",
      "http://www.w3.org/2001/XMLSchema",
      "string",
      getDateTime(reinterpret_cast<const char*>(lEnvelope->date)));
  }
  // create from node if needed
  if (lEnvelope->from) {
    createEmailAddressNode(lEnvelopeItem, "from", lEnvelope->from->personal, lEnvelope->from->mailbox, lEnvelope->from->host);
  }
  // create sender node if needed  
  if (lEnvelope->sender) {
    createEmailAddressNode(lEnvelopeItem, "sender", lEnvelope->sender->personal, lEnvelope->sender->mailbox, lEnvelope->sender->host);
  }
  // create replyTo node if needed
  if (lEnvelope->reply_to) {
    createEmailAddressNode(lEnvelopeItem, "replyTo", lEnvelope->reply_to->personal, lEnvelope->reply_to->mailbox, lEnvelope->reply_to->host);
  }
  // create subject node
  if (lEnvelope->subject) {
    std::string lSubject = lEnvelope->subject;
    std::string lDecodedSubject;
    decodeHeader(lSubject, lDecodedSubject);
    createInnerNodeWithText(lEnvelopeItem, "", "",  "subject", "http://www.w3.org/2001/XMLSchema", "string", lDecodedSubject);
  }
    
  ADDRESS* lRecipents;
  if (lEnvelope->to) {
    createRecipentNode(lEnvelopeItem, "to", lEnvelope->to->personal, lEnvelope->to->mailbox, lEnvelope->to->host);
    lRecipents = lEnvelope->to;
    while ((lRecipents = lRecipents->next)) {
      createRecipentNode(lEnvelopeItem, "to", lEnvelope->to->personal, lEnvelope->to->mailbox, lEnvelope->to->host);
    }
  }
    
  if (lEnvelope->cc) {
    createRecipentNode(lEnvelopeItem, "cc", lEnvelope->cc->personal, lEnvelope->cc->mailbox, lEnvelope->cc->host);
    lRecipents = lEnvelope->cc;
    while ((lRecipents = lRecipents->next)) {
      createRecipentNode(lEnvelopeItem, "cc", lEnvelope->cc->personal, lEnvelope->cc->mailbox, lEnvelope->cc->host);
    }
  }
    
  if ((lRecipents = lEnvelope->bcc)) {
    createRecipentNode(lEnvelopeItem, "bcc", lEnvelope->bcc->personal, lEnvelope->bcc->mailbox, lEnvelope->bcc->host);
    while ((lRecipents = lRecipents->next)) {
      createRecipentNode(lEnvelopeItem, "bcc", lEnvelope->bcc->personal, lEnvelope->bcc->mailbox, lEnvelope->bcc->host);
    }
  }
    
  // create messageId node
  if (lEnvelope->message_id) {
    createInnerNodeWithText(lEnvelopeItem,  SCHEMA_NAMESPACE, "email",  "messageId", "http://www.w3.org/2001/XMLSchema", "string", lEnvelope->message_id);
  }
  Item lFlagsItem;
  // create flags node
  createFlagsNode(lEnvelopeItem, lFlagsItem, lFlags, false);
    
  // if we only want the envelope, then here is a good place to stop
  if (aOnlyEnvelope) {
    aParent = lEnvelopeItem;
    return;
  } 
    
  // if we want the whole message, then build it together
    
  // <email:mimeVersion>1.0</email:mimeVersion>
  createInnerNodeWithText(aParent,  SCHEMA_NAMESPACE, "email", "mimeVersion", "http://www.w3.org/2001/XMLSchema", "string", "1.0");
        
  // make a tolower version of the subtype
  std::string lSubType(lBody->subtype);
  std::transform(lSubType.begin(), lSubType.end(), lSubType.begin(), tolower);
    
  // creating the <body> node
  Item lBodyName = theModule->getItemFactory()->createQName(SCHEMA_NAMESPACE, "email", "body");
  Item lBodyType = theModule->getItemFactory()->createQName(SCHEMA_NAMESPACE, "email", "bodyTypeChoice");
  Item lBodyItem = theModule->getItemFactory()->createElementNode(aParent, lBodyName, lBodyType, false, false, ns_binding); 
  // in case of non-multipart, just add the body to the message
    
  Item lMultipartParentName = theModule->getItemFactory()->createQName(SCHEMA_NAMESPACE, "email", "multipart");
  Item lMultipartParentType = theModule->getItemFactory()->createQName(SCHEMA_NAMESPACE, "email", "multipartType");
  Item lMultipartParent; 
  // using a vector instead of a stack, because including stack will clash with the c-client include ... 
  std::vector<BODY*> lBodies;
  lBodies.push_back(lBody);
  std::vector<Item> lParents;
  lParents.push_back(lBodyItem);
  std::vector<std::string> lSections;
  lSections.push_back("");  
    
  // make sure that the special case of a completely non-multipart message is handled correctly
  bool lNoMultipart = false;
  if (lBody->type != TYPEMULTIPART) {
    lNoMultipart = true;
  }
    
  BODY* lCurrentBody;
  Item lCurrentParent;
  std::string lCurrentSection; 
  // iterate and create all nodes (doing this recursive would have been nicer, but seems impossible without making a function containing a c-client structure in its signature, which seems impossible)
  while (lBodies.size() > 0) {
    lCurrentBody = lBodies.front();
    lCurrentParent = lParents.front();
    lCurrentSection = lSections.front(); 
    lSections.erase(lSections.begin());
    lParents.erase(lParents.begin()); 
    lBodies.erase(lBodies.begin());
    // get different attributes that we will need in any case, regardless if this is a content or multipart item
    std::string lContentType = getContentType(lCurrentBody->type, lCurrentBody->subtype);
    std::string lContentDisposition = "";
    if (lCurrentBody->disposition.type != NIL) {
      lContentDisposition = cpystr(lCurrentBody->disposition.type);
    }  
    std::string lContentDispositionFilename = "";
    std::string lContentDispositionModificationDate = "";
      
    PARAMETER* lCurrentParameter = lCurrentBody->disposition.parameter;
    while (lCurrentParameter != NIL) {
        
      if (!std::string("filename").compare(lCurrentParameter->attribute)) {
        lContentDispositionFilename = cpystr(lCurrentParameter->value);
      }  else if (!std::string("modification-date").compare(lCurrentParameter->attribute)) {
        lContentDispositionModificationDate = cpystr(lCurrentParameter->value);
      }  
        
      lCurrentParameter = lCurrentParameter->next;
    }  
      
    if (lCurrentBody->type != TYPEMULTIPART) {
      std::string lContentId = "";
      char * lId = lCurrentBody->id;
        
      if (lId != NIL) {
        lContentId = cpystr(lId);
      }
        
      std::string lSubType(lCurrentBody->subtype);                                                                  
      std::transform(lSubType.begin(), lSubType.end(), lSubType.begin(), tolower);
      // make sure that we haven't got a empty string (happens if there is no multipart in this message)
      if (lCurrentSection.size() != 0) {
        lCurrentSection.erase(lCurrentSection.end() - 1);
      }
        
      std::string lBodyContent = ImapClient::Instance().fetchBodyFull(aHostName, aUserName, aPassword, aMailbox, aMessageNumber, lNoMultipart ? "1" : lCurrentSection, aUid);

        // reading charset from email
        const char* lCharset = 0;
        PARAMETER* lParam = lCurrentBody->parameter;
        while (lParam) {
          if (strcmp(lParam->attribute, "charset") == 0) {
            lCharset = lParam->value;
          }
          lParam = lParam->next;
        }

        std::string lTransferEncodingDecoded;
        unsigned short lEncoding = lCurrentBody->encoding;
        // decode the body according the transfer encoding if it is quoted-printable
        decodeTextualTransferEncoding(lBodyContent, lContentType, lEncoding, lTransferEncodingDecoded);
        
        // decode the body according to the charset
        std::string lCharsetDecoded;
        toUtf8(lTransferEncodingDecoded, lCharset, lCharsetDecoded);

        createContentNode(lCurrentParent, lCharsetDecoded, lContentType,
          "utf-8", getEncoding(lEncoding), lContentDisposition,
          lContentDispositionFilename, lContentDispositionModificationDate,
          lContentId);

    } else {
      lMultipartParent = theModule->getItemFactory()->createElementNode(
        lCurrentParent, lMultipartParentName, lMultipartParentType, false,
        false, ns_binding);

      createContentTypeAttributes(lMultipartParent, lContentType, "utf-8",
        getEncoding(lCurrentBody->encoding), lContentDisposition,
        lContentDispositionFilename, lContentDispositionModificationDate);

      PART* lPart = lCurrentBody->nested.part;
      lBodies.insert(lBodies.begin(), &lPart->body);
      lParents.insert(lParents.begin(), lMultipartParent);      
      lSections.insert(lSections.begin(), lCurrentSection + "1.");
      for (int j = 1; lPart->next; ++j) {
        lPart = lPart->next; 
        std::stringstream lConverter;
        lConverter << j+1 << "."; 
        lSections.insert(lSections.begin() + j, lCurrentSection + lConverter.str());
        lConverter.clear(); 
        lBodies.insert(lBodies.begin() +j, &lPart->body);
        lParents.insert(lParents.begin(), lMultipartParent);
      }
    } 
  }
}

void
ImapFunction::toUtf8(
  const std::string& aValue,
  const char* aFromCharset,
  std::string& aResult) const
{
  if (!aFromCharset || std::string(aFromCharset) == "") {
    aResult = aValue;
    return;
  }

  // transforming the body from the declared charset to utf-8
  const char* lValueChar = aValue.c_str();
  // TODO array auto ptr
  UChar* lUnicodeBody = new UChar[aValue.length()+1];
  UErrorCode lStatus = U_ZERO_ERROR;
  UConverter *lConverter;

  // set up the converter
  lConverter = ucnv_open(aFromCharset, &lStatus);
  checkStatus(lStatus);

  // convert to Unicode
  int32_t lUnicodeLen = ucnv_toUChars(
      lConverter, 
      lUnicodeBody, 
      aValue.length()+1, 
      lValueChar, 
      -1, 
      &lStatus);
  checkStatus(lStatus);

  int32_t lUTF8Memory = lUnicodeLen*4 + 1;
  // TODO autoptr
  char* lUTF8Body = new char[lUTF8Memory];
  int32_t lUTF8Len;

  //convert from unicode to UTF8
  u_strToUTF8(lUTF8Body, 
              lUTF8Memory, 
              &lUTF8Len, 
              lUnicodeBody, 
              -1, 
              &lStatus);
  checkStatus(lStatus);

  aResult = lUTF8Body;
}


void
ImapFunction::checkStatus(UErrorCode aStatus) const
{
  if (!U_SUCCESS(aStatus)) {
    std::stringstream lStream;
    lStream << "Failed to convert email fragment (subject or body) to UTF-8. ICU error code: " << u_errorName(aStatus) << ".";
    raiseImapError("IMAP0001", lStream.str());
  }
}


void
ImapFunction::decodeHeader(
  const std::string& aValue,
  std::string& aResult) const
{
  std::stringstream lDecoded;
  std::size_t lMarker = 0;

  // size used many times
  std::size_t lLength = aValue.length();
  // to collect question mark positions
  std::vector<std::size_t> lQMs;

  // populate the above vectors
  for (std::size_t i = 0; i < lLength; i++) {
    if (aValue.at(i) == '?') {
      lQMs.push_back(i);
    }
  }

  // sizes we need below
  std::size_t lQLength = lQMs.size();

  // not enough questions marks to make up an encoding, give up
  if (lQLength < 4) {
    aResult = aValue;
    return;
  }

  // now we take groups of 4 question marks
  for (std::size_t j = 0; j <= lQLength - 4; ) {
    // a good encoding meets these conditions:
    if (
      // 2nd and 3rd question marks have only one character in between
      lQMs[j + 1] + 2 == lQMs[j + 2] &&
      // 1st question mark is prefixed by an equal sign
      (lQMs[j]  > 0 && aValue.at(lQMs[j] - 1) == '=') &&
      // 4th question mark is suffixes by an equal sign
      (lQMs[j + 3] < (lLength-1) && aValue.at(lQMs[j + 3] + 1) == '='))
    {
      // ok were good, so first save the text from the last marker
      // upto the starting equal sign
      lDecoded << aValue.substr(lMarker, lQMs[j] - lMarker - 1);

      // then take the entire region, including =? and ?= and try to decode it
      std::string lWords;
      decodeEncodedWords(aValue.substr(lQMs[j] - 1, lQMs[j + 3] - lQMs[j] + 3), lWords);
      lDecoded << lWords;

      // save the new marker at the end of this encoded word group
      lMarker = lQMs[j + 3] + 2;

      // decoding with the current 4 question marks worked well so we skip them and try the next 4
      j += 4;
    }
    else {
      // not a valid encoding, so iterate to the next question mark
      ++j;
    }
  }
  lDecoded << aValue.substr(lMarker);

  aResult = lDecoded.str();
}


void
ImapFunction::decodeTextualTransferEncoding(
  const std::string& aValue, 
  const std::string& aContentType, 
  unsigned short& aEncoding, 
  std::string& aResult) const
{
  if (aEncoding == ENCQUOTEDPRINTABLE) {
    unsigned long lNewLength;
    void* lNewData = rfc822_qprint((unsigned char*)aValue.c_str(), aValue.length(), &lNewLength);
    aResult = std::string((char *)lNewData, lNewLength);
    fs_give(&lNewData);
    aEncoding = ENC8BIT;
  }
  else if (aEncoding == ENCBASE64 &&
        (
          aContentType.find("text") != std::string::npos
       || aContentType.find("xml") != std::string::npos
        )) {
    unsigned long lNewLength;
    void* lNewData = rfc822_base64((unsigned char*)aValue.c_str(), aValue.length(), &lNewLength);
    aResult = std::string((char *)lNewData, lNewLength);
    fs_give(&lNewData);
    aEncoding = ENC8BIT;
  }
  else {
    aResult = aValue;
  }
}

} /* namespace emailmodule */
} /* namespace zorba */
