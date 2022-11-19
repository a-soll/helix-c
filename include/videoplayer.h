#ifndef videoplayer_h
#define videoplayer_h

#include "channel.h"
#include "client.h"
#include "util.h"

typedef struct VideoToken {
    char value[2048];
    char signature[85];
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

bool get_stream_url(Client *client, TwitchStream *stream, Video *player, bool is_vod, bool use_adblock);
void init_video_player(Video *player);

#endif /* videoplayer_h */
