// Bounded streaming reader for the versioned replay trailer. No object tree or
// dependency on the browser; the same codec is used by desktop and Web.
#pragma once
#include <charconv>
#include <cstdint>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>

namespace th12host {
class ReplayJson {
    std::string_view text;size_t at=0;unsigned depth=0;
    void space(){while(at<text.size()&&(text[at]==' '||text[at]=='\r'||text[at]=='\n'||text[at]=='\t'))++at;}
    void require(char c){if(!take(c))bad();}
    unsigned hex(){if(at==text.size())bad();const char c=text[at++];if(c>='0'&&c<='9')return c-'0';if(c>='a'&&c<='f')return c-'a'+10;if(c>='A'&&c<='F')return c-'A'+10;bad();}
    unsigned unit(){unsigned v=0;for(int n=0;n<4;++n)v=v*16+hex();return v;}
    static void utf8(std::string& out,unsigned cp){
        if(cp<128)out+=char(cp);else if(cp<2048){out+=char(0xc0|(cp>>6));out+=char(0x80|(cp&63));}
        else if(cp<65536){out+=char(0xe0|(cp>>12));out+=char(0x80|((cp>>6)&63));out+=char(0x80|(cp&63));}
        else{out+=char(0xf0|(cp>>18));out+=char(0x80|((cp>>12)&63));out+=char(0x80|((cp>>6)&63));out+=char(0x80|(cp&63));}
    }
public:
    explicit ReplayJson(std::string_view source):text(source){}
    [[noreturn]] static void bad(){throw std::runtime_error("Invalid TH12 Web replay metadata");}
    bool take(char c){space();if(at==text.size()||text[at]!=c)return false;++at;return true;}
    bool null(){space();if(text.substr(at,4)!="null")return false;at+=4;return true;}
    bool boolean(){space();if(text.substr(at,4)=="true"){at+=4;return true;}if(text.substr(at,5)=="false"){at+=5;return false;}bad();}
    int64_t integer(int64_t low=INT32_MIN,int64_t high=INT32_MAX){
        space();const size_t start=at;if(at<text.size()&&text[at]=='-')++at;
        if(at==text.size()||text[at]<'0'||text[at]>'9')bad();
        if(text[at]=='0')++at;else while(at<text.size()&&text[at]>='0'&&text[at]<='9')++at;
        int64_t value=0;const auto result=std::from_chars(text.data()+start,text.data()+at,value);
        if(result.ec!=std::errc{}||value<low||value>high)bad();return value;
    }
    std::string string(){
        require('"');std::string out;
        while(at<text.size()){
            const unsigned char c=text[at++];if(c=='"')return out;if(c<32)bad();
            if(c!='\\'){
                if(c<128){out+=char(c);continue;}
                unsigned cp=c;int n=0;unsigned minimum=0;
                if(c>=0xc2&&c<0xe0){n=1;cp&=31;minimum=128;}else if(c<0xf0&&c>=0xe0){n=2;cp&=15;minimum=2048;}else if(c>=0xf0&&c<0xf5){n=3;cp&=7;minimum=65536;}else bad();
                for(int i=0;i<n;++i){if(at==text.size())bad();const unsigned b=uint8_t(text[at++]);if((b&0xc0)!=0x80)bad();cp=(cp<<6)|(b&63);}
                if(cp<minimum||cp>0x10ffff||(cp>=0xd800&&cp<=0xdfff))bad();utf8(out,cp);continue;
            }
            if(at==text.size())bad();switch(text[at++]){
            case '"':out+='"';break;case '\\':out+='\\';break;case '/':out+='/';break;
            case 'b':out+='\b';break;case 'f':out+='\f';break;case 'n':out+='\n';break;case 'r':out+='\r';break;case 't':out+='\t';break;
            case 'u':{unsigned cp=unit();if(cp>=0xd800&&cp<=0xdbff){if(at+2>text.size()||text.substr(at,2)!="\\u")bad();at+=2;const unsigned low=unit();if(low<0xdc00||low>0xdfff)bad();cp=0x10000+((cp-0xd800)<<10)+(low-0xdc00);}else if(cp>=0xdc00&&cp<=0xdfff)bad();utf8(out,cp);break;}
            default:bad();}
            if(out.size()>1024*1024)bad();
        }bad();
    }
    template<class F>void object(F member){
        require('{');if(++depth>32)bad();std::set<std::string> keys;
        if(!take('}')){do{auto key=string();if(!keys.insert(key).second)bad();require(':');member(key);}while(take(','));require('}');}--depth;
    }
    template<class F>void array(size_t max,F element){
        require('[');if(++depth>32)bad();size_t n=0;
        if(!take(']')){do{if(n==max)bad();element(n++);}while(take(','));require(']');}--depth;
    }
    void skip(){
        space();if(at==text.size())bad();const char c=text[at];
        if(c=='{')object([&](const auto&){skip();});else if(c=='[')array(8000000,[&](size_t){skip();});
        else if(c=='"')string();else if(c=='n'){if(!null())bad();}else if(c=='t'||c=='f')boolean();
        else{
            if(text[at]=='-')++at;
            auto digits=[&]{const auto start=at;while(at<text.size()&&text[at]>='0'&&text[at]<='9')++at;if(at==start)bad();};
            if(at<text.size()&&text[at]=='0')++at;else digits();
            if(at<text.size()&&text[at]=='.'){++at;digits();}
            if(at<text.size()&&(text[at]=='e'||text[at]=='E')){++at;if(at<text.size()&&(text[at]=='+'||text[at]=='-'))++at;digits();}
        }
    }
    void end(){space();if(at!=text.size())bad();}
};
}
