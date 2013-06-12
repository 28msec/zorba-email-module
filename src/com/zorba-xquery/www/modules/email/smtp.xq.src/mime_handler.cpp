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

#include <vector>
#include <cstdio>
#include <sstream>

#include <zorba/base64.h>
#include <zorba/iterator.h>
#include <zorba/item_factory.h>
#include <zorba/store_consts.h>
#include <zorba/user_exception.h>
#include <zorba/xquery_functions.h>
#include <zorba/zorba_string.h>

#include "mime_handler.h"

#include "email_exception.h"

namespace zorba { namespace emailmodule {

// helper function for retrieving the NodeName of an Item
static void
getNodeName(const Item aElement, zorba::String& aValue)
{
  Item lNodeName;
  aElement.getNodeName(lNodeName);
  aValue = lNodeName.getLocalName();
}

// helper function for retrieving the string value of a Text Node
static void
getTextValue(const Item aElement, zorba::String& aValue)
{
  Iterator_t lChildrenIt;
  Item       lChild;

  aValue = String();
  lChildrenIt = aElement.getChildren();
  lChildrenIt->open();
  while (lChildrenIt->next(lChild)) {
    if (lChild.getNodeKind() == store::StoreConsts::textNode) {
      aValue.append(lChild.getStringValue());
    }
  }

  lChildrenIt->close();
}

/**
 * Encodes a zorba String if necessary (if it contains non-ascii chars)
 * and assigns it to the passed char-pointer-reference.
 */
void encodeStringForEMailHeader(const zorba::String& aString, char*& aCClientVal)
{
  // check if string contains non ascii chars
  bool lContainsNonAscii = false;
  for (
    zorba::String::const_iterator lIter = aString.begin();
    lIter != aString.end();
    ++lIter)
  {
    unsigned int u = static_cast<unsigned int>(*lIter);
    if (!(u == '\t' || u == '\n' || u == '\t' || (u >= 32 && u <= 127)))
    {
      lContainsNonAscii = true;
      break;
    }
  }

  if (lContainsNonAscii)
  {
    // if string contains non-ascii chars, we encode it with
    // base64 encoding and generate a header value according to
    // http://tools.ietf.org/html/rfc2047 (MIME encoded-word syntax).
    zorba::String lEncodedValue = zorba::base64::encode(aString);
    zorba::String lFullValue = zorba::String("=?UTF-8?B?") 
                             + lEncodedValue 
                             + zorba::String("?=");
    aCClientVal = cpystr(lFullValue.c_str());
  }
  else 
  {
    // if string contains ascii chars only, do don't encode anything
    aCClientVal = cpystr(aString.c_str());
  }
}

// helper function for retrieving the name and email address from an item
static void
getNameAndEmailAddress(
  Item& aEmailItem,
  String& aName,
  String& aMailbox,
  String& aHost)
{
  Iterator_t lChildren = aEmailItem.getChildren();
  lChildren->open();
  Item lChild;
  // name might not exist -> empty string by default
  aName = "";
  while (lChildren->next(lChild)) {
    if (lChild.getNodeKind() != store::StoreConsts::elementNode) {
      continue;
    }

    String lNodeName;
    getNodeName(lChild, lNodeName);
    if (lNodeName == "name") {
      aName = lChild.getStringValue();
    } else {
      String lEmail = lChild.getStringValue();
      int lIndexOfAt = lEmail.find('@'); 
      aMailbox = lEmail.substr(0, lIndexOfAt).c_str();
      aHost = lEmail.substr(lIndexOfAt + 1, lEmail.length() - lIndexOfAt - 1).c_str();
    }  
  }
}

mail_address*
create_mail_address(String& aName, String& aMailbox, String& aHost)
{
  mail_address* address = mail_newaddr();
  encodeStringForEMailHeader(aName, address->personal);
  encodeStringForEMailHeader(aMailbox, address->mailbox);
  encodeStringForEMailHeader(aHost, address->host);
  return address;
}


CClientMimeHandler::~CClientMimeHandler()
{
}

void
CClientMimeHandler::begin(const Item& aMimeItem)
{
  Iterator_t lChildIter;

  //initialize ENVELOPE
  theEnvelope = mail_newenvelope ();
  
  //initialize BODY
  theBody = mail_newbody ();
  mail_initbody(theBody);

  //set theMessageItem
  lChildIter = aMimeItem.getChildren();
  lChildIter->open();

  // read envelope and body elements but skip non-element nodes
  while (lChildIter->next(theEnvelopeItem)) {
    if (theEnvelopeItem.getNodeKind() == store::StoreConsts::elementNode) {
      break;
    }
  }
  while (lChildIter->next(theBodyItem)) {
    if (theBodyItem.getNodeKind() == store::StoreConsts::elementNode) {
      break;
    }
  }

  lChildIter->close();
}

void
CClientMimeHandler::end()
{
}

void
CClientMimeHandler::envelope()
{
  if (theEnvelopeItem.isNull()) {
    throw EmailException("PARSE_ERROR", "The message could not be parsed.");
  }

  //set the date from the client.
  //If this is not set it defaults to the date of the SMTP server.
  char line[MAILTMPLEN];
  rfc822_date (line);
  theEnvelope->date = (unsigned char *) fs_get (1+strlen (line));
  strcpy((char *)theEnvelope->date,line);


  Iterator_t    lChildIter;
  zorba::Item   lChild;
  String lNodeName, lNodeValue;
  String lName, lMailbox, lHost;

  lChildIter = theEnvelopeItem.getChildren();
  lChildIter->open();
  while (lChildIter->next(lChild)) {
    if (lChild.getNodeKind() != store::StoreConsts::elementNode) {
      continue;
    }

    getNodeName(lChild, lNodeName);
    getTextValue(lChild, lNodeValue);

    if (lNodeName == "date") {
      char lDate[MAILTMPLEN]; 
      parseXmlDateTime(lNodeValue, lDate);
      theEnvelope->date = (unsigned char *) fs_get (1+strlen (lDate));
      strcpy ((char *)theEnvelope->date, lDate);
    } else if (lNodeName == "from") {
      getNameAndEmailAddress(lChild, lName, lMailbox, lHost);
      theEnvelope->from = create_mail_address(lName, lMailbox, lHost);
    } else if (lNodeName == "sender") {
      getNameAndEmailAddress(lChild, lName, lMailbox, lHost);
      theEnvelope->sender = create_mail_address(lName, lMailbox, lHost);
    } else if (lNodeName == "replyto") {
      getNameAndEmailAddress(lChild, lName, lMailbox, lHost);
      theEnvelope->reply_to = create_mail_address(lName, lMailbox, lHost);
    } else if (lNodeName == "subject") {
      encodeStringForEMailHeader(lNodeValue, theEnvelope->subject);
    } else if (lNodeName == "recipient") {
      Iterator_t lRecipentChildren = lChild.getChildren(); 
      lRecipentChildren->open();
      Item lRecipentChild;
      // read the recipient element but skip comments
      while (lRecipentChildren->next(lRecipentChild)) {
        if (lRecipentChild.getNodeKind() == store::StoreConsts::elementNode) {
          break;
        }
      }
      getNodeName(lRecipentChild, lNodeName);
      lRecipentChildren->close();

      if (lNodeName == "to") {
        getNameAndEmailAddress(lRecipentChild, lName, lMailbox, lHost);
        // there can be multiple to nodes, iterate to the next free one!
        ADDRESS** lNext = &theEnvelope->to;
        while (*lNext) {
          lNext = &((*lNext)->next);
        }  
        *lNext = create_mail_address(lName, lMailbox, lHost);
      } else if(lNodeName == "cc") {
        getNameAndEmailAddress(lRecipentChild, lName, lMailbox, lHost);
        ADDRESS** lNext = &theEnvelope->cc;
        while (*lNext) {
          lNext = &((*lNext)->next);
        }  
        *lNext = create_mail_address(lName, lMailbox, lHost);
      } else if (lNodeName == "bcc") {
        getNameAndEmailAddress(lRecipentChild, lName, lMailbox, lHost);
        ADDRESS** lNext = &theEnvelope->bcc;
        while (*lNext) {
          lNext = &((*lNext)->next);
        }  
        *lNext = create_mail_address(lName, lMailbox, lHost);
      }
    }       
  }
  lChildIter->close();
}

void
CClientMimeHandler::body()
{
  theBody->type = TYPEOTHER;

  Iterator_t lChildIter;
  Item lChild;
  String lNodeName;

  lChildIter = theBodyItem.getChildren();
  lChildIter->open();
  while (lChildIter->next(lChild)) {
    if (lChild.getNodeKind() != store::StoreConsts::elementNode) {
      continue;
    }

    getNodeName(lChild, lNodeName);

    if (lNodeName == "content") {
      parse_content(theBody, lChild);
    }
    else if (lNodeName == "multipart") {
      parse_multipart(theBody, lChild);
    }
  }
  lChildIter->close();
}

void
CClientMimeHandler::set_text_body(BODY* aBody, String& aMessage)
{
  String lMessage;
  // special case for encoding of base64 which needs a new line after 64 characters
  if (aBody->encoding == ENCBASE64) {
    std::stringstream lInStream;
    std::stringstream lOutStream;
    // for loop that counts to 64 and then makes a new line
    lInStream << aMessage.c_str();
    char next;
    int counter = 0;
    while (lInStream >> next) {
      if (++counter == 64) {
        lOutStream << "\r\n";
        counter = 1;
      }
      lOutStream << next;
    }  
    lMessage = lOutStream.str(); 
  } else {
    lMessage = aMessage;
  }
    
  char *text = (char *) fs_get (strlen(lMessage.c_str()));
  text = cpystr (lMessage.c_str());
  aBody->contents.text.size = strlen(text);
  aBody->contents.text.data = (unsigned char *) text;   //message body

}

PARAMETER*
CClientMimeHandler::create_param(
  String aAttribute,
  String aValue,
  PARAMETER* aPrevious)
{
  PARAMETER *lParam;
  lParam = mail_newbody_parameter();
  lParam->attribute = cpystr (aAttribute.c_str());
  lParam->value = cpystr (aValue.c_str());

  if (aPrevious) {
    aPrevious->next = lParam;
  }

  return lParam;
}

void
CClientMimeHandler::set_encoding(BODY* aBody, String& aEncoding)
{
  String lUpperEnc = fn::upper_case(aEncoding);

  if (lUpperEnc == "ENC7BIT")
    aBody->encoding = ENC7BIT;
  else if (lUpperEnc == "ENC8BIT")
    aBody->encoding = ENC8BIT;
  else if (lUpperEnc == "ENCBINARY")
    aBody->encoding = ENCBINARY;
  else if (lUpperEnc == "ENCBASE64")
    aBody->encoding = ENCBASE64;
  else if (lUpperEnc == "ENCQUOTEDPRINTABLE")
    aBody->encoding = ENCQUOTEDPRINTABLE;
  else
    aBody->encoding = ENCOTHER;
}

bool
CClientMimeHandler::set_content_type_value(BODY* aBody, String& aValue)
{
  bool lRes = true;
  int lPos = aValue.find('/');
  String lLowerType = fn::lower_case(aValue.substr(0, lPos));

  //set the BODY content type
  if (lLowerType == "text")
    aBody->type = TYPETEXT;
  else if (lLowerType == "multipart")
    aBody->type = TYPEMULTIPART;
  else if (lLowerType == "message")
    aBody->type = TYPEMESSAGE;
  else if (lLowerType == "application")
    aBody->type = TYPEAPPLICATION;
  else if (lLowerType == "image")
    aBody->type = TYPEIMAGE;
  else if (lLowerType == "audio")
    aBody->type = TYPEAUDIO;
  else if (lLowerType == "video")
    aBody->type = TYPEVIDEO;
  else {
    aBody->type = TYPEOTHER;
    lRes = false;
  }

  //set the BODY content subtype
  if (lRes) {
    // the list of subtypes of each type is available at
    // http://www.iana.org/assignments/media-types/
    String lSubtype = aValue.substr(lPos + 1, aValue.length() - lPos);
    aBody->subtype = cpystr(fn::upper_case(lSubtype).c_str());
  }

  return lRes;
}

void
CClientMimeHandler::set_contentTypeCharsetCTF(
  BODY* aBody,
  const Item& aContentOrMultipartItem)
{     
  Iterator_t lAttributes = aContentOrMultipartItem.getAttributes();
  lAttributes->open();
  Item lAttributeItem;
  String lNodeName, lNodeValue;
  while (lAttributes->next(lAttributeItem)) {
    getNodeName(lAttributeItem, lNodeName); 
    lNodeValue = lAttributeItem.getStringValue();

    if (lNodeName == "contentType") {
      set_content_type_value(aBody, lNodeValue);
    } else if (lNodeName == "charset") {
      aBody->parameter = create_param("charset", fn::upper_case(lNodeValue), NIL);
    } else if (lNodeName == "contentTransferEncoding") {
      set_encoding(aBody, lNodeValue);  
    } else if (lNodeName == "contentDisposition") {
      aBody->disposition.type = cpystr(fn::upper_case(lNodeValue).c_str()); 
    } else if (lNodeName == "contentDisposition-filename") {
      if (!aBody->disposition.parameter) {  
        aBody->disposition.parameter = create_param("filename", lNodeValue, NIL);
      } else {
        aBody->disposition.parameter->next = create_param("filename", lNodeValue, NIL);
      }
    } else if (lNodeName == "contentDisposition-modification-date") {
      char lDate[MAILTMPLEN];
      parseXmlDateTime(lNodeValue, lDate);      
      if (!aBody->disposition.parameter) {
        aBody->disposition.parameter = create_param("modification-date", lDate); 
      } else {
        aBody->disposition.parameter->next = create_param("modification-date", lDate);
      } 
    }
  }
      
  lAttributes->close();
}


bool
CClientMimeHandler::parse_content(BODY* aBody, const Item aItemContent)
{
  zorba::String lValue;

  // set the contentType, charset and contentTransferEncoding (which are attributes of a content node)
  set_contentTypeCharsetCTF(aBody, aItemContent);
    
  lValue = aItemContent.getStringValue();
  set_text_body(aBody, lValue);
        
  return true;
}

bool
CClientMimeHandler::parse_multipart(BODY* aBody, const Item aItemMultipart)
{
  Iterator_t lChildIter;
  Item lChild;
  String lNodeName, lNodeValue;
  PART* lPartRoot = NIL;
  PART* lPartPrev = NIL;
 
  set_contentTypeCharsetCTF(aBody, aItemMultipart);
    
  // a multipart node constists of several content or multipart nodes
  lChildIter = aItemMultipart.getChildren();
  lChildIter->open();
  while (lChildIter->next(lChild)) {
    if (lChild.getNodeKind() != store::StoreConsts::elementNode) {
      continue;
    }

    // a simple content item
    getNodeName(lChild, lNodeName);

    if (lNodeName == "content") {
      PART* lPart;
      lPart = mail_newbody_part();
      parse_content(&lPart->body, lChild);
      if (lPartPrev) {
        lPartPrev->next = lPart;
      } else {
        lPartRoot = lPart;
      }  
      lPartPrev = lPart;
    }
    // a multipart item ... this calls for recursion 
    else if (lNodeName == "multipart") {
      PART* lPart;
      lPart = NIL;
      lPart = mail_newbody_part();

      parse_multipart(&lPart->body,lChild);
        
      if (lPartPrev) {
        lPartPrev->next = lPart;
      } else {
        lPartRoot = lPart;
      }  
      lPartPrev = lPart;
    }
  }
  lChildIter->close();

  if (lPartRoot) {
    aBody->nested.part = lPartRoot;
  }

  return true;
}

void 
CClientMimeHandler::parseXmlDateTime(String& aXmlDateTime, char* aCDateTime)
{
    // first we get year, month, day, hour, minute and seconds as zorba strings      

  int lTempIndex;
  lTempIndex = aXmlDateTime.find('-');
  String lYearString = aXmlDateTime.substr(0, lTempIndex);
  aXmlDateTime = aXmlDateTime.substr(lTempIndex+1);
  
  lTempIndex = aXmlDateTime.find('-');
    
  String lMonthString = aXmlDateTime.substr(0, lTempIndex);
  aXmlDateTime = aXmlDateTime.substr(lTempIndex+1);
      
  lTempIndex = aXmlDateTime.find('T');

  String lDayString = aXmlDateTime.substr(0, lTempIndex);
  aXmlDateTime = aXmlDateTime.substr(lTempIndex+1);
            
  lTempIndex = aXmlDateTime.find(':');
      
  String lHourString = aXmlDateTime.substr(0, lTempIndex);
  aXmlDateTime = aXmlDateTime.substr(lTempIndex+1);
      
  lTempIndex = aXmlDateTime.find(':');
      
  String lMinutesString = aXmlDateTime.substr(0, lTempIndex);
  aXmlDateTime = aXmlDateTime.substr(lTempIndex+1);
     
  // the next two digits specify the seconds
  String lSecondsString = aXmlDateTime.substr(0, 2);


  MESSAGECACHE * lDummyCache = mail_new_cache_elt (0);
  unsigned int lTempDatePart;
     

  // now, according to the specification of the dateTime xml type, we can have either: nothing, a Z (for UTC), -XXXX or +XXXX)
  String lUTCString = aXmlDateTime.substr(2);

  if (fn::starts_with(lUTCString,"+") || fn::starts_with(lUTCString,"-")) {
    lTempIndex = lUTCString.find(':');
    String lUTCHours = lUTCString.substr(1, lTempIndex-1);
    String lUTCMinutes = lUTCString.substr(lTempIndex + 1);
        
    std::stringstream lHoursConverter;
    lHoursConverter << lUTCHours.c_str();
    lHoursConverter >> lTempDatePart;
    lDummyCache->zhours  = lTempDatePart;
         
    std::stringstream lMinutesConverter;
    lMinutesConverter << lUTCMinutes.c_str();
    lMinutesConverter >> lTempDatePart;
    lDummyCache->zminutes = lTempDatePart;
       
    if (fn::starts_with(lUTCString,"-")) { 
      lDummyCache->zoccident = 1;
    }
  }      
 
  // now we convert them and throw them into a dummy message cache
  {
    std::stringstream lConverter;
    lConverter << lYearString.c_str();
    lConverter >> lTempDatePart;      
    lTempDatePart -= 1970;
    lDummyCache->year = lTempDatePart;
  }

  {
    std::stringstream lConverter;
    lConverter << lMonthString.c_str();
    lConverter >> lTempDatePart;
    lDummyCache->month = lTempDatePart;
  }

  {
    std::stringstream lConverter;
    lConverter << lDayString.c_str();
    lConverter >> lTempDatePart;
    lDummyCache->day = lTempDatePart;
  }
      
  {
    std::stringstream lConverter;
    lConverter << lHourString.c_str();
    lConverter >> lTempDatePart;
    lDummyCache->hours = lTempDatePart;
  }

  {
    std::stringstream lConverter;
    lConverter << lMinutesString.c_str();
    lConverter >> lTempDatePart;
    lDummyCache->minutes = lTempDatePart;
  }
      
  {
    std::stringstream lConverter;
    lConverter << lSecondsString.c_str();
    lConverter >> lTempDatePart;
    lDummyCache->seconds = lTempDatePart;
  }
          
  mail_cdate(aCDateTime, lDummyCache); 
  
  // like this, we have a string of the form: Sun Aug  8 08:40:40 2010 +0000\n
  // what we actually would like is follwing: Fri, 10 Dec 2010 14:14:28 +0100 (CET)

  std::stringstream lDate(aCDateTime);
      
  std::string lDayAsWord;
  std::string lMonthAsWord;
  std::string lDay;
  std::string lTime;
  std::string lYear;
  std::string lUTC;

  lDate >> lDayAsWord >> lMonthAsWord >> lDay >> lTime >> lYear >> lUTC;
      
  lDate.str(std::string());
  lDate << lDayAsWord << ", " << lDay << " " <<  lMonthAsWord << " " << lYear << " " << lTime << " " << lUTC << " (UTF)";  
    
  strcpy(aCDateTime, lDate.str().c_str());
}

} //namespace emailmodule
} //namespace zorba
