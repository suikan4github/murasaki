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

    ret_val = this->GetSampleDataSizeTx();

    switch (ret_val)
    {
        case 2:
            break;
        case 3:
            ret_val = 4;
            break;
        case 4:
            break;
        default:
            MURASAKI_ASSERT(false)
    }

    return ret_val;
}

/*
 * Convert from the data size in the I2S for the data size on memory;
 * truncate the size to the power of two.
 */
unsigned int AudioPortAdapterStrategy::GetSampleWordSizeRx()
{
    unsigned int ret_val;

    ret_val = this->GetSampleDataSizeRx();

    switch (ret_val)
    {
        case 2:
            break;
        case 3:
            ret_val = 4;
            break;
        case 4:
            break;
        default:
            MURASAKI_ASSERT(false)
    }

    return ret_val;
}

} /* namespace murasaki */

