#include <stdio.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"


#define SSID CONFIG_WIFI_SSID
#define PASSWORD CONFIG_WIFI_PASSWORD


int retry_num=0;

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id,void *event_data){
	if(event_id == WIFI_EVENT_STA_START){
	  ESP_LOGW("WiFi", "WIFI CONNECTING....");
	}
	else if (event_id == WIFI_EVENT_STA_CONNECTED){
	  ESP_LOGI("WiFi", "WiFi CONNECTED");
	}
	else if (event_id == WIFI_EVENT_STA_DISCONNECTED){
	  ESP_LOGE("WiFi", "WiFi lost connection");
	  if(retry_num<5){
	  	esp_wifi_connect();
	  	retry_num++;
	  	ESP_LOGW("WiFi", "Retrying to Connect...");
	  }
	}
	else if (event_id == IP_EVENT_STA_GOT_IP){
	  ESP_LOGI("WiFi", "Wifi got IP");
	}
}

void wifi_connection(){
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation);    
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = "",
            .password = "",
            
           }
    
        };
    strcpy((char*)wifi_configuration.sta.ssid, SSID);
    strcpy((char*)wifi_configuration.sta.password, PASSWORD);    
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    esp_wifi_start();
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_connect();
    // ESP_LOGE("WiFi", "You are not connected to %s.\n\n",SSID);
    
}