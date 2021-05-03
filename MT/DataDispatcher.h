/* 
 * File:   DataDispatcher.h
 * Author: 0007989
 *
 * Created on 2014年4月18日, 上午 10:30
 */

#ifndef UTILITY_DATADISPATCHER_H
#define	UTILITY_DATADISPATCHER_H

#include <queue>
#include <stdio.h>
#include <mutex>
//#include "ThreadWrapper.h"
#include <thread>
#include <functional>
#include "../Collect/BlockingQueue.h"
#include "ManualResetEvent.h"
#include "../CommonException.h"

namespace KGI_TW_Der_Utility
{
   
/* \class DataSiapatcher
 * \brief Use another thread to send data to receiver
 */
template < typename TData>
class DataDispatcher {  
public:
    class DataDispatcherReceiver{
    public:
        /* \brief for derived class to override to receive data from DataDispatcher
         * \param data the data that sent from DataDispatcher
         */        
        virtual void OnReceiveData( TData &data ) = 0;
    };
public:
    DataDispatcher():/*m_DispatchThread(Dispatcher),*/m_IsRunning(false),m_pReceiver(NULL),m_NotifyFlags(0){}
    virtual ~DataDispatcher(){}
    
    /* \brief Start dispatcher and specify the receiver
     * \param pReceiver the receiver for rceiving data and inherited from inner class DataDispatcherReceiver
     */
    void Start( DataDispatcherReceiver *pReceiver ){
        if( m_IsRunning )
            throw CommonException( __LINE__, __FILE__, "[DataDispatcher] is running could not be start twice");
        m_IsRunning = true;
        m_pReceiver = pReceiver;
        
        //m_DispatchThread.Start( this );
        std::thread tDispatch( std::bind( &DataDispatcher::Dispatcher, this ) );
        tDispatch.detach();
    }
    
    /*
     * \brief Stop dispatcher
     * 
     */
    void Stop(){
        
        SetNotifyFlags( NotifyFlags_Stop );
        m_EventNotify.Set( false );
        
        //m_DispatchThread.Stop();
        m_IsRunning = false;
    }
    
    /*
     * \brief add data into queue and dispatcher will dispatch it to receiver
     * \param data will be dispatched data
     */
    void Add( const TData& data )
    {        
        try
        {
            std::unique_lock<std::mutex> al(m_LockQueue);
            m_DataQueue.push( data );
        }
        catch( ... )
        {            
        }
        
        // Notify dispatch thread to dispatch data in queue
        SetNotifyFlags( NotifyFlags_DataIn );
        m_EventNotify.Set( false );
    }
    
    inline bool IsRunning() const{ return m_IsRunning; }
    
    int Size() const{ return m_DataQueue.size(); }
protected:
    bool m_IsRunning;                               //!< is dispatcher runing or not
    std::queue<TData> m_DataQueue;                  //!< queue for caching data
    std::mutex m_LockQueue;                             //!< locker for synchronizing access of queue
    DataDispatcherReceiver *m_pReceiver;            //!< the data receiver which is specified whe invoking Start();
    ManualResetEvent m_EventNotify;                 //!< Notify event for stop or data in
    //ThreadWrapper m_DispatchThread;                 //!< the thread for dispatching data in queue
    std::mutex m_LockNotifyFlags;                       //!< locker for modifing event flags
    int m_NotifyFlags;                              //!< event flags
    static const int NotifyFlags_Stop = 0x01;       //!< event flag for Stop dispatcher
    static const int NotifyFlags_DataIn = 0x02;     //!< event flag for notifying data in

protected:
    /* \brief Update the m_NotifyFlags
     * \param flags the new flags will be appended into m_NotifyFlags
     * \return return the m_NotifyFlags before appending
     */
    int SetNotifyFlags( int flags ){
        std::unique_lock<std::mutex> al( m_LockNotifyFlags );
        
        int copyFlags = m_NotifyFlags;
        m_NotifyFlags |= flags;
        
        return copyFlags;
    }
    
    /* \brief data dispatching function for thread
     */
    //static void Dispatcher( ThreadWrapper &wrapper, void *pData ){
    static void Dispatcher( void *pData ){
        DataDispatcher *pMySelf = static_cast<DataDispatcher*>(pData);
        
        while(true){           
            
            pMySelf->m_EventNotify.Wait( -1 ); // Wait until be notified
            pMySelf->m_EventNotify.Reset();
            
            int copyFlags = pMySelf->SetNotifyFlags( 0 ); // Reset falgs and get the current flags
            // Debug
            //printf( "Get falgs %d\r\n", copyFlags );
            if( (copyFlags & NotifyFlags_Stop) > 0 )
                break; // Stop dispatcher
            
            if( (copyFlags & NotifyFlags_DataIn) > 0  )
            {
                // Dispatch data in queue
                bool isStopDispatchData = false;
                while(!isStopDispatchData){
                    std::unique_lock<std::mutex> al( pMySelf->m_LockQueue );
                    
                    try
                    {
                        if( pMySelf->m_DataQueue.empty() == false ){
                            TData data = pMySelf->m_DataQueue.front(); // get a copy of the front data
                            pMySelf->m_DataQueue.pop(); // remove the front data

                            if( pMySelf->m_pReceiver != NULL )
                                pMySelf->m_pReceiver->OnReceiveData( data );
                        }
                        else
                            isStopDispatchData = true;
                    
                    }
                    catch( ... )
                    {                        
                    }
                }
            }
        }
        
        // Debug
        fprintf( stdout,"DataDiaptcher<TData> thread stopped\r\n" );
    }
};

}
#endif	/* UTILITY_DATADISPATCHER_H */

