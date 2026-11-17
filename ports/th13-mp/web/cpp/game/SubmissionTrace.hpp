#pragma once
#include "RenderSubmission.hpp"
#include <cstdio>
#include <cstring>
#include <string>
#include <tuple>

namespace th13 {
// Independent schema, NOT draw-v1. Read-only observer of production transactions.
// Float values are bit-exact hex bytes (including NaNs); no lossy JSON numbers.
class SubmissionTrace {
public:
    // Description copied from the real resolver, never an alternate GPU state.
    struct Surface {
        uint32_t width = 0, height = 0, format = 0, pitch = 0, renderScale = 1, version = 0;
        const void* data = nullptr;
        size_t size = 0;
    };
    struct Resource {
        Surface surface{};
        std::string identity;
        // GPU-written pixels are not CPU pixels: do not hash stale backing data.
        bool gpu_content = false;
        uint64_t revision = 0;
        std::string content_hash;
    };
    using Resolve = bool (*)(void*, uint32_t, Resource&);
    SubmissionTrace() = default;
    SubmissionTrace(const SubmissionTrace&) = delete;
    SubmissionTrace& operator=(const SubmissionTrace&) = delete;
    ~SubmissionTrace() { close(); }
    // CPU content cache is keyed by every surface descriptor plus the producer's
    // content revision. Writers must invalidate before exposing mutable pixels.
    class ContentHash {
        std::string value_;
        std::tuple<uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,const void*,size_t,uint64_t> key_{};
    public:
        void invalidate() { value_.clear(); }
        const std::string& get(const Surface& s, uint64_t revision) {
            const auto key = std::make_tuple(s.width,s.height,s.format,s.pitch,s.renderScale,s.version,s.data,s.size,revision);
            if (value_.empty() || key != key_) { key_ = key; value_ = sha256(s.data, s.size); }
            return value_;
        }
    };
    static std::string sha256(const void* data, size_t size) {
        static constexpr uint32_t k[] = {
            0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
            0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
            0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
            0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
            0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
            0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
            0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
            0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2};
        uint32_t h[] = {0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19};
        const auto* input = static_cast<const unsigned char*>(data);
        const size_t blocks = size / 64 + (size % 64 < 56 ? 1 : 2);
        auto rotr = [](uint32_t x, unsigned n) { return (x >> n) | (x << (32 - n)); };
        for (size_t block = 0; block < blocks; ++block) {
            unsigned char bytes[64]{};
            for (size_t j = 0; j < 64; ++j) {
                const size_t i = block * 64 + j;
                if (i < size) bytes[j] = input[i];
                else if (i == size) bytes[j] = 0x80;
                if (block == blocks - 1 && j >= 56) bytes[j] = static_cast<unsigned char>((uint64_t(size) * 8) >> ((63-j)*8));
            }
            uint32_t w[64];
            for (unsigned i = 0; i < 16; ++i) w[i] = uint32_t(bytes[i*4])<<24 | uint32_t(bytes[i*4+1])<<16 | uint32_t(bytes[i*4+2])<<8 | bytes[i*4+3];
            for (unsigned i = 16; i < 64; ++i) {
                const auto a = w[i-15], b = w[i-2];
                w[i] = w[i-16] + (rotr(a,7)^rotr(a,18)^(a>>3)) + w[i-7] + (rotr(b,17)^rotr(b,19)^(b>>10));
            }
            uint32_t a=h[0],b=h[1],c=h[2],d=h[3],e=h[4],f=h[5],g=h[6],v=h[7];
            for (unsigned i=0;i<64;++i) {
                const auto t1=v+(rotr(e,6)^rotr(e,11)^rotr(e,25))+((e&f)^(~e&g))+k[i]+w[i];
                const auto t2=(rotr(a,2)^rotr(a,13)^rotr(a,22))+((a&b)^(a&c)^(b&c));
                v=g;g=f;f=e;e=d+t1;d=c;c=b;b=a;a=t1+t2;
            }
            h[0]+=a;h[1]+=b;h[2]+=c;h[3]+=d;h[4]+=e;h[5]+=f;h[6]+=g;h[7]+=v;
        }
        std::string result;
        for (auto n : h) { char hex[9]; std::snprintf(hex,sizeof hex,"%08x",n); result += hex; }
        return result;
    }
    bool open(const char* path) {
        if (out_) return fail("trace already open");
        return open_stream(path ? std::fopen(path, "wb") : nullptr);
    }
    // Takes ownership, including on header-write failure. Allows platform I/O
    // adapters and exercises real write/flush/close failures without mock flags.
    bool open_stream(FILE* stream) {
        if (out_) return fail("trace already open");
        error_.clear(); frames_ = submissions_ = 0; pending_ = false;
        out_ = stream;
        if (!out_) return fail("trace open failed");
        const uint32_t endian = 1;
        if (sizeof(float) != 4 || *reinterpret_cast<const unsigned char*>(&endian) != 1)
            return fail("trace requires float32 little-endian");
        return write("{\"schema\":\"th13-submission-v2\",\"encoding\":\"float32-le-hex\",\"content_hash\":\"sha256\",\"qualification\":\"unqualified:title-RT-composition-not-reconstructed;GPU-content-not-read-back\"}\n");
    }
    bool close(uint64_t expected = UINT64_MAX) noexcept {
        if (out_) {
            if (pending_) fail("trace has incomplete frame (no present)");
            if (expected != UINT64_MAX && expected != frames_) fail("trace frame count mismatch");
            std::string footer = "{\"schema\":\"th13-submission-v2\",\"kind\":\"complete\",\"frames\":" + std::to_string(frames_) + ",\"events\":" + std::to_string(submissions_) + ",\"complete\":" + (good() ? "true" : "false") + ",\"error\":";
            quote(footer, error_); footer += "}\n";
            write(footer);
            if (std::fflush(out_) != 0) fail("trace flush failed");
            if (std::fclose(out_) != 0) fail("trace close failed");
            out_ = nullptr;
        }
        return error_.empty();
    }
    bool active() const { return out_ != nullptr; }
    bool good() const { return error_.empty(); }
    const std::string& error() const { return error_; }
    uint64_t frames_out() const { return frames_; }
    long output_position() {
        if (!out_ || !good()) return -1;
        if (std::fflush(out_) != 0) { fail("trace flush failed"); return -1; }
        const long n = std::ftell(out_);
        if (n < 0) fail("trace tell failed");
        return n;
    }
    template<class State> bool consume(const BasicRenderSubmission<State>& c, Resolve resolve, void* owner) {
        using Submission = BasicRenderSubmission<State>;
        if (!out_) return good();
        if (!good()) return false;
        if (unsigned(c.kind) > unsigned(Submission::Kind::Flush)) return fail("trace invalid submission kind");
        std::string line = "{\"frame\":" + std::to_string(frames_) + ",\"seq\":" + std::to_string(submissions_++) + ",\"kind\":";
        static const char* kinds[] = {"draw", "clear", "target", "update", "prepare", "release", "present", "flush"};
        quote(line, kinds[unsigned(c.kind)]);
        line += ",\"state\":"; state(line, c.state);
        line += ",\"resources\":[";
        const uint32_t ids[] = {c.state.texture, c.state.target, c.state.depth, c.resource};
        for (unsigned i = 0; i < 4; ++i) {
            if (i) line += ',';
            if (!ids[i]) { line += "{\"binding\":\"none\",\"handle\":0}"; continue; }
            Resource r;
            if (!resolve(owner, ids[i], r)) return fail("trace unknown resource (not null)");
            line += "{\"handle\":" + std::to_string(ids[i]) + ",\"identity\":"; quote(line, r.identity);
            line += ",\"revision\":" + std::to_string(r.revision);
            const auto& s = r.surface;
            line += ",\"surface\":[";
            numbers(line, s.width, s.height, uint32_t(s.format), s.pitch, s.renderScale, s.version, s.size);
            line += "],\"content\":";
            if (r.gpu_content || !s.data || !s.size) {
                quote(line, r.gpu_content ? "unqualified:GPU-written-content" : "unqualified:no-CPU-content");
            } else {
                if (r.content_hash.empty()) return fail("trace missing resource content hash");
                quote(line, r.content_hash);
            }
            line += '}';
        }
        line += "],\"topology\":" + std::to_string(unsigned(c.topology)) + ",\"count\":" + std::to_string(c.count) + ",\"stride\":" + std::to_string(c.stride) + ",\"direct\":" + std::to_string(c.direct);
        line += ",\"vertices\":";
        if (c.kind == Submission::Kind::Draw) {
            if (!c.vertices || !c.stride || !c.count || !c.vertex_bytes() ||
                unsigned(c.topology) > unsigned(touhou::graphics::Topology::Fan)) return fail("trace invalid geometry");
            bytes(line, c.vertices, c.vertex_bytes());
        } else quote(line, "");
        line += ",\"clear\":["; numbers(line, c.flags, c.color, 1, 0); line += "],\"rect\":[";
        if (c.rect) numbers(line, c.rect[0], c.rect[1], c.rect[2], c.rect[3]);
        line += "],\"resource\":" + std::to_string(c.resource) + ",\"previous\":" + std::to_string(c.previous) + ",\"region\":[";
        numbers(line, c.region[0], c.region[1], c.region[2], c.region[3]); line += "]}\n";
        if (!write(line)) return false;
        if (c.kind == Submission::Kind::Present) {
            ++frames_; pending_ = false;
        } else pending_ = true;
        return true;
    }
private:
    FILE* out_ = nullptr;
    std::string error_;
    uint64_t frames_ = 0, submissions_ = 0;
    bool pending_ = false;
    bool fail(const char* why) noexcept { if (error_.empty()) error_ = why; return false; }
    bool write(const std::string& line) {
        if (std::fwrite(line.data(), 1, line.size(), out_) != line.size() || std::fflush(out_) != 0)
            return fail("trace write failed");
        return true;
    }
    static void quote(std::string& out, const std::string& text) {
        out += '"';
        for (unsigned char c : text) {
            if (c == '"' || c == '\\') { out += '\\'; out += char(c); }
            else if (c < 32 || c >= 127) { char b[7]; std::snprintf(b, sizeof b, "\\u%04x", c); out += b; }
            else out += char(c);
        }
        out += '"';
    }
    template<class... T> static void numbers(std::string& out, T... values) {
        bool first = true;
        ((out += (first ? (first = false, "") : ",") + std::to_string(values)), ...);
    }
    static void bytes(std::string& out, const void* data, size_t size) {
        static constexpr char hex[] = "0123456789abcdef";
        out += '"'; const auto* p = static_cast<const unsigned char*>(data);
        for (size_t i = 0; i < size; ++i) { out += hex[p[i] >> 4]; out += hex[p[i] & 15]; }
        out += '"';
    }
    template<class State> static void state(std::string& out, const State& s) {
        const auto& p = s.pipeline;
        out += "{\"pipeline\":[";
        numbers(out, p.depthTest,p.depthWrite,p.blend,p.alphaTest,p.fog,p.rangeFog,p.dither,
            unsigned(p.depthCompare),unsigned(p.alphaCompare),unsigned(p.sourceBlend),unsigned(p.destinationBlend),unsigned(p.blendEquation),
            p.separateAlphaBlend,unsigned(p.sourceAlphaBlend),unsigned(p.destinationAlphaBlend),unsigned(p.alphaBlendEquation),unsigned(p.cull),
            p.colorMask,p.alphaReference,p.fogColor,p.textureFactor,unsigned(p.fogMode));
        out += ","; bytes(out, &p.fogNear, 4); out += ','; bytes(out, &p.fogFar, 4); out += ','; bytes(out, &p.fogDensity, 4);
        auto combiner = [&](const touhou::graphics::Combiner& c) {
            out += ','; numbers(out, unsigned(c.operation),unsigned(c.first.source),c.first.complement,c.first.alphaOnly,unsigned(c.second.source),c.second.complement,c.second.alphaOnly);
        };
        combiner(p.color); combiner(p.alpha); out += ',';
        numbers(out, p.textureTransform,unsigned(p.minFilter),unsigned(p.magFilter),unsigned(p.addressU),unsigned(p.addressV));
        out += "],\"matrices\":[";
        for (unsigned i = 0; i < 4; ++i) { if (i) out += ','; bytes(out, s.matrix[i].data(), 64); }
        out += "],\"viewport\":[";
        numbers(out, s.viewport.x,s.viewport.y,s.viewport.width,s.viewport.height); out += ',';
        bytes(out, &s.viewport.min, 4); out += ','; bytes(out, &s.viewport.max, 4);
        out += "],\"bindings\":["; numbers(out,s.texture,s.target,s.depth,s.stride);
        out += "],\"layout\":["; numbers(out,s.layout.screen,s.layout.diffuse,s.layout.specular,s.layout.uv); out += "]}";
    }
};
}
