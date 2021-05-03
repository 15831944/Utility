/* 
 * File:   TemplateUtil.h
 * Author: Justin Lin <hsuehwen.lin@kgi.com>
 *
 * Created on 2015年7月9日, 上午 9:19
 */

#ifndef UTILITY_TEMPLATEUTIL_H
#define	UTILITY_TEMPLATEUTIL_H

#include "CommonDef.h"

BEGIN_NAMESPACE( KGI_TW_Der_Utility )

class TemplateUtil{
public:
    template<typename TDev,typename TBase>
    static void CastToBaseAssignOperator(TDev* pDev, const TDev& orig ){
        *(reinterpret_cast<TBase*>( pDev )) = reinterpret_cast<const TBase&>( orig );
    }

};

END_NAMESPACE( KGI_TW_Der_Utility )
#endif	/* UTILITY_TEMPLATEUTIL_H */

