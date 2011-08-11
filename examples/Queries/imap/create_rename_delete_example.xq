(:
 : This example shows how to use the create, delete  function of the
 : http://www.zorba-xquery.com/modules/email/imap module.
 :
 : Using the host information stored in the variable $hostInfo a new mailbox
 : is created on the mail.28msec.com server for this account.
 :
 : The mailbox name is partly random (to make sure that there is no other
 : mailbox with that name). Then, we rename the mailbox to another random
 : generated name. In the end the renamed mailbox is deleted to make sure that
 : the account does not get swamped with useless mailboxes.
 : 
 : If no error is thrown, all operations were successful.
 :) 

import module namespace imap = 'http://www.zorba-xquery.com/modules/email/imap';
import module namespace random = 'http://www.zorba-xquery.com/modules/random';


(: This variable contains the information of the account on the IMAP server. :) 
let $hostInfo :=
  <hostInfo xmlns="http://www.zorba-xquery.com/modules/email">
    <hostName>mail.28msec.com/novalidate-cert</hostName>
    <userName>imaptest</userName>
    <password>cclient</password>
  </hostInfo>

let $mailboxName := fn:concat("INBOX.", fn:substring(random:uuid(), 0, 4))
let $newMailboxName := fn:concat("INBOX.", fn:substring(random:uuid(), 0, 5))

return
  {
    imap:create($hostInfo, $mailboxName);
    imap:rename($hostInfo, $mailboxName, $newMailboxName);
    imap:delete($hostInfo, $newMailboxName)
  }
