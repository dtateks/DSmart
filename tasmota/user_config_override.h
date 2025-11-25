#ifndef _USER_CONFIG_OVERRIDE_H_
#define _USER_CONFIG_OVERRIDE_H_


#define PROJECT           "DSmart"
#define FRIENDLY_NAME     "DSmart"

// #define STA_SSID1         "D'Home"
// #define STA_PASS1         "777777777"
#define WIFI_NO_SLEEP     true
#define WIFI_AP_NAME      "DSmart"
#define WEB_PASSWORD      "777777777"

#define APP_TIMEZONE      7
#define MQTT_HOST         "dta.business"
#define MQTT_USER         "T4rY8vL2qW7mP9xK5zB3nJ6tH1eF8dC"
#define MQTT_PASS         "N!5zB3cH6tQ@jF%kD+yM^uG1nP#xL$7vQ&9rW4zT2eJ5bC8hN*mK+R#9pX$2vL!8rT4wY"

#define USER_TEMPLATE     "{\"NAME\":\"DSmart\",\"GPIO\":[1,1,1,1,1,1,0,0,3552,0,3584,5792,3840,576,5600,0,0,0,0,5568,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0,1],\"FLAG\":0,\"BASE\":1}"
#define MODULE            USER_MODULE
#define FALLBACK_MODULE   USER_MODULE

#define LIGHT_MODE false

#define USE_ZIGBEE
#define USE_ZIGBEE_EZSP
#define USE_UFILESYS
#define USE_ZIGBEE_EEPROM
#define USE_TCP_BRIDGE
#undef USE_ZIGBEE_ZNP

#define USE_ETHERNET
#define ETH_TYPE          0
#define ETH_CLKMODE       0
#define ETH_ADDRESS       1

#define USE_MATTER_DEVICE
#define MATTER_ENABLED true
#define ZIGBEE_FRIENDLY_NAMES true

#undef USE_I2C
#undef USE_IR_REMOTE
#undef USE_AUTOCONF
#undef USE_TIMERS
#undef USE_DOMOTICZ
#undef USE_KNX_WEB_MENU
#undef USE_ADE7953
#undef USE_SONOFF_RF
#undef USE_SONOFF_SC
#undef USE_ARMTRONIX_DIMMERS
#undef USE_PS_16_DZ
#undef USE_SONOFF_IFAN
#undef USE_BUZZER
#undef USE_ARILUX_RF
#undef USE_SHUTTER
#undef USE_DEEPSLEEP
#undef USE_EXS_DIMMER
#undef USE_PWM_DIMMER
#undef USE_SONOFF_D1
#undef USE_SHELLY_DIMMER
#undef USE_LIGHT
#undef USE_MY92X1
#undef USE_SM16716
#undef USE_SM2135
#undef USE_SM2335
#undef USE_BP1658CJ
#undef USE_BP5758D
#undef USE_SONOFF_L1
#undef USE_ELECTRIQ_MOODL
#undef USE_LIGHT_PALETTE
#undef USE_LIGHT_VIRTUAL_CT
#undef USE_DGR_LIGHT_SEQUENCE
#undef USE_WS2812
#undef USE_COUNTER
#undef USE_DS18x20
#undef USE_ENERGY_SENSOR
#undef USE_ENERGY_MARGIN_DETECTION
#undef USE_ENERGY_DUMMY
#undef USE_HLW8012
#undef USE_CSE7766
#undef USE_PZEM004T
#undef USE_PZEM_AC
#undef USE_PZEM_DC
#undef USE_MCP39F501
#undef USE_BL09XX
#undef USE_DHT
#undef USE_ADC
#undef USE_CSE7761
#undef USE_DEVICE_GROUPS
#undef USE_SERIAL_BRIDGE
#undef USE_EMULATION_HUE
#undef USE_EMULATION_WEMO
#undef ROTARY_V1


#endif