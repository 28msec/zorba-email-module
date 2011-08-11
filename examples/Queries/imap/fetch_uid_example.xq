(:
 : This example shows how to use the fetch-uid function of the
 : http://www.zorba-xquery.com/modules/email/imap module.
 :
 : First, the message sequence numbers of all messages with the word "flags" in
 : the subject are retrieved. Then, the unique identifier of the first message
 : foundis fetched.
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

let $uids := imap:search($hostInfo, "INBOX", "SUBJECT flags", false())

return
  imap:fetch-uid($hostInfo, "INBOX", $uids[1])
