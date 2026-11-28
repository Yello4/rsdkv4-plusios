#include "RetroEngine.hpp"

#if !RETRO_USE_ORIGINAL_CODE

#if RETRO_PLATFORM == RETRO_WIN
#include "Windows.h"
#endif

void parseArguments(int argc, char *argv[])
{
    for (int a = 0; a < argc; ++a) {
        const char *find = "";

        find = strstr(argv[a], "stage=");
        if (find) {
            int b = 0;
            int c = 6;
            while (find[c] && find[c] != ';') Engine.startSceneFolder[b++] = find[c++];
            Engine.startSceneFolder[b] = 0;
        }

        find = strstr(argv[a], "scene=");
        if (find) {
            int b = 0;
            int c = 6;
            while (find[c] && find[c] != ';') Engine.startSceneID[b++] = find[c++];
            Engine.startSceneID[b] = 0;
        }

        find = strstr(argv[a], "console=true");
        if (find) {
            engineDebugMode       = true;
            Engine.devMenu        = true;
            Engine.consoleEnabled = true;
#if RETRO_PLATFORM == RETRO_WIN
            AllocConsole();
            freopen_s((FILE **)stdin, "CONIN$", "w", stdin);
            freopen_s((FILE **)stdout, "CONOUT$", "w", stdout);
            freopen_s((FILE **)stderr, "CONOUT$", "w", stderr);
#endif
        }

        find = strstr(argv[a], "usingCWD=true");
        if (find) {
            usingCWD = true;
        }
    }
}
#endif

#if RETRO_PLATFORM == RETRO_iOS
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>

const char *getDocumentsPath()
{
    static char documentsPath[0x200] = { 0 };
    if (!documentsPath[0]) {
        // $HOME is the app's sandbox container on iOS
        const char *home = getenv("HOME");
        snprintf(documentsPath, sizeof(documentsPath), "%s/Documents", home ? home : ".");
    }
    return documentsPath;
}

// ---------------------------------------------------------------------------------------------
// Documents/log.txt: always written on iOS (there is no console), so a user can send it in.
// Kept open for the whole run so a crash signal can still append a final line.
// ---------------------------------------------------------------------------------------------
static int iosLogFD          = -1;
static size_t iosLogBytes    = 0;
#define IOS_LOG_MAX_BYTES (2 * 1024 * 1024)

void iOSLogWrite(const char *text)
{
    if (iosLogFD < 0 || !text)
        return;
    size_t len = strlen(text);
    if (iosLogBytes + len > IOS_LOG_MAX_BYTES)
        return;
    write(iosLogFD, text, len);
    iosLogBytes += len;
    if (!len || text[len - 1] != '\n') {
        write(iosLogFD, "\n", 1);
        iosLogBytes++;
    }
}

void iOSLog(const char *msg, ...)
{
    char buffer[0x400];
    va_list args;
    va_start(args, msg);
    vsnprintf(buffer, sizeof(buffer), msg, args);
    va_end(args);
    iOSLogWrite(buffer);
}

static void iOSCrashHandler(int sig)
{
    if (iosLogFD >= 0) {
        const char *name = "unknown";
        switch (sig) {
            case SIGSEGV: name = "SIGSEGV (bad memory access)"; break;
            case SIGBUS: name = "SIGBUS (bad memory access)"; break;
            case SIGABRT: name = "SIGABRT (abort)"; break;
            case SIGILL: name = "SIGILL (illegal instruction)"; break;
            case SIGFPE: name = "SIGFPE (math error)"; break;
        }
        const char *a = "\n*** CRASH: the app received signal ";
        write(iosLogFD, a, strlen(a));
        write(iosLogFD, name, strlen(name));
        write(iosLogFD, " ***\n", 5);
        fsync(iosLogFD);
    }
    signal(sig, SIG_DFL);
    raise(sig);
}

static void iOSListDocuments()
{
    DIR *d = opendir(getDocumentsPath());
    if (!d) {
        iOSLog("Could not open the Documents folder '%s'", getDocumentsPath());
        return;
    }
    iOSLog("Files in the app folder (Files app > On My iPhone > RSDKv4+):");
    struct dirent *e;
    int count = 0;
    while ((e = readdir(d))) {
        if (e->d_name[0] == '.')
            continue;
        char full[0x400];
        snprintf(full, sizeof(full), "%s/%s", getDocumentsPath(), e->d_name);
        struct stat st;
        if (stat(full, &st) == 0 && S_ISDIR(st.st_mode))
            iOSLog("  [folder] %s", e->d_name);
        else
            iOSLog("  %s (%lld bytes)", e->d_name, stat(full, &st) == 0 ? (long long)st.st_size : -1LL);
        ++count;
    }
    if (!count)
        iOSLog("  (empty)");
    closedir(d);
}

void iOSInitLog()
{
    char path[0x400];
    snprintf(path, sizeof(path), "%s/log.txt", getDocumentsPath());
    iosLogFD = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    time_t now = time(NULL);
    char timeBuf[0x40];
    strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    iOSLog("RSDKv4+ iOS log - %s", timeBuf);
#ifdef DECOMP_VERSION
    iOSLog("Engine version: %s, RSDK revision %d", DECOMP_VERSION, (int)RSDK_REVISION);
#endif
    iOSLog("App folder: %s", getDocumentsPath());

    signal(SIGSEGV, iOSCrashHandler);
    signal(SIGBUS, iOSCrashHandler);
    signal(SIGABRT, iOSCrashHandler);
    signal(SIGILL, iOSCrashHandler);
    signal(SIGFPE, iOSCrashHandler);

    iOSListDocuments();
}

// Shows a popup. SDL needs its video subsystem for this on iOS.
void iOSShowMessage(const char *title, const char *message)
{
    iOSLog("[popup] %s: %s", title, message);
    if (!SDL_WasInit(SDL_INIT_VIDEO))
        SDL_InitSubSystem(SDL_INIT_VIDEO);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, Engine.window);
}

static bool endsWithNoCase(const char *str, const char *suffix)
{
    size_t a = strlen(str), b = strlen(suffix);
    return a >= b && strcasecmp(str + a - b, suffix) == 0;
}

// Reads the first bytes of a file and tells what kind of data pack it is.
// 0 = can't open, 1 = RSDKv4 pack ("RSDKvB"), 2 = RSDKv5 pack (Sonic Origins / Mania), 3 = something else
int iOSCheckDataPack(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f)
        return 0;
    char sig[6] = { 0 };
    size_t n    = fread(sig, 1, 6, f);
    fclose(f);
    if (n == 6 && memcmp(sig, "RSDKvB", 6) == 0)
        return 1;
    if (n >= 5 && memcmp(sig, "RSDKv", 5) == 0)
        return 2;
    return 3;
}

// Looks for the data pack in the app folder (cwd = Documents), ignoring upper/lower case.
// Tries the configured name first (Data.rsdk), then any single *.rsdk file, then one folder level down.
// Writes a path relative to Documents into 'out'. Returns true if a file was found.
bool iOSFindDataPack(const char *wantedName, char *out, size_t outSize)
{
    const char *docs = getDocumentsPath();
    char foundAny[0x100] = { 0 };
    int rsdkCount        = 0;

    DIR *d = opendir(docs);
    if (d) {
        struct dirent *e;
        while ((e = readdir(d))) {
            if (e->d_name[0] == '.')
                continue;
            if (strcasecmp(e->d_name, wantedName) == 0) {
                snprintf(out, outSize, "%s", e->d_name);
                closedir(d);
                return true;
            }
            if (endsWithNoCase(e->d_name, ".rsdk")) {
                if (!rsdkCount)
                    snprintf(foundAny, sizeof(foundAny), "%s", e->d_name);
                ++rsdkCount;
            }
        }
        closedir(d);
    }

    // "Data.rsdk.zip" style downloads, or an extra extension the Files app added
    d = opendir(docs);
    if (d) {
        struct dirent *e;
        size_t wl = strlen(wantedName);
        while ((e = readdir(d))) {
            if (strncasecmp(e->d_name, wantedName, wl) == 0 && e->d_name[wl] == '.' && !endsWithNoCase(e->d_name, ".zip")) {
                char full[0x400];
                snprintf(full, sizeof(full), "%s/%s", docs, e->d_name);
                if (iOSCheckDataPack(full) == 1) {
                    snprintf(out, outSize, "%s", e->d_name);
                    closedir(d);
                    return true;
                }
            }
        }
        closedir(d);
    }

    if (rsdkCount == 1 && StrComp(wantedName, "Data.rsdk")) {
        snprintf(out, outSize, "%s", foundAny);
        return true;
    }

    // one folder down, e.g. "On My iPhone/RSDKv4+/Sonic 1/Data.rsdk"
    d = opendir(docs);
    if (d) {
        struct dirent *e;
        while ((e = readdir(d))) {
            if (e->d_name[0] == '.' || !strcasecmp(e->d_name, "mods") || !strcasecmp(e->d_name, "Data") || !strcasecmp(e->d_name, "Scripts"))
                continue;
            char sub[0x400];
            snprintf(sub, sizeof(sub), "%s/%s", docs, e->d_name);
            DIR *sd = opendir(sub);
            if (!sd)
                continue;
            struct dirent *se;
            while ((se = readdir(sd))) {
                if (strcasecmp(se->d_name, wantedName) == 0) {
                    snprintf(out, outSize, "%s/%s", e->d_name, se->d_name);
                    closedir(sd);
                    closedir(d);
                    return true;
                }
            }
            closedir(sd);
        }
        closedir(d);
    }
    return false;
}

// True if a folder exists in the app folder (case-insensitive), e.g. "Scripts" or "Data/Scripts".
bool iOSFolderExists(const char *relPath)
{
    char resolved[0x400];
    snprintf(resolved, sizeof(resolved), "%s", getDocumentsPath());
    char tmp[0x200];
    snprintf(tmp, sizeof(tmp), "%s", relPath);
    char *save = NULL;
    for (char *part = strtok_r(tmp, "/", &save); part; part = strtok_r(NULL, "/", &save)) {
        DIR *d = opendir(resolved);
        if (!d)
            return false;
        bool found = false;
        struct dirent *e;
        while ((e = readdir(d))) {
            if (strcasecmp(e->d_name, part) == 0) {
                size_t l = strlen(resolved);
                snprintf(resolved + l, sizeof(resolved) - l, "/%s", e->d_name);
                found = true;
                break;
            }
        }
        closedir(d);
        if (!found)
            return false;
    }
    struct stat st;
    return stat(resolved, &st) == 0 && S_ISDIR(st.st_mode);
}
#endif

#if RETRO_PLATFORM == RETRO_SWITCH
#include <switch.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <arpa/inet.h>
#include <unistd.h>

static int s_nxlinkSock = -1;

static void initNxLink()
{
    if (R_FAILED(socketInitializeDefault()))
        return;

    s_nxlinkSock = nxlinkStdio();
    if (s_nxlinkSock >= 0)
        printf("printf output now goes to nxlink server\n");
    else
        socketExit();
}
#endif

int main(int argc, char *argv[])
{
#if RETRO_PLATFORM == RETRO_SWITCH
    initNxLink();
#endif

#if RETRO_PLATFORM == RETRO_iOS
    // Work out of the app's Documents folder so relative paths (Data.rsdk, settings.ini, saves, mods) resolve there.
    // With UIFileSharingEnabled this folder shows up in the Files app under "On My iPhone".
    mkdir(getDocumentsPath(), 0755);
    int chdirResult = chdir(getDocumentsPath());
    iOSInitLog();
    if (chdirResult != 0)
        iOSLog("WARNING: could not change into the app folder");

    // Landscape only, and don't turn the phone's motion sensor into a "controller" (that would hide the touch controls)
    SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
    SDL_SetHint(SDL_HINT_ACCELEROMETER_AS_JOYSTICK, "0");
    SDL_SetHint(SDL_HINT_IOS_HIDE_HOME_INDICATOR, "2");
    // touches are handled as touches; fake mouse clicks made from them would fight the multi-touch controls
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
#endif

#if !RETRO_USE_ORIGINAL_CODE
    parseArguments(argc, argv);
#endif

    SDL_SetHint(SDL_HINT_WINRT_HANDLE_BACK_BUTTON, "1");
    Engine.Init();
#if RETRO_PLATFORM == RETRO_iOS
    iOSLog("Init finished, running = %s", Engine.running ? "yes" : "no");
#endif
    Engine.Run();
#if RETRO_PLATFORM == RETRO_iOS
    iOSLog("Engine stopped, app is exiting");
#endif

#if !RETRO_USE_ORIGINAL_CODE
    if (Engine.consoleEnabled) {
#if RETRO_PLATFORM == RETRO_WIN
        FreeConsole();
#endif //! RETRO_PLATFORM == RETRO_WIN
    }
#endif //! !RETRO_USE_ORIGINAL_CODE

#if RETRO_PLATFORM == RETRO_SWITCH
    socketExit();
#endif //! NXLINK

    return 0;
}

#if RETRO_PLATFORM == RETRO_UWP
int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) { return SDL_WinRTRunApp(main, NULL); }
#endif
