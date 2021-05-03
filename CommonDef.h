/* 
 * File:   CommonDef.h
 * Author: 0007989
 *
 * Created on 2014年11月27日, 下午 6:15
 */

#ifndef UTILITY_COMMONDEF_H
#define	UTILITY_COMMONDEF_H

#define BEGIN_NAMESPACE(n) namespace n {
#define END_NAMESPACE(n) }
#define USING_NAMESPACE(n) using namespace n;

#define BEGIN_NAMESPACE_2(main,sub) namespace main { namespace sub {
#define END_NAMESPACE_2( main, sub ) } }
#define USING_NAMESPACE_2( main, sub ) using namespace main :: sub;

#define __CPP_NOEXCEPT
//#define __CPP_NOEXCEPT noexcept

#endif	/* UTILITY_COMMONDEF_H */

