/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#ifndef MQTT_H
#define MQTT_H

#include <mosquitto.h>

#include <map>
#include <memory>
#include <mutex>
#include <string>

#define DEFALUT_PORT 1883

#define CAMERA_PUB_TOPIC "[MQTT] Camera"
#define LANE_DETECT_PUB_TOPIC "[MQTT] Lane Detect"
#define TRAFFIC_LIGHT_PUB_TOPIC "[MQTT] Traffic Light"

#define CAMERA_SUB_TOPIC TRAFFIC_LIGHT_PUB_TOPIC
#define LANE_DETECT_SUB_TOPIC CAMERA_PUB_TOPIC
#define TRAFFIC_LIGHT_SUB_TOPIC LANE_DETECT_PUB_TOPIC

class Mqtt {
public: // NOLINT
  Mqtt() = delete;
  Mqtt(const char *id, const int port, void *arg);
  virtual ~Mqtt();

  int Publish(const std::string topic, const void *send_msg, std::size_t len);
  int Subscribe(const std::string topic, void *read_buf, std::size_t len);

private: // NOLINT
  typedef struct MqttData {
    std::string topic;
    void *read_buf;
    std::size_t len;
  } mqtt_data_t;

  int port;
  const char *id;
  struct mosquitto *mqtt;
  std::shared_ptr<mqtt_data_t> mqtt_data;

  static inline std::mutex internal_lock;
  static inline std::map<std::string, std::shared_ptr<mqtt_data_t>> sub_data;
  static void OnMessage(struct mosquitto *mqtt, void *arg,
                        const struct mosquitto_message *msg);
};

#endif /* MQTT_H */
