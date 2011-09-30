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

#ifndef ZORBA_EMAILMODULE_SMTPFUNCTION_H
#define ZORBA_EMAILMODULE_SMTPFUNCTION_H

#include <zorba/function.h>

#include <email_exception.h>

namespace zorba { namespace emailmodule {

class SmtpModule;

class SmtpFunction : public ContextualExternalFunction
{
  protected:
    const SmtpModule* theModule;

  public:
    void
    raiseSmtpError(
      EmailException& e) const;

    void
    raiseSmtpError(
      const std::string& qName,
      const std::string& message) const;

    virtual void 
    getHostUserPassword(
      const ExternalFunction::Arguments_t& aArgs,
      int aPos,
      String& aHostName,
      String& aUserName,
      String& aPassword) const;

  public:
    SmtpFunction(const SmtpModule* aModule);
    ~SmtpFunction();

    virtual String
    getURI() const;

}; //class SmtpFunction

} // namespace email
} // namespace zorba

#endif // ZORBA_EMAILMODULE_SMTPFUNCTION_H
