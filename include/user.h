#ifndef USER_H
#define USER_H

#include "client.h"
#include "defs.h"

typedef struct User {
    char id[ID_LEN];
    char login[USERNAME_LEN];
    char display_name[USERNAME_LEN];
    const char *type;
    const char *broadcaster_type;
    const char *description;
    char profile_image_url[URL_LEN];
    char offline_image_url[URL_LEN];
    int view_count;
    char created_at[STARTED_AT_LEN];
} User;

void get_user_by_id(Client *client, User *user, const char *id);
void get_user_by_login(Client *client, User *user, const char *login);

#endif /* USER_H */
