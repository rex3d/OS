#include "../kernel/video.h"
#include "../kernel/utils.h"

void app_echo(const char* args) {
    if (args == 0 || *args == 0) {
        video_print("Uzycie: echo <tekst>\n");
        return;
    }
    video_print(args);
    video_print("\n");
}
