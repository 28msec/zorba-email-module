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

#include "smtp_module.h"

#include "smtp.h"
#include "smtp_function.h"

namespace zorba { namespace emailmodule {

ItemFactory* SmtpModule::theFactory = 0;

SmtpModule::~SmtpModule()
{
  FuncMap_t::const_iterator lIter = theFunctions.begin();
  for (; lIter != theFunctions.end(); ++lIter) {
    delete lIter->second;
  }

  theFunctions.clear();
}

String
SmtpModule::getURIString()
{
  return "http://www.zorba-xquery.com/modules/email/smtp";
}
    
ExternalFunction*
SmtpModule::getExternalFunction(const String& aLocalname)
{
  ExternalFunction*& lFunc = theFunctions[aLocalname];
  if (!lFunc) {
    if (aLocalname == "send-impl") {
      lFunc = new SendFunction(this);
    }
  }
  return lFunc;
}

void
SmtpModule::destroy()
{
  if (!dynamic_cast<SmtpModule*>(this)) {
    return;
  }
  delete this;
}

} // namespace email
} // namespace zorba
