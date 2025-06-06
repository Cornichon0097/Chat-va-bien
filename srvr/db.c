#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <mongoc/mongoc.h>
#include <bson/bson.h>

#include <logger.h>

#include <db.h>

#define DB_USER_FIELD "user"
#define DB_PWD_FIELD "pwd"

struct db_connect {
        mongoc_client_t *clnt;
        mongoc_collection_t *collec;
};

static int db_build_uri(const void *const config, const int flag,
                        char *const uri, const size_t size)
{
        if (flag == DB_URI)
                strncpy(uri, config, size);

        return 0;
}

struct db_connect *db_init(const void *const config, const int flag)
{
        struct db_connect *dbc;
        bson_t *ping = NULL, reply = BSON_INITIALIZER;
        char db_uri[BUFSIZ];
        bson_error_t err;

        dbc = (struct db_connect *) malloc(sizeof(struct db_connect));

        if (dbc == NULL) {
                log_error("[db] malloc(): %s", strerror(errno));
                return NULL;
        }

        db_build_uri(config, flag, db_uri, BUFSIZ);
        log_debug("[db] Attempt connection to %s", db_uri);

        mongoc_init();
        dbc->clnt = mongoc_client_new(db_uri);

        ping = BCON_NEW("ping", BCON_INT32(1));

        if (!mongoc_client_command_simple(dbc->clnt, "cvb", ping, NULL, &reply, &err)) {
                log_error("[db] Connection failed: %s", err.message);
                return NULL;
        }

        log_info("[db] Successfully connected as %s on %s", "cornichon", "cvb");

        bson_destroy(&reply);
        bson_destroy(ping);

        dbc->collec = mongoc_client_get_collection(dbc->clnt, "cvb", "users");

        return dbc;
}

int db_insert(struct db_connect *const dbc, const char *const username,
              const char *const password)
{
        bson_t *doc = BCON_NEW("{", DB_USER_FIELD, BCON_UTF8(username),
                                    DB_PWD_FIELD, BCON_UTF8(password), "}");
        bson_error_t err;

        if (!mongoc_collection_insert_one(dbc->collec, doc, NULL, NULL, &err)) {
                log_error("[db] Insert operation failed: %s", err.message);
                return -1;
        }

        log_debug("[db] Inserted user %s", username);

        bson_destroy(doc);

        return 0;
}

int db_update(struct db_connect *const dbc, const char *const username,
              const char *const password)
{
        bson_t *query = BCON_NEW(DB_USER_FIELD, BCON_UTF8(username));
        bson_t *update = BCON_NEW("$set", "{", DB_PWD_FIELD,
                                  BCON_UTF8(password), "}");
        bson_error_t err;

        if (!mongoc_collection_update_one(dbc->collec, query, update, NULL, NULL, &err)) {
                log_error("[db] Update operation failed: %s", err.message);
                return -1;
        }

        log_debug("[db] Updated user %s", username);

        bson_destroy(update);
        bson_destroy(query);

        return 0;
}

int db_find(struct db_connect *const dbc, const char *const username,
            char *const password, const size_t size)
{
        bson_t *filter = BCON_NEW(DB_USER_FIELD, BCON_UTF8(username));
        mongoc_cursor_t *res = mongoc_collection_find_with_opts(dbc->collec, filter, NULL, NULL);
        const bson_t *doc;
        char *str;
        bson_error_t err;

        while (mongoc_cursor_next(res, &doc)) {
                str = bson_as_canonical_extended_json(doc, NULL);

                /* TODO */
                if (password != NULL)
                        strncpy(password, str, size);

                log_debug("[db] Retrieved document: %s", str);
                bson_free(str);
        }

        if (mongoc_cursor_error(res, &err)) {
                log_error("[db] Find operation failed: %s", err.message);
                return -1;
        }

        log_debug("[db] Found user %s", username);

        mongoc_cursor_destroy(res);
        bson_destroy(filter);

        return 0;
}

int db_delete(struct db_connect *const dbc, const char *const username)
{
        bson_t *filter = BCON_NEW(DB_USER_FIELD, BCON_UTF8(username));
        bson_error_t err;

        if (!mongoc_collection_delete_one(dbc->collec, filter, NULL, NULL, &err)) {
                log_error("[db] Delete operation failed: %s", err.message);
                return -1;
        }

        log_debug("[db] Deleted user %s", username);

        bson_destroy(filter);

        return 0;
}

void db_close(struct db_connect **const dbc)
{
        mongoc_collection_destroy((*dbc)->collec);
        mongoc_client_destroy((*dbc)->clnt);
        mongoc_cleanup();

        free(*dbc);
        *dbc = NULL;
}
