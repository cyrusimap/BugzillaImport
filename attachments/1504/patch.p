diff --git a/imap/index.c b/imap/index.c
index 0d51b20..9184c64 100644
--- a/imap/index.c
+++ b/imap/index.c
@@ -2226,6 +2226,11 @@ static void index_pruneheader(char *buf, const strarray_t *headers,
     p = buf;
     while (*p && *p != '\r') {
 	colon = strchr(p, ':');
+	/*
+	 * If there is no colon in remaining buffer,
+	 * there is no valid header, leave loop
+	 */
+	if (!colon) break;
 	if (colon && headers_not && headers_not->count) {
 	    goodheader = 1;
 	    for (l = headers_not->data ; *l ; l++) {
