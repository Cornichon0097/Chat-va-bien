#ifndef DB_H
#define DB_H

#define DB_URI 0x0
#define BD_PATH 0x1

struct db_connect *db_init(const void *config, int flag);

int db_insert(struct db_connect *dbc, const char *user, const char *pwd);

int db_update(struct db_connect *dbc, const char *user, const char *pwd);

int db_find(struct db_connect *dbc, const char *user, char *pwd, size_t size);

int db_delete(struct db_connect *dbc, const char *user);

void db_close(struct db_connect **dbc);

#endif /* db.h */
