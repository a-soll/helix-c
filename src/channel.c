#include "channel.h"
#include "string.h"
#include "util.h"
#include <string.h>

void __channel_init_from_json(Channel *channel, json_object *json) {
    memccpy(channel->game_id, get_key(json, "game_id"), '\0', sizeof(channel->game_id));
    memccpy(channel->game_name, get_key(json, "game_name"), '\0', sizeof(channel->game_name));
    memccpy(channel->title, get_key(json, "title"), '\0', sizeof(channel->title));
    memccpy(channel->broadcaster_language, get_key(json, "language"), '\0', sizeof(channel->broadcaster_language));
}

int __populate_channel_array(Client *client, const char *url, Channel **channels, Paginator *iterator, int items) {
    int chan_index = items;
    Response *response = curl_request(client, url, curl_GET);
    Channel *c;
    get_json_array(response, "data");
    if (*channels == NULL) {
        c = malloc(sizeof(Channel) * response->data_len);
    } else {
        c = realloc(*channels, sizeof(Channel) * (items + response->data_len));
    }
    int ret = response->data_len;
    for (int i = 0; i < response->data_len; i++) {
        Channel chan;
        json_object *data_array_object;
        data_array_object = json_object_array_get_idx(response->data, i);
        __channel_init_from_json(&chan, data_array_object);
        c[chan_index] = chan;
        chan_index++;
    }
    if (response->data_len > 0) {
        set_pagination(iterator->pagination, response->response);
    }
    *channels = c;
    clean_response(response);
    return ret;
}

void __searchedchannel_init_from_json(SearchedChannel *channel, json_object *json) {
    memccpy(channel->broadcaster_language, get_key(json, "broadcaster_language"), '\0', sizeof(channel->broadcaster_language));
    memccpy(channel->broadcaster_login, get_key(json, "broadcaster_login"), '\0', sizeof(channel->broadcaster_login));
    memccpy(channel->game_id, get_key(json, "game_id"), '\0', sizeof(channel->game_id));
    memccpy(channel->game_name, get_key(json, "game_name"), '\0', sizeof(channel->game_name));
    memccpy(channel->id, get_key(json, "id"), '\0', sizeof(channel->id));
    memccpy(channel->started_at, get_key(json, "started_at"), '\0', sizeof(channel->started_at));
    memccpy(channel->thumbnail_url, get_key(json, "thumbnail_url"), '\0', sizeof(channel->thumbnail_url));
    memccpy(channel->title, get_key(json, "title"), '\0', sizeof(channel->title));
}

int __populate_searched_channel_array(Client *client, const char *url, SearchedChannel **channels, Paginator *iterator, int items) {
    int chan_index = items;
    Response *response = curl_request(client, url, curl_GET);
    SearchedChannel *c;
    get_json_array(response, "data");
    if (*channels == NULL) {
        c = malloc(sizeof(SearchedChannel) * response->data_len);
    } else {
        c = realloc(*channels, sizeof(SearchedChannel) * (items + response->data_len));
    }
    int ret = response->data_len;
    for (int i = 0; i < response->data_len; i++) {
        SearchedChannel chan;
        json_object *data_array_object;
        data_array_object = json_object_array_get_idx(response->data, i);
        __searchedchannel_init_from_json(&chan, data_array_object);
        c[chan_index] = chan;
        chan_index++;
    }
    if (response->data_len > 0) {
        set_pagination(iterator->pagination, response->response);
    }
    *channels = c;
    clean_response(response);
    return ret;
}

void Channel_init(Channel *c) {
}

void Channel_deinit(Channel *c) {
    // free(c->user_id);
    // free(c->started_at);
    // free(c->title);
}

void get_channel_stream(Client *client, TwitchStream *stream, Channel *from) {
    get_stream_by_user_login(client, stream, from->broadcaster_login);
}

void get_channel_user(Client *client, User *user, Channel *from) {
    get_user_by_login(client, user, from->broadcaster_login);
}

int search_channels(Client *client, const char *keyword, SearchedChannel **channels, Paginator *iterator, int items, bool live_only) {
    SearchedChannel *c;
    const char *live_flag;
    live_flag = (live_only == true) ? "true" : "false";
    char *base_url = "https://api.twitch.tv/helix/search/channels?first=100&live_only=";
    int base_len = strlen(base_url);
    char url[URL_LEN];
    int chan_index = items;

    if (iterator->pagination[0] == '\0') {
        int len = fmt_string(url, "%s%s&query=%s", base_url, live_flag, keyword);
        url[len] = '\0';
    } else {
        int len = fmt_string(url, "%s%s&query=%s&after=%s", base_url, live_flag, keyword, iterator->pagination);
        url[len] = '\0';
    }
    int ret = __populate_searched_channel_array(client, url, channels, iterator, items);
    return ret;
}
