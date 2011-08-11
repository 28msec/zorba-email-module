(:
 : This example shows how to use the set-flags function of the
 : http://www.zorba-xquery.com/modules/email/imap module.
 :
 : First, the unique identifiers of all messages containing the word "flags" in
 : the subject are retrieved. Then, the flags of the first of the message found
 : are set to "seen" and "answered".
 :
 : The message will henceforth have exactly the flags that were set using the
 : XML structure, so all the existing flag before this call are lost.
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

let $uids := imap:search($hostInfo, "INBOX", "SUBJECT flags", true())

let $flags :=
  <flags xmlns="http://www.zorba-xquery.com/modules/email">
    <seen/>
    <answered/>
  </flags>

return
  imap:set-flags($hostInfo, "INBOX", $uids[1], $flags, true())