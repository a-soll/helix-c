#include "channel.h"
#include "client.h"
#include "util.h"

typedef struct Game {
    char id[15];
    char name[250];
    char box_art_url[2048]; // pic size: 188x251
    Channel *channels;
} Game;

// struct used for containing list of Games from Twitch queries
// contains query string to ensure consecutive queries are valid
typedef struct GameList {
    Paginator iterator;
    Game *games;
    const char *query;
    int len;
} GameList;

int get_top_games(Client *client, Game **games, Paginator *iterator, int items);
int get_game_streams(Client *client, TwitchStream **streams, Game *from, Paginator *iterator, int items);
void get_game_by_name(Client *client, char *name, Game *game);
void search_games(Client *client, GameList *games);
void GameList_init(GameList *games, const char *query);
void GameList_deinit(GameList *games);
