(:
 : This example uses the send function of the SMTP module to send an email
 : containing a simple text message from a GMail account.
 :)

import module namespace smtp = 'http://www.zorba-xquery.com/modules/email/smtp'; 


(: This variable contains the information of the account from which the email should be sent. :) 
let $hostInfo :=
  <hostInfo xmlns="http://www.zorba-xquery.com/modules/email">
    <hostName>smtp.gmail.com:587/tls/novalidate-cert</hostName>
    <userName>zorba.smtp.sender</userName>
    <password>1openssl!</password>
  </hostInfo>

(: the XML representation of the email to be sent, as described by the XML schema
   http://www.zorba-xquery.com/modules/email :)
let $email := 
  <message xmlns="http://www.zorba-xquery.com/modules/email">
    <envelope>
      <date>2010-11-26T15:50:39-04:01</date>
      <subject>An important message</subject>
      <recipient>
        <to>
          <name>Test Account</name>
          <email>imaptest@28msec.com</email>
        </to>
      </recipient>
    </envelope>
    <body>
      <content contentType="text/plain" charset="UTF-8" contentTransferEncoding="ENC8BIT">
        Zorba really rocks. 
      </content>
    </body>
  </message>

return
  smtp:send($hostInfo, $email)
