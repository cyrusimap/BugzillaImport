--- lib/message_uuid.h.orig	Mon Nov 27 09:30:17 2006
+++ lib/message_uuid.h	Mon Nov 27 09:31:02 2006
@@ -58,7 +58,7 @@
    * (Wrapper for memcpy() with current implementation) */
 
 int
-message_uuid_unpack(struct message_uuid *uuid, const char *packed);
+message_uuid_unpack(struct message_uuid *uuid, const unsigned char *packed);
   /* Fetch Message UID from packed sequence (MESSAGE_UUID_PACKED_SIZE)
    * (Wrapper for memcpy() with current implementation) */
 
