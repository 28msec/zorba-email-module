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

#include <zorba/zorba.h>
#include <zorba/iterator.h>
#include <zorba/user_exception.h>
#include <zorba/xquery_functions.h>
#include <zorba/store_consts.h>

#include "smtp_function.h"

#include "smtp_module.h"

namespace zorba { namespace emailmodule {

SmtpFunction::SmtpFunction(const SmtpModule* aModule)
  : theModule(aModule)
{
}

SmtpFunction::~SmtpFunction()
{
}

void
SmtpFunction::raiseSmtpError(EmailException& e) const
{
  std::string lCode;
  if (e.get_localname() == "PARSE_ERROR") {
    lCode = "SMTP0001";
  }
  else if (e.get_localname() == "NO_RECIPIENT") {
    lCode = "SMTP0002";
  }
  else if (e.get_localname() == "NOT_SENT") {
    lCode = "SMTP0003";
  }
  else {
    lCode = "SMTP9999";
  }
  raiseSmtpError(lCode, e.get_message());
}

void
SmtpFunction::raiseSmtpError(
  const std::string& aQName,
  const std::string& aMessage) const
{
  std::stringstream lErrorMessage;
  lErrorMessage << aMessage;
  Item lQName = theModule->getItemFactory()->createQName(getURI(), "smtp", aQName);
  throw USER_EXCEPTION(lQName, lErrorMessage.str());
}

String
SmtpFunction::getURI() const
{
  return theModule->getURI();
}

void 
SmtpFunction::getHostUserPassword(
  const ExternalFunction::Arguments_t& aArgs,
  int aPos,
  String& aHostName,
  String& aUserName,
  String& aPassword) const
{
  Item lNode;
  Iterator_t lArgsIter = aArgs[aPos]->getIterator();
  lArgsIter->open();
  lArgsIter->next(lNode);
  lArgsIter->close();

  Item lChild;
  Iterator_t lChildIter = lNode.getChildren();
  lChildIter->open();

  // now read the children but skipping non-element nodes
  while (lChildIter->next(lChild)) {
    if (lChild.getNodeKind() == store::StoreConsts::elementNode) {
      break;
    }
  }
  aHostName = lChild.getStringValue();
  while (lChildIter->next(lChild)) {
    if (lChild.getNodeKind() == store::StoreConsts::elementNode) {
      break;
    }
  }
  aUserName = lChild.getStringValue();
  while (lChildIter->next(lChild)) {
    if (lChild.getNodeKind() == store::StoreConsts::elementNode) {
      break;
    }
  }
  aPassword = lChild.getStringValue();
  lChildIter->close();  
}

} // namespace emailmodule
} // namespace zorba
