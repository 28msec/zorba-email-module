(:
 : This example uses the send function of the SMTP module to send an email
 : containing a simple text message with an image as attachement from a GMail
 : account.
 : To make things really interesting, the image is made on the spot with the
 : help o the image modules available in Zorba.
 :)

import module namespace smtp = 'http://www.zorba-xquery.com/modules/email/smtp'; 
import module namespace basic = 'http://www.zorba-xquery.com/modules/image/basic';
import module namespace paint = 'http://www.zorba-xquery.com/modules/image/paint';


(: This variable contains the information of the account from which the email should be sent. :) 
let $hostInfo :=
  <hostInfo xmlns="http://www.zorba-xquery.com/modules/email">
    <hostName>smtp.gmail.com:587/tls/novalidate-cert</hostName>
    <userName>zorba.smtp.sender</userName>
    <password>1openssl!!</password>
  </hostInfo>

(: The base64 representation of the image to be sent. :) 
let $image := paint:paint(
  basic:create(xs:unsignedInt(200), xs:unsignedInt(200), "GIF"),
  <text xmlns="http://www.zorba-xquery.com/modules/image/image">
    <origin><x>20</x><y>50</y></origin>
    <text>Zorba really rocks.</text>
    <font>Arial</font>
    <font-size>14</font-size>
  </text>)

(: the XML representation of the email to be sent, as described by the XML schema
   http://www.zorba-xquery.com/modules/email :)
let $email := 
  <message xmlns="http://www.zorba-xquery.com/modules/email">
    <envelope>
      <date>2010-11-26T15:50:39-04:01</date>
      <subject>An important Image message</subject>
      <recipient>
        <to>
          <name>Test Account</name>
          <email>imaptest@28msec.com</email>
        </to>
      </recipient>
    </envelope>
    <body>
      <multipart contentType="multipart/mixed" charset="UTF-8" contentTransferEncoding="ENC8BIT">
        <content contentType="text/plain" charset="UTF-8" contentTransferEncoding="ENC8BIT">
          Zorba really rocks. 
        </content>
        <content contentType="image/gif" charset="UTF-8" contentTransferEncoding="ENCBASE64"
          contentDisposition="attachement" contentDisposition-filename="the-truth.gif">{

          $image

        }</content>
      </multipart>  
    </body>
  </message>

return
  smtp:send($hostInfo, $email)
