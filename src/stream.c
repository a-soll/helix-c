#include "stream.h"
#include "util.h"
#include <cstr/cstr.h>
#include <json-c/json.h>

static const char *base_url = "https://api.twitch.tv/helix/streams";

int __populate_stream_array(Client *client, const char *url, TwitchStream **streams, Paginator *iterator, int items) {
    int chan_index = items;
    Response *response = curl_request(client, url, curl_GET);
    TwitchStream *s;
    get_json_array(response, "data");
    if (*streams == NULL) {
        s = malloc(sizeof(TwitchStream) * response->data_len);
    } else {
        s = realloc(*streams, sizeof(TwitchStream) * (items + response->data_len));
    }
    int ret = response->data_len;
    for (int i = 0; i < response->data_len; i++) {
        TwitchStream str;
        json_object *data_array_object;
        data_array_object = json_object_array_get_idx(response->data, i);
        __stream_init_from_json(&str, data_array_object);
        s[chan_index] = str;
        chan_index++;
    }
    if (response->data_len > 0) {
        paginator_set(iterator->pagination, response->response);
    }
    *streams = s;
    response_clean(response);
    return ret;
}

void __stream_init_from_json(TwitchStream *stream, json_object *json) {
    strcpy(stream->game_id, get_key(json, "game_id"));
    strcpy(stream->game_name, get_key(json, "game_name"));
    strcpy(stream->id, get_key(json, "id"));
    if (strcmp(get_key(json, "is_mature"), "true") == 0) {
        stream->is_mature = true;
    } else {
        stream->is_mature = false;
    }
    strcpy(stream->language, get_key(json, "language"));
    strcpy(stream->started_at, get_key(json, "started_at"));
    strcpy(stream->title, get_key(json, "title"));
    strcpy(stream->type, get_key(json, "type"));
    strcpy(stream->user_id, get_key(json, "user_id"));
    strcpy(stream->user_login, get_key(json, "user_login"));
    strcpy(stream->user_name, get_key(json, "user_name"));
    stream->viewer_count = atoi(get_key(json, "viewer_count"));
    strcpy(stream->type, get_key(json, "type"));
    // need to replace width/height with default twitch values
    cstr thumb = cstrInit(get_key(json, "thumbnail_url"));
    cstrReplace(thumb, "{width}x{height}", "344x194");
    strcpy(stream->thumbnail_url, thumb->string);
    cstrDealloc(thumb);
}

void get_stream_by_user_login(Client *client, TwitchStream *stream, const char *user_login) {
    cstr url = cstrInit(base_url);
    cstrCatFmt(url, "?user_login=%s", user_login);
    client_reset_headers(client);
    Response *response = curl_request(client, url->string, curl_GET);
    get_json_array(response, "data");
    if (response->data_len > 0) {
        json_object *data_array_object;
        data_array_object = json_object_array_get_idx(response->data, 0);
        __stream_init_from_json(stream, data_array_object);
    }
    response_clean(response);
}

int get_followed_streams(Client *client, TwitchStream **follows, int count) {
    Response *response;
    cstr url = client->__url;
    cstrUpdateString(url, base_url);
    cstrCatFmt(url, "/followed?user_id=%s", client->user_id);
    response = curl_request(client, url->string, curl_GET);
    get_json_array(response, "data");
    *follows = calloc(response->data_len, sizeof(TwitchStream));
    int ret = response->data_len;

    for (int i = 0; i < response->data_len; i++) {
        TwitchStream stream;
        json_object *data_array_object = json_object_array_get_idx(response->data, i);
        __stream_init_from_json(&stream, data_array_object);
        (*follows)[i] = stream;
    }
    response_clean(response);
    return ret;
}
