
#ifndef SQL_H
#define SQL_H

typedef int (*dbx_callback)(void *, int, char **, char **);

int dbx_init(void);
void dbx_close(void);
int dbx_query(const char *cmd);
int dbx_query_cb(const char *cmd,void *pCtx,dbx_callback cb);
uint64_t dbx_get_id(void);

#endif //SQL_H
