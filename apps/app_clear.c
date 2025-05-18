#include "../kernel/video.h"

void app_clear(const char* args) {
    (void)args;
    video_clear();
}
