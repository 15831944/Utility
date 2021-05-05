/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ExecuteResult.cpp
 * Author: Justin Lin
 * 
 * Created on 2018年6月20日, 下午 9:56
 */

#include "ExecuteResult.h"

namespace ProgLab{ namespace Utility{ 

ExecuteResult::ExecuteResult(bool result, const std::string& resultMsg ):Result_(result),ResultMessage_(resultMsg) {
}

//ExecuteResult::ExecuteResult(const ExecuteResult& orig) {
//}

ExecuteResult::~ExecuteResult() {
}

}