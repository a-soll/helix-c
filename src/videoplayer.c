//
//  videoplayer.c
//  native_twitch
//
//  Created by Adam Solloway on 3/17/22.
//

#include "videoplayer.h"
#include <string.h>

// convert the token to an html string by replacing " with %22
static void token_encode(VideoToken *token) {
    size_t len = strlen(token->value) + 1;
    int i = 0;
    int j = 0;

    while (i < len - 1) {
        if (token->value[i] == '"') {
            token->encoded_value[j] = '%';
            j++;
            token->encoded_value[j] = '2';
            j++;
            token->encoded_value[j] = '2';
            j++;
        } else {
            token->encoded_value[j] = token->value[i];
            j++;
        }
        i++;
    }
    token->encoded_value[j] = '\0';
}

static void get_video_token(Client *client, Video *player, TwitchStream *stream) {
    Response *response;
    const char *url = "https://gql.twitch.tv/gql";
    struct curl_slist *headerlist = NULL;
    client_clear_headers(client);
    client->headers = curl_slist_append(client->headers, "Client-ID: kimne78kx3ncx6brgo4mv6wki5h1ko");

    int len = fmt_string(client->post_data, "{\
    \"operationName\": \"PlaybackAccessToken\",\
    \"extensions\": {\
        \"persistedQuery\": {\
        \"version\": 1,\
        \"sha256Hash\": \"0828119ded1c13477966434e15800ff57ddacf13ba1911c129dc2200705b0712\"\
        }\
    },\
    \"variables\": {\
        \"isLive\": true,\
        \"login\": \"%s\",\
        \"isVod\": false,\
        \"vodID\": \"%s\",\
        \"playerType\": \"embed\"\
    }\
    }\0",
                         stream->user_login, stream->user_login);
    response = curl_request(client, url, curl_POST);
    response->data = json_object_object_get(response->response, "data");
    json_object *res = json_object_object_get(response->data, "streamPlaybackAccessToken");
    player->token.value = get_key(res, "value");
    player->token.signature = get_key(res, "signature");
    if (player->token.value != NULL) {
        token_encode(&player->token);
    }
    response_clean(response);
}

/**
 * parses the m3u8 output from the request
 * pass the m3u8 string, the value to populate,
 * and the chars to start search from and end on
 */
static void parse_video_token(char *buf, char *value, char start, char end) {
    int ind = 0;
    int val_ind = 0;
    while (buf[ind]) {
        if (buf[ind] == start) {
            ind++;
            while (buf[ind] != end) {
                value[val_ind] = buf[ind];
                ind++;
                val_ind++;
            }
            value[val_ind] = '\0';
            break;
        }
        ind++;
    }
}

static void parse_links(Video *video, char *data) {
    const char *https = "https";
    const char *name = "NAME";
    const char *resolution = "RESOLUTION";
    size_t https_len = strlen(https);
    size_t name_len = strlen(name);
    size_t resolution_len = strlen(resolution);
    int count = 0;
    char *tmp = data;

    while ((tmp = strstr(tmp, resolution))) {
        ++count;
        tmp = tmp + resolution_len;
    }

    int ind = 0;
    tmp = data;
    while (ind < count) {
        tmp = strstr(tmp, name);
        parse_video_token(tmp, video->resolution_list[ind].name, '"', '"');
        tmp = tmp + name_len;

        tmp = strstr(tmp, resolution);
        parse_video_token(tmp, video->resolution_list[ind].resolution, '=', ',');
        tmp = tmp + resolution_len;

        tmp = strstr(tmp, https);
        parse_video_token(tmp - 1, video->resolution_list[ind].link, '\n', '\n');
        tmp = tmp + https_len;
        ind++;
    }
}

static void adblock_url(Client *client, Video *video, const char *user_login) {
    char url[URL_LEN];
    fmt_string(url, "https://api.ttv.lol/playlist/%s.m3u8%%3Fallow_source=true?fast_bread=true", user_login);
    client_set_header(client, "X-Donate-To", "https://ttv.lol/donate");
    Response *response = curl_request(client, url, curl_GET);
    parse_links(video, response->memory);
    response_clean(response);
}

static void non_adblock_url(Client *client, Response *response, TwitchStream *stream, Video *player, bool is_vod) {
    get_video_token(client, player, stream);
    const char *vod_or_channel = player->vod;
    char url[URL_LEN];

    if (!is_vod) {
        vod_or_channel = player->channel;
    }

    int len = fmt_string(
        url, "https://usher.ttvnw.net/%s/%s.m3u8?client_id=%s&token=%s&sig=%s&allow_source=true&allow_audio_only=true&fast_bread=true",
        vod_or_channel, stream->user_login, "kimne78kx3ncx6brgo4mv6wki5h1ko", player->token.encoded_value,
        player->token.signature);
    response = curl_request(client, url, curl_GET);
    parse_links(player, response->memory);
}

// https://usher.ttvnw.net/${VOD|CHANNEL}/${user_id}.m3u8?client_id=${clientId}&token=${accessToken.value}&sig=${accessToken.signature}&allow_source=true&allow_audio_only=true
// accessToken.value = json from request
void get_stream_url(Client *client, TwitchStream *stream, Video *player, bool is_vod, bool use_adblock) {
    Response *response;
    client_clear_headers(client);
    bool adblock_available = true;

    if (use_adblock) {
        response = curl_request(client, "http://ttv.lol/api/ping", curl_GET);
        if (response->res != CURLE_OK) {
            adblock_available = false;
        }
        if (adblock_available) {
            adblock_url(client, player, stream->user_login);
        }
    }
    if (!use_adblock) {
        non_adblock_url(client, response, stream, player, is_vod);
    }
    response_clean(response);
}

Video init_video_player() {
    Video player;
    player.vod = "vod";
    player.channel = "api/channel/hls";
    return player;
}
