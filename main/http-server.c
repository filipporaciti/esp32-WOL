#include <esp_http_server.h>
#include <stdio.h>
#include "wol.c"
#include "nvs.h"
#include "esp_wifi.h"
#include "esp_spiffs.h"


char home_http_site[1300];


#define DESTINATION_MAC_ADDRESS CONFIG_DESTINATION_MAC_ADDRESS
char dest_mac_addr[12] = DESTINATION_MAC_ADDRESS;


void get_index_site() {
    
    esp_vfs_spiffs_conf_t spiffs = {
        .base_path = "/storage",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = false
    };

    esp_err_t ret = esp_vfs_spiffs_register(&spiffs);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE("Http server", "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE("Http server", "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE("Http server", "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE("Http server", "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI("Http server", "Partition size: total: %d, used: %d", total, used);
    }

    // Open a file in read mode
    FILE* f = fopen("/storage/index.html", "r");
    if (f == NULL) {
        ESP_LOGE("Http server", "Failed to open index.html");
        return;
    }

    memset(home_http_site, 0, sizeof(home_http_site));
    fread(home_http_site, 1, sizeof(home_http_site), f);

    fclose(f);
}


void write_mac_address(nvs_handle_t nvs_handle, char new_mac[]) {
	esp_err_t err;

	err = nvs_set_str(nvs_handle, "dest mac addr", new_mac);
    if (err != ESP_OK) {
    	ESP_LOGE("Http server", "Error (%s) NVS writing!\n", esp_err_to_name(err));
    	return;
    }

    ESP_LOGI("Http server", "NVS set destination address");
    err = nvs_commit(nvs_handle);
    ESP_LOGI("Http server", "NVS commit");
    if (err != ESP_OK) {
    	ESP_LOGE("Http server", "Error (%s) NVS commit!\n", esp_err_to_name(err));
    	return;
    }

    memcpy(dest_mac_addr, new_mac, sizeof(dest_mac_addr));

}


esp_err_t home_get_handler(httpd_req_t *req){

	ESP_LOGI("Http server", "Http request: \n\turl: \"/\" \n\tmethod: GET\n");

	esp_err_t err = httpd_resp_send(req, home_http_site, HTTPD_RESP_USE_STRLEN);
	if (err == ESP_OK){
       	ESP_LOGI("Http server", "Http response sent (url: /, type: GET)");
	}
	return err;
}
esp_err_t home_post_handler(httpd_req_t *req){
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

	ESP_LOGI("Http server", "Http request: \n\turl: \"/\" \n\tmethod: POST\n");
    send_wol_packet(dest_mac_addr);


	esp_err_t err = httpd_resp_send(req, "OK!", HTTPD_RESP_USE_STRLEN);
	if (err != ESP_OK) {
    	ESP_LOGE("Http server", "Error (%s) send response (url: /, type: POST)", esp_err_to_name(err));
	} else {
    	ESP_LOGI("Http server", "Http response sent (url: /, type: POST)");
	}
	return err;
}


esp_err_t mac_get_handler(httpd_req_t *req){
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Content-Type", "application/json");

    ESP_LOGI("Http server", "Http request: \n\turl: \"/mac\" \n\tmethod: GET\n");

    char resp[25];

    sprintf(resp, "{\"mac\": \"%.12s\"}", dest_mac_addr);
	esp_err_t err = httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
	if (err == ESP_OK){
       	ESP_LOGI("Http server", "Http response sent (url: /mac, type: GET)");

	}
	return err;
}
esp_err_t mac_post_handler(httpd_req_t *req){
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

	ESP_LOGI("Http server", "Http request: \n\turl: \"/mac\" \n\tmethod: POST\n");
	char str_resp[12];

	esp_err_t err;
	nvs_handle_t nvs_handle;
   	err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE("Http server", "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    	memcpy(str_resp, "Error", 5);
    } else {
    	ESP_LOGI("Http server", "NVS open");

		char content[12];


	    size_t recv_size = req->content_len;
	    
        if (recv_size != 12) {
            ESP_LOGE("Http server", "Wring request data length (length != 12)");
            err = httpd_resp_send(req, "Error: len != 12", HTTPD_RESP_USE_STRLEN);
            if (err != ESP_OK) {
                ESP_LOGE("Http server", "Error (%s) send response (url: /mac, type: POST)", esp_err_to_name(err));
            } else {
                ESP_LOGI("Http server", "Http response sent (url: /mac, type: POST)");
            }
            return ESP_FAIL;
        }

	    int ret = httpd_req_recv(req, content, recv_size);
	    if (ret <= 0) { 
	        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
	            httpd_resp_send_408(req);
	        }
	        return ESP_FAIL;
	    }

	    write_mac_address(nvs_handle, content);

    	memcpy(str_resp, content, sizeof(content));
	}

	err = httpd_resp_send(req, str_resp, HTTPD_RESP_USE_STRLEN);
	if (err != ESP_OK) {
    	ESP_LOGE("Http server", "Error (%s) send response (url: /mac, type: POST)", esp_err_to_name(err));
	} else {
    	ESP_LOGI("Http server", "Http response sent (url: /mac, type: POST)");
	}

	return err;
}
esp_err_t mac_options_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "*");

    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}


void update_mac_address(nvs_handle_t nvs_handle) {
	esp_err_t err;
	char possible_mac[12];
	size_t string_size;
	err = nvs_get_str(nvs_handle, "dest mac addr", NULL, &string_size);
	err = nvs_get_str(nvs_handle, "dest mac addr", &possible_mac, &string_size);
	switch (err) {
        case ESP_OK:
        	memcpy(dest_mac_addr, possible_mac, sizeof(dest_mac_addr));
        	ESP_LOGI("Http server", "NVS get destination MAC address: %.12s", dest_mac_addr);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            write_mac_address(nvs_handle, dest_mac_addr);
            break;
        default :
            ESP_LOGE("Http server", "Error (%s) NVS reading!\n", esp_err_to_name(err));
            break;
    }
    nvs_close(nvs_handle);
}



void http_server_start(nvs_handle_t nvs_handle){

    get_index_site();
	update_mac_address(nvs_handle);

	httpd_uri_t home_get_uri = {
       .uri      = "/",
       .method   = HTTP_GET,
       .handler  = home_get_handler,
       .user_ctx = NULL
   };
   httpd_uri_t home_post_uri = {
       .uri      = "/",
       .method   = HTTP_POST,
       .handler  = home_post_handler,
       .user_ctx = NULL
   };


   httpd_uri_t mac_get_uri = {
       .uri      = "/mac",
       .method   = HTTP_GET,
       .handler  = mac_get_handler,
       .user_ctx = NULL
   };
   httpd_uri_t mac_post_uri = {
       .uri      = "/mac",
       .method   = HTTP_POST,
       .handler  = mac_post_handler,
       .user_ctx = NULL
   };


   httpd_uri_t mac_options_uri = {
       .uri      = "/*",
       .method   = HTTP_OPTIONS,
       .handler  = mac_options_handler,
       .user_ctx = NULL
   };


   httpd_config_t config = HTTPD_DEFAULT_CONFIG();
   httpd_handle_t server = NULL;
   if (httpd_start(&server, &config) == ESP_OK) {
       httpd_register_uri_handler(server, &home_get_uri);
       httpd_register_uri_handler(server, &home_post_uri);

       httpd_register_uri_handler(server, &mac_get_uri);
       httpd_register_uri_handler(server, &mac_post_uri);
       httpd_register_uri_handler(server, &mac_options_uri);

   }
}
