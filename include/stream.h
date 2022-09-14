#ifndef STREAM_H
#define STREAM_H

#include "client.h"
#include "defs.h"
#include <stdbool.h>

typedef struct TwitchStream {
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
} TwitchStream;

void get_stream_by_user_login(Client *client, TwitchStream *stream, const char *user_login);
int get_followed_streams(Client *client, TwitchStream **followed, int count);
void __stream_init_from_json(TwitchStream *stream, json_object *json);
int __populate_stream_array(Client *client, const char *url, TwitchStream **streams, Paginator *iterator, int items);

#endif /* STREAM_H */
