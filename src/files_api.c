#include "files_essentials.h"

/* API's */
void filesSystemInit(void)
{
    esp_vfs_spiffs_conf_t fs = {
        .base_path = "/spiffs",  // Will be used to store fonts.
        .format_if_mount_failed = true,
        .max_files = 5,         // Not many files for fonts.
        .partition_label = NULL // Investigate what other options do we have other than NULL.
    };

    esp_err_t ret = esp_vfs_spiffs_register(&fs);
    assert(ret == ESP_OK);

    size_t total=0, used=0;
    ret = esp_spiffs_info(NULL, &total, &used);
    assert(ret == ESP_OK);
    ESP_LOGI("MOUNT", "System Mount Successfull: { %d : Total, %d : Used}", total, used);
}

void filesSystemDeInit(void)
{
    esp_vfs_spiffs_unregister(NULL);
}

int filesBufferLength(uint8_t* buff)
{
    uint8_t* temp = buff;
    int length = 0; 
    while(*temp++ != '\0'){
        length++;
    }
    return length;
}


uint8_t* filesRead(char *fname)
{
    
    int res = 0;
	FILE *fd = fopen(fname, "rb");
    if (fd == NULL) {
        ESP_LOGE("[read]", "fopen failed");
        return NULL;
    }
    ESP_LOGI("[read]", "Entered SEEK");
    fseek(fd, 0, SEEK_END);
    int position = ftell(fd)+10;
    ESP_LOGI("[read]", "Position : %d", position);
    fseek(fd, 0, SEEK_SET);
    uint8_t* temp = (uint8_t*)malloc(position*sizeof(uint8_t));
    if(temp){
        ESP_LOGI("[read]", "Reading");
        res = fread(temp, 1, position - 1, fd);
        ESP_LOGI("[read]", "Reading Done");
        *(temp + position) = 0;
    }
    if (res <= 0) {
        ESP_LOGE("[read]", "fread failed: %d", res);
        res = fclose(fd);
        if (res) {
            ESP_LOGE("[read]", "fclose failed: %d", res);
            return NULL;
        }
        return NULL;
    }
	res = fclose(fd);
	if (res) {
        ESP_LOGE("[read]", "fclose failed: %d", res);
    	return NULL;
	}
    // int len = filesBufferLength(temp);
    // printf("The length of temp is %d", len);
    return temp;
}

esp_err_t filesWrite(char *fname, uint8_t* buff)
{
    ESP_LOGI("[write]", "Entered API");
    FILE *fd = fopen(fname, "w");
    if (fd == NULL) {
        ESP_LOGE("[write]", "fopen failed");
    	return -1;
    }
    ESP_LOGI("[write]", "Reading Buffer Length");
    int len = filesBufferLength(buff);
	int res = fwrite(buff, 1, len, fd);
    ESP_LOGI("[write]", "Done Writing.");
	if (res != len) {
        ESP_LOGE("[write]", "fwrite failed: %d <> %d ", res, len);
        res = fclose(fd);
        if (res) {
            ESP_LOGE("[write]", "fclose failed: %d", res);
            return -2;
        }
        return -3;
    }
	res = fclose(fd);
	if (res) {
        ESP_LOGE("[write]", "fclose failed: %d", res);
    	return -4;
	}
    return 0;
}