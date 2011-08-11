(:
 : This example shows how to use the status function of the
 : http://www.zorba-xquery.com/modules/email/imap module.
 :
 : Using the host information stored in the variable $hostInfo, the account
 : status of the user imaptest on the mail.28msec.com server is retrieved.
 :
 : The XML output of this script should look something like:
 :
 :  <status xmlns:imaps="http://www.zorba-xquery.com/modules/email">
 :    <messages>27</messages>
 :    <recent>0</recent>
 :    <unseen>5</unseen>
 :    <uidnext>77</uidnext>
 :    <uidvalidity>1285079359</uidvalidity>
 :  </status>
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
  imap:status($hostInfo, "INBOX")
