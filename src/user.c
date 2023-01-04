#include "user.h"
#include "util.h"

static char *base_url = "https://api.twitch.tv/helix/users";

void __user_init_from_json(User *user, json_object *json) {
    memccpy(user->id, get_key(json, "id"), '\0', sizeof(user->id));
    user->broadcaster_type = get_key(json, "broadcaster_type");
    memccpy(user->login, get_key(json, "login"), '\0', sizeof(user->login));
    memccpy(user->display_name, get_key(json, "display_name"), '\0', sizeof(user->display_name));
    user->description = get_key(json, "description");
    memccpy(user->profile_image_url, get_key(json, "profile_image_url"), '\0', sizeof(user->profile_image_url));
    memccpy(user->offline_image_url, get_key(json, "offline_image_url"), '\0', sizeof(user->offline_image_url));
    user->view_count = atoi(get_key(json, "view_count"));
    memccpy(user->created_at, get_key(json, "created_at"), '\0', sizeof(user->created_at));
}

void get_user_by_id(Client *client, User *user, const char *id) {
    cstr url = client->__url;
    cstrUpdateString(url, base_url);
    cstrCatFmt(url, "?id=%s", id);
    Response *response = curl_request(client, url->string, curl_GET);
    get_json_array(response, "data");
    if (response->data_len > 0) {
        json_object *data_array_object = json_object_array_get_idx(response->data, 0);
        __user_init_from_json(user, data_array_object);
    }
    response_clean(response);
}

void get_user_by_login(Client *client, User *user, const char *login) {
    cstr url = client->__url;
    cstrUpdateString(url, base_url);
    cstrCatFmt(url, "?login=%s", login);
    Response *response = curl_request(client, url->string, curl_GET);
    get_json_array(response, "data");
    if (response->data_len > 0) {
        json_object *data_array_object = json_object_array_get_idx(response->data, 0);
        __user_init_from_json(user, data_array_object);
    }
    response_clean(response);
}
