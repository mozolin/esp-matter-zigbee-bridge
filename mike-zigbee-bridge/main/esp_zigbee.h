
#pragma once
#include "esp_err.h"
#ifdef __cplusplus
extern "C" {
#endif

//#include "esp_zigbee_zdo_command.h"
//#include "esp_zigbee_type.h"
//#include <zigbee_bridge.h>
#include "esp_zigbee_core.h"


void esp_zb_zdo_find_on_off_switch(esp_zb_zdo_match_desc_req_param_t *cmd_req, esp_zb_zdo_match_desc_callback_t user_cb, void *user_ctx);
void esp_zb_zdo_find_temperature_sensor(esp_zb_zdo_match_desc_req_param_t *cmd_req, esp_zb_zdo_match_desc_callback_t user_cb, void *user_ctx);
void esp_zb_zdo_find_pressure_sensor(esp_zb_zdo_match_desc_req_param_t *cmd_req, esp_zb_zdo_match_desc_callback_t user_cb, void *user_ctx);
void esp_zb_zdo_find_humidity_sensor(esp_zb_zdo_match_desc_req_param_t *cmd_req, esp_zb_zdo_match_desc_callback_t user_cb, void *user_ctx);

#ifdef __cplusplus
}
#endif
