/* 
 * File:   ResourceAccessor.h
 * Author: 0007989
 *
 * Created on 2014年5月6日, 上午 11:56
 */

#ifndef UTILITY_RESOURCEACCESSOR_H
#define	UTILITY_RESOURCEACCESSOR_H

#include <mutex>

namespace ProgLab{ namespace Utility{   
    
template<typename TResource >
class ResourceAccessor{
public:
    ResourceAccessor(){
        m_IsLocked = false;
    }
    virtual ~ResourceAccessor(){}
    
    bool Request(){
        if( m_IsLocked )
            return false;
        
        m_LockResource.lock();
        m_IsLocked = true;
        return true;
    }
    
    void Release(){
        if( m_IsLocked )
        {
            m_IsLocked = false;
            m_LockResource.unlock();
        }
    }
    
    TResource& GetResource(){
        return m_Resource;
    }
    
private:
    std::mutex m_LockResource;
    TResource m_Resource;
    bool m_IsLocked;
        
};    

}


#endif	/* UTILITY_RESOURCEACCESSOR_H */

