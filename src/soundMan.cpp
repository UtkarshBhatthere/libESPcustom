#include "soundMan.hpp"

/** Default constructor when no param is provided, defined for mems microphone.
 ** 
 */
soundMan::soundMan()
{
    this->port      = i2s_PORT_0;
    this->channel   = I2S_CHANNEL_MONO;
    {   // Config Setup.
        this->config.bits_per_sample      = I2S_BITS_PER_SAMPLE_16BIT;
        this->config.channel_format       = I2S_CHANNEL_FMT_ONLY_RIGHT;
        this->config.communication_format = I2S_COMM_FORMAT_PCM;
        this->config.dma_buf_count        = 12;  // 12 DMA buffers.
        this->config.dma_buf_len          = 512; // 256 samples per buffer.
        this->config.use_apll             = false;
        this->config.communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB);
        this->config.intr_alloc_flags     = ESP_INTR_FLAG_LEVEL1;
        this->config.mode                 = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX);
        this->config.sample_rate          = standard_MIC_SAMPLE_RATE;
    }   // End Config Setup.

    {
        this->pinout.bck_io_num     = GPIO_NUM_22;
        this->pinout.data_in_num    = GPIO_NUM_19;
        this->pinout.ws_io_num      = GPIO_NUM_21;
        this->pinout.data_out_num   = -1;   // Not used.
    }

}

/** Parametrised Constructor when specifications are provided.
 **
 */
soundMan::soundMan(i2s_port_t param_port, i2s_channel_t param_channel, i2s_config_t param_config, i2s_pin_config_t param_pinout)
{
    this->pinout    = param_pinout;
    this->port      = param_port;
    this->config    = param_config;
    this->channel   = param_channel;
}

/** Initialiser
 **
 */
esp_err_t soundMan::init()
{
    esp_err_t err;
    // Installing Drivers for I2S.
    if((err = i2s_driver_install(this->port, &this->config, 0, NULL)) != ESP_OK){
        ESP_LOGE(soundManTag, "Error Installing drivers at I2S port: %d", this->port);
        return err;
    }
    // Setup for I2S pinblock.
    if((err = i2s_set_pin(this->port, &this->pinout)) != ESP_OK){
        ESP_LOGE(soundManTag, "Error Setting Pin to port: %d", this->port);
        return err;
    }
    // Initiating Clock Pulses.
    if((err = i2s_set_sample_rates(this->port, this->config.sample_rate/2)) != ESP_OK){
        ESP_LOGE(soundManTag, "Error setting SampleRate at I2S port: %d", this->port);
        return err;
    }
    // Starting I2S.
    if((err = i2s_start(this->port)) != ESP_OK){
        ESP_LOGE(soundManTag, "Error Installing I2S at port: %d", this->port);
        return err;
    }
    return ESP_OK;
}

/** To recieve Audio
 **
 */
esp_err_t soundMan::rx_audio()
{
    esp_err_t err;
    size_t bytes_read;
    uint16_t input_bfr[this->config.dma_buf_len*4+4];

    if((err = i2s_read(this->port, &input_bfr, 4*this->config.dma_buf_len, &bytes_read, 100)) != ESP_OK)
        ESP_LOGI(soundManTag, "Specified %d but read %d bytes.", this->config.dma_buf_len, bytes_read);

    if((err = this->post_receive_callback(input_bfr, bytes_read)) != ESP_OK){
        return err;
    }
    return ESP_OK;
}

/** To transmit Audio.
 **
 */
esp_err_t soundMan::tx_audio(uint16_t* op_bfr, uint16_t sample_len)
{
    esp_err_t err;
    size_t bytes_written = 0;
    
    if((err = i2s_write(this->port, op_bfr, sample_len, &bytes_written, 200)) != ESP_OK)
        ESP_LOGE(soundManTag, "Specified %d but wrote %d bytes.", sample_len, bytes_written);

    return err;
}

esp_err_t soundMan::attach_receive_cb(esp_err_t (*function)(uint16_t* ip_bfr, uint16_t sample_len))
{
    this->post_receive_callback = function;
    return ESP_OK;
}

esp_err_t soundMan::attach_transmit_cb(esp_err_t (*function)(uint16_t* ip_bfr, uint16_t sample_len))
{
    this->post_transmit_callback = function;
    return ESP_OK;
}