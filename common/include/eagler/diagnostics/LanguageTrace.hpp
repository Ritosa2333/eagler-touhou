#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace eagler::diagnostics {

// Fixed-capacity, opt-in observations of work that has already happened in the
// Runtime. The recorder owns no game data and never asks a producer to look up
// a key or glyph on its behalf.
class LanguageTrace {
public:
    static constexpr std::uint32_t schema_version = 1;
    static constexpr std::size_t event_capacity = 2048;
    static constexpr std::size_t preparation_glyph_capacity = 64;
    static constexpr std::size_t string_capacity = 128;

    enum class Phase : std::uint8_t { None=0, Preparation=1, Runtime=2 };
    enum class Kind : std::uint8_t { Lookup=1, Raster=2, Glyph=3, Present=4 };
    enum class Encoding : std::uint8_t { None=0, Raw=1, Utf8=2, Cp932=3 };
    enum class GlyphSource : std::uint8_t {
        None=0, Bitmap=1, Msgothic=2, Pack=3, Unifont=4, MsgothicNotdef=5, AsciiAtlas=6
    };
    enum class LookupOperation : std::uint8_t {
        SpellName=1, StageName=2, MusicTitle=3, MusicComment=4,
        SpellComment=5, SpellCommentOwner=6, LookupAscii=7, AsciiString=8,
        AsciiStringById=9, StringById=10, FormatStringById=11
    };

    // Lookup flags: bit 0 translated, bit 1 found. Glyph flags: bit 0 requested
    // code has coverage, bit 1 actual raster is .notdef, bit 2 cache applies,
    // bit 3 cache hit, bit 4 fallback was used, bit 5 actual raster has coverage.
    struct Event {
        std::uint64_t first=0,last=0,tuple_hash=0;
        std::uint32_t count=0;
        std::int32_t scene=-1,a=0,b=0,c=0,d=0;
        std::uint8_t kind=0,phase=0,operation=0,encoding=0,source=0,flags=0;
        std::uint16_t text_size=0,value_size=0,extra_size=0;
        bool text_truncated=false,value_truncated=false,extra_truncated=false;
        char text[string_capacity]{};
        char value[string_capacity]{};
        char extra[string_capacity]{};
    };

    struct PreparationGlyphGroup {
        std::uint64_t first=0,last=0;
        std::uint32_t count=0;
        std::int32_t height=0,weight=0;
        std::uint8_t source=0,flags=0;
    };

    void enable(bool enabled) noexcept {
        reset();
        enabled_=enabled;
        phase_=enabled?Phase::Preparation:Phase::None;
    }
    bool enabled() const noexcept { return enabled_; }
    Phase phase() const noexcept { return phase_; }
    void set_phase(Phase phase) noexcept { if(enabled_)phase_=phase; }
    void set_scene(std::int32_t scene) noexcept { if(enabled_)scene_=scene; }

    void lookup(LookupOperation operation,const char* key,const char* result,
                const char* fallback,bool translated,bool found,
                std::int32_t a=0,std::int32_t b=0) noexcept {
        if(!enabled_)return;
        Event event=base(Kind::Lookup);
        event.operation=static_cast<std::uint8_t>(operation);
        event.flags=std::uint8_t((translated?1:0)|(found?2:0));
        event.a=a;event.b=b;
        copy_cstr(event.text,event.text_size,event.text_truncated,key);
        copy_cstr(event.value,event.value_size,event.value_truncated,result);
        copy_cstr(event.extra,event.extra_size,event.extra_truncated,fallback);
        append(event);
    }

    void raster(const std::uint8_t* bytes,std::size_t size,Encoding encoding,
                std::int32_t height,std::int32_t weight) noexcept {
        if(!enabled_)return;
        Event event=base(Kind::Raster);event.encoding=static_cast<std::uint8_t>(encoding);
        event.a=height;event.b=weight;
        copy_bytes(event.text,event.text_size,event.text_truncated,bytes,size);
        append(event);
    }

    void glyph(std::int32_t height,std::int32_t weight,std::uint16_t requested,
               std::int32_t actual,GlyphSource source,bool requested_has_glyph,
               bool notdef,bool cache_applicable,bool cache_hit,bool fallback,
               bool actual_has_glyph) noexcept {
        if(!enabled_)return;
        Event event=base(Kind::Glyph);event.source=static_cast<std::uint8_t>(source);
        event.a=height;event.b=weight;event.c=requested;event.d=actual;
        event.flags=std::uint8_t((requested_has_glyph?1:0)|(notdef?2:0)|
            (cache_applicable?4:0)|(cache_hit?8:0)|(fallback?16:0)|
            (actual_has_glyph?32:0));
        if(phase_==Phase::Preparation)append_preparation_glyph(event);else append(event);
    }

    void present(std::uint32_t sequence,std::int32_t scene) noexcept {
        if(!enabled_)return;
        Event event=base(Kind::Present);event.scene=scene;
        event.a=static_cast<std::int32_t>(sequence);event.b=static_cast<std::int32_t>(sequence);
        append(event);
    }

    std::uint64_t observations() const noexcept { return observations_; }
    std::uint32_t unique_events() const noexcept { return size_; }
    std::uint64_t dropped_events() const noexcept { return dropped_; }
    bool strings_truncated() const noexcept { return strings_truncated_; }
    const Event& event(std::size_t index) const noexcept { return events_[index]; }

    // Size includes the final NUL. Copy returns JSON bytes excluding the NUL,
    // or zero without writing a partial document when capacity is insufficient.
    // Strings are hex encoded so CP932 bytes remain lossless and the document
    // is always valid UTF-8.
    std::size_t json_size() const noexcept {
        JsonWriter measure(nullptr,0);
        write_json_to(measure);
        return measure.size()+1;
    }
    std::size_t write_json(char* output,std::size_t capacity) const noexcept {
        const auto required=json_size();
        if(!output||capacity<required)return 0;
        JsonWriter writer(output,capacity);
        write_json_to(writer);
        output[writer.size()]='\0';
        return writer.size();
    }

private:
    static void copy_cstr(char* out,std::uint16_t& copied,bool& truncated,const char* source) noexcept {
        copied=0;truncated=false;
        if(!source)return;
        while(copied<string_capacity&&source[copied]){out[copied]=source[copied];++copied;}
        if(copied==string_capacity&&source[copied])truncated=true;
    }
    static void copy_bytes(char* out,std::uint16_t& copied,bool& truncated,
                           const std::uint8_t* source,std::size_t size) noexcept {
        copied=static_cast<std::uint16_t>(size<string_capacity?size:string_capacity);
        truncated=size>string_capacity;
        if(source&&copied)std::memcpy(out,source,copied);
    }
    Event base(Kind kind) const noexcept {
        Event event;event.kind=static_cast<std::uint8_t>(kind);
        event.phase=static_cast<std::uint8_t>(phase_);event.scene=scene_;return event;
    }
    static std::uint64_t hash_event(const Event& event) noexcept {
        std::uint64_t hash=1469598103934665603ull;
        const auto add=[&](std::uint8_t value){hash=(hash^value)*1099511628211ull;};
        add(event.kind);add(event.phase);add(event.operation);add(event.encoding);add(event.source);add(event.flags);
        const auto add32=[&](std::uint32_t value){for(unsigned i=0;i<4;++i)add(std::uint8_t(value>>(i*8)));};
        add32(static_cast<std::uint32_t>(event.scene));
        if(event.kind!=static_cast<std::uint8_t>(Kind::Present)){
            add32(static_cast<std::uint32_t>(event.a));add32(static_cast<std::uint32_t>(event.b));
        }
        add32(static_cast<std::uint32_t>(event.c));add32(static_cast<std::uint32_t>(event.d));
        add32(event.text_size);add32(event.value_size);add32(event.extra_size);
        for(std::size_t i=0;i<event.text_size;++i)add(static_cast<std::uint8_t>(event.text[i]));
        for(std::size_t i=0;i<event.value_size;++i)add(static_cast<std::uint8_t>(event.value[i]));
        for(std::size_t i=0;i<event.extra_size;++i)add(static_cast<std::uint8_t>(event.extra[i]));
        return hash;
    }
    static bool same_tuple(const Event& left,const Event& right) noexcept {
        const bool present=left.kind==static_cast<std::uint8_t>(Kind::Present);
        return left.kind==right.kind&&left.phase==right.phase&&left.scene==right.scene&&
            left.operation==right.operation&&left.encoding==right.encoding&&left.source==right.source&&
            left.flags==right.flags&&(present||(left.a==right.a&&left.b==right.b))&&left.c==right.c&&left.d==right.d&&
            left.text_size==right.text_size&&left.value_size==right.value_size&&left.extra_size==right.extra_size&&
            std::memcmp(left.text,right.text,left.text_size)==0&&
            std::memcmp(left.value,right.value,left.value_size)==0&&
            std::memcmp(left.extra,right.extra,left.extra_size)==0;
    }
    void append(Event event) noexcept {
        ++observations_;
        event.tuple_hash=hash_event(event);
        const bool complete=!(event.text_truncated||event.value_truncated||event.extra_truncated);
        if(complete)for(std::uint32_t i=0;i<size_;++i){
            auto& previous=events_[i];
            if(previous.tuple_hash==event.tuple_hash&&same_tuple(previous,event)){
                previous.last=observations_;if(previous.count!=UINT32_MAX)++previous.count;
                if(event.kind==static_cast<std::uint8_t>(Kind::Present))previous.b=event.b;
                return;
            }
        }
        if(event.text_truncated||event.value_truncated||event.extra_truncated)strings_truncated_=true;
        if(size_==event_capacity){++dropped_;return;}
        event.first=event.last=observations_;event.count=1;events_[size_++]=event;
    }
    void append_preparation_glyph(const Event& event) noexcept {
        ++observations_;
        for(std::uint32_t i=0;i<preparation_glyph_size_;++i){
            auto& previous=preparation_glyphs_[i];
            if(previous.height==event.a&&previous.weight==event.b&&
               previous.source==event.source&&previous.flags==event.flags){
                previous.last=observations_;if(previous.count!=UINT32_MAX)++previous.count;
                return;
            }
        }
        if(preparation_glyph_size_==preparation_glyph_capacity){++preparation_glyph_dropped_;return;}
        auto& group=preparation_glyphs_[preparation_glyph_size_++];
        group.first=group.last=observations_;group.count=1;group.height=event.a;group.weight=event.b;
        group.source=event.source;group.flags=event.flags;
    }
    void reset() noexcept {
        size_=preparation_glyph_size_=0;observations_=dropped_=preparation_glyph_dropped_=0;
        scene_=-1;strings_truncated_=false;
    }

    class JsonWriter {
    public:
        JsonWriter(char* output,std::size_t capacity):output_(output),capacity_(capacity){}
        void literal(const char* text) noexcept {while(*text)byte(*text++);}
        void byte(char value) noexcept {if(output_&&size_<capacity_)output_[size_]=value;++size_;}
        void number(std::uint64_t value) noexcept {
            char digits[24];unsigned count=0;do{digits[count++]=char('0'+value%10);value/=10;}while(value);
            while(count)byte(digits[--count]);
        }
        void signed_number(std::int32_t value) noexcept {
            if(value<0){byte('-');number(std::uint64_t(-std::int64_t(value)));}else number(std::uint32_t(value));
        }
        void boolean(bool value) noexcept {literal(value?"true":"false");}
        void hex(const char* value,std::size_t length) noexcept {
            static constexpr char digits[]="0123456789abcdef";
            for(std::size_t i=0;i<length;++i){const auto byte_value=static_cast<std::uint8_t>(value[i]);byte(digits[byte_value>>4]);byte(digits[byte_value&15]);}
        }
        std::size_t size() const noexcept {return size_;}
    private:
        char* output_;std::size_t capacity_,size_=0;
    };

    static const char* kind_name(std::uint8_t value) noexcept {
        switch(static_cast<Kind>(value)){case Kind::Lookup:return "lookup";case Kind::Raster:return "raster";case Kind::Glyph:return "glyph";case Kind::Present:return "present";}return "unknown";
    }
    static const char* phase_name(std::uint8_t value) noexcept {
        switch(static_cast<Phase>(value)){case Phase::Preparation:return "prepare";case Phase::Runtime:return "runtime";default:return "none";}
    }
    static const char* operation_name(std::uint8_t value) noexcept {
        switch(static_cast<LookupOperation>(value)){
        case LookupOperation::SpellName:return "spell-name";case LookupOperation::StageName:return "stage-name";
        case LookupOperation::MusicTitle:return "music-title";case LookupOperation::MusicComment:return "music-comment";
        case LookupOperation::SpellComment:return "spell-comment";case LookupOperation::SpellCommentOwner:return "spell-comment-owner";
        case LookupOperation::LookupAscii:return "lookup-ascii";case LookupOperation::AsciiString:return "ascii-string";
        case LookupOperation::AsciiStringById:return "ascii-string-by-id";case LookupOperation::StringById:return "string-by-id";
        case LookupOperation::FormatStringById:return "format-string-by-id";default:return "none";}
    }
    static const char* encoding_name(std::uint8_t value) noexcept {
        switch(static_cast<Encoding>(value)){case Encoding::Raw:return "raw";case Encoding::Utf8:return "utf8";case Encoding::Cp932:return "cp932";default:return "none";}
    }
    static const char* source_name(std::uint8_t value) noexcept {
        switch(static_cast<GlyphSource>(value)){case GlyphSource::Bitmap:return "bitmap";case GlyphSource::Msgothic:return "msgothic";
        case GlyphSource::Pack:return "pack";case GlyphSource::Unifont:return "unifont";case GlyphSource::MsgothicNotdef:return "msgothic-notdef";
        case GlyphSource::AsciiAtlas:return "ascii-atlas";default:return "none";}
    }
    void write_json_to(JsonWriter& out) const noexcept {
        out.literal("{\"schema\":\"eagler-touhou/language-trace/1\",\"enabled\":");out.boolean(enabled_);
        out.literal(",\"eventCapacity\":");out.number(event_capacity);out.literal(",\"totalObservations\":");out.number(observations_);
        out.literal(",\"uniqueEvents\":");out.number(size_);out.literal(",\"droppedEvents\":");out.number(dropped_);
        out.literal(",\"overflow\":");out.boolean(dropped_!=0);
        out.literal(",\"stringTruncated\":");out.boolean(strings_truncated_);out.literal(",\"events\":[");
        for(std::uint32_t i=0;i<size_;++i){const auto& e=events_[i];if(i)out.byte(',');
            out.literal("{\"first\":");out.number(e.first);out.literal(",\"last\":");out.number(e.last);out.literal(",\"count\":");out.number(e.count);
            out.literal(",\"kind\":\"");out.literal(kind_name(e.kind));out.literal("\",\"phase\":\"");out.literal(phase_name(e.phase));
            out.literal("\",\"scene\":");out.signed_number(e.scene);out.literal(",\"operation\":\"");out.literal(operation_name(e.operation));
            out.literal("\",\"encoding\":\"");out.literal(encoding_name(e.encoding));out.literal("\",\"source\":\"");out.literal(source_name(e.source));
            out.literal("\",\"flags\":");out.number(e.flags);out.literal(",\"a\":");out.signed_number(e.a);out.literal(",\"b\":");out.signed_number(e.b);
            out.literal(",\"c\":");out.signed_number(e.c);out.literal(",\"d\":");out.signed_number(e.d);
            out.literal(",\"textHex\":\"");out.hex(e.text,e.text_size);out.literal("\",\"valueHex\":\"");out.hex(e.value,e.value_size);
            out.literal("\",\"extraHex\":\"");out.hex(e.extra,e.extra_size);
            out.literal("\",\"textTruncated\":");out.boolean(e.text_truncated);out.literal(",\"valueTruncated\":");out.boolean(e.value_truncated);
            out.literal(",\"extraTruncated\":");out.boolean(e.extra_truncated);out.byte('}');
        }
        out.literal("],\"preparationGlyphs\":{\"observations\":");out.number(preparation_glyph_observations());
        out.literal(",\"groupCapacity\":");out.number(preparation_glyph_capacity);
        out.literal(",\"droppedObservations\":");out.number(preparation_glyph_dropped_);
        out.literal(",\"overflow\":");out.boolean(preparation_glyph_dropped_!=0);
        out.literal(",\"groups\":[");
        for(std::uint32_t i=0;i<preparation_glyph_size_;++i){const auto& group=preparation_glyphs_[i];if(i)out.byte(',');
            out.literal("{\"first\":");out.number(group.first);out.literal(",\"last\":");out.number(group.last);
            out.literal(",\"count\":");out.number(group.count);out.literal(",\"height\":");out.signed_number(group.height);
            out.literal(",\"weight\":");out.signed_number(group.weight);out.literal(",\"source\":\"");out.literal(source_name(group.source));
            out.literal("\",\"flags\":");out.number(group.flags);out.byte('}');
        }
        out.literal("]}}");
    }

    std::uint64_t preparation_glyph_observations() const noexcept {
        std::uint64_t count=preparation_glyph_dropped_;
        for(std::uint32_t i=0;i<preparation_glyph_size_;++i)count+=preparation_glyphs_[i].count;
        return count;
    }

    bool enabled_=false,strings_truncated_=false;
    Phase phase_=Phase::None;
    std::int32_t scene_=-1;
    std::uint32_t size_=0;
    std::uint64_t observations_=0,dropped_=0,preparation_glyph_dropped_=0;
    std::uint32_t preparation_glyph_size_=0;
    Event events_[event_capacity]{};
    PreparationGlyphGroup preparation_glyphs_[preparation_glyph_capacity]{};
};

inline LanguageTrace& language_trace() noexcept {
    static LanguageTrace recorder;
    return recorder;
}

} // namespace eagler::diagnostics
