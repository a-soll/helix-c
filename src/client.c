//
//  client.c
//  native_twitch
//
//  Created by Adam Solloway on 3/8/22.
//

#include "client.h"
#include "videoplayer.h"

void Client_init(Client *client, const char *access_token, const char *client_id, const char *user_id,
                 const char *user_login, const char *oauth) {
    strcpy(client->client_id, client_id);
    strcpy(client->token, access_token);
    strcpy(client->user_id, user_id);
    strcpy(client->user_login, user_login);
    strcpy(client->oauth, oauth);
    client->headers = NULL;
    client->curl_handle = NULL;
    char header[URL_LEN];
    client->__url = cstrInit("https://api.twitch.tv/helix");
    client_reset_headers(client);
}

void Client_deinit(Client *c) {
    curl_slist_free_all(c->headers);
}

bool validate_token(Client *client) {
    bool ret = false;
    const char *url = "https://id.twitch.tv/oauth2/validate";
    Response *response = curl_request(client, url, curl_GET);
    if (response->response_code == 200) {
        const char *login = get_key(response->response, "login");
        strcpy(client->user_login, login);
        const char *id = get_key(response->response, "user_id");
        strcpy(client->user_id, id);
        ret = true;
    }
    response_clean(response);
    return ret;
}

Response *curl_request(Client *client, const char *url, CurlMethod method) {
    Response *response = malloc(sizeof(Response));
    response->memory = malloc(sizeof(char) * 1);
    response->size = 0;
    client->curl_handle = curl_easy_init();
    response->error[0] = 0;
    response->data_len = 0;

    switch (method) {
    case curl_GET:
        curl_easy_setopt(client->curl_handle, CURLOPT_CUSTOMREQUEST, "GET");
        break;
    case curl_POST:
        curl_easy_setopt(client->curl_handle, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(client->curl_handle, CURLOPT_POSTFIELDS, client->post_data);
        break;
    case curl_DELETE:
        curl_easy_setopt(client->curl_handle, CURLOPT_CUSTOMREQUEST, "DELETE");
        curl_easy_setopt(client->curl_handle, CURLOPT_POSTFIELDS, client->post_data);
        break;
    case curl_PATCH:
        curl_easy_setopt(client->curl_handle, CURLOPT_CUSTOMREQUEST, "PATCH");
        curl_easy_setopt(client->curl_handle, CURLOPT_POSTFIELDS, client->post_data);
        break;
    default:
        curl_easy_setopt(client->curl_handle, CURLOPT_CUSTOMREQUEST, "GET");
    }

    curl_easy_setopt(client->curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(client->curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(client->curl_handle, CURLOPT_HTTPHEADER, client->headers);
    curl_easy_setopt(client->curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(client->curl_handle, CURLOPT_WRITEDATA, (void *)response);
    curl_easy_setopt(client->curl_handle, CURLOPT_ERRORBUFFER, response->error);
    curl_easy_setopt(client->curl_handle, CURLOPT_NOSIGNAL, 1L);

    // curl_easy_setopt(client->curl_handle, CURLOPT_VERBOSE, client->curl_handle);

    response->res = curl_easy_perform(client->curl_handle);
    if (response->res == CURLE_OK) {
        curl_easy_getinfo(client->curl_handle, CURLINFO_RESPONSE_CODE, &response->response_code);
    }
    if (response->memory) {
        response->response = json_tokener_parse(response->memory);
    }
    curl_easy_cleanup(client->curl_handle);
    return response;
}

void response_clean(void *response) {
    struct Response *res = (struct Response *)response;
    json_object_put(res->response);
    free(res->memory);
    free(res);
}

void client_clean_up(Client *client) {
    client_clear_headers(client);
    cstrDealloc(client->__url);
}

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct Response *mem = (struct Response *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL) {
        printf("error: not enough memory\n");
        return 0;
    }
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

void client_reset_headers(Client *client) {
    client_clear_headers(client);
    char header[URL_LEN];
    size_t len;

    client->headers = curl_slist_append(client->headers, "Content-Type: application/json");
    client->headers = curl_slist_append(client->headers, "Accept: application/json");

    fmt_string(header, URL_LEN, "Authorization: Bearer %s", client->token);
    client->headers = curl_slist_append(client->headers, header);

    fmt_string(header, URL_LEN, "Client-Id: %s", client->client_id);
    client->headers = curl_slist_append(client->headers, header);
}

void client_set_header(Client *client, const char *key, const char *value) {
    char header[URL_LEN];
    fmt_string(header, URL_LEN, "%s: %s", key, value);
    client->headers = curl_slist_append(client->headers, header);
}

void client_clear_headers(Client *client) {
    curl_slist_free_all(client->headers);
    client->headers = NULL;
}

void get_json_array(Response *response, const char *key) {
    if (response->response != NULL) {
        json_object_object_get_ex(response->response, key, &response->data);
        if (response->data != NULL) {
            response->data_len = json_object_array_length(response->data);
        }
    }
}

void paginator_set(char *pagination, json_object *json) {
    json_object *pagination_json;
    json_object_object_get_ex(json, "pagination", &pagination_json);
    const char *cursor = get_key(pagination_json, "cursor");
    if (cursor) {
        strcpy(pagination, cursor);
    }
}

void paginator_clear(Paginator *paginator) {
    paginator->pagination[0] = '\0';
}

Paginator paginator_init() {
    Paginator paginator;
    paginator.pagination[0] = '\0';
    return paginator;
}
