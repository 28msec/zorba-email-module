(:
 : This example shows how to use the move function of the
 : http://www.zorba-xquery.com/modules/email/imap module.
 :
 : First, we search in the "INBOX.MoveFolder" mailbox for messages containing
 : the word "move" in the subject. If such messages are found, they are moved
 : to the INBOX. If not messages were found in the "INBOX.MoveFolder" mailbox,
 : the move is performed inthe other direction.
 :) 

import module namespace imap = 'http://www.zorba-xquery.com/modules/email/imap';


(: This variable contains the information of the account on the IMAP server. :) 
let $hostInfo :=
  <hostInfo xmlns="http://www.zorba-xquery.com/modules/email">
    <hostName>mail.28msec.com/novalidate-cert</hostName>
    <userName>imaptest</userName>
    <password>cclient</password>
  </hostInfo>

let $uids := imap:search($hostInfo, "INBOX.MoveFolder", "SUBJECT move", true())

return
  if ($uids) then
    imap:move($hostInfo, "INBOX.MoveFolder", "INBOX", $uids, true());
  else
    let $uids := imap:search($hostInfo, "INBOX", "SUBJECT move", true())
    return
      imap:move($hostInfo, "INBOX", "INBOX.MoveFolder", $uids, true())
