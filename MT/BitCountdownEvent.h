/* 
 * File:   BitCountdownEvent.h
 * Author: 0007989
 *
 * Created on 2015年1月14日, 下午 6:04
 * Test 2
 * Test 2
 * Test 3 Modify
 */

#ifndef UTILITY_BITCOUNTDOWNEVENT_H
#define	UTILITY_BITCOUNTDOWNEVENT_H

#include <mutex>
#include "ManualResetEvent.h"

namespace ProgLab{ namespace Utility{

/// \brief 將每個bit視為一個資源，當所有資源都完成(每個bit都設為0)時，就會觸發
class BitCountdownEvent {
public:
    BitCountdownEvent();    
    virtual ~BitCountdownEvent();
    
    /// \brief 重設
    /// \param bitValue 重設後的值，有需要等候的bit就設為1
    /// \example Reset( 0x000F ) 設定後四個bit為1，表示等候四個資源完成
    void Reset( int bitValue );
    /// \brief 設定第N個bit為0
    /// \param idxBit 要設為0的第N個bit
    void Set( int idxBit );
    /// \brief 等候所有bit均為0
    /// \param waitMS 要等待的時間(單位為minisecond)。預設值為0，表示永久等待
    /// \return true or false(表示timeout)
    bool Wait( int waitMS = 0 );
    /// \brief 取得目前的狀態
    /// \return 目前的狀態值
    unsigned int GetBitValue() const{ return m_BitValue; }
private:
    mutable std::mutex m_LockBitValue;      //!< 變更狀態值時使用的lock物件
    unsigned int m_BitValue;                //!< 目前狀態值
    ManualResetEvent m_Event;               //!< 用來等候的事件
};

}
#endif	/* UTILITY_BIDCOUNTDOWNEVENT_H */

