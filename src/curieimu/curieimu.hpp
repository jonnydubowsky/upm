/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Author: Ron Evans (@deadprogram)
 * Author: Justin Zemlyansky (@JustInDevelopment)
 * Copyright (c) 2016 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#pragma once

#include <mraa/firmata.h>
#include <queue>

namespace upm {

/**
 * @brief CurieIMU sensor for Genuino 101 running Firmata
 * @defgroup curieimu libupm-curieimu
 * @ingroup firmata
 */

/**
 * @library curieimu
 * @sensor curieimu
 * @comname Curie IMU sensor over Genuino 101 running Firmata
 * @altname Curie Firmata IMU
 * @type firmata
 * @man firmata imu genuino
 * @con firmata
 *
 * @brief API for the Curie IMU via Firmata
 *
 * Curie IMU is a 6-axxis acclerometer
 *
 * This module has been tested on an Genuino 101 running ConfigurableFirmata with CurieIMU
 *
 * @snippet curieimu.cxx Interesting
 */

#define FIRMATA_START_SYSEX                 0xF0
#define FIRMATA_END_SYSEX                   0xF7
#define FIRMATA_CURIE_IMU                   0x11
#define FIRMATA_CURIE_IMU_READ_ACCEL        0x00
#define FIRMATA_CURIE_IMU_READ_GYRO         0x01
#define FIRMATA_CURIE_IMU_READ_TEMP         0x02
#define FIRMATA_CURIE_IMU_SHOCK_DETECT      0x03
#define FIRMATA_CURIE_IMU_STEP_COUNTER      0x04
#define FIRMATA_CURIE_IMU_TAP_DETECT        0x05
#define FIRMATA_CURIE_IMU_READ_MOTION       0x06

struct IMUDataItem {
    int axis;
    int direction;
};

class CurieImu {
    public:

      /**
      * Instantiates a CurieImu object
      *
      * @param subplatformoffset Subplatform offset
      */
      CurieImu (int subplatform_offset=512);

      /**
      * Destructor for CurieImu object
      */
      ~CurieImu();

      /**
       */
      void updateAccel();

      /**
       *
       */
      void updateGyro();

      /**
       *
       */
      void updateMotion();

      /**
       * Read accelerometer X, Y, and Z axis
       *
       * @param xVal Pointer to returned X-axis value
       * @param yVal Pointer to returned Y-axis value
       * @param zVal Pointer to returned Z-axis value
       */
      int16_t* getAccel();

      /**
       * Read gyroscope X, Y, and Z axis
       *
       * @param xVal Pointer to returned X-axis value
       * @param yVal Pointer to returned Y-axis value
       * @param zVal Pointer to returned Z-axis value
       */
      int16_t* getGyro();

      /**
       * Reads the internal temperature
       *
       * @return 16-bit integer containing the scaled temperature reading
       */
      int16_t getTemperature();

      /**
       * Reads the X, Y, and Z axis of both gyroscope and accelerometer
       *
       * @param xA Pointer to returned X-axis value of accelerometer
       * @param yA Pointer to returned Y-axis value of accelerometer
       * @param zA Pointer to returned Z-axis value of accelerometer
       * @param xG Pointer to returned X-axis value of Gyroscope
       * @param yG Pointer to returned Y-axis value of Gyroscope
       * @param zG Pointer to returned Z-axis value of Gyroscope
       */
      int16_t* getMotion();

      /**
       * Turns shock detection notifications on/off
       *
       * @param enable enables/disables notifications
       */
      void enableShockDetection(bool enable);

      /**
       * Has there been a shock detected?
       *
       * @return true if any unprocessed shock notifications are in the queue
       */
      bool isShockDetected();

      /**
       * Gets shock detect data from queue
       *
       * @param axis gets axis data
       * @param direction gets direction data
       */
      void getShockDetectData(int *axis, int *direction);

      /**
       * Turns step counter notifications on/off
       *
       * @param enable enables/disables notifications
       */
      void enableStepCounter(bool enable);

      /**
       * Has there been a step detected?
       *
       * @return true if any unprocessed step notifications are in the queue
       */
      bool isStepDetected();

      /**
       * Gets step count data from queue
       *
       * @param count the total number of steps taken
       */
      void getStepCount(int *count);

      /**
       * Turns tap detection notifications on/off
       *
       * @param enable enables/disables notifications
       */
      void enableTapDetection(bool enable);

      /**
       * Has there been a tap detected?
       *
       * @return true if any unprocessed tap notifications are in the queue
       */
      bool isTapDetected();

      /**
       * Gets tap detect data from queue
       *
       * @param axis gets axis data
       * @param direction gets direction data
       */
      void getTapDetectData(int *axis, int *direction);

      /**
       * Locks responses from Firmata
       */
      void lock();

      /**
       * Unlocks responses from Firmata
       */
      void unlock();

      /**
       * Wait for a response from Firmata before proceeding
       */
      void waitForResponse();

      /**
       * Proceed with original function call now that response
       * from Firmata has been received
       */
      void proceed();

      /**
       * Set results being returned from Firmata for processing
       *
       * @param buf is the buffer
       * @param length is the length of results buffer
       */
      void setResults(uint8_t* buf, int length);

      /**
       * Processes asyncronous responses returned from Firmata
       */
      void processResponse();

    private:
        mraa_firmata_context m_firmata;
        pthread_mutex_t m_responseLock;
        pthread_cond_t m_responseCond;
        char* m_results;

        std::queue<IMUDataItem*> m_shockData;
        std::queue<int> m_stepData;
        std::queue<IMUDataItem*> m_tapData;

        int16_t accel[3];
        int16_t gyro[3];
        int16_t motion[6];
};

}