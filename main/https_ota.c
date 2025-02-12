/* HTTPS OTA */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"

#if CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
#include "esp_efuse.h"
#endif

#if CONFIG_EXAMPLE_CONNECT_WIFI
#include "esp_wifi.h"
#endif

static const char *TAG = "https_ota";
extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_ca_cert_pem_end");

#define OTA_URL_SIZE 256
#define OTA_CHECK_DELAY 300 // in seconds

static esp_err_t validate_image_header(esp_app_desc_t *new_app_info)
{
    if (new_app_info == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_app_desc_t running_app_info;
    if (esp_ota_get_partition_description(running, &running_app_info) == ESP_OK)
    {
        ESP_LOGI(TAG, "Running firmware version: %s", running_app_info.version);
    }

#ifndef CONFIG_SKIP_VERSION_CHECK
    if (memcmp(new_app_info->version, running_app_info.version, sizeof(new_app_info->version)) == 0)
    {
        ESP_LOGW(TAG, "Current running version is the same as a new. We will not continue the update.");
        return ESP_FAIL;
    }
#endif

#ifdef CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
    /**
     * Secure version check from firmware image header prevents subsequent download and flash write of
     * entire firmware image. However this is optional because it is also taken care in API
     * esp_https_ota_finish at the end of OTA update procedure.
     */
    const uint32_t hw_sec_version = esp_efuse_read_secure_version();
    if (new_app_info->secure_version < hw_sec_version)
    {
        ESP_LOGW(TAG, "New firmware security version is less than eFuse programmed, %d < %d", new_app_info->secure_version, hw_sec_version);
        return ESP_FAIL;
    }
#endif

    return ESP_OK;
}

static esp_err_t _http_client_init_cb(esp_http_client_handle_t http_client)
{
    esp_err_t err = ESP_OK;
    /* Uncomment to add custom headers to HTTP request */
    // err = esp_http_client_set_header(http_client, "Custom-Header", "Value");
    return err;
}

void ota_task(void *pvParameter)
{
    // Initialize NVS.
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // 1.OTA app partition table has a smaller NVS partition size than the non-OTA
        // partition table. This size mismatch may cause NVS initialization to fail.
        // 2.NVS partition contains data in new format and cannot be recognized by this version of code.
        // If this happens, we erase NVS partition and initialize NVS again.
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
    */
    ESP_ERROR_CHECK(example_connect());

#if defined(CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE) && defined(CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK)
    /**
     * We are treating successful WiFi connection as a checkpoint to cancel rollback
     * process and mark newly updated firmware image as active. For production cases,
     * please tune the checkpoint behavior per end application requirement.
     */
    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_ota_img_states_t ota_state;
    if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK)
    {
        if (ota_state == ESP_OTA_IMG_PENDING_VERIFY)
        {
            if (esp_ota_mark_app_valid_cancel_rollback() == ESP_OK)
            {
                ESP_LOGI(TAG, "App is valid, rollback cancelled successfully");
            }
            else
            {
                ESP_LOGE(TAG, "Failed to cancel rollback");
            }
        }
    }
#endif
    while (1)
    {

#if CONFIG_EXAMPLE_CONNECT_WIFI
        /* Ensure to disable any WiFi power save mode, this allows best throughput
     * and hence timings for overall OTA operation.
     */
        esp_wifi_set_ps(WIFI_PS_NONE);

#endif // CONFIG_EXAMPLE_CONNECT_WIFI
        ESP_LOGI(TAG, "Starting Advanced OTA");

        esp_err_t ota_finish_err = ESP_OK;
        esp_http_client_config_t config = {
            .url = CONFIG_FIRMWARE_UPGRADE_URL,
            .cert_pem = (char *)server_cert_pem_start,
            .timeout_ms = CONFIG_OTA_RECV_TIMEOUT,
            .keep_alive_enable = true,
        };

#ifdef CONFIG_FIRMWARE_UPGRADE_URL_FROM_STDIN
        char url_buf[OTA_URL_SIZE];
        if (strcmp(config.url, "FROM_STDIN") == 0)
        {
            example_configure_stdin_stdout();
            fgets(url_buf, OTA_URL_SIZE, stdin);
            int len = strlen(url_buf);
            url_buf[len - 1] = '\0';
            config.url = url_buf;
        }
        else
        {
            ESP_LOGE(TAG, "Configuration mismatch: wrong firmware upgrade image url");
            abort();
        }
#endif

#ifdef CONFIG_SKIP_COMMON_NAME_CHECK
        config.skip_cert_common_name_check = true;
#endif

        esp_https_ota_config_t ota_config = {
            .http_config = &config,
            .http_client_init_cb = _http_client_init_cb, // Register a callback to be invoked after esp_http_client is initialized
        };

        esp_https_ota_handle_t https_ota_handle = NULL;
        err = esp_https_ota_begin(&ota_config, &https_ota_handle);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "ESP HTTPS OTA Begin failed");
            vTaskDelete(NULL);
        }

        esp_app_desc_t app_desc;
        err = esp_https_ota_get_img_desc(https_ota_handle, &app_desc);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "esp_https_ota_read_img_desc failed");
            goto ota_end;
        }
        err = validate_image_header(&app_desc);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "image header verification failed");
            goto ota_end;
        }

        while (1)
        {
            err = esp_https_ota_perform(https_ota_handle);
            if (err != ESP_ERR_HTTPS_OTA_IN_PROGRESS)
            {
                break;
            }
            // esp_https_ota_perform returns after every read operation which gives user the ability to
            // monitor the status of OTA upgrade by calling esp_https_ota_get_image_len_read, which gives length of image
            // data read so far.
            ESP_LOGD(TAG, "Image bytes read: %d", esp_https_ota_get_image_len_read(https_ota_handle));
        }

        if (esp_https_ota_is_complete_data_received(https_ota_handle) != true)
        {
            // the OTA image was not completely received and user can customise the response to this situation.
            ESP_LOGE(TAG, "Complete data was not received.");
        }
        else
        {
            ota_finish_err = esp_https_ota_finish(https_ota_handle);
            if ((err == ESP_OK) && (ota_finish_err == ESP_OK))
            {
                ESP_LOGI(TAG, "ESP_HTTPS_OTA upgrade successful. Rebooting ...");
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                esp_restart();
            }
            else
            {
                if (ota_finish_err == ESP_ERR_OTA_VALIDATE_FAILED)
                {
                    ESP_LOGE(TAG, "Image validation failed, image is corrupted");
                }
                ESP_LOGE(TAG, "ESP_HTTPS_OTA upgrade failed 0x%x", ota_finish_err);
                // vTaskDelete(NULL);
            }
        }

    ota_end:
        esp_https_ota_abort(https_ota_handle);
        ESP_LOGE(TAG, "ESP_HTTPS_OTA upgrade failed");

        vTaskDelay(OTA_CHECK_DELAY * 1000 / portTICK_PERIOD_MS);
    }
}
