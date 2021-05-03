/* 
 * File:   CommonDebug.h
 * Author: 0007989
 *
 * Created on 2014年6月9日, 上午 8:20
 */

#ifndef UTILITY_COMMONDEBUG_H
#define	UTILITY_COMMONDEBUG_H

#ifdef _DEBUG
#include <assert.h>
#include <float.h>
#define assert_double_equal( lhs, rhs ) assert( abs( lhs - rhs ) <= DBL_EPSILON )
#define assert_float_equal( lhs, rhs ) assert( abs( lhs - rhs ) <= FLT_EPSILON )
#else
#define assert(func)
#endif

#endif	/* UTILITY_COMMONDEBUG_H */

