(:
 : This example shows how to use the fetch-message function of the
 : http://www.zorba-xquery.com/modules/email/imap module.
 :
 : First, the sequence numbers of all messages containing the word "test" in
 : the subject are retrieved. Then, the first message found is fetched.
 :
 : The resulting XML should look something like this:
 :
 :  <message xmlns="http://www.zorba-xquery.com/modules/email">
 :    <envelope>
 :      <date>2011-02-14T13:07:38</date>
 :      <from><name>Daniel Thomas</name><email>thomas.daniel.james@gmail.com</email></from>
 :      <sender><name>Daniel Thomas</name><email>thomas.daniel.james@gmail.com</email></sender>
 :      <replyTo><name>Daniel Thomas</name><email>thomas.daniel.james@gmail.com</email></replyTo>
 :      <subject>test</subject><recipient><to><email>imaptest@28msec.com</email></to></recipient>
 :      <messageId>&lt;AANLkTinSujfq9-UgGDvX+RcOrhvQf5JTnTdwSLNTiqZ5@mail.gmail.com&gt;</messageId>
 :      <flags/>
 :    </envelope>
 :    <mimeVersion>1.0</mimeVersion>
 :    <body>
 :      <multipart contentType="multipart/alternative" charset="us-ascii" contentTransferEncoding="ENC7BIT">
 :        <content contentType="text/plain" charset="us-ascii" contentTransferEncoding="ENC8BIT">This is a test message&#xD;</content>
 :        <content contentType="text/html" charset="us-ascii" contentTransferEncoding="ENC8BIT">This is a test message&#xD;</content>
 :      </multipart>
 :    </body>
 :  </message>
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
  imap:fetch-message($hostInfo, "INBOX", $uids[1], false())  
