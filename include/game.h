#include "client.h"
#include "channel.h"
#include "util.h"

typedef struct Game {
    char id[15];
    char name[250];
    char box_art_url[2048]; // pic size: 188x251
    Channel *channels;
} Game;

int get_top_games(Client *client, Game **games, Paginator *iterator, int items);
int get_game_streams(Client *client, Stream **streams, Game *from, Paginator *iterator, int items);
