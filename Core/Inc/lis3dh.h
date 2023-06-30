#ifndef __LIS3DH_H__
#define __LIS3DH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lis3dh_reg.h"

#include <stdbool.h>

typedef struct Lis3dh_dev {
    stmdev_ctx_t *dev_ctx;

    bool (*Read_Acceleration)(struct Lis3dh_dev *dev, float acceleration_mg[3]);
    bool (*Read_Temperature)(struct Lis3dh_dev *dev, float *temperature_degC);
} Lis3dh_dev_t;

Lis3dh_dev_t *Lis3dh_New();
void Lis3dh_Free(Lis3dh_dev_t *dev);

#endif
