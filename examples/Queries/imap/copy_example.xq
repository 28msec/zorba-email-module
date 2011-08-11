(:
 : This example shows how to use the copy function of the http://www.zorba-xquery.com/modules/email/imap module.
 :
 : First we search for a message with the word copy in the subject. When we have found this message, we
 : use the copy function to copy it internally to the CopyFolder folder.
 : This creates a new message in the copy folder with a own unique identifier and message sequence number.
 :) 

import module namespace imap = 'http://www.zorba-xquery.com/modules/email/imap';


(: This variable contains the information of the account on the IMAP server. :) 
let $hostInfo :=
  <hostInfo xmlns="http://www.zorba-xquery.com/modules/email">
    <hostName>mail.28msec.com/novalidate-cert</hostName>
    <userName>imaptest</userName>
    <password>cclient</password>
  </hostInfo>

let $uids := imap:search($hostInfo, "INBOX", "SUBJECT copy2", true())

return
  imap:copy($hostInfo, "INBOX", "INBOX.CopyFolder", $uids, true())
