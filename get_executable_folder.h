#ifndef get_executable_folder_h
#define get_executable_folder_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
#else
    #include <unistd.h>
    #include <limits.h>
#endif

char *get_executable_folder(void) {
    char path[4096];
    char *folder;
    ssize_t len = 0;

#if defined(_WIN32)
    GetModuleFileNameA(NULL, path, sizeof(path));
#elif defined(__APPLE__)
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) != 0)
        return strdup(".");  // fallback
    len = strlen(path);
#else
    len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (len == -1)
        return strdup(".");  // fallback
    path[len] = '\0';
#endif

    // Find the last slash or backslash
    char *last_slash = strrchr(path,
        #if defined(_WIN32)
            '\\'
        #else
            '/'
        #endif
    );

    if (last_slash)
        *last_slash = '\0'; // Remove executable name

    folder = strdup(path);  // allocate dynamically
    return folder;
}
#endif