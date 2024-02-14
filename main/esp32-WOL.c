#include "http-server.c"
#include "nvs_flash.h"
#include "nvs.h"
#include "rainmaker.c"


void app_main() {

   esp_err_t err = nvs_flash_init();
   if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
         ESP_ERROR_CHECK(nvs_flash_erase());
         err = nvs_flash_init();
   }
   ESP_ERROR_CHECK(err);
   
   nvs_handle_t nvs_handle;
   err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
   if (err != ESP_OK) {
      ESP_LOGE("Main", "Error opening NVS handle! %s\n", esp_err_to_name(err));
   } else {
      ESP_LOGI("Main", "NVS open");

      start_rainmaker(nvs_handle);
      http_server_start(nvs_handle);

   }
}
