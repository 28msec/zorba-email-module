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

#ifndef ZORBA_EMAILMODULE_SMTP_H
#define ZORBA_EMAILMODULE_SMTP_H

#include <zorba/zorba_string.h>

#include "smtp_function.h"

namespace zorba { namespace emailmodule {

class SendFunction : public SmtpFunction
{
  public:
    SendFunction(const SmtpModule* aModule);

    virtual String
    getLocalName() const { return "send-impl"; }

    virtual ItemSequence_t
    evaluate(
      const ExternalFunction::Arguments_t& args,
      const StaticContext* aSctxCtx,
      const DynamicContext* aDynCtx) const;

};

} // namespace emailmodule
} // namespace zorba

#endif // ZORBA_EMAILMODULE_SMTP_H
