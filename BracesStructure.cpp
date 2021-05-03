/* 
 * File:   BracesStructure.cpp
 * Author: 0007989
 * 
 * Created on 2015年1月20日, 下午 6:37
 */

#include "BracesStructure.h"

#include <stack>
#include <assert.h>

namespace KGI_TW_Der_Utility{ 

void BracesStructure::Parse( const std::string& opaqueData ){    
        
    std::string pureString;
    if( opaqueData.size() <= 0 )
        return;
    if( opaqueData[0] == '{' && opaqueData[opaqueData.size()-1] == '}' ){
        // trim enclosed braces
        pureString = opaqueData.substr( 1, opaqueData.size()-2 );
    }else
        pureString = opaqueData;
    
    std::lock_guard<std::mutex> al( m_LockData );
    m_Data.clear();
    
    if( pureString.empty() == false  ){
        std::stack<int> parsePos;
        ParsePhaseEnum crtParsePhase = ParsePhaseEnum::None;
        const char* pTmpStr = pureString.c_str();
        std::string key;
        int pureStringLength = pureString.size();
        for( int i = 0; i < pureStringLength; ++i ){
            if( crtParsePhase == ParsePhaseEnum::SubStructure ){
                switch( pTmpStr[i] ){
                    case '{':
                        parsePos.push( i ); // 進入下一層的 { } 資料折解，並維持crtParsePahse = SubStructure
                        break;
                    case '}':
                    {
                        int myPairBracesPos = parsePos.top();
                        parsePos.pop();
                        if( parsePos.empty() ){
                            BracesStructure* pBS = new BracesStructure();
                            std::string bsStr = pureString.substr( myPairBracesPos + 1, i - myPairBracesPos - 1 );
                            pBS->Parse( bsStr.c_str() );

                            m_Data.insert( std::make_pair( key, std::shared_ptr<BSValue>(pBS) ) );

                            crtParsePhase = ParsePhaseEnum::None;
                        }
                    }
                        break;
                }
                
            }else{
                switch( pTmpStr[i] ){
                    case '=':
                    {
                        // 20161129 Justin 因為在Liq策略下單時，交易員會在Comment加上說明，如果其中含有'='時要也支援
                        // 所以只有在目前是要拆解key的部候時，才會在第一次遇到 '=' 時視為分段點而取得key的部份。
                        // 不然的話，就會等到遇到 '{' 或 '|' 時才會處理
                        if( crtParsePhase == ParsePhaseEnum::Key ){
                            // got key
                            {
                                int keyStartPos = parsePos.top();
                                parsePos.pop();
                                key = pureString.substr( keyStartPos, i - keyStartPos );
                            }
                            crtParsePhase = ParsePhaseEnum::EqualSplitter;                            
                        }
                    }   
                        break;
                    case '{':
                        if( crtParsePhase == ParsePhaseEnum::EqualSplitter ){
                            parsePos.push( i );
                            crtParsePhase = ParsePhaseEnum::SubStructure;
                        }
                        break;
                    case '|':
                        if( crtParsePhase == ParsePhaseEnum::Value ){
                            // got value
                            {
                                AddBSStringVluae( key, pureString, parsePos.top(), i );
                                parsePos.pop();
                            }
                            crtParsePhase = ParsePhaseEnum::None;
                        }else{ // 不合理的分段字元
                            
                            if( crtParsePhase == ParsePhaseEnum::Key ){
                                parsePos.pop(); // 在搜尋key時遇到分段字元，就將先前記錄的起始位置移除
                            }                            
                            crtParsePhase = ParsePhaseEnum::None;
                        }
                        break;
                    case ' ': // trim the space character
                        break;                    
                    default:
                        if( crtParsePhase == ParsePhaseEnum::None ){
                            parsePos.push( i );
                            crtParsePhase = ParsePhaseEnum::Key;
                        }else if( crtParsePhase == ParsePhaseEnum::EqualSplitter ){
                            parsePos.push(i);
                            crtParsePhase = ParsePhaseEnum::Value;
                        }
                        break;
                }
            }
        } // end for loop
        if( parsePos.empty() == false ){
            AddBSStringVluae( key, pureString, parsePos.top(), pureStringLength );
            parsePos.pop();
        }
    }
}
//
//void BracesStructure::ParseV2( const std::string& opaqueData ){    
//        
//    const char* pPureString = opaqueData.c_str();
//    int tail = opaqueData.length();    
//    
//    if( opaqueData.size() <= 0 )
//        return;
//    if( pPureString[0] == '{' && pPureString[tail-1] == '}' ){
//        // trim enclosed braces
//        //opaqueData.substr( 1, opaqueData.size()-2 );
//        pPureString = pPureString + 1;
//        tail = tail - 2;
//    }
//    
//    std::lock_guard<decltype(m_LockData)> al( m_LockData );
//    m_Data.clear();
//    
//    if( tail > 0  ){
//        std::stack<int> parsePos;
//        ParsePhaseEnum crtParsePhase = ParsePhaseEnum::None;
//        const char* pTmpStr = pPureString;
//        std::string key;
//        int pureStringLength = tail;
//        for( int i = 0; i < pureStringLength; ++i ){
//            if( crtParsePhase == ParsePhaseEnum::SubStructure ){
//                switch( pTmpStr[i] ){
//                    case '{':
//                        parsePos.push( i ); // 進入下一層的 { } 資料折解，並維持crtParsePahse = SubStructure
//                        break;
//                    case '}':
//                    {
//                        int myPairBracesPos = parsePos.top();
//                        parsePos.pop();
//                        if( parsePos.empty() ){
//                            std::shared_ptr<BracesStructure> pBS = std::make_shared<BracesStructure>();
//                            //std::string bsStr = pureString.substr( myPairBracesPos + 1, i - myPairBracesPos - 1 );                            
//                            pBS->ParseV2( &pPureString[myPairBracesPos + 1], i - myPairBracesPos - 1 );
//
//                            m_Data.insert( std::make_pair( key, pBS ) );
//
//                            crtParsePhase = ParsePhaseEnum::None;
//                        }
//                    }
//                        break;
//                }
//                
//            }else{
//                switch( pTmpStr[i] ){
//                    case '=':
//                    {
//                        // 20161129 Justin 因為在Liq策略下單時，交易員會在Comment加上說明，如果其中含有'='時要也支援
//                        // 所以只有在目前是要拆解key的部候時，才會在第一次遇到 '=' 時視為分段點而取得key的部份。
//                        // 不然的話，就會等到遇到 '{' 或 '|' 時才會處理
//                        if( crtParsePhase == ParsePhaseEnum::Key ){
//                            // got key
//                            {
//                                int keyStartPos = parsePos.top();
//                                parsePos.pop();
//                                //key = pureString.substr( keyStartPos, i - keyStartPos );
//                                key.assign( &(pPureString[keyStartPos]), i - keyStartPos );
//                            }
//                            crtParsePhase = ParsePhaseEnum::EqualSplitter;                            
//                        }
//                    }   
//                        break;
//                    case '{':
//                        if( crtParsePhase == ParsePhaseEnum::EqualSplitter ){
//                            parsePos.push( i );
//                            crtParsePhase = ParsePhaseEnum::SubStructure;
//                        }
//                        break;
//                    case '|':
//                        if( crtParsePhase == ParsePhaseEnum::Value ){
//                            // got value
//                            {
//                                AddBSStringVluae( key, std::string(pPureString + parsePos.top(),i - parsePos.top()) );
//                                parsePos.pop();
//                            }
//                            crtParsePhase = ParsePhaseEnum::None;
//                        }else{ // 不合理的分段字元
//                            
//                            if( crtParsePhase == ParsePhaseEnum::Key ){
//                                parsePos.pop(); // 在搜尋key時遇到分段字元，就將先前記錄的起始位置移除
//                            }                            
//                            crtParsePhase = ParsePhaseEnum::None;
//                        }
//                        break;
//                    case ' ': // trim the space character
//                        break;                    
//                    default:
//                        if( crtParsePhase == ParsePhaseEnum::None ){
//                            parsePos.push( i );
//                            crtParsePhase = ParsePhaseEnum::Key;
//                        }else if( crtParsePhase == ParsePhaseEnum::EqualSplitter ){
//                            parsePos.push(i);
//                            crtParsePhase = ParsePhaseEnum::Value;
//                        }
//                        break;
//                }
//            }
//        } // end for loop
//        if( parsePos.empty() == false ){
//            AddBSStringVluae( key, std::string(pPureString + parsePos.top(),pureStringLength - parsePos.top()) );
//            parsePos.pop();
//        }
//    }
//}

void BracesStructure::ParseV2( const char* pStr, int length ){
    const char* pPureString = pStr;
    int tail = length;    
    
    if( length <= 0 )
        return;
    if( pPureString[0] == '{' && pPureString[tail-1] == '}' ){
        // trim enclosed braces
        //opaqueData.substr( 1, opaqueData.size()-2 );
        pPureString = pPureString + 1;
        tail = tail - 2;
    }
    
    std::lock_guard<decltype(m_LockData)> al( m_LockData );
    m_Data.clear();
    
    if( tail > 0  ){
        std::stack<int> parsePos;
        ParsePhaseEnum crtParsePhase = ParsePhaseEnum::None;
        const char* pTmpStr = pPureString;
        std::string key;
        int pureStringLength = tail;
        for( int i = 0; i < pureStringLength; ++i ){
            if( crtParsePhase == ParsePhaseEnum::SubStructure ){
                switch( pTmpStr[i] ){
                    case '{':
                        parsePos.push( i ); // 進入下一層的 { } 資料折解，並維持crtParsePahse = SubStructure
                        break;
                    case '}':
                    {
                        int myPairBracesPos = parsePos.top();
                        parsePos.pop();
                        if( parsePos.empty() ){
                            std::shared_ptr<BracesStructure> pBS = std::make_shared<BracesStructure>();
                            //std::string bsStr = pureString.substr( myPairBracesPos + 1, i - myPairBracesPos - 1 );                            
                            pBS->ParseV2( &pPureString[myPairBracesPos + 1], i - myPairBracesPos - 1 );

                            m_Data.insert( std::make_pair( key, pBS ) );

                            crtParsePhase = ParsePhaseEnum::None;
                        }
                    }
                        break;
                }
                
            }else{
                switch( pTmpStr[i] ){
                    case '=':
                    {
                        // 20161129 Justin 因為在Liq策略下單時，交易員會在Comment加上說明，如果其中含有'='時要也支援
                        // 所以只有在目前是要拆解key的部候時，才會在第一次遇到 '=' 時視為分段點而取得key的部份。
                        // 不然的話，就會等到遇到 '{' 或 '|' 時才會處理
                        if( crtParsePhase == ParsePhaseEnum::Key ){
                            // got key
                            {
                                int keyStartPos = parsePos.top();
                                parsePos.pop();
                                //key = pureString.substr( keyStartPos, i - keyStartPos );
                                key.assign( &(pPureString[keyStartPos]), i - keyStartPos );
                            }
                            crtParsePhase = ParsePhaseEnum::EqualSplitter;                            
                        }
                    }   
                        break;
                    case '{':
                        if( crtParsePhase == ParsePhaseEnum::EqualSplitter ){
                            parsePos.push( i );
                            crtParsePhase = ParsePhaseEnum::SubStructure;
                        }
                        break;
                    case '|':
                        if( crtParsePhase == ParsePhaseEnum::Value ){
                            // got value
                            {
                                AddBSStringVluae( key, std::string(pPureString + parsePos.top(),i - parsePos.top()) );
                                parsePos.pop();
                            }
                            crtParsePhase = ParsePhaseEnum::None;
                        }else{ // 不合理的分段字元
                            
                            if( crtParsePhase == ParsePhaseEnum::Key ){
                                parsePos.pop(); // 在搜尋key時遇到分段字元，就將先前記錄的起始位置移除
                            }                            
                            crtParsePhase = ParsePhaseEnum::None;
                        }
                        break;
                    case ' ': // trim the space character
                        break;                    
                    default:
                        if( crtParsePhase == ParsePhaseEnum::None ){
                            parsePos.push( i );
                            crtParsePhase = ParsePhaseEnum::Key;
                        }else if( crtParsePhase == ParsePhaseEnum::EqualSplitter ){
                            parsePos.push(i);
                            crtParsePhase = ParsePhaseEnum::Value;
                        }
                        break;
                }
            }
        } // end for loop
        if( parsePos.empty() == false ){
            AddBSStringVluae( key, std::string(pPureString + parsePos.top(),pureStringLength - parsePos.top()) );
            parsePos.pop();
        }
    }
}

void BracesStructure::AddBSStringVluae( const std::string& key, const std::string& org, int start, int end ){
    std::shared_ptr<BSStringValue> value = std::make_shared<BSStringValue>();
    // debug
    //std::string crtValue = org.substr( start, end - start );

    value->SetValue( org.substr( start, end - start ) );

    m_Data.insert( std::make_pair( key, value ) );
}

void BracesStructure::AddBSStringVluae( const std::string& key, std::string&& org ){
    std::shared_ptr<BSStringValue> value = std::make_shared<BSStringValue>( std::forward<std::string>(org) );
    m_Data.insert( std::make_pair( key, value ) );
}

std::shared_ptr<BSValue> BracesStructure::Query( const std::string& key ) const{
    std::lock_guard<std::mutex> al( m_LockData );
    auto itrValue = m_Data.find( key );
    if( itrValue != m_Data.end() )
        return itrValue->second;
    else
        return std::shared_ptr<BSValue>(); // empty
}

std::string BracesStructure::ToString() const{
    std::lock_guard<std::mutex> al( m_LockData );
    std::string result;
    for( auto itrValue = m_Data.begin(); itrValue != m_Data.end(); ++itrValue ){
        if( result.empty() == false )
            result += "|";
        result += itrValue->first + "=" + itrValue->second->ToString();
    }    
    return "{" + result + "}";
}

    
}

