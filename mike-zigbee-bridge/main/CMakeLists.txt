idf_component_register(SRCS "app_main.cpp"
                       "app_zboss.cpp" 
                       "zigbee_bridge.cpp"
                       "esp_zigbee.cpp"
                       "driver_reset_button.cpp"
                       INCLUDE_DIRS "."
                       PRIV_INCLUDE_DIRS "." "${ESP_MATTER_PATH}/examples/common/utils")
