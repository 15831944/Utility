/* 
 * File:   MacroRecipie.h
 * Author: 0007989
 *
 * Created on 20140321
 */

#ifndef UTILITY_MACRORECIPIE_H
#define	UTILITY_MACRORECIPIE_H

#define PROP_GETSET(type,fname,pname) \
type Get##fname() const { \
        return pname; \
} \
void Set##fname( const type & value ) \
{ \
        pname = value; \
} 

#define PROP_GETSET_STR(type,fname,pname) \
const type& Get##fname() const { \
        return pname; \
} \
void Set##fname( const type & value ) \
{ \
        pname = value; \
} 

#define PROP_GETSET_WRAP(type,fname,pname) \
type Get##fname() const { \
        return pname.Get##fname(); \
} \
void Set##fname( const type & value ) \
{ \
        pname.Set##fname( value ); \
}

#define PROP_GETSET_INOUT(typeIn,typeOut,fname,pname) \
const typeOut Get##fname() const { \
        return pname; \
} \
void Set##fname( typeIn value ) \
{ \
        pname = value; \
}

#define PROP_GETSET_BYVALUE(type,fname,pname) \
type Get##fname() const { \
        return pname; \
} \
void Set##fname( type value ) \
{ \
        pname = value; \
}

#define PROP_GETSET_BYVALUE_WRAP(type,fname,pname) \
type Get##fname() const { \
        return pname.Get##fname(); \
} \
void Set##fname( type value ) \
{ \
        pname.Set##fname( value ); \
}


#define FOREACH_BEGIN( type, container, itr )\
for( type::iterator itr = container.begin(); itr != container.end(); ++itr ){

#define FOREACH_BEGIN_CONST( type, container, itr )\
for( type::const_iterator itr = container.begin(); itr != container.end(); ++itr ){

#define FOREACH_END() }

#endif	/* UTILITY_MACRORECIPIE_H */