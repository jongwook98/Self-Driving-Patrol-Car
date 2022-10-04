/* Copyright 2022. Kim, Jinseong all rights reserved */

#include <lane_detection/common/common.h>
#include <lane_detection/lidar/lidar.h>

#define LIDAR_BOUDRATE 115200
#define LIDAR_OPT_CHANNEL "/dev/ttyUSB0"
#define LIDAR_PATH "/lidar_data_to_core"

#define LIDAR_MQ_SIZE sizeof(uint8_t) * 45

Lidar::Lidar()
    : mq(std::make_unique<MessageQueue>(LIDAR_PATH, LIDAR_MQ_SIZE)) {}

sl::ILidarDriver* Lidar::Init() {
    sl::ILidarDriver * drv = *sl::createLidarDriver();
    if (!drv) {
        fprintf(stderr, "insufficent memory, exit\n");
        exit(2);
    }
    const char * opt_channel = LIDAR_OPT_CHANNEL;
    sl_u32 baudrate = LIDAR_BOUDRATE;
    sl_result op_result;
    sl::IChannel * _channel;

    sl_lidar_response_device_info_t devinfo;

    _channel = (*sl::createSerialPortChannel(opt_channel, baudrate));
    if (SL_IS_OK((drv)->connect(_channel))) {
        op_result = drv->getDeviceInfo(devinfo);

        if (!(SL_IS_OK(op_result))) {
            Lidar::Exit(drv);
            fprintf(stderr, "lidar isn't connected, exit\n");
            exit(2);
        }
    }
    return drv;
}

void Lidar::Exit(sl::ILidarDriver * drv) {
    delete drv;
    drv = NULL;
}

void Lidar::Operate(sl::ILidarDriver * drv, struct Lidar::obstacle * ob) {
    sl_lidar_response_measurement_node_hq_t nodes[8192];
    size_t count = _countof(nodes);

    sl_result op_result = drv->grabScanDataHq(nodes, count);

    if (!(SL_IS_OK(op_result))) {
    Lidar::Exit(drv);
    fprintf(stderr, "lidar don't operated, exit\n");
    exit(2);
    }

    drv->ascendScanData(nodes, count);
    int ob_ID = 0;
    float pre_angle = -1;

    float raw_dis;
    float raw_angle;

    memset(ob, 0, sizeof(struct Lidar::obstacle));

    for (int pos = 0; pos < static_cast<int>(count); ++pos) {
        if (nodes[pos].dist_mm_q2/4.0f > min_dis_mm &&
            nodes[pos].dist_mm_q2/4.0f < max_dis_mm) {
            raw_dis = nodes[pos].dist_mm_q2;
            raw_angle = nodes[pos].angle_z_q14;
            if (raw_angle < left_end || (raw_angle > right_start &&
                 raw_angle < right_end)) {
                if (pre_angle == -1) {
                    ob[ob_ID].min_dis = raw_dis;
                    ob[ob_ID].s_data.min_dis = static_cast<uint16_t>(
                        raw_dis/4.0f);

                    ob[ob_ID].start_angle = raw_angle;
                    pre_angle = raw_angle;
                } else if (pre_angle - raw_angle <= same_ob &&
                    pre_angle - raw_angle >= -same_ob) {
                        if (ob[ob_ID].min_dis > raw_dis) {
                            ob[ob_ID].min_dis = raw_dis;
                            ob[ob_ID].s_data.min_dis = static_cast<uint16_t>(
                                raw_dis/4.0f);

                            ob[ob_ID].min_angle = raw_angle;
                            ob[ob_ID].s_data.min_angle = 360 - static_cast<
                                uint16_t>(raw_angle*90.f/16384.f);
                        }

                    ob[ob_ID].end_angle = raw_angle;
                    pre_angle = raw_angle;
                } else {
                    ob_ID += 1;
                    ob[ob_ID].start_angle = raw_angle;
                    ob[ob_ID].min_dis = raw_dis;
                    ob[ob_ID].s_data.min_dis = static_cast<uint16_t>(
                        raw_dis/4.0f);

                    ob[ob_ID].min_angle = raw_angle;
                    ob[ob_ID].s_data.min_angle = 360 - static_cast<uint16_t>(
                        raw_angle*90.f/16384.f);
                    pre_angle = raw_angle;
                }
                if (ob_ID >= 9) {
                    break;
                }
            } else if (raw_dis/4.0f > min_dis_mm && raw_dis/4.0f < max_dis_mm) {
                if (pre_angle != -1) {
                    ob_ID += 1;
                    pre_angle = -1;
                }
            }
        }

        if (ob[0].start_angle <= same_ob_start &&
            ob[ob_ID].end_angle >= same_ob_opposite) {
            ob[0].start_angle = ob[ob_ID].start_angle;
            if (ob[0].min_dis > ob[ob_ID].min_dis) {
                ob[0].min_dis = ob[ob_ID].min_dis;
                ob[0].min_angle = ob[ob_ID].min_angle;
            }
        ob_ID -= 1;
        }
        Lidar::Publisher(ob_ID, ob);
        Lidar::SendMQ(ob_ID, ob);
    }
}

int Lidar::Publisher(int number, struct Lidar::obstacle * ob) {
    char num[3];
    char info[31];
    char buf[512] = { 0, };

    for (int pos = 0; pos <= number; pos++) {
        snprintf(info, sizeof(info), ",angle : %06.2f dis : %07.2f ",
                ob[pos].min_angle*90.f/16384.f, ob[pos].min_dis/4.0f);

        if (pos == 0) {
            std::string tmp = std::to_string(number);
            char const *num_char = tmp.c_str();

            snprintf(buf, sizeof(num), "%s", num_char);
        }
        snprintf(buf + (pos * (sizeof(info) -1)) + 1, sizeof(info), "%s", info);
    }

    std::cout << buf << std::endl;
    return 1;
}

int Lidar::SendMQ(int number, struct Lidar::obstacle *ob) {
    uint8_t data[42] = {0, };

    data[1] = static_cast<uint8_t>(number);

    for (int pos = 0; pos <= number; pos++) {
        uint16_t angle = ob[pos].s_data.min_angle;
        uint16_t dis = ob[pos].s_data.min_dis;

        data[2 + pos*2] = (angle & 0xff);
        data[3 + pos*2] = (angle >> 8 & 0xff);

        data[22 + pos*2] = (dis & 0xff);
        data[23 + pos*2] = (dis >> 8 & 0xff);
    }

    struct message_q lidar_mq;
    mq->BuildMessage(reinterpret_cast<uint8_t *>(&lidar_mq), MqMode::LIDAR,
                    data, sizeof(lidar_mq), sizeof(data));

    mq->Send(reinterpret_cast<const char *>(&lidar_mq), sizeof(lidar_mq));
    return 0;
}

void *Lidar::Run(void *arg) {
    sl::ILidarDriver * drv = Lidar::Init();
    struct Lidar::obstacle * ob = new struct Lidar::obstacle[10]{};

    drv->setMotorSpeed();
    drv->startScan(0, 1);

    while (status) {
        DEBUG_MSG("[LIDAR] in Thread!");
        Lidar::Operate(drv, ob);
    }

    delete[] ob;
    Lidar::Exit(drv);

    return nullptr;
}
