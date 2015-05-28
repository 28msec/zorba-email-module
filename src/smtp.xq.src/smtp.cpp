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

#include <iostream>
#include <sstream>
#ifdef WIN32
#include <windows.h>
#endif

#include <zorba/empty_sequence.h>
#include <zorba/diagnostic_list.h>
#include <zorba/user_exception.h>
#include <zorba/singleton_item_sequence.h>

#include "smtp_module.h"
#include "imap_client.h"
#include "email_exception.h"
#include "mime_parser.h"
#include "smtp.h"

namespace zorba { namespace emailmodule {

SendFunction::SendFunction(const SmtpModule* aModule)
  : SmtpFunction(aModule)
{
}
    
ItemSequence_t
SendFunction::evaluate(
  const ExternalFunction::Arguments_t& args,
  const StaticContext* aSctxCtx,
  const DynamicContext* aDynCtx) const
{
  try {
    // getting host, username and password 
    String lHostName, lUserName, lPassword;
    getHostUserPassword(args, 0, lHostName, lUserName, lPassword);      
        
    std::stringstream lDiagnostics; 
    // getting message as item
    Item messageItem;
    Iterator_t arg1_iter = args[1]->getIterator();
    arg1_iter->open();
    arg1_iter->next(messageItem);
    arg1_iter->close();
        
    CClientMimeHandler lHandler;
    MimeParser lParser(&lHandler);
    lParser.parse(messageItem, lDiagnostics);

    bool lHasRecipient = (lHandler.getEnvelope()->to ||
                          lHandler.getEnvelope()->cc ||
                          lHandler.getEnvelope()->bcc);

    if (!lHasRecipient) {
      throw EmailException("NO_RECIPIENT", "Message has no recipient.");
    } else {
      bool lRes = ImapClient::Instance().send(
        lHostName.c_str(), lUserName.c_str(), lPassword.c_str(),
        lHandler.getEnvelope(), lHandler.getBody(), lDiagnostics);
      if (!lRes) {
        std::stringstream lReport;
        lReport
          << "Message could not be sent. Reason: "
          << lDiagnostics.str();
        throw EmailException("NOT_SENT", lReport.str());
      }
    }
  } catch (EmailException& e) {
    raiseSmtpError(e);
  }
  return ItemSequence_t(NULL);
}
    
} // namespace emailmodule
} // namespace zorba

#ifdef WIN32
#  define DLL_EXPORT __declspec(dllexport)
#else
#  define DLL_EXPORT __attribute__ ((visibility("default")))
#endif

extern "C" DLL_EXPORT zorba::ExternalModule* createModule() {
  return new zorba::emailmodule::SmtpModule();
}
