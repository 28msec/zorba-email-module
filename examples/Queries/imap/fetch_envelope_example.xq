(:
 : This example shows how to use the fetch-envelope function of
 : the http://www.zorba-xquery.com/modules/email/imap module.
 :
 : Using the host information stored in the variable $hostInfo, first a message
 : containing the word "test" in the subject is searched for, using the search
 : function. Then, the envelope of the first message found is fetched.
 :
 : The XML output of this script should look something like:
 :
 :  <envelope xmlns="http://www.zorba-xquery.com/modules/email">
 :    <date>2011-02-14T13:07:38</date>
 :    <from>
 :      <name>Daniel Thomas</name>
 :      <email>thomas.daniel.james@gmail.com</email>
 :    </from>
 :    <sender>
 :      <name>Daniel Thomas</name>
 :      <email>thomas.daniel.james@gmail.com</email>
 :    </sender>
 :    <replyTo>
 :      <name>Daniel Thomas</name>
 :      <email>thomas.daniel.james@gmail.com</email>
 :    </replyTo>
 :    <subject>test</subject>
 :    <recipient>
 :      <to>
 :        <email>imaptest@28msec.com</email>
 :      </to>
 :    </recipient>
 :    <messageId>&lt;AANLkTinSujfq9-UgGDvX+RcOrhvQf5JTnTdwSLNTiqZ5@mail.gmail.com&gt;</messageId>
 :    <flags/>
 :  </envelope>
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

let $uids := imap:search($hostInfo, "INBOX", "SUBJECT test", false())
return
  imap:fetch-envelope($hostInfo, "INBOX", $uids[1], false())
