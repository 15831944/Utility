/* 
 * File:   SingletonBase.h
 * Author: Justin Lin <hsuehwen.lin@kgi.com>
 *
 * Created on 2015年3月10日, 下午 3:46
 */

#ifndef UTILITY_SINGLETONBASE_H
#define	UTILITY_SINGLETONBASE_H

namespace ProgLab{ namespace Utility{   
    
/// \brief 讓Class變成是Singleton模式的基底類別
template<typename T>
class SingletonBase{
public:
    virtual ~SingletonBase(){}
    
    static T& GetInstance(){
        if( m_pInstance == NULL ){
            std::lock_guard<std::mutex> al(m_LockInstance);
            if( m_pInstance == NULL ){
                m_pInstance = new T();
            }
        }
        
        return *m_pInstance;
    }
    
    static void Terminate(){
        std::lock_guard<std::mutex> al( m_LockInstance );
        if( m_pInstance != NULL )
            delete m_pInstance;
        m_pInstance = NULL;
    }
protected:
    SingletonBase(){}
    SingletonBase( const SingletonBase& orig ) = delete;
    
protected:    
    static std::mutex m_LockInstance; 
    static T* m_pInstance;
};

template<typename T> std::mutex SingletonBase<T>::m_LockInstance;
template<typename T> T* SingletonBase<T>::m_pInstance = NULL;

}

#endif	/* UTILITY_SINGLETONBASE_H */

