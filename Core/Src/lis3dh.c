#include "lis3dh.h"
#include "main.h"
#include "spi.h"

#include <stdlib.h>

static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp,
        uint16_t len)
{
    /* Write multiple command */
    reg |= 0x40;
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(handle, &reg, 1, 1000);
    HAL_SPI_Transmit(handle, (uint8_t*) bufp, len, 1000);
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);

    return 0;
}

static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
        uint16_t len)
{
    /* Read multiple command */
    reg |= 0xC0;
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(handle, &reg, 1, 1000);
    HAL_SPI_Receive(handle, bufp, len, 1000);
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);

    return 0;
}
static bool Lis3dh_Read_Accleration(struct Lis3dh_dev *dev, float acceleration_mg[3])
{
    lis3dh_reg_t reg;
    // read output only if new value available
    lis3dh_xl_data_ready_get(dev->dev_ctx, &reg.byte);

    if (!reg.byte) {
        return false;
    }

    int16_t data_raw_acceleration[3] = { 0 };
    /* Read accelerometer data */
    lis3dh_acceleration_raw_get(dev->dev_ctx, data_raw_acceleration);
    acceleration_mg[0] =
        lis3dh_from_fs2_hr_to_mg(data_raw_acceleration[0]);
    acceleration_mg[1] =
        lis3dh_from_fs2_hr_to_mg(data_raw_acceleration[1]);
    acceleration_mg[2] =
        lis3dh_from_fs2_hr_to_mg(data_raw_acceleration[2]);


    return true;
}

static bool Lis3dh_Read_Temperature(struct Lis3dh_dev *dev, float *temperature_degC)
{
    lis3dh_reg_t reg;
    // read output only if new value available
    lis3dh_temp_data_ready_get(dev->dev_ctx, &reg.byte);

    if (!reg.byte) {
        return false;
    }

    int16_t data_raw_temperature = 0;
    /* Read temperature data */
    lis3dh_temperature_raw_get(dev->dev_ctx, &data_raw_temperature);
    *temperature_degC = lis3dh_from_lsb_hr_to_celsius(data_raw_temperature);

    return true;
}

Lis3dh_dev_t *Lis3dh_New()
{
    stmdev_ctx_t *dev_ctx = malloc(sizeof(stmdev_ctx_t));
    dev_ctx->read_reg = platform_read;
    dev_ctx->write_reg = platform_write;
    dev_ctx->handle = &hspi1;

    Lis3dh_dev_t *dev = malloc(sizeof(Lis3dh_dev_t));
    dev->dev_ctx = dev_ctx;
    dev->Read_Acceleration = Lis3dh_Read_Accleration;
    dev->Read_Temperature = Lis3dh_Read_Temperature;

    // check device ID
    uint8_t whoamI;
    lis3dh_device_id_get(dev_ctx, &whoamI);

    if (whoamI != LIS3DH_ID) {
        while (1) {
            // device is not found
        }
    }

    /* Enable Block Data Update. */
    lis3dh_block_data_update_set(dev_ctx, PROPERTY_ENABLE);
    /* Set Output Data Rate to 1Hz. */
    lis3dh_data_rate_set(dev_ctx, LIS3DH_ODR_1Hz);
    /* Set full scale to 2g. */
    lis3dh_full_scale_set(dev_ctx, LIS3DH_2g);
    /* Enable temperature sensor. */
    lis3dh_aux_adc_set(dev_ctx, LIS3DH_AUX_ON_TEMPERATURE);
    /* Set device in continuous mode with 12 bit resol. */
    lis3dh_operating_mode_set(dev_ctx, LIS3DH_HR_12bit);

    return dev;
}

void Lis3dh_Free(Lis3dh_dev_t *dev)
{
    free(dev->dev_ctx);
    free(dev);
}
