(:
 : This example shows how to use the fetch-from function of the
 : http://www.zorba-xquery.com/modules/email/imap module.
 :
 : First, the sequence numbers of all messages containing the word "flags" in
 : the subject are retrieved. Then, the from field of the first message found
 : is fetched. This contains the name and the email address of the sender.
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
  imap:fetch-from($hostInfo, "INBOX", $uids[1])
