/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   UtilityVersion.cpp
 * Author: Justin Lin
 * 
 * Created on 2017年2月23日, 下午 6:27
 */

#include "UtilityVersion.h"

namespace KGI_TW_Der_Utility{
    
std::string UtilityVersion::BuildDate_(__DATE__);
std::string UtilityVersion::BuildTime_(__TIME__);

UtilityVersion::UtilityVersion() {
}

UtilityVersion::UtilityVersion(const UtilityVersion& orig) {
}

UtilityVersion::~UtilityVersion() {
}

std::string UtilityVersion::GetBranchInfo(){
    return std::string( "main trunk" );
}

}