#ifndef STREAM_H
#define STREAM_H

#include <stdbool.h>
#include "defs.h"
#include "client.h"

typedef struct Stream {
    char id[ID_LEN];
    char user_id[ID_LEN];
    char user_login[USER_LOGIN_LEN];
    char user_name[USERNAME_LEN];
    char game_id[ID_LEN];
    char game_name[GAMENAME_LEN];
    char type[20];
    char title[TITLE_LEN];
    int viewer_count;
    char started_at[STARTED_AT_LEN];
    char language[LANGUAGE_LEN];
    char thumbnail_url[URL_LEN];
    char *tag_ids;
    bool is_mature;
} Stream;

void get_stream_by_user_login(Client *client, Stream *stream, const char *user_login);
int get_followed_streams(Client *client, Stream **followed, int count);
void __stream_init_from_json(Stream *stream, json_object *json);
int __populate_stream_array(Client *client, const char *url, Stream **streams, Paginator *iterator, int items);

#endif /* STREAM_H */
