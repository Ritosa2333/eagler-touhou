#pragma once
#include <array>
#include <cstdint>
#include <cstdio>
#include <span>
#include <stdexcept>
#include <string>
namespace th128::record {
struct NativeReplaySlotPreview {
    bool occupied=false,known=false;
    std::array<std::uint8_t,112> metadata{};
    std::array<unsigned,5> calendar{};
    static unsigned word(std::span<const std::uint8_t> bytes,unsigned offset){
        return unsigned(bytes[offset])|(unsigned(bytes[offset+1])<<8)|(unsigned(bytes[offset+2])<<16)|(unsigned(bytes[offset+3])<<24);
    }
    static std::array<NativeReplaySlotPreview,25> decode(std::span<const std::uint8_t> bytes){
        std::array<NativeReplaySlotPreview,25> result{};
        if(bytes.size()==25){
            for(unsigned index=0;index<25;++index){if(bytes[index]>1)throw std::runtime_error("invalid original replay occupancy");result[index].occupied=bytes[index]!=0;}
            return result;
        }
        if(bytes.size()!=3412||word(bytes,0)!=0x31565052u||word(bytes,4)!=25||word(bytes,8)!=136)
            throw std::runtime_error("original replay preview metadata packet identity rejected");
        for(unsigned index=0;index<25;++index){
            const unsigned base=12+index*136,flag=word(bytes,base);
            if(flag>1)throw std::runtime_error("invalid original replay slot flag");
            auto& row=result[index];row.occupied=flag!=0;
            if(!row.occupied)continue;
            for(unsigned offset=0;offset<112;++offset)row.metadata[offset]=bytes[base+4+offset];
            for(unsigned field=0;field<5;++field)row.calendar[field]=word(bytes,base+116+field*4);
            const auto view=std::span<const std::uint8_t>(row.metadata);
            for(unsigned letter=0;letter<8&&row.metadata[letter];++letter)
                if(row.metadata[letter]<32||row.metadata[letter]>129)throw std::runtime_error("original replay name glyph unavailable");
            if(word(view,20)>999999999||word(view,88)<1||word(view,88)>3||word(view,92)>6||word(view,100)>4||word(view,104)>23||word(view,108)>9)
                throw std::runtime_error("original replay preview native metadata rejected");
            const auto year=row.calendar[0],month=row.calendar[1],day=row.calendar[2];
            constexpr std::array<unsigned,12> days{31,28,31,30,31,30,31,31,30,31,30,31};
            if(year<1970||year>9999||month<1||month>12||day<1||row.calendar[3]>23||row.calendar[4]>59)
                throw std::runtime_error("actual replay local calendar rejected");
            const auto limit=days[month-1]+unsigned(month==2&&year%4==0&&(year%100!=0||year%400==0));
            if(day>limit)throw std::runtime_error("actual replay local calendar day rejected");
            row.known=true;
        }
        return result;
    }
    std::string line(unsigned slot)const{
        if(slot>=25)throw std::runtime_error("original replay slot is outside25");
        char buffer[160]{};
        if(!occupied){std::snprintf(buffer,sizeof(buffer),"%2d -------- --/--/-- --------- - ------",int(slot+1));return buffer;}
        if(!known)throw std::runtime_error("occupied original replay slot lacks actual metadata");
        constexpr std::array<const char*,5> ranks{"E","N","H","L","X"};
        constexpr std::array<const char*,24> stages{"test  ","A1-1  ","A1-2  ","A1-3  ","A2-2  ","A2-3  ","B1-1  ","B1-2  ","B1-3  ","B2-2  ","B2-3  ","C1-1  ","C1-2  ","C1-3  ","C2-2  ","C2-3  ","Extra ","A1 All","A2 All","B1 All","B2 All","C1 All","C2 All","Ex All"};
        char name[9]{};for(unsigned index=0;index<8;++index)name[index]=char(metadata[index]);
        const auto view=std::span<const std::uint8_t>(metadata);
        std::snprintf(buffer,sizeof(buffer),"%2d %s %.2d/%.2d/%.2d %8d%d %s %s",int(slot+1),name,
                      int(calendar[0]%100),int(calendar[1]),int(calendar[2]),int(word(view,20)),int(word(view,108)),
                      ranks[word(view,100)],stages[word(view,104)]);
        return buffer;
    }
};
}
