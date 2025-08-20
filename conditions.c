#include "famine.h"

int check_user(const char *expected_user) {
    return strcmp(getenv("USER"), expected_user);
}

int check_time() {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    if (tm->tm_sec % 3 == 0)
        return 1;
    return 0;
}
