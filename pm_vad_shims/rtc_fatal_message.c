/* Minimal C stub so WebRTC VAD .c files can include rtc_base/checks.h without
 * linking Abseil or rtc_base/checks.cc. */
#include <stdio.h>
#include <stdlib.h>

void rtc_FatalMessage(const char* file, int line, const char* msg)
{
    fprintf(stderr, "FATAL %s:%d: %s\n", file ? file : "?", line, msg ? msg : "");
    fflush(stderr);
    abort();
}
