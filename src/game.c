#include "game.h"
#include "stream.h"
#include <cstr/cstr.h>

// example box art url: https://static-cdn.jtvnw.net/ttv-boxart/1678360288_IGDB-{width}x{height}.jpg
// placeholder https://static-cdn.jtvnw.net/ttv-static/404_boxart.jpg
void init_game(Game *game, json_object *json) {
    const char *name = get_key(json, "name");
    const char *id = get_key(json, "id");
    memcpy(game->name, name, strlen(name) + 1);
    memcpy(game->id, id, strlen(id) + 1);
    cstr box_url = cstrInit(get_key(json, "box_art_url"));
    cstrReplace(box_url, "{width}x{height}", "188x251");
    memcpy(game->box_art_url, box_url->string, box_url->len + 1);
    cstrDealloc(box_url);
}

void get_top_games(Client *client, GameList *games) {
    int game_index = games->len;
    cstr url = client->__url;
    cstrUpdateString(url, "https://api.twitch.tv/helix/games/top?first=100");

    if (games->iterator.pagination[0] != '\0') {
        cstrCatFmt(url, "&after=%s", games->iterator.pagination);
    }
    Response *response = curl_request(client, url->string, curl_GET);
    get_json_array(response, "data");
    if (!games->games) {
        games->games = malloc(sizeof(Game) * response->data_len);
    } else {
        games->games = realloc(games->games, sizeof(Game) * (games->len + response->data_len));
    }
    for (int i = 0; i < response->data_len; i++) {
        Game game;
        json_object *data_array_object;
        data_array_object = json_object_array_get_idx(response->data, i);
        init_game(&game, data_array_object);
        games->games[game_index] = game;
        game_index++;
    }
    games->len += response->data_len;
    paginator_set(games->iterator.pagination, response->response);
    response_clean(response);
}

int get_game_streams(Client *client, TwitchStream **streams, Game *from, Paginator *iterator, int items) {
    Channel *c;
    cstr url = client->__url;
    cstrUpdateString(url, "https://api.twitch.tv/helix/streams?first=100");

    if (iterator->pagination[0] == '\0') {
        cstrCatFmt(url, "&game_id=%s", from->id);
    } else {
        cstrCatFmt(url, "&after=%s&game_id=%s", iterator->pagination, from->id);
    }
    int ret = __populate_stream_array(client, url->string, streams, iterator, items);
    return ret;
}

void GameList_init(GameList *games, const char *query) {
    games->iterator = paginator_init();
    games->games = NULL;
    games->len = 0;
    if (query) {
        games->query = strdup(query);
    }
}

void GameList_deinit(GameList *games) {
    if (games->games) {
        free(games->games);
    }
    free(games->query);
}

void get_game_by_name(Client *client, const char *name, Game *game) {
    cstr url = client->__url;
    cstrUpdateString(url, "https://api.twitch.tv/helix/games");
    cstrCatFmt(url, "?name=%s", name);
    cstrReplace(url, " ", "%20");
    Response *response = curl_request(client, url->string, curl_GET);
    get_json_array(response, "data");
    json_object *json = json_object_array_get_idx(response->data, 0);
    init_game(game, json);
    response_clean(response);
}

void search_games(Client *client, GameList *games) {
    cstr url = client->__url;
    int game_index = games->len;
    cstrUpdateString(url, "https://api.twitch.tv/helix/search/categories?first=100");
    if (games->iterator.pagination[0] != '\0') {
        cstrCatFmt(url, "&after=%s", games->iterator.pagination);
    }
    cstrCatFmt(url, "&query=%s", games->query);
    cstrReplace(url, " ", "%20");
    Response *response = curl_request(client, url->string, curl_GET);
    get_json_array(response, "data");
    if (!games->games) {
        games->games = malloc(sizeof(Game) * response->data_len);
    } else {
        games->games = realloc(games->games, sizeof(Game) * (response->data_len + games->len));
    }
    for (int i = 0; i < response->data_len; i++) {
        Game game;
        json_object *data_array_object;
        data_array_object = json_object_array_get_idx(response->data, i);
        const char *name = get_key(data_array_object, "name");
        const char *id = get_key(data_array_object, "id");
        const char *box_art_url = get_key(data_array_object, "box_art_url");
        memcpy(game.name, name, strlen(name) + 1);
        memcpy(game.id, id, strlen(id) + 1);
        memcpy(game.box_art_url, box_art_url, strlen(box_art_url) + 1);
        games->games[game_index] = game;
        game_index++;
    }
    games->len += response->data_len;
    paginator_set(games->iterator.pagination, response->response);
    response_clean(response);
}
