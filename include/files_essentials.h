#ifndef _FILES_ESSENTIALS_H_
#define _FILES_ESSENTIALS_H_

/* Includes */
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "string.h"

/* API's */
void filesSystemInit(void);
void filesSystemDeInit(void);
int filesBufferLength(uint8_t* buff);
uint8_t* filesRead(char *fname);
esp_err_t filesWrite(char *fname, uint8_t* buff);

/* STORE handler API's */
void storeHandlerOne(void* param);
void storeHandlerTwo(void* param);

#endif //_FILES_ESSENTIALS_H_