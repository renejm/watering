#ifndef ARDUINO_CREDENTIALS_H
#define ARDUINO_CREDENTIALS_H

/* WiFi Credentials to connect Internet */
#define STA_SSID "My SSID"
#define STA_PASS "My_AP_password"

/* Provide MQTT broker credentials as denoted in maqiatto.com. */
#define MQTT_BROKER       "maqiatto.com"
#define MQTT_BROKER_PORT  1883
#define MQTT_USERNAME     "my_maqiatto_username"
#define MQTT_KEY          "MY_MAQIATTO_KEY"


/* Provide topic as it is denoted in your topic list.
 * For example mine is : cadominna@gmail.com/topic1
 * To add topics, see https://www.maqiatto.com/configure
 */

#define TOPIC1    "my_maqiatto_username/umidade"
#define TOPIC2    "my_maqiatto_username/status"

#endif /* ARDUINO_CREDENTIALS_H */
