#include "../kernel/video.h"

void app_help(const char* args) {
    (void)args;
    video_print("Dostepne komendy:\n");
    video_print(" - help     : pokazuje to gowno\n");
    video_print(" - echo     : wypierdziela tekst\n");
    video_print(" - clear    : czysci ekran\n");
    video_print(" - test     : testowa apka do debugowania\n");
}
