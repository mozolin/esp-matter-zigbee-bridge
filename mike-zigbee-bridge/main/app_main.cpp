/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_err.h>
#include <esp_log.h>
#include <nvs_flash.h>

#include <esp_matter.h>
#include <esp_matter_console.h>
#include <esp_matter_ota.h>

#include <common_macros.h>
#include <app_bridged_device.h>
#include <app_zboss.h>
#include <zigbee_bridge.h>

// ДОБАВЛЕНО: Библиотеки для работы с кнопками и светодиодом
#include <driver/gpio.h>
#include <esp_timer.h>

static const char *TAG = "app_main";

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;

uint16_t aggregator_endpoint_id = chip::kInvalidEndpointId;

// ДОБАВЛЕНО: Конфигурация кнопок и светодиода для ESP32-S3
#define BUTTON_BOOT_GPIO   GPIO_NUM_0   // BOOT button
#define BUTTON_RESET_GPIO  GPIO_NUM_1   // RESET button (проверьте спецификацию вашей платы)
#define LED_GPIO           GPIO_NUM_2   // Встроенный светодиод (может быть GPIO2, GPIO8 или другим - проверьте плату)

// ДОБАВЛЕНО: Переменные для антидребезга и управления светодиодом
static uint64_t last_button_press_time = 0;
static const uint32_t DEBOUNCE_DELAY_MS = 500;
static bool discovery_in_progress = false;

// ДОБАВЛЕНО: Функции для управления светодиодом
static void led_init(void)
{
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << LED_GPIO);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);
    
    // Выключить светодиод при инициализации
    gpio_set_level(LED_GPIO, 0);
    ESP_LOGI(TAG, "LED initialized on GPIO%d", LED_GPIO);
}

static void led_set(bool on)
{
    gpio_set_level(LED_GPIO, on ? 1 : 0);
}

static void led_blink(int count, int delay_ms)
{
    for (int i = 0; i < count; i++) {
        led_set(true);
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
        led_set(false);
        if (i < count - 1) {
            vTaskDelay(pdMS_TO_TICKS(delay_ms));
        }
    }
}

// ДОБАВЛЕНО: Функция для мигания светодиодом во время обнаружения
static void discovery_led_indicator(void* pvParameters)
{
    discovery_in_progress = true;
    ESP_LOGI(TAG, "Starting discovery LED indicator");
    
    // Быстрое мигание во время процесса обнаружения
    for (int i = 0; i < 30; i++) { // Мигаем 30 раз (примерно 15 секунд)
        led_set(true);
        vTaskDelay(pdMS_TO_TICKS(250));
        led_set(false);
        vTaskDelay(pdMS_TO_TICKS(250));
        
        // Проверяем, не отменили ли обнаружение
        if (!discovery_in_progress) {
            break;
        }
    }
    
    // Завершаем индикацию
    led_set(false);
    discovery_in_progress = false;
    ESP_LOGI(TAG, "Discovery LED indicator stopped");
    vTaskDelete(NULL);
}

// ДОБАВЛЕНО: Запуск индикации обнаружения
static void start_discovery_indication(void)
{
    if (discovery_in_progress) {
        ESP_LOGW(TAG, "Discovery already in progress");
        return;
    }
    
    // Создаем задачу для индикации
    xTaskCreate(discovery_led_indicator, "discovery_led", 2048, NULL, 5, NULL);
}

// ДОБАВЛЕНО: Остановка индикации обнаружения
static void stop_discovery_indication(void)
{
    discovery_in_progress = false;
    led_set(false); // Гасим светодиод
}

// ДОБАВЛЕНО: Функция обработки прерывания от кнопки
static void IRAM_ATTR button_isr_handler(void* arg)
{
    uint64_t current_time = esp_timer_get_time() / 1000; // Convert to milliseconds
    
    // Антидребезг
    if (current_time - last_button_press_time < DEBOUNCE_DELAY_MS) {
        return;
    }
    
    last_button_press_time = current_time;
    int gpio_num = (int)arg;
    
    if (gpio_num == BUTTON_BOOT_GPIO) {
        ESP_LOGI(TAG, "BOOT button pressed - triggering Zigbee discovery");
        // Короткое мигание для подтверждения нажатия
        led_blink(2, 100);
        // Запускаем индикацию процесса обнаружения
        start_discovery_indication();
        // Запускаем обнаружение устройств
        zigbee_bridge_trigger_device_discovery();
    }
    else if (gpio_num == BUTTON_RESET_GPIO) {
        ESP_LOGI(TAG, "RESET button pressed - triggering Zigbee discovery");
        // Короткое мигание для подтверждения нажатия
        led_blink(2, 100);
        // Запускаем индикацию процесса обнаружения
        start_discovery_indication();
        // Запускаем обнаружение устройств
        zigbee_bridge_trigger_device_discovery();
    }
}

// ДОБАВЛЕНО: Инициализация кнопок
static void button_init(void)
{
    // Конфигурация BOOT button
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_NEGEDGE; // Прерывание по спаду (нажатие)
    io_conf.pin_bit_mask = (1ULL << BUTTON_BOOT_GPIO) | (1ULL << BUTTON_RESET_GPIO);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&io_conf);

    // Установка обработчиков прерываний
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_BOOT_GPIO, button_isr_handler, (void*)BUTTON_BOOT_GPIO);
    gpio_isr_handler_add(BUTTON_RESET_GPIO, button_isr_handler, (void*)BUTTON_RESET_GPIO);

    ESP_LOGI(TAG, "Buttons initialized - BOOT: GPIO%d, RESET: GPIO%d", BUTTON_BOOT_GPIO, BUTTON_RESET_GPIO);
    ESP_LOGI(TAG, "Press BOOT or RESET button to start Zigbee device discovery");
}

static void app_event_cb(const ChipDeviceEvent *event, intptr_t arg)
{
    switch (event->Type) {
    case chip::DeviceLayer::DeviceEventType::kInterfaceIpAddressChanged:
        ESP_LOGI(TAG, "Interface IP Address Changed");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningComplete:
        ESP_LOGI(TAG, "Commissioning complete");
        // ДОБАВЛЕНО: Индикация успешной комиссии
        led_blink(3, 200);
        break;

    case chip::DeviceLayer::DeviceEventType::kFailSafeTimerExpired:
        ESP_LOGI(TAG, "Commissioning failed, fail safe timer expired");
        // ДОБАВЛЕНО: Индикация ошибки
        led_blink(5, 100);
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStarted:
        ESP_LOGI(TAG, "Commissioning session started");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStopped:
        ESP_LOGI(TAG, "Commissioning session stopped");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowOpened:
        ESP_LOGI(TAG, "Commissioning window opened");
        // ДОБАВЛЕНО: Медленное мигание при открытом окне комиссии
        led_blink(1, 1000);
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowClosed:
        ESP_LOGI(TAG, "Commissioning window closed");
        led_set(false);
        break;

    default:
        break;
    }
}

// This callback is called for every attribute update. The callback implementation shall
// handle the desired attributes and return an appropriate error code. If the attribute
// is not of your interest, please do not return an error code and strictly return ESP_OK.
static esp_err_t app_attribute_update_cb(callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                         uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data)
{
    esp_err_t err = ESP_OK;

    if (type == PRE_UPDATE) {
        err = zigbee_bridge_attribute_update(endpoint_id, cluster_id, attribute_id, val, (app_bridged_device_t *)priv_data);
    }
    return err;
}

// This callback is invoked after the creation or resumption of a bridge endpoint.
// It can be used to add data model elements (e.g., attributes, commands, etc.) to the bridge endpoint.
esp_err_t create_bridge_devices(esp_matter::endpoint_t *ep, uint32_t device_type_id, void *priv_data)
{
    esp_err_t err = ESP_OK;

    switch (device_type_id) {
    case ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_ID: {
        on_off_light::config_t on_off_light_conf;
        err = on_off_light::add(ep, &on_off_light_conf);
        break;
    }
    case ESP_MATTER_DIMMABLE_LIGHT_DEVICE_TYPE_ID: {
        dimmable_light::config_t dimmable_light_conf;
        err = dimmable_light::add(ep, &dimmable_light_conf);
        break;
    }
    case ESP_MATTER_COLOR_TEMPERATURE_LIGHT_DEVICE_TYPE_ID: {
        color_temperature_light::config_t color_temperature_light_conf;
        err = color_temperature_light::add(ep, &color_temperature_light_conf);
        break;
    }
    case ESP_MATTER_EXTENDED_COLOR_LIGHT_DEVICE_TYPE_ID: {
        extended_color_light::config_t extended_color_light_conf;
        err = extended_color_light::add(ep, &extended_color_light_conf);
        break;
    }
    case ESP_MATTER_ON_OFF_SWITCH_DEVICE_TYPE_ID: {
        on_off_switch::config_t switch_config;
        err = on_off_switch::add(ep, &switch_config);
        break;
    }
    // ДОБАВЛЕНО: Обработка сенсорных устройств
    case ESP_MATTER_TEMPERATURE_SENSOR_DEVICE_TYPE_ID: {
        temperature_sensor::config_t temp_sensor_conf;
        err = temperature_sensor::add(ep, &temp_sensor_conf);
        break;
    }
    case ESP_MATTER_PRESSURE_SENSOR_DEVICE_TYPE_ID: {
        pressure_sensor::config_t pressure_sensor_conf;
        err = pressure_sensor::add(ep, &pressure_sensor_conf);
        break;
    }
    case ESP_MATTER_HUMIDITY_SENSOR_DEVICE_TYPE_ID: {
        humidity_sensor::config_t humidity_sensor_conf;
        err = humidity_sensor::add(ep, &humidity_sensor_conf);
        break;
    }
    default: {
        ESP_LOGE(TAG, "Unsupported bridged matter device type");
        return ESP_ERR_INVALID_ARG;
    }
    }
    return err;
}

extern "C" void app_main()
{
    esp_err_t err = ESP_OK;

    /* Initialize the ESP NVS layer */
    nvs_flash_init();

    // ДОБАВЛЕНО: Инициализация светодиода
    led_init();
    
    // ДОБАВЛЕНО: Индикация старта системы
    led_blink(1, 500);

    /* Create a Matter node and add the mandatory Root Node device type on endpoint 0 */
    node::config_t node_config;
    node_t *node = node::create(&node_config, app_attribute_update_cb, NULL);
    ABORT_APP_ON_FAILURE(node != nullptr, ESP_LOGE(TAG, "Failed to create Matter node"));

    aggregator::config_t aggregator_config;
    endpoint_t *aggregator = endpoint::aggregator::create(node, &aggregator_config, ENDPOINT_FLAG_NONE, NULL);
    ABORT_APP_ON_FAILURE(aggregator != nullptr, ESP_LOGE(TAG, "Failed to create aggregator endpoint"));

    aggregator_endpoint_id = endpoint::get_id(aggregator);

    /* Matter start */
    err = esp_matter::start(app_event_cb);
    ABORT_APP_ON_FAILURE(err == ESP_OK, ESP_LOGE(TAG, "Failed to start Matter, err:%d", err));

    err = app_bridge_initialize(node, create_bridge_devices);
    ABORT_APP_ON_FAILURE(err == ESP_OK, ESP_LOGE(TAG, "Failed to resume the bridged endpoints: %d", err));

    // ДОБАВЛЕНО: Инициализация кнопок
    button_init();

#if CONFIG_ENABLE_CHIP_SHELL
    esp_matter::console::diagnostics_register_commands();
    esp_matter::console::wifi_register_commands();
    esp_matter::console::factoryreset_register_commands();
    esp_matter::console::init();
#endif

    // ДОБАВЛЕНО: Индикация готовности системы
    led_blink(3, 100);
    vTaskDelay(pdMS_TO_TICKS(500));
    led_blink(3, 100);

    ESP_LOGI(TAG, "==========================================");
    ESP_LOGI(TAG, "Zigbee Bridge Started");
    ESP_LOGI(TAG, "Auto-discovery: DISABLED");
    ESP_LOGI(TAG, "Manual discovery: Press BOOT or RESET button");
    ESP_LOGI(TAG, "LED will blink during discovery process");
    ESP_LOGI(TAG, "==========================================");

    launch_app_zboss();
}
