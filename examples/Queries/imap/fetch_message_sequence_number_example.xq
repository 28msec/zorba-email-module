(:
 : This example shows how to use the fetch-message-sequence-number function of
 : the http://www.zorba-xquery.com/modules/email/imap module.
 :
 : First, the unique identifiers of all messages containing the word "flags" in
 : the subject are retrieved. Then, the message sequence number of the first
 : message found is fetched.
 :
 : If no error is thrown, all operations were successful.
 :) 

import module namespace imap = 'http://www.zorba-xquery.com/modules/email/imap';


(: This variable contains the information of the account on the IMAP server. :) 
let $hostInfo :=
  <hostInfo xmlns="http://www.zorba-xquery.com/modules/email">
    <hostName>mail.28msec.com/novalidate-cert</hostName>
    <userName>imaptest</userName>
    <password>cclient</password>
  </hostInfo>

let $sequenceNumbers := imap:search($hostInfo, "INBOX", "SUBJECT flags", true())

return
  imap:fetch-message-sequence-number($hostInfo, "INBOX", $sequenceNumbers[1])
