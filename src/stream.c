#include "stream.h"
#include "util.h"
#include <json-c/json.h>

static char *base_url = "https://api.twitch.tv/helix/streams";

int __populate_stream_array(Client *client, const char *url, Stream **streams, Paginator *iterator, int items) {
    int chan_index = items;
    Response *response = curl_request(client, url, curl_GET);
    Stream *s;
    get_json_array(response, "data");
    if (*streams == NULL) {
        s = malloc(sizeof(Stream) * response->data_len);
    } else {
        s = realloc(*streams, sizeof(Stream) * (items + response->data_len));
    }
    int ret = response->data_len;
    for (int i = 0; i < response->data_len; i++) {
        Stream str;
        json_object *data_array_object;
        data_array_object = json_object_array_get_idx(response->data, i);
        __stream_init_from_json(&str, data_array_object);
        s[chan_index] = str;
        chan_index++;
    }
    if (response->data_len > 0) {
        set_pagination(iterator->pagination, response->response);
    }
    *streams = s;
    clean_response(response);
    return ret;
}

void __stream_init_from_json(Stream *stream, json_object *json) {
    memccpy(stream->game_id, get_key(json, "game_id"), '\0', sizeof(stream->game_id));
    memccpy(stream->game_name, get_key(json, "game_name"), '\0', sizeof(stream->game_name));
    memccpy(stream->id, get_key(json, "id"), '\0', sizeof(stream->id));
    if (strcmp(get_key(json, "is_mature"), "true") == 0) {
        stream->is_mature = true;
    } else {
        stream->is_mature = false;
    }
    memccpy(stream->language, get_key(json, "language"), '\0', sizeof(stream->language));
    memccpy(stream->started_at, get_key(json, "started_at"), '\0', sizeof(stream->started_at));
    memccpy(stream->thumbnail_url, get_key(json, "thumbnail_url"), '\0', sizeof(stream->thumbnail_url));
    memccpy(stream->title, get_key(json, "title"), '\0', sizeof(stream->title));
    memccpy(stream->type, get_key(json, "type"), '\0', sizeof(stream->type));
    memccpy(stream->user_id, get_key(json, "user_id"), '\0', sizeof(stream->user_id));
    memccpy(stream->user_login, get_key(json, "user_login"), '\0', sizeof(stream->user_login));
    memccpy(stream->user_name, get_key(json, "user_name"), '\0', sizeof(stream->user_name));
    stream->viewer_count = atoi(get_key(json, "viewer_count"));
    replace_substr(stream->thumbnail_url, stream->thumbnail_url, "{width}x{height}", "344x194");
}

void get_stream_by_user_login(Client *client, Stream *stream, const char *user_login) {
    char url[URL_LEN];
    int len = fmt_string(url, "%s?user_login=%s", base_url, user_login);
    url[len] = '\0';
    Response *response = curl_request(client, url, curl_GET);
    get_json_array(response, "data");

    if (response->data_len > 0) {
        json_object *data_array_object;
        data_array_object = json_object_array_get_idx(response->data, 0);
        __stream_init_from_json(stream, data_array_object);
    }
    clean_response(response);
}

int get_followed_streams(Client *client, Stream **follows, int count) {
    Response *response;
    char url[URL_LEN];
    fmt_string(url, "%s/streams/followed?user_id=42045317", client->base_url);
    response = curl_request(client, url, curl_GET);
    get_json_array(response, "data");
    *follows = calloc(response->data_len, sizeof(Stream));

    for (int i = 0; i < response->data_len; i++) {
        Stream stream;
        json_object *data_array_object = json_object_array_get_idx(response->data, i);
        __stream_init_from_json(&stream, data_array_object);
        (*follows)[i] = stream;
    }
    clean_response(response);
    return response->data_len;
}
