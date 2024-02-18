#include "lwip/sockets.h"
#include "esp_log.h"


/* Send Wake On Lan packet to d_mac (parameter) target */
esp_err_t send_wol_packet(char d_mac[12]) {

    char dest_mac[13];
    memset(dest_mac, 0, sizeof(dest_mac));
    memcpy(dest_mac, d_mac, sizeof(char)*12);

   	ESP_LOGI("WOL", "Sending WoL packet to %s", dest_mac);

    /* Create a socket */
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        ESP_LOGE("WOL", "Unable to create socket");
        return ESP_OK;
    }

    /* Setup the socket */
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(9); // port 9
    inet_pton(AF_INET, "255.255.255.255", &dest_addr.sin_addr.s_addr);

    // Wake On Lan packet
    uint8_t magic_packet[102];
    memset(magic_packet, 0xFF, 6);
    uint8_t target_mac[6];
    char slice[2];

    // Destination mac address from string to int
    for (int i = 0; i < 12; i+=2) {
    	slice[0] = dest_mac[i];
    	slice[1] = dest_mac[i+1];
    	target_mac[i/2] = (uint8_t)strtol(slice, NULL, 16);
    }

    // Add destination mac address to magic packet
    for (int i = 0; i < 16; i++) {
        memcpy(magic_packet + (i + 1) * 6, target_mac, 6);
    }

    // Send WoL packet
    if (sendto(sock, magic_packet, sizeof(magic_packet), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        ESP_LOGE("WOL", "Failed to send Wake-on-LAN packet");
    } else {
        ESP_LOGI("WOL", "Wake-on-LAN packet sent successfully");
    }

    close(sock);

    return ESP_OK;
}