(:
 : This example shows how to use the search function of the
 : http://www.zorba-xquery.com/modules/email/imap module.
 :
 : The mailbox INBOX of the user described by the $hostInfo variable is
 : searched for messages containing the word "test" in their subject.
 :
 : The search function returns the sequence of unique identifiers of mesages
 : that comply with the search criteria. By setting the last argument to
 : fn:false(), the message sequence numbers are returned instead.
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
  imap:search($hostInfo, "INBOX", "SUBJECT test", true())
