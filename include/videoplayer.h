#ifndef videoplayer_h
#define videoplayer_h

#include "channel.h"
#include "client.h"
#include "util.h"

typedef struct VideoToken {
    const char *value;
    const char *signature;
    char encoded_value[2048];
} VideoToken;

typedef struct Resolution {
    char name[25];
    char resolution[15];
    char link[URL_LEN];
} Resolution;

typedef struct Video {
    VideoToken token;
    const char *token_url;
    const char *vod;
    const char *channel;
    Resolution resolution_list[6];
} Video;

void get_stream_url(Client *client, TwitchStream *stream, Video *player, bool is_vod);
void get_video_token(Client *client, Video *player, TwitchStream *stream);
Video init_video_player();
void token_encode(VideoToken *token);
void parse_links(Video *video, char *data);

#endif /* videoplayer_h */
