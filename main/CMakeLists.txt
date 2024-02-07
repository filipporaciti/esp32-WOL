idf_component_register(SRCS "esp32-WOL.c"
                    INCLUDE_DIRS ".")
spiffs_create_partition_image(storage spiffs_image FLASH_IN_PROJECT)