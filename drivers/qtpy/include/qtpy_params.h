/*
 * Copyright (C) 2021 University of Oslo
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_qtpy
 *
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      J. David Ibáñez <jdavid.ibp@gmail.com>
 */

#ifndef QTPY_PARAMS_H
#define QTPY_PARAMS_H

#include "board.h"
#include "qtpy.h"
#include "qtpy_constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */

#ifndef QTPY_PARAM_PIN
#define QTPY_PARAM_PIN 13
#endif

#ifndef QTPY_PARAM_ADDRESS
#define QTPY_PARAM_ADDRESS '5'
#endif

#ifndef QTPY_PARAMS
#define QTPY_PARAMS \
    { \
        .pin = QTPY_PARAM_PIN, \
        .address = QTPY_PARAM_ADDRESS, \
    }
#endif
/**@}*/

/**
 * @brief   Configuration struct
 */
static const qtpy_params_t qtpy_params[] =
{
    QTPY_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* QTPY_PARAMS_H */
/** @} */
