#ifndef _USER_CONFIG_OVERRIDE_H_
#define _USER_CONFIG_OVERRIDE_H_


#define PROJECT           "DSmart"
#define FRIENDLY_NAME     "DSmart"

// #define STA_SSID1         "D'Home"
// #define STA_PASS1         "777777777"
#define WIFI_AP_NAME      "DSmart"
#define WEB_PASSWORD      "777777777"

#define MQTT_HOST         "dta.business"
#define MQTT_USER         "T4rY8vL2qW7mP9xK5zB3nJ6tH1eF8dC"
#define MQTT_PASS         "N!5zB3cH6tQ@jF%kD+yM^uG1nP#xL$7vQ&9rW4zT2eJ5bC8hN*mK+R#9pX$2vL!8rT4wY"

#define USER_TEMPLATE     "{\"NAME\":\"DSmart\",\"GPIO\":[1,640,608,9920,9888,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1],\"FLAG\":0,\"BASE\":1}"
#define USER_RULE1        "ON Presence DO IF (%value% != %Var1%) MtrUpdate {\"Name\":\"Presence\", \"Occupancy\":%value%} ; Var1 %value% ENDIF ENDON"
#define USER_BACKLOG      "RULE1 1; I2cDriver12 0; TelePeriod 59"

#define APP_TIMEZONE      7
#define WIFI_NO_SLEEP     true
#define MATTER_ENABLED    true
#define MODULE            USER_MODULE
#define FALLBACK_MODULE   USER_MODULE

#define USE_AHT3x
#define USE_SHT3X
#define USE_LD2410
#define USE_MATTER_DEVICE

#undef USE_IR_REMOTE
#undef USE_AUTOCONF
#undef USE_TIMERS
#undef USE_DOMOTICZ
#undef USE_KNX_WEB_MENU
#undef USE_ADE7953
#undef USE_SONOFF_RF
#undef USE_SONOFF_SC
#undef USE_TUYA_MCU
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