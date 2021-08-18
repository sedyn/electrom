#ifndef ANDROID_APP_LOG_H
#define ANDROID_APP_LOG_H

int start_redirecting_stdout_stderr();

int log(int prio, const char *text);

#endif //ANDROID_APP_LOG_H
