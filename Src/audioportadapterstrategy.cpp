/**
 * @file audioportadapterstrategy.cpp
 *
 *  @date 2020/02/25
 *  @author Seiichi "Suikan" Horie
 *
 *  @brief Strategy of the Audio device adaptor.
 */

#include "audioportadapterstrategy.hpp"
#include "murasaki_assert.hpp"

namespace murasaki {

/*
 * Convert from the data size in the I2S for the data size on memory;
 * truncate the size to the power of two.
 */
unsigned int AudioPortAdapterStrategy::GetSampleWordSizeTx()
{
    unsigned int ret_val;

    unsigned int bits = this->GetSampleDataSizeTx();

    if (bits <= 16)
        ret_val = 2;
    else if (bits <= 24)
        ret_val = 3;
    else
        ret_val = 4;

    return ret_val;
}

/*
 * Convert from the data size in the I2S for the data size on memory;
 * truncate the size to the power of two.
 */
unsigned int AudioPortAdapterStrategy::GetSampleWordSizeRx()
{
    unsigned int ret_val;

    unsigned int bits = this->GetSampleDataSizeRx();

    if (bits <= 16)
        ret_val = 2;
    else if (bits <= 24)
        ret_val = 3;
    else
        ret_val = 4;

    return ret_val;
}

} /* namespace murasaki */

