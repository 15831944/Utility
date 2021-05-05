/* 
 * File:   BitCountdownEvent.cpp
 * Author: 0007989
 * 
 * Created on 2015年1月14日, 下午 6:04
 */

#include "BitCountdownEvent.h"

namespace ProgLab{ namespace Utility{

BitCountdownEvent::BitCountdownEvent() {
}

BitCountdownEvent::~BitCountdownEvent() {
}

void BitCountdownEvent::Reset( int bitValue ){
    std::unique_lock<std::mutex> al(m_LockBitValue);
    m_BitValue = bitValue;
}
    
void BitCountdownEvent::Set( int idxBit ){
    std::unique_lock<std::mutex> al(m_LockBitValue);
    m_BitValue &= ~( 1 << idxBit );
    if( m_BitValue == 0 )
        m_Event.Set(false);
}
    
bool BitCountdownEvent::Wait( int waitMS ){
    try{
        std::unique_lock<std::mutex> al(m_LockBitValue);
        if( m_BitValue == 0 )
            return true;
    }
    catch(...)
    {    
    }
    return m_Event.Wait( waitMS );
}

}