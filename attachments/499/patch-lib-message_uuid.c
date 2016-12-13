--- lib/message_uuid.c.orig	Thu Nov 23 10:09:39 2006
+++ lib/message_uuid.c	Mon Nov 27 09:31:21 2006
@@ -409,7 +409,7 @@
  ************************************************************************/
 
 int
-message_uuid_unpack(struct message_uuid *uuid, const char *packed)
+message_uuid_unpack(struct message_uuid *uuid, const unsigned char *packed)
 {
     assert(MESSAGE_UUID_SIZE == MESSAGE_UUID_PACKED_SIZE);
 
@@ -455,7 +455,7 @@
 message_uuid_from_text(struct message_uuid *uuid, const char *text)
 {
     const char *p = text;
-    char *buf = &uuid->value[0];
+    unsigned char *buf = &uuid->value[0];
     int i;
 
     for (i = 0 ; i < MESSAGE_UUID_SIZE ; i++) {
