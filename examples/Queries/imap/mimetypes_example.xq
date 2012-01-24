(:
 : This should issue 4
 :) 

import schema namespace email = 'http://www.zorba-xquery.com/modules/email';

let $messages := 
  for $type in ("application/3gpp-ims+xml","application/cals-1840","application/pkcs7-signature","application/vnd.openxmlformats-officedocument.presentationml.comments+xml")
  return
    fn:validate {
      <email:message xmlns:email="http://www.zorba-xquery.com/modules/email/email">
        <envelope>
          <date>2010-11-26T15:50:39</date>
          <from>
            <email>zorba.smtp.sender@gmail.com</email>
          </from>
          <sender>
            <email>zorba.smtp.sender@gmail.com</email>
          </sender>
          <replyTo>
            <email>zorba.smtp.sender@gmail.com</email>
          </replyTo>
          <subject>RegexTest</subject>
          <recipient>
            <to>
              <email>imaptest@28msec.com</email>
            </to>
          </recipient>
          <messageId>&lt;4ea85e91.679e440a.0f97.241e@mx.google.com&gt;</messageId>
          <flags>
            <seen/>
          </flags>
        </envelope>
        <mimeVersion>1.0</mimeVersion>
        <body>
          <multipart contentType="multipart/mixed" charset="us-ascii" contentTransferEncoding="ENC7BIT">
            <content contentType="text/plain" charset="us-ascii" contentTransferEncoding="ENCQUOTEDPRINTABLE">=0A          Zorba really rocks. =0A        </content>
            <content contentType="{$type}" charset="us-ascii" contentTransferEncoding="ENCBASE64" contentDisposition="the-truth.gif">some content</content>
          </multipart>
        </body>
      </email:message>
    }
return
  fn:count($messages)
