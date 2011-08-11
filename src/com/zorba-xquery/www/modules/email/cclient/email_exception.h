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

#ifndef ZORBA_EMAILMODULE_EMAILEXCEPTION_H
#define ZORBA_EMAILMODULE_EMAILEXCEPTION_H


#ifdef WIN32
#include <windows.h>
#endif
#include "c-client.h"
#include <string>
#undef max
#include <list>
#include <vector>
#include <exception>
#include <sstream>

namespace zorba { namespace emailmodule {

class EmailException : std::exception
{
  public:
    explicit EmailException(
      const std::string& aLocalName,
      const std::string& message) throw();
  
    virtual ~EmailException() throw();

    virtual const char*
    what() const throw();

    const std::string&
    get_message() const;

    const std::string&
    get_localname() const;

  private:
    std::string theLocalName;
    std::string theMessage;
};
    
} // namespace emailmodule
} // namespace zorba

#endif // ZORBA_EMAILMODULE_EMAILEXCEPTION_H
