#ifndef ANDROID_APP_LOG_H
#define ANDROID_APP_LOG_H

int start_redirecting_stdout_stderr();

int log(int prio, const char *text);

#define LOG_INFO(text) log(5, text);

#endif //ANDROID_APP_LOG_H
