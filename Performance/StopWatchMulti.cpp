/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   StopWatchMulti.cpp
 * Author: Justin Lin
 * 
 * Created on 2016年11月10日, 上午 8:16
 */

#include "StopWatchMulti.h"
#include <stdexcept>

namespace KGI_TW_Der_Utility{

StopWatchMulti::StopWatchMulti( int count ) : m_Total(count), m_pTimePoints(NULL) {
    if( count == 0 ){
        throw std::runtime_error( "count is zero" );
    }
    m_pTimePoints = new TimePointType[count];
}

//StopWatchMulti::StopWatchMulti(const StopWatchMulti& orig) {
//}

StopWatchMulti::~StopWatchMulti() {
    delete[] m_pTimePoints;
}

}