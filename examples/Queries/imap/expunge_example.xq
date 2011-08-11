(:
 : This example shows how to use the expunge function of the http://www.zorba-xquery.com/modules/email/imap module.
 : Using the host information stored in the variable $hostInfo first the search function is used to get
 : the unique id of the all messages with the word delete in its subject.
 : Then the delete flag is set on these messages using the set-flags method of the imap-module.
 : The call to expunge on the mailbox then deletes all messages that have the delete flag set.
 : 
 : This example is concluded by using the send function to send a new mail to the inbox to make sure that
 : there will be a message to delete next time the example is used.
 :) 

import module namespace imap = 'http://www.zorba-xquery.com/modules/email/imap';
import module namespace smtp = 'http://www.zorba-xquery.com/modules/email/smtp';


(: This variable contains the information of the account on the IMAP server. :) 
let $hostInfo :=
  <hostInfo xmlns="http://www.zorba-xquery.com/modules/email">
    <hostName>mail.28msec.com/novalidate-cert</hostName>
    <userName>imaptest</userName>
    <password>cclient</password>
  </hostInfo>

(: This variable contains the information of the account from which the email should be sent. :) 
let $senderHostInfo :=
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
      <subject>delete</subject>
      <recipient>
        <to>
          <name>Test Account</name>
          <email>imaptest@28msec.com</email>
        </to>
      </recipient>
     </envelope>
    <body>
      <content contentType="text/plain" charset="UTF-8" contentTransferEncoding="ENC8BIT">
        Oh yeah
      </content>
    </body>
  </message>

let $ids := imap:search($hostInfo, "INBOX", "SUBJECT delete", true());

(: if there are no emails to delete, then the send function of the SMTP library is probably not working, or the expunge is working excellently ... :)

if (fn:empty($ids)) then
  fn:true()
else
  {
    for $id in $ids 
    return
      imap:set-flags($hostInfo, "INBOX", $id, <email:flags><deleted/></email:flags>, true());

    imap:expunge($hostInfo, "INBOX");
  };


smtp:send($senderHostInfo, $email)
