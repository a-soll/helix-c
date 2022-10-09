//
//  c_file.h
//  native_twitch
//
//  Created by Adam Solloway on 3/8/22.
//

#ifndef channel_h
#define channel_h

#include "client.h"
#include "defs.h"
#include "stream.h"
#include "user.h"
#include <stdbool.h>
#include <stdio.h>

typedef struct Channel {
    char broadcaster_id[ID_LEN];
    char broadcaster_login[USERNAME_LEN];
    char broadcaster_name[USERNAME_LEN];
    char broadcaster_language[LANGUAGE_LEN];
    char game_id[ID_LEN];
    char game_name[GAMENAME_LEN];
    char title[TITLE_LEN];
    int delay;
} Channel;

typedef struct SearchedChannel {
    char broadcaster_language[3];
    char broadcaster_login[USERNAME_LEN];
    char game_id[10];
    char game_name[50];
    char id[26];
    bool is_live;
    char *tag_ids[10];
    char thumbnail_url[URL_LEN];
    char title[500];
    char started_at[30];
} SearchedChannel;

void Channel_init(Channel *c);
void Channel_deinit(Channel *c);
void get_channel_stream(Client *client, TwitchStream *stream, Channel *from);
void get_channel_user(Client *client, User *user, Channel *from);
int search_channels(Client *client, const char *keyword, SearchedChannel **channels, Paginator *iterator, int items, bool live_only);
// internal functions
int __populate_searched_channel_array(Client *client, const char *url, SearchedChannel **channels, Paginator *iterator, int items);
void __channel_init_from_json(Channel *channel, json_object *json);
int __populate_channel_array(Client *client, const char *url, Channel **channels, Paginator *iterator, int items);
void __searchedchannel_init_from_json(SearchedChannel *channel, json_object *json);

#endif /* c_file_h */
