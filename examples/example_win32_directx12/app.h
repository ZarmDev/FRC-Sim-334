#ifndef APP_H
#define APP_H

enum windows {
    Demo,
    Test
};

void run_app(windows currentWindow);
void cleanup();

#endif // APP_H
