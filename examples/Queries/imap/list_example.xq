(:
 : This example shows how to use the list function of the
 : http://www.zorba-xquery.com/modules/email/imap module.
  :
 : Using the host information stored in the variable $hostInfo, all folders on
 : the given account that match the pattern "INBOX.Dr*" are listed.
 :
 : The resulting XML should look something like this:
 : 
 :  <mailbox xmlns="http://www.zorba-xquery.com/modules/email">
 :    <hostName/>
 :    <mailboxName>INBOX.Drafts</mailboxName>
 :  </mailbox> 
 : 
 : The parameters for the list function are interesting. The second parameter
 : is applied to the pattern in an implementation dependent fashion to search
 : for matching mailbox names. At this point we haven't found an example where
 : this could be useful or even usable, so you could pass the empty string.
 :
 : The fourth and last parameter lets you choose if only subscribed mailboxes
 : should be considered for the search.
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
  imap:list($hostInfo, "", "INBOX.Dra*", false())
