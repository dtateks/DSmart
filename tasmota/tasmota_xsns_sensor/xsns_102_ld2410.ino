#ifdef USE_LD2410

#define XSNS_102                         102

#define LD2410_BUFFER_SIZE               TM_SERIAL_BUFFER_SIZE

#define LD2410_CMND_START_CONFIGURATION  0xFF
#define LD2410_CMND_END_CONFIGURATION    0xFE
#define LD2410_CMND_SET_DISTANCE         0x60
#define LD2410_CMND_READ_PARAMETERS      0x61
#define LD2410_CMND_START_ENGINEERING    0x62
#define LD2410_CMND_END_ENGINEERING      0x63
#define LD2410_CMND_GET_FIRMWARE         0xA0
#define LD2410_CMND_FACTORY_RESET        0xA2
#define LD2410_CMND_REBOOT               0xA3

const uint8_t LD2410_config_header[4] = {0xFD, 0xFC, 0xFB, 0xFA};
const uint8_t LD2410_config_footer[4] = {0x04, 0x03, 0x02, 0x01};
const uint8_t LD2410_target_header[4] = {0xF4, 0xF3, 0xF2, 0xF1};
const uint8_t LD2410_target_footer[4] = {0xF8, 0xF7, 0xF6, 0xF5};

#include <TasmotaSerial.h>
TasmotaSerial *LD2410Serial = nullptr;

struct {
  uint8_t *buffer;
  uint16_t no_one_duration;
  uint8_t step;
  uint8_t retry;
  uint8_t settings;
  uint8_t byte_counter;
  bool valid_response;
  uint8_t set_engin_mode;
  uint8_t web_engin_mode;
  struct {
    uint16_t light;
    uint8_t out_pin;
  } engineering;
  uint16_t last_published_light;
  uint8_t last_published_presence;
} LD2410;


uint32_t ToBcd(uint32_t value) {
  return ((value >> 4) * 10) + (value & 0xF);
}


void Ld1410HandleTargetData(void) {
  if ((0x23 == LD2410.buffer[4]) && (0x01 == LD2410.buffer[6]) && (0x55 == LD2410.buffer[39])) {
    LD2410.web_engin_mode = 1;
    LD2410.engineering.light = (uint16_t)LD2410.buffer[37] * 4;
    LD2410.engineering.out_pin = LD2410.buffer[38];
  } else {
     LD2410.web_engin_mode = 0;
     AddLog(LOG_LEVEL_DEBUG, PSTR("DSmart: Received non-engineering or corrupt target frame"));
  }
}


void Ld1410HandleConfigData(void) {
  if (LD2410_CMND_START_CONFIGURATION == LD2410.buffer[6]) {
    LD2410.valid_response = true;
  }
  else if (LD2410_CMND_GET_FIRMWARE == LD2410.buffer[6]) {
    AddLog(LOG_LEVEL_INFO, PSTR("DSmart: Firmware version V%d.%02d.%02d%02d%02d%02d"),
      ToBcd(LD2410.buffer[13]), ToBcd(LD2410.buffer[12]),
      ToBcd(LD2410.buffer[17]), ToBcd(LD2410.buffer[16]), ToBcd(LD2410.buffer[15]), ToBcd(LD2410.buffer[14]));
  }
}


bool Ld2410Match(const uint8_t *header, uint32_t offset) {
  for (uint32_t i = 0; i < 4; i++) {
    if (LD2410.buffer[offset +i] != header[i]) { return false; }
  }
  return true;
}


void Ld2410Input(void) {
  while (LD2410Serial->available()) {
    yield();

    LD2410.buffer[LD2410.byte_counter++] = LD2410Serial->read();

    if (LD2410.byte_counter < 4) { continue; }

    uint32_t header_start = LD2410.byte_counter - 4;
    bool target_header = Ld2410Match(LD2410_target_header, header_start);
    bool config_header = Ld2410Match(LD2410_config_header, header_start);
    if ((target_header || config_header) && (header_start != 0)) {
      memmove(LD2410.buffer, LD2410.buffer + header_start, 4);
      LD2410.byte_counter = 4;
      AddLog(LOG_LEVEL_DEBUG_MORE, PSTR("DSmart: Resynced buffer"));
    }

    if (LD2410.byte_counter < 6) { continue; }

    target_header = Ld2410Match(LD2410_target_header, 0);
    config_header = Ld2410Match(LD2410_config_header, 0);

    if (target_header || config_header) {
      uint32_t len = LD2410.buffer[4] + 10;

      if (len > LD2410_BUFFER_SIZE || len < 10) {
          AddLog(LOG_LEVEL_DEBUG, PSTR("DSmart: Invalid frame length %d"), len);
          LD2410.byte_counter = 0;
          break;
      }

      if (LD2410.byte_counter < len) { continue; }

      AddLog(LOG_LEVEL_DEBUG_MORE, PSTR("DSmart: Rcvd %d bytes: %*_H"), len, len, LD2410.buffer);

      if (target_header) {
        if (Ld2410Match(LD2410_target_footer, len - 4)) {
          Ld1410HandleTargetData();
        } else {
          AddLog(LOG_LEVEL_DEBUG, PSTR("DSmart: Target footer mismatch"));
        }
      } else if (config_header) {
        if (Ld2410Match(LD2410_config_footer, len - 4)) {
          Ld1410HandleConfigData();
          LD2410Serial->setReadChunkMode(0);
        } else {
           AddLog(LOG_LEVEL_DEBUG, PSTR("DSmart: Config footer mismatch"));
        }
      }

      LD2410.byte_counter = 0;
      break;
    } else if (LD2410.byte_counter >= LD2410_BUFFER_SIZE) {
        AddLog(LOG_LEVEL_DEBUG, PSTR("DSmart: Buffer full, resetting"));
        LD2410.byte_counter = 0;
    }
  }
}


void Ld2410SendCommand(uint32_t command, uint8_t *val = nullptr, uint32_t val_len = 0);
void Ld2410SendCommand(uint32_t command, uint8_t *val, uint32_t val_len) {
  uint32_t len = val_len + 12;
  uint8_t buffer[len];
  buffer[0] = 0xFD;
  buffer[1] = 0xFC;
  buffer[2] = 0xFB;
  buffer[3] = 0xFA;
  buffer[4] = val_len + 2;
  buffer[5] = 0x00;
  buffer[6] = command;
  buffer[7] = 0x00;
  if (val && val_len > 0) {
    memcpy(buffer + 8, val, val_len);
  }
  buffer[8 + val_len] = 0x04;
  buffer[9 + val_len] = 0x03;
  buffer[10 + val_len] = 0x02;
  buffer[11 + val_len] = 0x01;

  AddLog(LOG_LEVEL_DEBUG_MORE, PSTR("DSmart: Send %*_H"), len, buffer);

  LD2410Serial->setReadChunkMode(1);
  LD2410Serial->flush();
  LD2410Serial->write(buffer, len);
}


void Ld2410SetConfigMode(void) {
  uint8_t value[2] = { 0x01, 0x00 };
  Ld2410SendCommand(LD2410_CMND_START_CONFIGURATION, value, sizeof(value));
}


void Ld2410SetMaxDistancesAndNoneDuration(uint32_t max_moving_distance_range, uint32_t max_static_distance_range, uint32_t no_one_duration) {
  uint8_t lsb_nd = no_one_duration & 0xFF;
  uint8_t msb_nd = (no_one_duration >> 8) & 0xFF;
  uint8_t value[18] = { 0x00, 0x00, (uint8_t)max_moving_distance_range, 0x00, 0x00, 0x00,
                        0x01, 0x00, (uint8_t)max_static_distance_range, 0x00, 0x00, 0x00,
                        0x02, 0x00, lsb_nd, msb_nd, 0x00, 0x00 };
  Ld2410SendCommand(LD2410_CMND_SET_DISTANCE, value, sizeof(value));
}


void Ld2410Every100MSecond(void) {
  if (LD2410.step) {
    LD2410.step--;
    switch (LD2410.step) {
      case 59:
        Ld2410SetConfigMode();
        break;
      case 57:
        Ld2410SendCommand(LD2410_CMND_FACTORY_RESET);
        break;
      case 56:
        Ld2410SendCommand(LD2410_CMND_REBOOT);
        break;
      case 45:
        LD2410.step = 12;
        AddLog(LOG_LEVEL_DEBUG, PSTR("DSmart: Settings factory reset requested, rebooting..."));
        break;

      case 39:
        Ld2410SetConfigMode();
        break;
      case 37:
        Ld2410SetMaxDistancesAndNoneDuration(8, 8, LD2410.no_one_duration);
        break;
      case 36:
        LD2410.step = 3;
        AddLog(LOG_LEVEL_DEBUG, PSTR("DSmart: Timeout setting saved"));
        break;

      case 11:
         LD2410.step = 7;
         break;
      case 6:
        Ld2410SetConfigMode();
        break;
      case 4:
        if (!LD2410.valid_response && LD2410.retry) {
          LD2410.retry--;
          if (LD2410.retry) {
            AddLog(LOG_LEVEL_DEBUG, PSTR("DSmart: No response to config start, retrying..."));
            LD2410.step = 7;
          } else {
            LD2410.step = 0;
            AddLog(LOG_LEVEL_ERROR, PSTR("DSmart: No response from sensor, disabling driver."));
          }
        } else {
           LD2410.valid_response = false;
           Ld2410SendCommand(LD2410_CMND_GET_FIRMWARE);
        }
        break;
      case 3:
        if (LD2410.set_engin_mode) {
           Ld2410SendCommand(LD2410_CMND_START_ENGINEERING);
        } else {
           LD2410.step = 1;
        }
        break;
      case 2:
         Ld2410SendCommand(LD2410_CMND_END_CONFIGURATION);
         AddLog(LOG_LEVEL_INFO, PSTR("DSmart: Initialized and Engineering Mode enabled"));
         break;

      case 0:
         break;

    }
  } else {
    if (1 == LD2410.settings) {
      LD2410.settings = 0;
      LD2410.step = 40;
    }
    else if (2 == LD2410.settings) {
      LD2410.settings = 0;
      LD2410.step = 60;
    }
  }
}


void Ld2410EverySecond(void) {
  uint16_t current_light = LD2410.engineering.light;
  uint8_t current_presence = LD2410.engineering.out_pin;

  uint16_t light_diff = (current_light >= LD2410.last_published_light) ?
                        (current_light - LD2410.last_published_light) :
                        (LD2410.last_published_light - current_light);

  bool presence_changed = (current_presence != LD2410.last_published_presence);
  bool light_changed_significantly = (light_diff >= 28);

  if (presence_changed || light_changed_significantly) {
    MqttPublishSensor();
    LD2410.last_published_light = current_light;
    LD2410.last_published_presence = current_presence;
  }
}


void Ld2410Detect(void) {
  if (PinUsed(GPIO_LD2410_RX) && PinUsed(GPIO_LD2410_TX)) {
    LD2410.buffer = (uint8_t*)malloc(LD2410_BUFFER_SIZE);
    if (!LD2410.buffer) {
        AddLog(LOG_LEVEL_ERROR, PSTR("DSmart: Buffer allocation failed"));
        return;
    }
    LD2410Serial = new TasmotaSerial(Pin(GPIO_LD2410_RX), Pin(GPIO_LD2410_TX), 2);
    if (LD2410Serial->begin(256000)) {
      if (LD2410Serial->hardwareSerial()) { ClaimSerial(); }
#ifdef ESP32
      AddLog(LOG_LEVEL_DEBUG, PSTR("DSmart: Serial UART%d initialized at 256000 bps"), LD2410Serial->getUart());
#else
      AddLog(LOG_LEVEL_DEBUG, PSTR("DSmart: Serial initialized at 256000 bps"));
#endif

      LD2410.retry = 3;
      LD2410.step = 12;
      LD2410.set_engin_mode = 1;
      LD2410.no_one_duration = 11;
      memset(&LD2410.engineering, 0, sizeof(LD2410.engineering));
      LD2410.last_published_light = 0;
      LD2410.last_published_presence = 255;
    } else {
        AddLog(LOG_LEVEL_ERROR, PSTR("DSmart: Serial begin failed"));
        free(LD2410.buffer);
        LD2410.buffer = nullptr;
        delete LD2410Serial;
        LD2410Serial = nullptr;
    }
  }
}


const char kLd2410Commands[] PROGMEM = "DSmart|"
  "Timeout";

void CmndDSmartTimeout(void);

void (* const Ld2410Command[])(void) PROGMEM = {
  &CmndDSmartTimeout };


void Ld2410Response(void) {
  Response_P(PSTR("{\"DSmart\":{\"Timeout\":%d}}"), LD2410.no_one_duration);
}

void CmndDSmartTimeout(void) {
  if (0 == XdrvMailbox.payload) {
    LD2410.settings = 2;
    Response_P(PSTR("DSmart: Factory reset requested"));
  }
  else if ((XdrvMailbox.payload > 0) && (XdrvMailbox.payload <= 65535)) {
    LD2410.no_one_duration = XdrvMailbox.payload;
    LD2410.settings = 1;
    Ld2410Response();
  } else {
     Response_P(PSTR("DSmart: Invalid timeout. Current:"));
     Ld2410Response();
  }
}


#ifdef USE_WEBSERVER
const char HTTP_SNS_LD2410[] PROGMEM =
  "{s}Presence{m}%s{e}"
  "{s}Illuminance{m}%d lux{e}";
#endif


void Ld2410Show(bool json) {
  const char* presence_state = (LD2410.engineering.out_pin == 1) ? "Detected" : "Clear";
  uint8_t presence_json_val = (LD2410.engineering.out_pin == 1) ? 1 : 0;

  if (json) {
    ResponseAppend_P(PSTR(",\"Presence\":%d,\"Illuminance\":%u"),
                     presence_json_val, LD2410.engineering.light);
  }
#ifdef USE_WEBSERVER
  else {
    WSContentSend_PD(HTTP_SNS_LD2410, presence_state, LD2410.engineering.light);
  }
#endif
}


bool Xsns102(uint32_t function) {
  bool result = false;

  if (FUNC_INIT == function) {
    Ld2410Detect();
  }
  else if (LD2410Serial) {
    switch (function) {
      case FUNC_LOOP:
      case FUNC_SLEEP_LOOP:
        Ld2410Input();
        break;
      case FUNC_EVERY_100_MSECOND:
        Ld2410Every100MSecond();
        break;
      case FUNC_EVERY_SECOND:
        Ld2410EverySecond();
        break;
      case FUNC_JSON_APPEND:
        Ld2410Show(true);
        break;
#ifdef USE_WEBSERVER
      case FUNC_WEB_SENSOR:
        Ld2410Show(false);
        break;
#endif
      case FUNC_COMMAND:
        result = DecodeCommand(kLd2410Commands, Ld2410Command);
        break;

    }
  }
  return result;
}

#endif