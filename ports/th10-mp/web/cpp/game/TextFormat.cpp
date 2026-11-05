#include "TextFormat.hpp"
#include <charconv>
#include <algorithm>
#include <cmath>
namespace th10 {
namespace {
struct Output {
    char* bytes;u32 capacity,size=0;
    void put(char c){if(size+1<capacity)bytes[size]=c;++size;}
    void append(const char* text,u32 count){for(u32 i=0;i<count;i++)put(text[i]);}
    void repeat(char c,i32 count){while(count-->0)put(c);}
    void finish(){if(capacity)bytes[std::min(size,capacity-1)]=0;}
};
struct Digits {char text[1024]{};i32 size=0;void append(char c){text[size++]=c;}};
Digits decimal(u64 value,u32 base,bool upper){Digits reversed,result;const char* alphabet=upper?"0123456789ABCDEF":"0123456789abcdef";do{reversed.append(alphabet[value%base]);value/=base;}while(value);while(reversed.size)result.append(reversed.text[--reversed.size]);return result;}
struct Scientific {char digits[18]{};i32 point=1;};
Scientific scientific(double value){
    Scientific result;char buffer[40];const auto end=std::to_chars(buffer,buffer+sizeof(buffer),value,std::chars_format::scientific,16).ptr;
    const char* p=buffer;i32 count=0;while(p<end&&*p!='e'){if(*p!='.')result.digits[count++]=*p;++p;}
    if(p<end)++p;const bool negative=p<end&&*p=='-';if(p<end&&(*p=='-'||*p=='+'))++p;i32 exponent=0;while(p<end)exponent=exponent*10+*p++-'0';result.point=1+(negative?-exponent:exponent);return result;
}
Digits rounded(const Scientific& source,i32 count){
    Digits result;if(count<0){result.append('0');return result;}
    for(i32 i=0;i<count;i++)result.append(i<17?source.digits[i]:'0');
    // MSVCRT first creates 17 significant decimal digits, then rounds the
    // requested decimal position up when its next digit is at least five.
    if(count<17&&source.digits[count]>='5'){
        i32 i=result.size-1;while(i>=0&&result.text[i]=='9')result.text[i--]='0';
        if(i>=0)++result.text[i];else{std::memmove(result.text+1,result.text,result.size);result.text[0]='1';++result.size;}
    }
    if(!result.size)result.append('0');return result;
}
void fixed(Digits& result,const Digits& digits,i32 precision,bool point){
    const i32 whole=digits.size-precision;
    if(whole>0)for(i32 i=0;i<whole;i++)result.append(digits.text[i]);else result.append('0');
    if(precision||point)result.append('.');
    for(i32 i=whole;i<digits.size;i++)result.append(i<0?'0':digits.text[i]);
}
Digits floating(double value,char type,i32 precision,bool alternate,const char* special){
    const bool upper=type=='E'||type=='G';if(upper)type+=32;
    if(precision<0)precision=6;if(type=='g'&&!precision)precision=1;
    Scientific source;if(special){std::memset(source.digits,'0',17);std::memcpy(source.digits,special,std::strlen(special));}else source=scientific(value);
    if(value==0&&type=='g')source.point=0;Digits result;
    if(type=='f'){const auto digits=rounded(source,source.point+precision);fixed(result,digits,precision,alternate);return result;}
    const i32 significant=type=='e'?precision+1:precision;auto digits=rounded(source,significant);
    if(digits.size>significant){++source.point;digits.size=significant;}
    if(type=='g'&&source.point>=-3&&source.point<=precision){fixed(result,digits,std::max(0,precision-source.point),alternate);}
    else{
        result.append(digits.text[0]);if(significant>1||alternate)result.append('.');
        for(i32 i=1;i<significant;i++)result.append(digits.text[i]);
        if(type=='g'&&!alternate){while(result.size&&result.text[result.size-1]=='0')--result.size;if(result.size&&result.text[result.size-1]=='.')--result.size;}
        result.append(upper?'E':'e');const i32 exponent=source.point-1;result.append(exponent<0?'-':'+');auto power=decimal(exponent<0?-exponent:exponent,10,false);for(i32 i=power.size;i<3;i++)result.append('0');for(i32 i=0;i<power.size;i++)result.append(power.text[i]);return result;
    }
    if(type=='g'&&!alternate&&std::memchr(result.text,'.',result.size)){while(result.size&&result.text[result.size-1]=='0')--result.size;if(result.size&&result.text[result.size-1]=='.')--result.size;}
    return result;
}
bool digit(char c){return c>='0'&&c<='9';}
}
i32 format_text(char* output,u32 capacity,const char* format,const u32* arguments,u32 available) noexcept {
    Output out{output,capacity};u32 used=0;bool valid=true;
    const auto argument=[&](){if(!arguments||used>=available){valid=false;return 0u;}return arguments[used++];};
    while(*format&&valid){
        if(*format!='%'){out.put(*format++);continue;}++format;
        bool left=false,plus=false,space=false,zero=false,alternate=false;
        for(bool flags=true;flags;){switch(*format){case '-':left=true;break;case '+':plus=true;break;case ' ':space=true;break;case '0':zero=true;break;case '#':alternate=true;break;default:flags=false;continue;}++format;}
        i32 width=0,precision=-1;
        if(*format=='*'){width=static_cast<i32>(argument());++format;if(width<0){left=true;if(width==INT32_MIN){valid=false;break;}width=-width;}}
        else while(digit(*format)){width=width*10+*format++-'0';if(width>8192){valid=false;break;}}
        if(*format=='.'){++format;precision=0;if(*format=='*'){precision=static_cast<i32>(argument());++format;}else while(digit(*format)){precision=precision*10+*format++-'0';if(precision>512){valid=false;break;}}}
        if(width>8192||precision>512||!valid){valid=false;break;}
        u32 size=32;if(*format=='h'){++format;size=16;if(*format=='h'){++format;size=8;}}
        else if(*format=='l'){++format;if(*format=='l'){++format;size=64;}}
        else if(*format=='I'){++format;if(format[0]=='6'&&format[1]=='4'){size=64;format+=2;}else if(format[0]=='3'&&format[1]=='2')format+=2;}
        const char type=*format;if(type)++format;else{valid=false;break;}
        Digits value;char prefix[3]{};i32 prefix_size=0;const char* text=value.text;i32 length=0;bool numeric=false;
        if(type=='%'){value.append('%');}
        else if(type=='c'){value.append(static_cast<char>(argument()));}
        else if(type=='s'){const auto* str=reinterpret_cast<const char*>(static_cast<uintptr_t>(argument()));text=str?str:"(null)";length=std::strlen(text);if(precision>=0)length=std::min(length,precision);}
        else if(type=='d'||type=='i'||type=='u'||type=='o'||type=='x'||type=='X'||type=='p'){
            numeric=true;u64 bits=argument();if(size==64)bits|=static_cast<u64>(argument())<<32;else if(size==16)bits&=65535;else if(size==8)bits&=255;
            const bool signed_value=type=='d'||type=='i',negative=signed_value&&(bits&(u64{1}<<(size-1)));if(negative){bits=0-bits;if(size<64)bits&=(u64{1}<<size)-1;prefix[prefix_size++]='-';}else if(signed_value&&(plus||space))prefix[prefix_size++]=plus?'+':' ';
            const u32 base=type=='o'?8:type=='x'||type=='X'||type=='p'?16:10;const bool upper=type=='X'||type=='p';
            if(bits||precision!=0||type=='p')value=decimal(bits,base,upper);
            if(type=='p')precision=8;
            if(alternate&&base==16&&bits){prefix[prefix_size++]='0';prefix[prefix_size++]=upper?'X':'x';}
            if(alternate&&base==8&&(!value.size||value.text[0]!='0'))precision=std::max(precision,value.size+1);
            if(precision>=0){zero=false;const i32 pad=precision-value.size;if(pad>0){std::memmove(value.text+pad,value.text,value.size);std::memset(value.text,'0',pad);value.size+=pad;}}
        }else if(type=='f'||type=='e'||type=='E'||type=='g'||type=='G'){
            numeric=true;u64 bits=argument();bits|=static_cast<u64>(argument())<<32;double number;std::memcpy(&number,&bits,8);
            const u64 magnitude=bits&0x7fffffffffffffffULL;const bool negative=(bits>>63)&&magnitude;
            if(negative)prefix[prefix_size++]='-';else if(plus||space)prefix[prefix_size++]=plus?'+':' ';
            const char* special=nullptr;
            if((magnitude>>52)==0x7ff){const auto fraction=magnitude&0xfffffffffffffULL;special=!fraction?"1#INF":bits==0xfff8000000000000ULL?"1#IND":fraction&0x8000000000000ULL?"1#QNAN":"1#SNAN";}
            value=floating(std::fabs(number),type,precision,alternate,special);
        }else{valid=false;break;}
        if(type!='s')length=value.size;
        if(!valid)break;const i32 pad=width-length-prefix_size;
        if(!left&&(!zero||!numeric))out.repeat(' ',pad);out.append(prefix,prefix_size);
        if(!left&&zero&&numeric)out.repeat('0',pad);out.append(text,length);if(left)out.repeat(' ',pad);
    }
    out.finish();return valid?static_cast<i32>(out.size):-1;
}
}
