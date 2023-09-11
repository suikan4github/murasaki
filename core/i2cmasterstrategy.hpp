/**
 * \file i2cmasterstrategy.hpp
 *
 *  \date  2018/02/11
 *  \author: Seiichi "Suikan" Horie
 *  \brief Root class definition of the I2C Master.
 */

#ifndef I2CMASTERSTRATEGY_HPP_
#define I2CMASTERSTRATEGY_HPP_

#include "i2ccallbackstrategy.hpp"

namespace murasaki {
/**
 * \ingroup MURASAKI_ABSTRACT_GROUP
 * \brief Definition of the root class of I2C master.
 * \details
 * This member function is a prototype of the I2C master peripheral.

 * This prototype assumes the derived class will transmit/receive data in the
 task context on RTOS.
 * And these member functions should be synchronous.
 * That means, both member functions don't return until the transmit/receive
 terminates.

 * These two callback member functions synchronize with the interrupt,
 * which tells the end of Transmit/Receive.
 */

class I2cMasterStrategy : public murasaki::I2cCallbackStrategy {
 public:
  /**
   * @brief Thread safe, synchronous transmission over I2C.
   * @param addrs 7bit address of the I2C device.
   * @param tx_data Data array to transmit.
   * @param tx_size Data counts[bytes] to transmit.
   * @param transfered_count the count of the bytes transfered during the API
   * execution.
   * @param timeout_ms Time ou [mS]. By default, there is not timeout.
   * @return Result of the processing
   * @details
   * This member function is programmed to run in the task context of RTOS.
   * This function should be internally exclusive between multiple task access.
   * In other words, it should be thread-safe.
   */
  virtual murasaki::I2cStatus Transmit(
      unsigned int addrs, const uint8_t *tx_data, unsigned int tx_size,
      unsigned int *transfered_count = nullptr,
      unsigned int timeout_ms = murasaki::kwmsIndefinitely) = 0;
  /**
   * @brief Thread safe, synchronous receiving over I2C.
   * @param addrs 7bit address of the I2C device.
   * @param rx_data Data array to transmit.
   * @param rx_size Data counts[bytes] to transmit.
   * @param transfered_count the count of the bytes transfered during the API
   * execution.
   * @param timeout_ms Time ou [mS]. By default, there is not timeout.
   * @return Result of the processing
   * @details
   * This member function is programmed to run in the task context of RTOS.
   * This function should be internally exclusive between multiple task access.
   * In other words, it should be thread-safe.
   */
  virtual murasaki::I2cStatus Receive(
      unsigned int addrs, uint8_t *rx_data, unsigned int rx_size,
      unsigned int *transfered_count = nullptr,
      unsigned int timeout_ms = murasaki::kwmsIndefinitely) = 0;
  /**
   * @brief Thread safe, synchronous transmission and then receiving over I2C.
   * @param addrs 7bit address of the I2C device.
   * @param tx_data Data array to transmit.
   * @param tx_size Data counts[bytes] to transmit.
   * @param rx_data Data array to transmit.
   * @param rx_size Data counts[bytes] to transmit.
   * @param tx_transfered_count the count of the bytes transmitted during the
   * API execution.
   * @param rx_transfered_count the count of the bytes received during the API
   * execution.
   * @param timeout_ms Time ou [mS]. By default, there is not timeout.
   * @return Result of the processing
   * @details
   * First, this member function to transmit the data, and then, by repeated
   * start function, it receives data. The transmission device address and
   * receiving device address is the same.
   *
   * This member function is programmed to run in the task context of RTOS.
   * And also exclusive between the racing tasks. In other words, this function
   * is thread-safe.
   *
   */
  virtual murasaki::I2cStatus TransmitThenReceive(
      unsigned int addrs, const uint8_t *tx_data, unsigned int tx_size,
      uint8_t *rx_data, unsigned int rx_size,
      unsigned int *tx_transfered_count = nullptr,
      unsigned int *rx_transfered_count = nullptr,
      unsigned int timeout_ms = murasaki::kwmsIndefinitely) = 0;
};

} /* namespace murasaki */

#ifdef __MURASAKI_UTEST__

#include "gmock/gmock.h"
#include "gtest/gtest.h"

/**
 * @brief Test mock for the I2C master.
 * @details
 * Dedicated class for the Google Test / Google Mock. Use this class as stub
 * to test the client class of the Murasaki Library.
 *
 * To use this calss, define __MURASAKI_UTEST__. Usually, this macro
 * is defined in CMakeLists.txt as commandline predefined macro.
 */
class MockI2cMaster : public murasaki::I2cMasterStrategy {
 public:
  MOCK_METHOD5(Transmit,
               murasaki::I2cStatus(unsigned int addrs, const uint8_t *tx_data,
                                   unsigned int tx_size,
                                   unsigned int *transfered_count,
                                   unsigned int timeout_ms));
  MOCK_METHOD4(Transmit,
               murasaki::I2cStatus(unsigned int addrs, const uint8_t *tx_data,
                                   unsigned int tx_size,
                                   unsigned int *transfered_count));
  MOCK_METHOD3(Transmit,
               murasaki::I2cStatus(unsigned int addrs, const uint8_t *tx_data,
                                   unsigned int tx_size));

  MOCK_METHOD5(Receive,
               murasaki::I2cStatus(unsigned int addrs, uint8_t *rx_data,
                                   unsigned int rx_size,
                                   unsigned int *transfered_count,
                                   unsigned int timeout_ms));
  MOCK_METHOD4(Receive,
               murasaki::I2cStatus(unsigned int addrs, uint8_t *rx_data,
                                   unsigned int rx_size,
                                   unsigned int *transfered_count));
  MOCK_METHOD3(Receive,
               murasaki::I2cStatus(unsigned int addrs, uint8_t *rx_data,
                                   unsigned int rx_size));

  MOCK_METHOD8(TransmitThenReceive,
               murasaki::I2cStatus(unsigned int addrs, const uint8_t *tx_data,
                                   unsigned int tx_size, uint8_t *rx_data,
                                   unsigned int rx_size,
                                   unsigned int *tx_transfered_count,
                                   unsigned int *rx_transfered_count,
                                   unsigned int timeout_ms));
  MOCK_METHOD7(TransmitThenReceive,
               murasaki::I2cStatus(unsigned int addrs, const uint8_t *tx_data,
                                   unsigned int tx_size, uint8_t *rx_data,
                                   unsigned int rx_size,
                                   unsigned int *tx_transfered_count,
                                   unsigned int *rx_transfered_count));
  MOCK_METHOD6(TransmitThenReceive,
               murasaki::I2cStatus(unsigned int addrs, const uint8_t *tx_data,
                                   unsigned int tx_size, uint8_t *rx_data,
                                   unsigned int rx_size,
                                   unsigned int *tx_transfered_count));
  MOCK_METHOD5(TransmitThenReceive,
               murasaki::I2cStatus(unsigned int addrs, const uint8_t *tx_data,
                                   unsigned int tx_size, uint8_t *rx_data,
                                   unsigned int rx_size));

  MOCK_METHOD1(TransmitCompleteCallback, bool(void *ptr));
  MOCK_METHOD1(ReceiveCompleteCallback, bool(void *ptr));
  MOCK_METHOD1(HandleError, bool(void *ptr));

 protected:
  MOCK_METHOD0(GetPeripheralHandle, void *());
};

#endif  // __MURASAKI_UTEST__

#endif /* I2CMASTERSTRATEGY_HPP_ */
