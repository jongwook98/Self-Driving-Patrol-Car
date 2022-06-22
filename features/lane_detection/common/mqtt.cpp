/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include <lane_detection/common/common.h>
#include <lane_detection/common/mqtt.h>

#define MQTT_HOST "localhost"

enum class Qos : uint8_t { AT_MOST_ONCE = 0, AT_LEAST_ONCE, EXACTLY_ONCE };

Mqtt::Mqtt(const char *id, const int port, void *arg)
    : port(port), id(id), mqtt(nullptr),
      mqtt_data(std::make_shared<mqtt_data_t>()) {

  /* init the mosquitto */
  int ret = mosquitto_lib_init();
  EXIT_GUARD(ret != MOSQ_ERR_SUCCESS, "[MQTT] lib_init in Constructor ", ret);

  /* alloc the mosquitto */
  mqtt = mosquitto_new(id, true, arg);
  EXIT_GUARD(mqtt == NULL, "[MQTT] new in Constructor", ret);

  mosquitto_message_callback_set(mqtt, &Mqtt::OnMessage);

  /* connect */
  ret = mosquitto_connect(mqtt, MQTT_HOST, port, 60);
  EXIT_GUARD(ret != MOSQ_ERR_SUCCESS, "[MQTT] connect in Constructor ", ret);

  /* checking the brocker */
  ret = mosquitto_loop_start(mqtt);
  EXIT_GUARD(ret != MOSQ_ERR_SUCCESS, "[MQTT] loop_start in Constructor ", ret);
}

Mqtt::~Mqtt() {
  if (mqtt) {
    mosquitto_disconnect(mqtt);
    mosquitto_loop_stop(mqtt, false);
    mosquitto_destroy(mqtt);
  }

  mqtt = nullptr;
  mosquitto_lib_cleanup();
}

int Mqtt::Publish(const std::string topic, const void *send_msg,
                  std::size_t len) {
  FORMULA_GUARD((send_msg && len == 0) || (send_msg == nullptr && len), -EPERM,
                ERR_INVALID_PTR);

  int ret =
      mosquitto_publish(mqtt, nullptr, topic.c_str(), static_cast<int>(len),
                        send_msg, static_cast<int>(Qos::EXACTLY_ONCE), false);
  FORMULA_GUARD(ret != MOSQ_ERR_SUCCESS, -EPERM,
                "Failed to publish the message ", ret);

  return 0;
}

int Mqtt::Subscribe(const std::string topic, void *read_buf, std::size_t len) {
  FORMULA_GUARD(read_buf == nullptr || len == 0, -EPERM, ERR_INVALID_PTR);

  /* lock */
  std::scoped_lock<std::mutex> lock(Mqtt::internal_lock);

  /* subscribe */
  int ret = mosquitto_subscribe(mqtt, nullptr, topic.c_str(),
                                static_cast<int>(Qos::EXACTLY_ONCE));
  FORMULA_GUARD(ret != MOSQ_ERR_SUCCESS, -EPERM,
                "Failed to subscribe the message ", ret);

  /* register the mqtt data */
  mqtt_data->topic = topic;
  mqtt_data->read_buf = read_buf;
  mqtt_data->len = len;
  Mqtt::sub_data[topic] = mqtt_data;

  return 0;
}

void Mqtt::OnMessage(struct mosquitto *mqtt, void *arg,
                     const struct mosquitto_message *msg) {
  FORMULA_GUARD(msg == nullptr || msg->topic == nullptr, , ERR_INVALID_PTR);

  (void)mqtt;
  (void)arg;

  bool status = false;
  std::shared_ptr<mqtt_data_t> mqtt_data = nullptr;

  /* lock */
  std::scoped_lock<std::mutex> lock(Mqtt::internal_lock);
  mqtt_data = Mqtt::sub_data[msg->topic];

  int ret = mosquitto_topic_matches_sub(mqtt_data->topic.c_str(), msg->topic,
                                        &status);
  FORMULA_GUARD(ret != MOSQ_ERR_SUCCESS, , "Failed to get the message. ", ret);

  if (status == true) {
    std::size_t cp_len =
        std::max(mqtt_data->len, static_cast<std::size_t>(msg->payloadlen));
    memcpy(mqtt_data->read_buf, msg->payload, cp_len);
  }
}
