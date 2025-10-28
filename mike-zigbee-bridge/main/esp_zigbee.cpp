

#include "esp_zigbee.h"

// Стандартные Zigbee широковещательные адреса
#define ZB_BROADCAST_ALL_DEVICES 0xFFFF
#define ZB_BROADCAST_RX_ON_WHEN_IDLE 0xFFFD
#define ZB_BROADCAST_ROUTERS_AND_COORD 0xFFFC

// Универсальная функция поиска устройств по кластерам
esp_err_t esp_zb_zdo_find_device_by_clusters(
    uint16_t profile_id,
    uint8_t num_in_clusters,
    uint16_t *in_cluster_list,
    uint8_t num_out_clusters, 
    uint16_t *out_cluster_list,
    esp_zb_zdo_match_desc_callback_t user_cb,
    void *user_ctx)
{
    esp_zb_zdo_match_desc_req_param_t match_desc_req = {
        .dst_nwk_addr = 0xFFFC,
        .addr_of_interest = 0xFFFC,
        .profile_id = profile_id,
        .num_in_clusters = num_in_clusters,
        .num_out_clusters = num_out_clusters,
        .cluster_list = in_cluster_list
    };

    return esp_zb_zdo_match_cluster(&match_desc_req, user_cb, user_ctx);
}

void esp_zb_zdo_find_on_off_switch(esp_zb_zdo_match_desc_req_param_t *cmd_req, esp_zb_zdo_match_desc_callback_t user_cb, void *user_ctx)
{
	//cmd_req.dst_nwk_addr = 0xFFFC;
  //cmd_req.addr_of_interest = 0xFFFC;
	
	uint16_t on_off_cluster = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF;
  
  /*return */esp_zb_zdo_find_device_by_clusters(
      ESP_ZB_AF_HA_PROFILE_ID,
      0, NULL,                  // 0 входных кластеров
      1, &on_off_cluster,       // 1 выходной кластер
      user_cb, user_ctx
  );
}
void esp_zb_zdo_find_temperature_sensor(esp_zb_zdo_match_desc_req_param_t *cmd_req, esp_zb_zdo_match_desc_callback_t user_cb, void *user_ctx)
{
	
}
void esp_zb_zdo_find_pressure_sensor(esp_zb_zdo_match_desc_req_param_t *cmd_req, esp_zb_zdo_match_desc_callback_t user_cb, void *user_ctx)
{
	
}
void esp_zb_zdo_find_humidity_sensor(esp_zb_zdo_match_desc_req_param_t *cmd_req, esp_zb_zdo_match_desc_callback_t user_cb, void *user_ctx)
{
	
}
