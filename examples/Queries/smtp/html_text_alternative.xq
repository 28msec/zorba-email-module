(:
 : This example uses the send function of the SMTP module to send an email
 : containing an HTML and text message from a GMail account. If an email client
 : does not support HTML email messages, it can use the text version. The order
 : of the alternative contents might be relevant for some email clients.
 : This message is also sent to the sender itself in CC.
 :)

import module namespace smtp = 'http://www.zorba-xquery.com/modules/email/smtp'; 


(: Note the use of the serialize function to create a string from the HTML content. :)
let $htmlContent := fn:serialize(
    <html>
      <head></head>
      <body>
        <h1>Zorba really rocks.</h1>
        <p>XQuery brought to a new level</p>
      </body>
    </html>
  )
let $textContent := "Simple text version ... get yourself a nice and fancy email client."

(: This variable contains the information of the account from which the email should be sent. :) 
let $hostInfo :=
  <hostInfo xmlns="http://www.zorba-xquery.com/modules/email">
    <hostName>smtp.gmail.com:587/tls/novalidate-cert</hostName>
    <userName>zorba.smtp.sender</userName>
    <password>1openssl!!</password>
  </hostInfo>

(: the XML representation of the email to be sent, as described by the XML schema
   http://www.zorba-xquery.com/modules/email :)
let $email := 
  <message xmlns="http://www.zorba-xquery.com/modules/email">
    <envelope>
      <date>2010-11-26T15:50:39-04:01</date>
      <subject>An important HTML or Text message</subject>
      <recipient>
        <to>
          <name>Test Account</name>
          <email>imaptest@28msec.com</email>
        </to>
      </recipient>
      <recipient>
        <cc>
          <name>Myself</name>
          <email>zorba.smtp.sender@gmail.com</email>
        </cc>
      </recipient>
    </envelope>
    <body>
      <multipart contentType="multipart/alternative" charset="UTF-8" contentTransferEncoding="ENC8BIT">
        <content contentType="text/html" charset="UTF-8" contentTransferEncoding="ENC8BIT">{  
          $htmlContent
        }</content>
        <content contentType="text/plain" charset="UTF-8" contentTransferEncoding="ENC8BIT">{
          $textContent
        }</content>
      </multipart>  
    </body>
  </message>

return
  smtp:send($hostInfo, $email)
