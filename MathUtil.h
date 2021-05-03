/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MathUtil.h
 * Author: Justin Lin
 *
 * Created on 2017年6月8日, 上午 10:50
 */

#ifndef UTILITY_MATHUTIL_H
#define UTILITY_MATHUTIL_H

#include <cmath>
#include <cfloat>

namespace KGI_TW_Der_Utility{ 

/// \brief 數學運算函式庫
///
class MathUtil {
public: 
    /// \brief 確認兩個 double 數值是否一致(或很接近)
    /// 因為直接用 == 時如果有數值上的誤差，可能會造成誤以為兩個值不相等，所以採用相減後跟 DBL_EPSILON 比較來確認兩個值是否在誤差範圍內
    static bool IsEpsilonEqual( double x, double y ){
        return std::fabs( x - y ) <= DBL_EPSILON;
    }
    /// \brief 確認兩個 float 數值是否一致(或很接近)
    /// 因為直接用 == 時如果有數值上的誤差，可能會造成誤以為兩個值不相等，所以採用相減後跟 FLT_EPSILON 比較來確認兩個值是否在誤差範圍內
    static bool IsEpsilonEqual( float x, float y ){
        return std::fabs( x - y ) <= FLT_EPSILON;
    }
private:
    MathUtil();
    MathUtil(const MathUtil& orig);
    virtual ~MathUtil();

};

}
#endif /* UTILITY_MATHUTIL_H */

