/* 
 * File:   StringTemplateUtil.h
 * Author: Justin Lin <hsuehwen.lin@kgi.com>
 *
 * Created on 2015年3月3日, 上午 11:39
 */

#ifndef UTILITY_STRINGTEMPLATEUTIL_H
#define	UTILITY_STRINGTEMPLATEUTIL_H

#include "CommonDef.h"
#include <stdio.h>

BEGIN_NAMESPACE( KGI_TW_Der_Utility )
    
/*
 * 參考 https://msdn.microsoft.com/en-us/magazine/dn913181.aspx
 */

class StringTemplateUtil{
public:
    template <typename T>
    static T Argument(T value) 
    {
        return value;
    }

    template <typename ... Args>
    static void Print(char const * const format,
               Args const & ... args)
    {
        printf(format, Argument(args) ...);
    }

    template <typename ... Args>
    static int StringPrint(char * const buffer,
                    size_t const bufferCount,
                    char const * const format,
                    Args const & ... args) __CPP_NOEXCEPT
    {
        int const result = snprintf(buffer,
                                bufferCount,
                                format,
                                Argument(args) ...);
        //ASSERT(-1 != result);
        return result;
    }

    template <typename ... Args>
    static int StringPrint(wchar_t * const buffer,
                    size_t const bufferCount,
                    wchar_t const * const format,
                    Args const & ... args) __CPP_NOEXCEPT
    {
        int const result = swprintf(buffer,
                                bufferCount,
                                format,
                                Argument(args) ...);
        //ASSERT(-1 != result);
        return result;
    }

    template <typename T, typename ... Args>
    static void Format(std::basic_string<T> & buffer,
                T const * const format,
                Args const & ... args)
    {
        size_t const size = StringPrint(&buffer[0],
                                    buffer.size() + 1,
                                    format,
                                    args ...);
        if (size > buffer.size()){
            buffer.resize(size);
            StringPrint(&buffer[0], buffer.size() + 1, format, args ...);
        }
        else if (size < buffer.size()){
            buffer.resize(size);
        }
    }
};

END_NAMESPACE( KGI_TW_Der_Utility )


#endif	/* UTILITY_STRINGTEMPLATEUTIL_H */

