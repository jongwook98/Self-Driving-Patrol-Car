#include <sdpc/common/sdpc_camera.h>
#include <sdpc/common/sdpc_common.h>
#include <sdpc/common/sdpc_lidar.h>
#include <sdpc/common/sdpc_main.h>
#include <sdpc/common/sdpc_msgque.h>
#include <sdpc/common/sdpc_uart.h>
#include <sdpc/lane_recognition/sdpc_lane.h>
#include <sdpc/vehicle_recognition/sdpc_vehicle.h>

static int g_escape_flag = 1;

static void _sdpc_ctrl_c(int signo)
{
    (void)signo;

    ERR_MSG("Input the \"Ctrl+C\".");
    g_escape_flag = 0;
}

int sdpc_main_init(void)
{
    /* Only Root! */
    FORMULA_GUARD(check_permission(NULL) < 0, -EPERM, "");

    FORMULA_GUARD(sdpc_camera_init() < 0, -EPERM, "");
    FORMULA_GUARD(sdpc_lidar_init() < 0, -EPERM, "");
    FORMULA_GUARD(sdpc_uart_init() < 0, -EPERM, "");
    FORMULA_GUARD(sdpc_message_queue_init() < 0, -EPERM, "");

    FORMULA_GUARD(sdpc_lane_init() < 0, -EPERM, "");
    FORMULA_GUARD(sdpc_vehicle_init() < 0, -EPERM, "");

    return 0;
}

int sdpc_main_exit(void)
{
    sdpc_vehicle_exit();
    sdpc_lane_exit();

    sdpc_message_queue_exit();
    sdpc_uart_exit();
    sdpc_lidar_exit();
    sdpc_camera_exit();

    return 0;
}

#ifdef _STANDALONE_VERSION
int main(void)
{
    struct sigaction sig_act = {
        .sa_handler = _sdpc_ctrl_c,
        .sa_flags = 0,
    };
    sigemptyset(&sig_act.sa_mask);
    sigaction(SIGINT, &sig_act, NULL);

    int ret = sdpc_main_init();
    FORMULA_GUARD_WITH_EXIT_FUNC(ret < 0, -EPERM, sdpc_main_exit(), "");

    while (g_escape_flag)
    {
        sleep(1);
    }

    sdpc_main_exit();
    return 0;
}
#endif
