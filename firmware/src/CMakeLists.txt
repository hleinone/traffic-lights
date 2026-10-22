idf_component_register(SRCS "main.c" "esp_hid_gap.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_hid
                       PRIV_REQUIRES nvs_flash esp_wifi esp_driver_gpio)
