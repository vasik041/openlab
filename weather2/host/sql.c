
#include "main.h"
#include "sqlite3.h"
#include "sql.h"

static sqlite3 *db = NULL;

//=============================================================================

int dbx_init(void)
{
	if(sqlite3_open("meas.db3", &db)) {
		fprintf(stderr, "Can\'t open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return 0;
	}
	if(sqlite3_busy_timeout(db,30000)) {
		fprintf(stderr, "Can\'t open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return 0;
	}
	return 1;
}

//=============================================================================

void dbx_close(void)
{
	if(db) {
		sqlite3_close(db);
		db = NULL;
	}
}

//=============================================================================

int dbx_query_cb(const char *cmd,void *pCtx,dbx_callback cb)
{
int rc = 0;
char *zErrMsg = NULL;

	rc = sqlite3_exec(db, cmd, cb, (void *)pCtx, &zErrMsg);
	if(rc != SQLITE_OK ) {
	    fprintf(stderr, "SQL error: %s\n", zErrMsg);
	    sqlite3_free(zErrMsg);
	    return 0;
	}
	return 1;
}

int dbx_query(const char *cmd)
{
	return dbx_query_cb(cmd,NULL,NULL);
}

//=============================================================================

uint64_t dbx_get_id(void)
{
	return sqlite3_last_insert_rowid(db);
}
