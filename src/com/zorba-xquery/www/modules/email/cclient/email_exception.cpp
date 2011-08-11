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

#include "email_exception.h"


namespace zorba { namespace emailmodule {

EmailException::EmailException(
  const std::string& aLocalName,
  const std::string& msg) throw()
  : theLocalName(aLocalName), theMessage(msg)
{
}

EmailException::~EmailException() throw()
{
}

const char*
EmailException::what() const throw()
{
  return theMessage.c_str();
}

const std::string&
EmailException::get_message() const
{
  return theMessage;
}

const std::string&
EmailException::get_localname() const
{
  return theLocalName;
}

} // namespace emailmodule
} // namespace zorba
