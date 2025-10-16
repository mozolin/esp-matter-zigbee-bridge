/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <app_bridged_device.h>
#include <esp_check.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_bridge.h>
#include <zigbee_bridge.h>

// ДОБАВЛЕНО: Для управления светодиодом
#include <driver/gpio.h>


#include "esp_zigbee.h"


static const char *TAG = "zigbee_bridge";

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

extern uint16_t aggregator_endpoint_id;

// ДОБАВЛЕНО: GPIO для светодиода
#define LED_GPIO GPIO_NUM_2

// ДОБАВЛЕНО: Функции для управления светодиодом
static void led_blink(int count, int delay_ms)
{
    for (int i = 0; i < count; i++) {
        gpio_set_level(LED_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
        gpio_set_level(LED_GPIO, 0);
        if (i < count - 1) {
            vTaskDelay(pdMS_TO_TICKS(delay_ms));
        }
    }
}

// ДОБАВЛЕНО: Переменная для подсчета найденных устройств
static int devices_found_count = 0;

// ДОБАВЛЕНО: Callback для обнаружения Zigbee датчиков температуры
void zigbee_bridge_find_bridged_temperature_sensor_cb(esp_zb_zdp_status_t zdo_status, uint16_t addr, uint8_t endpoint, void *user_ctx)
{
    ESP_LOGI(TAG, "temperature_sensor found: address:0x%" PRIx16 ", endpoint:%" PRId8 ", response_status:%d", addr, endpoint, zdo_status);
    if (zdo_status == ESP_ZB_ZDP_STATUS_SUCCESS) {
        devices_found_count++;
        node_t *node = node::get();
        if (!node) {
            ESP_LOGE(TAG, "Could not find esp_matter node");
            return;
        }
        if (app_bridge_get_device_by_zigbee_shortaddr(addr)) {
            ESP_LOGI(TAG, "Bridged node for 0x%04" PRIx16 " zigbee temperature sensor on endpoint %" PRId16 " has been created", addr,
                     app_bridge_get_matter_endpointid_by_zigbee_shortaddr(addr));
        } else {
            // ДОБАВЛЕНО: Создание бриджированного устройства для датчика температуры
            app_bridged_device_t *bridged_device =
                app_bridge_create_bridged_device(node, aggregator_endpoint_id, ESP_MATTER_TEMPERATURE_SENSOR_DEVICE_TYPE_ID,
                                                 ESP_MATTER_BRIDGED_DEVICE_TYPE_ZIGBEE,
                                                 app_bridge_zigbee_address(endpoint, addr), NULL);
            if (!bridged_device) {
                ESP_LOGE(TAG, "Failed to create zigbee bridged device (temperature sensor)");
                return;
            }
            ESP_LOGI(TAG, "Create/Update bridged node for 0x%04" PRIx16 " zigbee temperature sensor on endpoint %" PRId16 "", addr,
                     app_bridge_get_matter_endpointid_by_zigbee_shortaddr(addr));
            // ДОБАВЛЕНО: Индикация успешного добавления устройства
            led_blink(1, 100);
        }
    }
}

// ДОБАВЛЕНО: Callback для обнаружения Zigbee датчиков давления
void zigbee_bridge_find_bridged_pressure_sensor_cb(esp_zb_zdp_status_t zdo_status, uint16_t addr, uint8_t endpoint, void *user_ctx)
{
    ESP_LOGI(TAG, "pressure_sensor found: address:0x%" PRIx16 ", endpoint:%" PRId8 ", response_status:%d", addr, endpoint, zdo_status);
    if (zdo_status == ESP_ZB_ZDP_STATUS_SUCCESS) {
        devices_found_count++;
        node_t *node = node::get();
        if (!node) {
            ESP_LOGE(TAG, "Could not find esp_matter node");
            return;
        }
        if (app_bridge_get_device_by_zigbee_shortaddr(addr)) {
            ESP_LOGI(TAG, "Bridged node for 0x%04" PRIx16 " zigbee pressure sensor on endpoint %" PRId16 " has been created", addr,
                     app_bridge_get_matter_endpointid_by_zigbee_shortaddr(addr));
        } else {
            // ДОБАВЛЕНО: Создание бриджированного устройства для датчика давления
            app_bridged_device_t *bridged_device =
                app_bridge_create_bridged_device(node, aggregator_endpoint_id, ESP_MATTER_PRESSURE_SENSOR_DEVICE_TYPE_ID,
                                                 ESP_MATTER_BRIDGED_DEVICE_TYPE_ZIGBEE,
                                                 app_bridge_zigbee_address(endpoint, addr), NULL);
            if (!bridged_device) {
                ESP_LOGE(TAG, "Failed to create zigbee bridged device (pressure sensor)");
                return;
            }
            ESP_LOGI(TAG, "Create/Update bridged node for 0x%04" PRIx16 " zigbee pressure sensor on endpoint %" PRId16 "", addr,
                     app_bridge_get_matter_endpointid_by_zigbee_shortaddr(addr));
            // ДОБАВЛЕНО: Индикация успешного добавления устройства
            led_blink(1, 100);
        }
    }
}

// ДОБАВЛЕНО: Callback для обнаружения Zigbee датчиков влажности
void zigbee_bridge_find_bridged_humidity_sensor_cb(esp_zb_zdp_status_t zdo_status, uint16_t addr, uint8_t endpoint, void *user_ctx)
{
    ESP_LOGI(TAG, "humidity_sensor found: address:0x%" PRIx16 ", endpoint:%" PRId8 ", response_status:%d", addr, endpoint, zdo_status);
    if (zdo_status == ESP_ZB_ZDP_STATUS_SUCCESS) {
        devices_found_count++;
        node_t *node = node::get();
        if (!node) {
            ESP_LOGE(TAG, "Could not find esp_matter node");
            return;
        }
        if (app_bridge_get_device_by_zigbee_shortaddr(addr)) {
            ESP_LOGI(TAG, "Bridged node for 0x%04" PRIx16 " zigbee humidity sensor on endpoint %" PRId16 " has been created", addr,
                     app_bridge_get_matter_endpointid_by_zigbee_shortaddr(addr));
        } else {
            // ДОБАВЛЕНО: Создание бриджированного устройства для датчика влажности
            app_bridged_device_t *bridged_device =
                app_bridge_create_bridged_device(node, aggregator_endpoint_id, ESP_MATTER_HUMIDITY_SENSOR_DEVICE_TYPE_ID,
                                                 ESP_MATTER_BRIDGED_DEVICE_TYPE_ZIGBEE,
                                                 app_bridge_zigbee_address(endpoint, addr), NULL);
            if (!bridged_device) {
                ESP_LOGE(TAG, "Failed to create zigbee bridged device (humidity sensor)");
                return;
            }
            ESP_LOGI(TAG, "Create/Update bridged node for 0x%04" PRIx16 " zigbee humidity sensor on endpoint %" PRId16 "", addr,
                     app_bridge_get_matter_endpointid_by_zigbee_shortaddr(addr));
            // ДОБАВЛЕНО: Индикация успешного добавления устройства
            led_blink(1, 100);
        }
    }
}

void zigbee_bridge_find_bridged_on_off_switch_cb(esp_zb_zdp_status_t zdo_status, uint16_t addr, uint8_t endpoint, void *user_ctx)
{
    ESP_LOGI(TAG, "on_off_switch found: address:0x%" PRIx16 ", endpoint:%" PRId8 ", response_status:%d", addr, endpoint, zdo_status);
    if (zdo_status == ESP_ZB_ZDP_STATUS_SUCCESS) {
        devices_found_count++;
        node_t *node = node::get();
        if (!node) {
            ESP_LOGE(TAG, "Could not find esp_matter node");
            return;
        }
        if (app_bridge_get_device_by_zigbee_shortaddr(addr)) {
            ESP_LOGI(TAG, "Bridged node for 0x%04" PRIx16 " zigbee switch on endpoint %" PRId16 " has been created", addr,
                     app_bridge_get_matter_endpointid_by_zigbee_shortaddr(addr));
        } else {
            app_bridged_device_t *bridged_device =
                app_bridge_create_bridged_device(node, aggregator_endpoint_id, ESP_MATTER_ON_OFF_SWITCH_DEVICE_TYPE_ID,
                                                 ESP_MATTER_BRIDGED_DEVICE_TYPE_ZIGBEE,
                                                 app_bridge_zigbee_address(endpoint, addr), NULL);
            if (!bridged_device) {
                ESP_LOGE(TAG, "Failed to create zigbee bridged device (on_off switch)");
                return;
            }
            ESP_LOGI(TAG, "Create/Update bridged node for 0x%04" PRIx16 " zigbee switch on endpoint %" PRId16 "", addr,
                     app_bridge_get_matter_endpointid_by_zigbee_shortaddr(addr));
            // ДОБАВЛЕНО: Индикация успешного добавления устройства
            led_blink(1, 100);
        }
    }
}

void zigbee_bridge_find_bridged_on_off_light_cb(esp_zb_zdp_status_t zdo_status, uint16_t addr, uint8_t endpoint, void *user_ctx)
{
    ESP_LOGI(TAG, "on_off_light found: address:0x%" PRIx16 ", endpoint:%" PRId8 ", response_status:%d", addr, endpoint, zdo_status);
    if (zdo_status == ESP_ZB_ZDP_STATUS_SUCCESS) {
        devices_found_count++;
        node_t *node = node::get();
        if (!node) {
            ESP_LOGE(TAG, "Could not find esp_matter node");
            return;
        }
        if (app_bridge_get_device_by_zigbee_shortaddr(addr)) {
            ESP_LOGI(TAG, "Bridged node for 0x%04" PRIx16 " zigbee device on endpoint %" PRId16 " has been created", addr,
                     app_bridge_get_matter_endpointid_by_zigbee_shortaddr(addr));
        } else {
            app_bridged_device_t *bridged_device =
                app_bridge_create_bridged_device(node, aggregator_endpoint_id, ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_ID,
                                                 ESP_MATTER_BRIDGED_DEVICE_TYPE_ZIGBEE,
                                                 app_bridge_zigbee_address(endpoint, addr), NULL);
            if (!bridged_device) {
                ESP_LOGE(TAG, "Failed to create zigbee bridged device (on_off light)");
                return;
            }
            ESP_LOGI(TAG, "Create/Update bridged node for 0x%04" PRIx16 " zigbee device on endpoint %" PRId16 "", addr,
                     app_bridge_get_matter_endpointid_by_zigbee_shortaddr(addr));
            // ДОБАВЛЕНО: Индикация успешного добавления устройства
            led_blink(1, 100);
        }
    }
}

// ДОБАВЛЕНО: Функция для запуска обнаружения устройств по кнопке
void zigbee_bridge_trigger_device_discovery(void)
{
    ESP_LOGI(TAG, "=== MANUAL ZIGBEE DEVICE DISCOVERY TRIGGERED ===");
    
    // Сброс счетчика найденных устройств
    devices_found_count = 0;
    
    // Поиск устройств с широковещательным адресом
    esp_zb_zdo_match_desc_req_param_t cmd_req;
    cmd_req.dst_nwk_addr = 0xFFFC; // Широковещательный адрес для всех устройств
    cmd_req.addr_of_interest = 0xFFFC;
    
    // Поиск всех типов устройств
    esp_zb_zdo_find_on_off_light(&cmd_req, zigbee_bridge_find_bridged_on_off_light_cb, NULL);
    esp_zb_zdo_find_on_off_switch(&cmd_req, zigbee_bridge_find_bridged_on_off_switch_cb, NULL);
    esp_zb_zdo_find_temperature_sensor(&cmd_req, zigbee_bridge_find_bridged_temperature_sensor_cb, NULL);
    esp_zb_zdo_find_pressure_sensor(&cmd_req, zigbee_bridge_find_bridged_pressure_sensor_cb, NULL);
    esp_zb_zdo_find_humidity_sensor(&cmd_req, zigbee_bridge_find_bridged_humidity_sensor_cb, NULL);
    
    ESP_LOGI(TAG, "Discovery requests sent to all Zigbee devices");
    
    // ДОБАВЛЕНО: Задержка для завершения процесса обнаружения
    vTaskDelay(pdMS_TO_TICKS(10000)); // 10 секунд
    
    // ДОБАВЛЕНО: Индикация результатов обнаружения
    if (devices_found_count > 0) {
        ESP_LOGI(TAG, "Discovery completed: Found %d devices", devices_found_count);
        // Успешное завершение - быстро мигаем 5 раз
        led_blink(5, 50);
    } else {
        ESP_LOGW(TAG, "Discovery completed: No devices found");
        // Неудача - медленно мигаем 3 раза
        led_blink(3, 500);
    }
}

esp_err_t zigbee_bridge_attribute_update(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id,
                                         esp_matter_attr_val_t *val, app_bridged_device_t *zigbee_device)
{
    if (zigbee_device && zigbee_device->dev && zigbee_device->dev->endpoint) {
        // Обработка кластера OnOff для переключателей и светильников
        if (cluster_id == OnOff::Id) {
            if (attribute_id == OnOff::Attributes::OnOff::Id) {
                ESP_LOGD(TAG, "Update Bridged Device, ep: %" PRId16 ", cluster: %" PRId32 ", att: %" PRId32 "", endpoint_id, cluster_id,
                         attribute_id);
                esp_zb_zcl_on_off_cmd_t cmd_req;
                cmd_req.zcl_basic_cmd.dst_addr_u.addr_short = zigbee_device->dev_addr.zigbee_shortaddr;
                cmd_req.zcl_basic_cmd.dst_endpoint = zigbee_device->dev_addr.zigbee_endpointid;
                cmd_req.zcl_basic_cmd.src_endpoint = esp_matter::endpoint::get_id(zigbee_device->dev->endpoint);
                cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
                cmd_req.on_off_cmd_id = val->val.b ? ESP_ZB_ZCL_CMD_ON_OFF_ON_ID : ESP_ZB_ZCL_CMD_ON_OFF_OFF_ID;
                if (esp_zb_lock_acquire(portMAX_DELAY)) {
                    esp_zb_zcl_on_off_cmd_req(&cmd_req);
                    esp_zb_lock_release();
                }
            }
        }
        // ДОБАВЛЕНО: Обработка команд для сенсоров (если требуется)
        // Например, запрос отчетов от сенсоров
    }
    else{
        ESP_LOGE(TAG, "Unable to Update Bridge Device, ep: %" PRId16 ", cluster: %" PRId32 ", att: %" PRId32 "", endpoint_id, cluster_id, attribute_id);
    }
    return ESP_OK;
}