(:
 : This example shows how to use the subscribe and unsubscribe functions of the
 : http://www.zorba-xquery.com/modules/email/imap module.
 :
 : Using the host information stored in the variable $hostInfo the user
 : imaptest is subscribed to the folder INBOX.Test on the account. After this,
 : the user is unsubscribed from this folder using the unsubscribe function.
 :
 : Subscribing to a folder will make the folder visible for email clients that
 : by default only list the subscribed folders.
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

return
  {
    imap:subscribe($hostInfo, "INBOX.Test");
    imap:unsubscribe($hostInfo, "INBOX.Test")
  }
