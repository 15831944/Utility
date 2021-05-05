/* 
 * File:   BracesStructure.h
 * Author: 0007989
 *
 * Created on 2015年1月20日, 下午 6:37
 */

#ifndef UTILITY_BRACESSTRUCTURE_H
#define	UTILITY_BRACESSTRUCTURE_H

#include <map>
#include <memory>
#include <string>
#include <mutex>

namespace ProgLab{ namespace Utility{  

class BSValue{
public:
    enum BSValueTypeEnum{
        String,
        Structure,
    };
public:
    BSValue( BSValueTypeEnum type = BSValueTypeEnum::String ):m_Type(type){}
        
    BSValueTypeEnum GetType() const{ return m_Type; }
    
    virtual std::string ToString() const = 0;
protected:
    BSValueTypeEnum m_Type;
};

class BSStringValue : public BSValue{
public:
    BSStringValue():BSValue(BSValueTypeEnum::String){};
    BSStringValue( const BSStringValue& orig ):BSValue(orig){
        *this = orig;
    }
    BSStringValue( std::string&& v ):BSValue(BSValueTypeEnum::String),m_Value( v ){}

    BSStringValue& operator=( const BSStringValue& rhs ){
        if( this != &rhs ){
            m_Value = rhs.m_Value;
        }
        return *this;
    }

    const std::string& GetValue() const{ return m_Value; }
    std::string& GetValue() { return m_Value; }
    void SetValue( const std::string& value ){ m_Value = value; }

    virtual std::string ToString() const{ return m_Value; }
protected:
    std::string m_Value;
};


class BracesStructure : public BSValue {
public:
    BracesStructure():BSValue(BSValueTypeEnum::Structure){}
    void Parse( const std::string& opaqueData );
    //void ParseV2( const std::string& opaqueData );
    void ParseV2( const std::string& opaqueData ){ ParseV2( opaqueData.c_str(), opaqueData.length() ); }
    void ParseV2( const char* pStr, int length );

    std::shared_ptr<BSValue> Query( const std::string& key ) const;

    virtual std::string ToString() const;

    const std::map<std::string, std::shared_ptr<BSValue>>& GetData() const{ return m_Data; }
private:
    enum ParsePhaseEnum{
        None,
        Key,
        EqualSplitter,
        Value,
        PipeSplitter,
        SubStructure,
    };

    mutable std::mutex m_LockData;
    std::map<std::string, std::shared_ptr<BSValue>> m_Data;

private:
    void AddBSStringVluae( const std::string& key, const std::string& org, int start, int end );
    void AddBSStringVluae( const std::string& key, std::string&& org );
};

}
#endif	/* UTILITY_BRACESSTRUCTURE_H */

