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

#include "imap.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>

#include <zorba/singleton_item_sequence.h>
#include <zorba/vector_item_sequence.h>
#include <zorba/empty_sequence.h>
#include <zorba/user_exception.h>
#include <zorba/xquery_functions.h>

#include "imap_module.h"
#include "imap_client.h"
#include "email_exception.h"


namespace zorba { namespace emailmodule {

  
//*****************************************************************************

StatusFunction::StatusFunction(const ImapModule* aModule)
  : ImapFunction(aModule)
{
}
  
ItemSequence_t
StatusFunction::evaluate(
  const ExternalFunction::Arguments_t& aArgs,
  const StaticContext*                          aSctxCtx,
  const DynamicContext*                         aDynCtx) const
{
  try {
    std::string lHostName;
    std::string lUserName;
    std::string lPassword;
    getHostUserPassword(aArgs, 0, lHostName, lUserName, lPassword);
    String lMailbox = getOneStringArg(aArgs, 1);
    ImapClient::Instance().status(lHostName, lUserName, lPassword, lMailbox.c_str());
    std::string lErrorMessage = ImapClient::Instance().getError();
    if (lErrorMessage.size() != 0) {
      raiseImapError("IMAP0001", lErrorMessage);
    }  
    
    unsigned long lMessages;
    unsigned long lRecent;
    unsigned long lUnseen;
    unsigned long lUIDNext;
    unsigned long lUIDValidity;
    
    ImapClient::Instance().getStatus(&lMessages, &lRecent, &lUnseen, &lUIDNext, &lUIDValidity);
    
    std::stringstream lResult;
    lResult << lMessages << "," << lRecent << "," << lUnseen << "," << lUIDNext << "," << lUIDValidity;
    
    std::string lResultString = lResult.str(); 
    return ItemSequence_t(new SingletonItemSequence(
      theModule->getItemFactory()->createString(lResultString.c_str())));
  } catch (EmailException& e) {
    raiseImapError(e);
  }
  return ItemSequence_t(NULL);
}

//*****************************************************************************

CreateFunction::CreateFunction(const ImapModule* aModule)
  : ImapFunction(aModule)
{
}

ItemSequence_t
CreateFunction::evaluate(
  const ExternalFunction::Arguments_t& aArgs,
  const StaticContext*                          aSctxCtx,
  const DynamicContext*                         aDynCtx) const
{
  try {
    std::string lHostName;
    std::string lUserName;
    std::string lPassword;
    getHostUserPassword(aArgs, 0, lHostName, lUserName, lPassword);
    String lMailboxName = getOneStringArg(aArgs, 1);
    bool lSuccess = ImapClient::Instance().create(lHostName, lUserName, lPassword, lMailboxName.c_str());
    if (!lSuccess) {
      std::string lErrorMessage = ImapClient::Instance().getError();
      raiseImapError("IMAP0001", lErrorMessage);
    }
    
  } catch (EmailException& e) {
    raiseImapError(e);
  }
  return ItemSequence_t(NULL);
}

//*****************************************************************************

DeleteFunction::DeleteFunction(const ImapModule* aModule)
  : ImapFunction(aModule)
{
}

ItemSequence_t
DeleteFunction::evaluate(
  const ExternalFunction::Arguments_t& aArgs,
  const StaticContext*                          aSctxCtx,
  const DynamicContext*                         aDynCtx) const
{
  try {
    std::string lHostName;
    std::string lUserName;
    std::string lPassword;
    getHostUserPassword(aArgs, 0, lHostName, lUserName, lPassword);
    String lMailboxName = getOneStringArg(aArgs, 1);
    bool lSuccess = ImapClient::Instance().delete_mailbox(lHostName, lUserName, lPassword, lMailboxName.c_str());
    
    if (!lSuccess) {
      std::string lErrorMessage = ImapClient::Instance().getError();
      raiseImapError("IMAP0001", lErrorMessage);
    }
    
  } catch (EmailException& e) {
    raiseImapError(e);
  }
  return ItemSequence_t(NULL);
}

//*****************************************************************************

RenameFunction::RenameFunction(const ImapModule* aModule)
  : ImapFunction(aModule)
{
}

ItemSequence_t
RenameFunction::evaluate(
  const ExternalFunction::Arguments_t& aArgs,
  const StaticContext*                          aSctxCtx,
  const DynamicContext*                         aDynCtx) const
{
  try {
    std::string lHostName;
    std::string lUserName;
    std::string lPassword;
    getHostUserPassword(aArgs, 0, lHostName, lUserName, lPassword);
    String lMailboxFromName = getOneStringArg(aArgs, 1);
    String lMailboxToName = getOneStringArg(aArgs, 2);
    bool lSuccess = ImapClient::Instance().rename(lHostName, lUserName, lPassword, lMailboxFromName.c_str(), lMailboxToName.c_str());
    if (!lSuccess) {
      std::string lErrorMessage = ImapClient::Instance().getError();
      raiseImapError("IMAP0001", lErrorMessage);
    }
    
  } catch (EmailException& e) {
    raiseImapError(e);
  }
  return ItemSequence_t(NULL);
}

//*****************************************************************************

ListFunction::ListFunction(const ImapModule* aModule)
  : ImapFunction(aModule)
{
}

ItemSequence_t
ListFunction::evaluate(
  const ExternalFunction::Arguments_t& aArgs,
  const StaticContext*                          aSctxCtx,
  const DynamicContext*                         aDynCtx) const
{
  try {
    std::string lHostName;
    std::string lUserName;
    std::string lPassword;
    getHostUserPassword(aArgs, 0, lHostName, lUserName, lPassword);
    String lReferencePath = getOneStringArg(aArgs, 1);
    String lPattern = getOneStringArg(aArgs, 2);
    bool lOnlySuscribed = getOneBoolArg(aArgs, 3);
    
    std::vector<std::string> lListedMailboxes = ImapClient::Instance().list(lHostName, lUserName, lPassword, lReferencePath.c_str(), lPattern.c_str(), lOnlySuscribed);
    std::string lErrorMessage = ImapClient::Instance().getError();
    if (lErrorMessage.size() != 0) {
      raiseImapError("IMAP0001", lErrorMessage);
    }
    // explicitly return an empty sequence if there are no elements to return
    if (lListedMailboxes.size() == 0) {
      return ItemSequence_t(new EmptySequence());
    }   
    
    int lVectorSize = (int)lListedMailboxes.size();
    std::vector<Item> lItemVector;
    for (int i = 0; i < lVectorSize; i++) {
      lItemVector.push_back(theModule->getItemFactory()->createString(lListedMailboxes[i]));
    }  
    
    return ItemSequence_t(new VectorItemSequence(lItemVector));
  } catch (EmailException& e) {
    raiseImapError(e);
  }
  return ItemSequence_t(NULL);
}

//*****************************************************************************

SubscribeFunction::SubscribeFunction(const ImapModule* aModule)
  : ImapFunction(aModule)
{
}

ItemSequence_t
SubscribeFunction::evaluate(
  const ExternalFunction::Arguments_t& aArgs,
  const StaticContext*                          aSctxCtx,
  const DynamicContext*                         aDynCtx) const
{
  try {
    std::string lHostName;
    std::string lUserName;
    std::string lPassword;
    getHostUserPassword(aArgs, 0, lHostName, lUserName, lPassword);
    String lMailbox = getOneStringArg(aArgs, 1);
    
    bool lSuccess = ImapClient::Instance().subscription(lHostName, lUserName, lPassword, lMailbox.c_str(), true);
    if (!lSuccess) {
      std::string lErrorMessage = ImapClient::Instance().getError();
      raiseImapError("IMAP0001", lErrorMessage);
    }
    
  } catch (EmailException& e) {
    raiseImapError(e);
  }
  return ItemSequence_t(NULL);
}

//*****************************************************************************

UnsubscribeFunction::UnsubscribeFunction(const ImapModule* aModule)
  : ImapFunction(aModule)
{
}

ItemSequence_t
UnsubscribeFunction::evaluate(
  const ExternalFunction::Arguments_t& aArgs,
  const StaticContext*                          aSctxCtx,
  const DynamicContext*                         aDynCtx) const
{
  try {
    std::string lHostName;
    std::string lUserName;
    std::string lPassword;
    getHostUserPassword(aArgs, 0, lHostName, lUserName, lPassword);
    String lMailbox = getOneStringArg(aArgs, 1);
    
    bool lSuccess = ImapClient::Instance().subscription(lHostName, lUserName, lPassword, lMailbox.c_str(), false);
    if (!lSuccess) {
      std::string lErrorMessage = ImapClient::Instance().getError();
      raiseImapError("IMAP0001", lErrorMessage);
    }
    
  } catch (EmailException& e) {
    raiseImapError(e);
  }
  return ItemSequence_t(NULL);
}

//*****************************************************************************

ExpungeFunction::ExpungeFunction(const ImapModule* aModule)
  : ImapFunction(aModule)
{
}

ItemSequence_t
ExpungeFunction::evaluate(
  const ExternalFunction::Arguments_t& aArgs,
  const StaticContext*                          aSctxCtx,
  const DynamicContext*                         aDynCtx) const
{
  try {
    std::string lHostName;
    std::string lUserName;
    std::string lPassword;
    getHostUserPassword(aArgs, 0, lHostName, lUserName, lPassword);
    String lMailbox = getOneStringArg(aArgs, 1);
    
    bool lSuccess = ImapClient::Instance().expunge(lHostName, lUserName, lPassword, lMailbox.c_str());
    if (!lSuccess) {
      std::string lErrorMessage = ImapClient::Instance().getError();
      raiseImapError("IMAP0001", lErrorMessage);
    }
    
  } catch (EmailException& e) {
    raiseImapError(e);
  }
  return ItemSequence_t(NULL);
}

//*****************************************************************************

SearchFunction::SearchFunction(const ImapModule* aModule)
  : ImapFunction(aModule)
{
}

ItemSequence_t
SearchFunction::evaluate(
  const ExternalFunction::Arguments_t& aArgs,
  const StaticContext*                          aSctxCtx,
  const DynamicContext*                         aDynCtx) const
{
  try {
    std::string lHostName;
    std::string lUserName;
    std::string lPassword;
    getHostUserPassword(aArgs, 0, lHostName, lUserName, lPassword);
    String lMailbox = getOneStringArg(aArgs, 1);
    String lCriteria = getOneStringArg(aArgs, 2);
    // get none or one boolean arg
    bool lUid = false;
    Item lItem;
    Iterator_t arg3_iter = aArgs[3]->getIterator();
    arg3_iter->open();
    if (arg3_iter->next(lItem)) {
      lUid = lItem.getBooleanValue();
    }  
    arg3_iter->close();
    std::vector<long> lFoundSequenceNumbers = ImapClient::Instance().search(lHostName, lUserName, lPassword, lMailbox.c_str(), lCriteria.c_str(), lUid);
    
    // throw zorba exception if we have an error log
    std::string lErrorMessage = ImapClient::Instance().getError();
    if (lErrorMessage.size() != 0) {
      raiseImapError("IMAP0001", lErrorMessage);
    }
    
    int lVectorSize =(int)lFoundSequenceNumbers.size();
    std::vector<Item> lItemVector;
    for (int i = 0; i < lVectorSize; i++) {
      lItemVector.push_back(theModule->getItemFactory()->createLong(lFoundSequenceNumbers[i]));
    } 
    
    return ItemSequence_t(new VectorItemSequence(lItemVector));
  } catch (EmailException& e) {
    raiseImapError(e);
  }
  return ItemSequence_t(NULL);
}

//*****************************************************************************

CopyFunction::CopyFunction(const ImapModule* aModule)
  : ImapFunction(aModule)
{
}

ItemSequence_t
CopyFunction::evaluate(
  const ExternalFunction::Arguments_t& aArgs,
  const StaticContext*                          aSctxCtx,
  const DynamicContext*                         aDynCtx) const
{
  try {
    std::string lHostName;
    std::string lUserName;
    std::string lPassword;
    getHostUserPassword(aArgs, 0, lHostName, lUserName, lPassword);
    String lMailboxFrom = getOneStringArg(aArgs, 1);
    String lMailboxTo = getOneStringArg(aArgs, 2);
    // find out if we are working with uid's instead of sequence numbers.
    bool lUid = false;
    Item lItem;
    
    Iterator_t arg4_iter = aArgs[4]->getIterator();
    arg4_iter->open();
    if (arg4_iter->next(lItem)) {
      lUid = lItem.getBooleanValue();
    }
    arg4_iter->close();
    Iterator_t arg5_iter = aArgs[5]->getIterator();
    arg5_iter->open();
    bool lCopy = false;
    if (arg5_iter->next(lItem)) {
      lCopy = lItem.getBooleanValue();
    }
    arg5_iter->close();
    
    std::string lMessageNumbers = getMessageNumbers(aArgs, 3); 
    
    bool lSuccess = ImapClient::Instance().copy(lHostName, lUserName, lPassword, lMailboxFrom.c_str(), lMailboxTo.c_str(), lMessageNumbers, lUid, lCopy);
    if (!lSuccess) {
      std::string lErrorMessage = ImapClient::Instance().getError();
      raiseImapError("IMAP0001", lErrorMessage);
    }
    
  } catch (EmailException& e) {
    raiseImapError(e);
  }
  return ItemSequence_t(NULL);
}

//*****************************************************************************

FetchEnvelopeFunction::FetchEnvelopeFunction(const ImapModule* aModule)
  : ImapFunction(aModule)
{
}

ItemSequence_t
FetchEnvelopeFunction::evaluate(
  const ExternalFunction::Arguments_t& aArgs,
  const StaticContext*                          aSctxCtx,
  const DynamicContext*                         aDynCtx) const
{
  try {
    std::string lHostName;
    std::string lUserName;
    std::string lPassword;
    getHostUserPassword(aArgs, 0, lHostName, lUserName, lPassword);
    String lMailbox = getOneStringArg(aArgs, 1);
    
    unsigned long lMessageNumber = getOneMessageNumber(aArgs, 2);  
    
    bool lUid = false;
    Item lItem;
    Iterator_t arg3_iter = aArgs[3]->getIterator();
    arg3_iter->open();
    if (arg3_iter->next(lItem)) {
      lUid = lItem.getBooleanValue();
    }
    arg3_iter->close();
    
    Item lParent; 
    getMessage(lParent,
      lHostName.c_str(), lUserName.c_str(), lPassword.c_str(),
      lMailbox.c_str(), lMessageNumber,
      lUid, true);
    
    std::string lErrorMessage = ImapClient::Instance().getError();
    if (lErrorMessage.size() != 0) {
      raiseImapError("IMAP0001", lErrorMessage);
    }

    return ItemSequence_t(new SingletonItemSequence(lParent));

  } catch (EmailException& e) {
    raiseImapError(e);
  }
  return ItemSequence_t(NULL);
}

//*****************************************************************************

FetchSubjectFunction::FetchSubjectFunction(const ImapModule* aModule)
  : ImapFunction(aModule)
{
}

ItemSequence_t
FetchSubjectFunction::evaluate(
  const ExternalFunction::Arguments_t& aArgs,
  const StaticContext*                          aSctxCtx,
  const DynamicContext*                         aDynCtx) const
{
  try {
    std::string lHostName;
    std::string lUserName;
    std::string lPassword;
    getHostUserPassword(aArgs, 0, lHostName, lUserName, lPassword);
    String lMailbox = getOneStringArg(aArgs, 1);
    unsigned long lMessageNumber = getOneMessageNumber(aArgs, 2);
    std::string lRawSubject =
      ImapClient::Instance().fetchSubject(
        lHostName,
        lUserName,
        lPassword,
        lMailbox.c_str(),
        lMessageNumber);
    
    // decode the subject header because it can contain encoded words
    std::string lDecodedSubject;
    decodeHeader(lRawSubject, lDecodedSubject);

    std::string lErrorMessage = ImapClient::Instance().getError();
    if (lErrorMessage.size() != 0) {
      raiseImapError("IMAP0001", lErrorMessage);
    }
    
    return ItemSequence_t(new SingletonItemSequence(theModule->getItemFactory()->createString(lDecodedSubject)));

  } catch (EmailException& e) {
    raiseImapError(e);
  }
  return ItemSequence_t(NULL);
}

//*****************************************************************************

FetchFromFunction::FetchFromFunction(const ImapModule* aModule)
  : ImapFunction(aModule)
{
}

ItemSequence_t
FetchFromFunction::evaluate(
  const ExternalFunction::Arguments_t& aArgs,
  const StaticContext*                          aSctxCtx,
  const DynamicContext*                         aDynCtx) const
{
  try {
    std::string lHostName;
    std::string lUserName;
    std::string lPassword;
    getHostUserPassword(aArgs, 0, lHostName, lUserName, lPassword);
    String lMailbox = getOneStringArg(aArgs, 1);
    unsigned long lMessageNumber = getOneMessageNumber(aArgs, 2);
    String lResult = ImapClient::Instance().fetchFrom(lHostName, lUserName, lPassword, lMailbox.c_str(), lMessageNumber);
    
    std::string lErrorMessage = ImapClient::Instance().getError();
    if (lErrorMessage.size() != 0) {
      raiseImapError("IMAP0001", lErrorMessage);
    }
    
    
    return ItemSequence_t(new SingletonItemSequence(
                                                    theModule->getItemFactory()->createString(lResult)));
  } catch (EmailException& e) {
    raiseImapError(e);
  }
  return ItemSequence_t(NULL);
}

//*****************************************************************************

FetchFlagsFunction::FetchFlagsFunction(const ImapModule* aModule)
  : ImapFunction(aModule)
{
}

ItemSequence_t
FetchFlagsFunction::evaluate(
  const ExternalFunction::Arguments_t& aArgs,
  const StaticContext*                          aSctxCtx,
  const DynamicContext*                         aDynCtx) const
{
  try {
    std::string lHostName;
    std::string lUserName;
    std::string lPassword;
    getHostUserPassword(aArgs, 0, lHostName, lUserName, lPassword);
    String lMailbox = getOneStringArg(aArgs, 1);
    unsigned long lMessageNumber = getOneMessageNumber(aArgs, 2);
    bool lUid = false;
    Item lItem;
    Iterator_t arg3_iter = aArgs[3]->getIterator();
    arg3_iter->open();
    if (arg3_iter->next(lItem)) {
      lUid = lItem.getBooleanValue();
    }
    arg3_iter->close();
    
    // null parent
    Item lParent;
    std::vector<int> lFlags(6, 0);
    
    ImapClient::Instance().fetchFlags(lHostName, lUserName, lPassword, lMailbox.c_str(), lMessageNumber,lFlags, lUid);
    
    std::string lErrorMessage = ImapClient::Instance().getError();
    if (lErrorMessage.size() != 0) {
      raiseImapError("IMAP0001", lErrorMessage);
    }
    
    
    Item lFlagsItem;
    createFlagsNode(lParent, lFlagsItem, lFlags, true);
    
    return ItemSequence_t(new SingletonItemSequence(lFlagsItem));
  } catch (EmailException& e) {
    raiseImapError(e);
  }
  return ItemSequence_t(NULL);
}

//*****************************************************************************

SetFlagsFunction::SetFlagsFunction(const ImapModule* aModule)
  : ImapFunction(aModule)
{
}

ItemSequence_t
SetFlagsFunction::evaluate(
  const ExternalFunction::Arguments_t&  aArgs,
  const StaticContext*                  aSctxCtx,
  const DynamicContext*                 aDynCtx) const
{
  try {
    std::string lHostName;
    std::string lUserName;
    std::string lPassword;
    getHostUserPassword(aArgs, 0, lHostName, lUserName, lPassword);
    String lMailbox = getOneStringArg(aArgs, 1);
    unsigned long lMessageNumber = getOneMessageNumber(aArgs, 2);
    
    std::vector<int> lFlags(6, 0);
    SetFlagsFunction::getFlagsVector(aArgs, lFlags); 
    
    bool lUid = false;
    Item lItem;
    Iterator_t arg4_iter = aArgs[4]->getIterator();
    arg4_iter->open();
    if (arg4_iter->next(lItem)) {
      lUid = lItem.getBooleanValue();
    }
    arg4_iter->close();
    
    ImapClient::Instance().setFlags(lHostName, lUserName, lPassword, lMailbox.c_str(), lMessageNumber, lFlags, lUid);
    
    std::string lErrorMessage = ImapClient::Instance().getError();
    if (lErrorMessage.size() != 0) {
      raiseImapError("IMAP0001", lErrorMessage);
    }
    
  } catch (EmailException& e) {
    raiseImapError(e);
  }
  return ItemSequence_t(NULL);
}

void
SetFlagsFunction::getFlagsVector(
  const ExternalFunction::Arguments_t& aArgs,
  std::vector<int>& aFlags)
{
  Item lFlagsNode;
  Iterator_t arg3_iter = aArgs[3]->getIterator();
  arg3_iter->open();
  arg3_iter->next(lFlagsNode);
  arg3_iter->close();
  Iterator_t lChildren = lFlagsNode.getChildren();
  lChildren->open();
  Item lChild;
  while(lChildren->next(lChild)) {
    Item lNameNode; 
    lChild.getNodeName(lNameNode);
    String lName = lNameNode.getStringValue();
    /* checking for ending of name as it is not sure how the prefix looks like */
    if (fn::ends_with(lName,"n")) {
      // seen
      aFlags[0] = 1; 
    } else if (fn::ends_with(lName,"ted")) {
      // deleted
      aFlags[1] = 1;
    } else if (fn::ends_with(lName,"ged")) {
      // flagged
      aFlags[2] = 1;
    } else if (fn::ends_with(lName,"red")) {
      // answered
      aFlags[3] = 1;
    } else if (fn::ends_with(lName,"ft")) {
      // draft
      aFlags[4] = 1;
    }
  }
}

//*****************************************************************************

FetchUidFunction::FetchUidFunction(const ImapModule* aModule)
  : ImapFunction(aModule)
{
}

ItemSequence_t
FetchUidFunction::evaluate(
  const ExternalFunction::Arguments_t& aArgs,
  const StaticContext*                          aSctxCtx,
  const DynamicContext*                         aDynCtx) const
{
  try {
    std::string lHostName;
    std::string lUserName;
    std::string lPassword;
    getHostUserPassword(aArgs, 0, lHostName, lUserName, lPassword);
    String lMailbox = getOneStringArg(aArgs, 1);
    unsigned long lMessageNumber = getOneMessageNumber(aArgs, 2);
    unsigned long lResult = ImapClient::Instance().convertNumber(lHostName, lUserName, lPassword, lMailbox.c_str(), lMessageNumber, true);
    
    return ItemSequence_t(new SingletonItemSequence(
                                                    theModule->getItemFactory()->createLong(lResult)));
  } catch (EmailException& e) {
    raiseImapError(e);
  }
  return ItemSequence_t(NULL);
}

//*****************************************************************************

FetchMessageSequenceNumberFunction::FetchMessageSequenceNumberFunction(const ImapModule* aModule)
  : ImapFunction(aModule)
{
}

ItemSequence_t
FetchMessageSequenceNumberFunction::evaluate(
  const ExternalFunction::Arguments_t& aArgs,
  const StaticContext*                          aSctxCtx,
  const DynamicContext*                         aDynCtx) const
{
  try {
    std::string lHostName;
    std::string lUserName;
    std::string lPassword;
    getHostUserPassword(aArgs, 0, lHostName, lUserName, lPassword);
    String lMailbox = getOneStringArg(aArgs, 1);
    unsigned long lMessageNumber = getOneMessageNumber(aArgs, 2);
    unsigned long lResult = ImapClient::Instance().convertNumber(lHostName, lUserName, lPassword, lMailbox.c_str(), lMessageNumber, false);
    
    return ItemSequence_t(new SingletonItemSequence(
                                                    theModule->getItemFactory()->createLong(lResult)));
  } catch (EmailException& e) {
    raiseImapError(e);
  }
  return ItemSequence_t(NULL);
}

//*****************************************************************************               
                                                                                              
FetchMessageFunction::FetchMessageFunction(const ImapModule* aModule)                       
  : ImapFunction(aModule)                                                                     
{                                                                                             
}      

ItemSequence_t
FetchMessageFunction::evaluate(
  const ExternalFunction::Arguments_t& aArgs,
  const StaticContext*                          aSctxCtx,
  const DynamicContext*                         aDynCtx) const
{
  try {
    std::string lHostName;  std::string lUserName;  std::string lPassword;
    getHostUserPassword(aArgs, 0, lHostName, lUserName, lPassword);
    String lMailbox = getOneStringArg(aArgs, 1);
    
    unsigned long lMessageNumber = getOneMessageNumber(aArgs, 2);
    
    bool lUid = getOneBoolArg(aArgs, 3);
    
    Item lParent;
    getMessage(lParent,
      lHostName.c_str(), lUserName.c_str(), lPassword.c_str(),
      lMailbox.c_str(), lMessageNumber,
      lUid, false);
    
    return ItemSequence_t(new SingletonItemSequence(lParent));
  } catch (EmailException& e) {
    raiseImapError(e);
  }
  return ItemSequence_t(NULL);
}


// This function is defined here because if defined in imap_function.cpp
// some header file conflict are raised and it does not compile
void
ImapFunction::decodeEncodedWords(
  const std::string& aRawSubject,
  std::string& aDecodedSubject) const
{
  std::size_t lLength = aRawSubject.length();
  // if this is not a good format return the raw subject
  if (lLength < 4 || (aRawSubject.find("=?") != 0 && aRawSubject.find("?=", lLength - 2) != lLength - 2)) {
    aDecodedSubject = aRawSubject;
    return;
  }

  // take out the part between the "=?" and "?="
  std::string lToSplit = aRawSubject.substr(2, lLength - 4);

  // get the charset
  std::size_t lQPosTmp;
  std::size_t lQPos = lToSplit.find("?");
  if (lQPos == std::string::npos)
  {
    aDecodedSubject = aRawSubject;
    return;
  }
  std::string lCharset = lToSplit.substr(0, lQPos);
  lQPosTmp = lQPos + 1;

  // get the encoding
  lQPos = lToSplit.find("?", lQPosTmp);
  if (lQPos == std::string::npos)
  {
    aDecodedSubject = aRawSubject;
    return;
  }
  std::string lEncoding = lToSplit.substr(lQPosTmp, lQPos - lQPosTmp);
  lQPosTmp = lQPos + 1;

  // get the encoded data
  lQPos = lToSplit.find("?", lQPos + 1);
  if (lQPos != std::string::npos)
  {
    aDecodedSubject = aRawSubject;
    return;
  }
  std::string lData = lToSplit.substr(lQPosTmp);

  // binary
  if (lEncoding == "B" || lEncoding == "b")
  {
    unsigned long lNewLength;
    void* lNewData = rfc822_base64((unsigned char*)lData.c_str(), lData.length(), &lNewLength);
    lData = std::string((char *)lNewData, lNewLength);
  }
  // quoted
  else if (lEncoding == "Q" || lEncoding == "q")
  {
    unsigned long lNewLength;
    void* lNewData = rfc822_qprint((unsigned char*)lData.c_str(), lData.length(), &lNewLength);
    lData = std::string((char *)lNewData, lNewLength);
    std::replace( lData.begin(), lData.end(), '_', ' ' );
  }
  // not a valid encoding?
  else
  {
    aDecodedSubject = aRawSubject;
    return;
  }

  // decode this fragment according to the charset
  toUtf8(lData, lCharset.c_str(), aDecodedSubject);
}

} /* namespace emailmodule */
} /* namespace zorba */

#ifdef WIN32
#  define DLL_EXPORT __declspec(dllexport)
#else
#  define DLL_EXPORT __attribute__ ((visibility("default")))
#endif

extern "C" DLL_EXPORT zorba::ExternalModule* createModule() {
  return new zorba::emailmodule::ImapModule();
}
