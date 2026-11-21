#include "engine_adapter.h"
#include "game/game.h"
#ifndef __PSP__
#include "game/flow.h"
#endif
#include "host.h"
#include "mem.h"
#include "modules.h"
#include "platform/ega.h"
#include "platform/platform.h"
#include "port_platform.h"
#include "port_keyboard.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef __PSP__
#include <pspkernel.h>
PSP_MODULE_INFO("Street Rod", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(4096);
#endif

const char *sr_save_dir = "Save";

static void finish(void)
{
    host_shutdown();
#ifdef __PSP__
    sceKernelExitGame();
#endif
}

static int startup_error(const char *message)
{
    fprintf(stderr, "Street Rod: %s\n", message);
#ifdef __PSP__
    static uint8_t frame[320 * 200];
    const PortConfig config = {"Street Rod", 320, 200, true, ".", false};
    if (port_platform_init(&config)) {
        port_keyboard_draw_text(frame, 320, 12, 42, "STREET ROD COULD NOT START", 3);
        port_keyboard_draw_text(frame, 320, 12, 62, message, 3);
        port_keyboard_draw_text(frame, 320, 12, 86, "CIRCLE BACK", 3);
        port_platform_present_indexed(frame, 320, 200, 320, port_keyboard_palette(), 4);
        PortInput input;
        while (port_platform_poll_input(&input) && !input.quit && !(input.pressed & PORT_ALT))
            port_platform_sleep_us(10000);
        port_platform_shutdown();
    }
#endif
    return 1;
}

int main(int argc, char **argv)
{
    const char *game_dir = "Game";
    char error[256];
    char *game_argv[10] = {"Street Rod"};
    int game_argc = 1;
    atexit(finish);
#ifdef __PSP__
    char root[768];
    if (argc > 0 && argv[0] && strlen(argv[0]) < sizeof(root)) {
        strcpy(root, argv[0]);
        char *slash = strrchr(root, '/');
        if (slash) {
            *slash = 0;
            if (chdir(root) != 0)
                return startup_error("COULD NOT OPEN THE EBOOT FOLDER");
        }
    }
#else
    bool test_ending = false;
    for (int arg = 1; arg < argc && game_argc < 9; ++arg) {
        if (!strcmp(argv[arg], "--game-dir") && arg + 1 < argc)
            game_dir = argv[++arg];
        else if (!strcmp(argv[arg], "--save-dir") && arg + 1 < argc)
            sr_save_dir = argv[++arg];
        else if (!strcmp(argv[arg], "--test-ending"))
            test_ending = true;
        else
            game_argv[game_argc++] = argv[arg];
    }
#endif
    struct stat save_info;
    if (mkdir(sr_save_dir, 0777) != 0 &&
        (stat(sr_save_dir, &save_info) != 0 || !S_ISDIR(save_info.st_mode)))
        return startup_error("COULD NOT CREATE THE SAVE FOLDER");

    char path[1024];
    const char *required[] = {"SR.EXE", "LIB1", "LIB2", "HOT_DATA"};
    for (unsigned file = 0; file < sizeof(required) / sizeof(*required); ++file) {
        int length = snprintf(path, sizeof(path), "%s/%s", game_dir, required[file]);
        FILE *stream = length > 0 && (size_t)length < sizeof(path) ? fopen(path, "rb") : NULL;
        if (!stream) {
            snprintf(error, sizeof(error), "MISSING GAME/%s", required[file]);
            return startup_error(error);
        }
        fclose(stream);
        if (file == 0 && !mem_load_exe(path, error, sizeof(error)))
            return startup_error(error);
    }
    if (!host_init(game_dir, 1, false))
        return startup_error("COULD NOT INITIALIZE VIDEO OR AUDIO");
    ega_init();
    modules_init();
    platform_main_init(game_argc, game_argv);
#ifndef __PSP__
    if (test_ending) {
        title_and_setup();
        strcpy(ds_str(DS_player_name), "TEST");
        new_game();
        ending_king();
        hall_of_fame_screen(2);
        puts("Street Rod ending and hall of fame completed");
        platform_exit();
    }
#endif
    return game_main();
}
