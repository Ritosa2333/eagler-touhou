#include "ReplayFile.hpp"

namespace th13 {

namespace {
constexpr u32 kHeaderSize = 0x24;
constexpr u32 kTopSize = 0x74;
constexpr u32 kStageHeaderSize = 0xC4;
constexpr u32 kFrameBytes = 6;

u16 read_u16(const u8* p) { return u16(p[0]) | u16(p[1]) << 8; }
u32 read_u32(const u8* p) { return u32(p[0]) | u32(p[1]) << 8 | u32(p[2]) << 16 | u32(p[3]) << 24; }
i32 read_i32(const u8* p) { return i32(read_u32(p)); }
float read_f32(const u8* p) {
    const u32 bits = read_u32(p);
    float value;
    std::memcpy(&value, &bits, sizeof value);
    return value;
}
}

ReplayFrame ReplayStage::frame(u32 index) const {
    ReplayFrame result;
    if (index < frame_count && (index + 1) * kFrameBytes <= input.size()) {
        const u8* p = input.data() + index * kFrameBytes;
        result.state = read_u16(p);
        result.pressed = read_u16(p + 2);
        result.released = read_u16(p + 4);
    }
    return result;
}

namespace {
std::vector<size_t> transform_plan(size_t size, u32 block) {
    const size_t remainder = size % block;
    const size_t tail = remainder < block / 4 ? remainder : 0;
    const size_t aligned = size - (size & 1);
    size_t remaining = aligned >= tail ? aligned - tail : 0;
    std::vector<size_t> chunks;
    while (remaining > 0) {
        const size_t chunk = remaining < block ? remaining : block;
        chunks.push_back(chunk);
        remaining -= chunk;
    }
    return chunks;
}
}

void ReplayFile::xor_decrypt(u8* buf, size_t size, u32 init_key, u32 step, u32 block) {
    const std::vector<size_t> chunks = transform_plan(size, block);
    const std::vector<u8> source(buf, buf + size);
    u32 key = init_key & 0xFF;
    size_t pos = 0;
    for (size_t chunk : chunks) {
        const size_t base = pos;
        const size_t half = (chunk + 1) / 2;
        size_t write = base + chunk - 1;
        for (size_t i = 0; i < half; ++i) {
            buf[write] = u8(source[pos] ^ key);
            key = (key + step) & 0xFF;
            write -= 2;
            ++pos;
        }
        write = base + chunk;
        for (size_t i = 0; i < chunk - half; ++i) {
            write -= 2;
            buf[write] = u8(source[pos] ^ key);
            key = (key + step) & 0xFF;
            ++pos;
        }
    }
}

void ReplayFile::xor_encrypt(u8* buf, size_t size, u32 init_key, u32 step, u32 block) {
    const std::vector<size_t> chunks = transform_plan(size, block);
    const std::vector<u8> source(buf, buf + size);
    u32 key = init_key & 0xFF;
    size_t pos = 0;
    for (size_t chunk : chunks) {
        const size_t base = pos;
        const size_t half = (chunk + 1) / 2;
        size_t read = base + chunk - 1;
        for (size_t i = 0; i < half; ++i) {
            buf[pos] = u8(source[read] ^ key);
            key = (key + step) & 0xFF;
            read -= 2;
            ++pos;
        }
        read = base + chunk;
        for (size_t i = 0; i < chunk - half; ++i) {
            read -= 2;
            buf[pos] = u8(source[read] ^ key);
            key = (key + step) & 0xFF;
            ++pos;
        }
    }
}

bool ReplayFile::inflate(const u8* src, size_t src_size, std::vector<u8>& out, size_t limit) {
    out.clear();
    if (limit == 0) return true;
    out.reserve(limit);
    u8 ring[0x2000] = {};
    u32 ring_pos = 1;
    size_t bit = 0;
    const auto read_bit = [&]() -> u32 {
        const u32 byte = bit >> 3 < src_size ? src[bit >> 3] : 0;
        const u32 value = byte >> (7 - (bit & 7)) & 1;
        ++bit;
        return value;
    };
    const auto read_bits = [&](u32 count) {
        u32 value = 0;
        for (u32 i = 0; i < count; ++i)
            value = value << 1 | read_bit();
        return value;
    };
    while (true) {
        if (read_bit()) {
            const u32 byte = read_bits(8);
            out.push_back(u8(byte));
            ring[ring_pos] = u8(byte);
            ring_pos = (ring_pos + 1) & 0x1FFF;
        } else {
            const u32 address = read_bits(13);
            if (address == 0) break;
            const u32 count = read_bits(4);
            for (u32 i = 0; i < count + 3; ++i) {
                const u8 byte = ring[(address + i) & 0x1FFF];
                out.push_back(byte);
                ring[ring_pos] = byte;
                ring_pos = (ring_pos + 1) & 0x1FFF;
            }
        }
        if (out.size() > limit) return false;
    }
    return out.size() == limit;
}

namespace {
constexpr u32 kDeflateRing = 0x2000;
constexpr u32 kDeflateMask = 0x1FFF;
constexpr u32 kDeflateMaxMatch = 0x12;
constexpr u32 kDeflateMinMatch = 3;
constexpr u32 kDeflateSentinel = 0x2000;

void deflate_tree_remove(std::vector<i32>& dad, std::vector<i32>& lft, std::vector<i32>& rgt, u32 x) {
    if (dad[x] == 0) return;
    u32 repl;
    if (rgt[x] == 0) {
        repl = u32(lft[x]);
    } else if (lft[x] == 0) {
        repl = u32(rgt[x]);
    } else {
        u32 v = u32(lft[x]);
        while (rgt[v] != 0)
            v = u32(rgt[v]);
        deflate_tree_remove(dad, lft, rgt, v);
        const i32 dd = dad[x];
        if (lft[dd] == i32(x))
            lft[dd] = i32(v);
        else
            rgt[dd] = i32(v);
        dad[v] = dd;
        lft[v] = lft[x];
        rgt[v] = rgt[x];
        dad[lft[v]] = i32(v);
        dad[rgt[v]] = i32(v);
        dad[x] = 0;
        return;
    }
    const i32 dd = dad[x];
    dad[repl] = dd;
    if (rgt[dd] == i32(x))
        rgt[dd] = i32(repl);
    else
        lft[dd] = i32(repl);
    dad[x] = 0;
}
}

void LzssEncoder::begin(const u8* src, size_t size) {
    src_.assign(src, src + size);
    out_.clear();
    acc_ = 0;
    mask_ = 0x80;
    ring_.assign(size_t(2) * kDeflateRing, 0);
    dad_.assign(kDeflateSentinel + 1, 0);
    lft_.assign(kDeflateSentinel + 1, 0);
    rgt_.assign(kDeflateSentinel + 1, 0);
    u32 n = 0;
    size_t pos = 0;
    while (n < kDeflateMaxMatch && pos < size) {
        ring_[1 + n] = src[pos];
        ring_[1 + size_t(n) + kDeflateRing] = src[pos];
        ++pos;
        ++n;
    }
    lookahead_ = n;
    fill_ = pos;
    wp_ = 1;
    rgt_[kDeflateSentinel] = 1;
    dad_[1] = i32(kDeflateSentinel);
    match_len_ = 0;
    match_pos_ = 0;
    active_ = true;
    finished_ = false;
}

void LzssEncoder::put_bit(u32 bit) {
    if (bit) acc_ |= mask_;
    mask_ >>= 1;
    if (!mask_) {
        out_.push_back(u8(acc_));
        acc_ = 0;
        mask_ = 0x80;
    }
}

u32 LzssEncoder::tree_insert(u32 p) {
    if (p == 0) return 0;
    u32 node = u32(rgt_[kDeflateSentinel]);
    u32 best = 0;
    while (true) {
        u32 i = 0;
        i32 c = 0;
        for (; i < kDeflateMaxMatch; ++i) {
            c = i32(ring_[p + i]) - i32(ring_[node + i]);
            if (c != 0) break;
        }
        if (i >= best) {
            best = i;
            match_pos_ = node;
        }
        if (i >= kDeflateMaxMatch) {
            const i32 dd = dad_[node];
            if (lft_[dd] == i32(node))
                lft_[dd] = i32(p);
            else
                rgt_[dd] = i32(p);
            dad_[p] = dd;
            lft_[p] = lft_[node];
            rgt_[p] = rgt_[node];
            dad_[lft_[p]] = i32(p);
            dad_[rgt_[p]] = i32(p);
            dad_[node] = 0;
            return kDeflateMaxMatch;
        }
        const u32 child = c < 0 ? u32(lft_[node]) : u32(rgt_[node]);
        if (child == 0) {
            if (c < 0)
                lft_[node] = i32(p);
            else
                rgt_[node] = i32(p);
            dad_[p] = i32(node);
            lft_[p] = 0;
            rgt_[p] = 0;
            return best;
        }
        node = child;
    }
}

void LzssEncoder::advance(u32 count) {
    for (u32 step = 0; step < count; ++step) {
        const u32 r = (wp_ + kDeflateMaxMatch) & kDeflateMask;
        if (dad_[r] != 0) deflate_tree_remove(dad_, lft_, rgt_, r);
        if (fill_ < src_.size()) {
            const u8 byte = src_[fill_];
            ++fill_;
            ring_[r] = byte;
            ring_[size_t(r) + kDeflateRing] = byte;
        } else {
            --lookahead_;
        }
        wp_ = (wp_ + 1) & kDeflateMask;
        if (lookahead_ != 0) match_len_ = tree_insert(wp_);
    }
}

bool LzssEncoder::step(u32 tokens) {
    if (!active_ || finished_) return true;
    while (lookahead_ > 0 && tokens > 0) {
        --tokens;
        const u32 ml = match_len_ < lookahead_ ? match_len_ : lookahead_;
        u32 count;
        if (ml <= 2) {
            put_bit(1);
            const u8 byte = ring_[wp_];
            for (int shift = 7; shift >= 0; --shift)
                put_bit(u32(byte >> shift) & 1);
            count = 1;
        } else {
            put_bit(0);
            for (int shift = 12; shift >= 0; --shift)
                put_bit(match_pos_ >> shift & 1);
            for (int shift = 3; shift >= 0; --shift)
                put_bit((ml - kDeflateMinMatch) >> shift & 1);
            count = ml;
        }
        advance(count);
    }
    if (lookahead_ > 0) return false;
    put_bit(0);
    for (int i = 0; i < 13; ++i)
        put_bit(0);
    finished_ = true;
    return true;
}

std::vector<u8> LzssEncoder::take() {
    std::vector<u8> out = std::move(out_);
    out_.clear();
    src_.clear();
    ring_.clear();
    dad_.clear();
    lft_.clear();
    rgt_.clear();
    active_ = false;
    return out;
}

std::vector<u8> ReplayFile::deflate(const u8* src, size_t size) {
    LzssEncoder encoder;
    encoder.begin(src, size);
    while (!encoder.step(1u << 16)) {
    }
    return encoder.take();
}

bool ReplayFile::decode(const u8* data, size_t size) {
    if (size < kHeaderSize) return fail("file too small");
    if (std::memcmp(data, "t13r", 4) != 0) return fail("bad magic");
    version = read_u16(data + 4);
    if (version != 2) return fail("unsupported replay version");
    user_offset = read_u32(data + 0xC);
    comp_size = read_u32(data + 0x1C);
    uncomp_size = read_u32(data + 0x20);
    if (user_offset != comp_size + kHeaderSize) return fail("user offset mismatch");
    if (size < size_t(comp_size) + kHeaderSize) return fail("truncated stage data");
    if (uncomp_size == 0 || uncomp_size > 0x4000000) return fail("bad uncompressed size");

    std::vector<u8> body(data + kHeaderSize, data + kHeaderSize + comp_size);
    xor_decrypt(body.data(), body.size(), 0x5C, 0xE1, 0x400);
    xor_decrypt(body.data(), body.size(), 0x7D, 0x3A, 0x100);

    std::vector<u8> plain;
    if (!inflate(body.data(), body.size(), plain, uncomp_size)) return fail("inflate failed");

    if (plain.size() < kTopSize) return fail("stage data too small");
    top_header.assign(plain.begin(), plain.begin() + ptrdiff_t(kTopSize));
    player_name.assign(reinterpret_cast<const char*>(plain.data()), std::size_t(12));
    const auto cut = player_name.find('\0');
    if (cut != std::string::npos) player_name.resize(cut);
    while (!player_name.empty() && player_name.back() == ' ')
        player_name.pop_back();
    score = read_i32(plain.data() + 0x14);
    slow_rate = read_f32(plain.data() + 0x54);
    stage_count = plain[0x58];
    character = read_i32(plain.data() + 0x5C);
    shot_type = read_i32(plain.data() + 0x60);
    rank = read_i32(plain.data() + 0x64);
    practice_stage = read_i32(plain.data() + 0x70);
    stages.clear();
    stages.reserve(stage_count);

    size_t pos = kTopSize;
    for (u32 s = 0; s < stage_count; ++s) {
        if (pos + kStageHeaderSize > plain.size()) return fail("stage header out of range");
        ReplayStage stage;
        stage.header.assign(plain.begin() + ptrdiff_t(pos), plain.begin() + ptrdiff_t(pos + kStageHeaderSize));
        stage.number = read_u16(plain.data() + pos);
        stage.stamp = read_u16(plain.data() + pos + 2);
        stage.frame_count = read_u32(plain.data() + pos + 4);
        stage.score = read_i32(plain.data() + pos + 0x1C);
        stage.blob_size = read_u32(plain.data() + pos + 8);
        const size_t input_bytes = size_t(stage.frame_count) * kFrameBytes;
        if (input_bytes > stage.blob_size) return fail("stage input exceeds blob");
        if (pos + kStageHeaderSize + stage.blob_size > plain.size()) return fail("stage blob out of range");
        stage.input.assign(plain.begin() + ptrdiff_t(pos + kStageHeaderSize),
                           plain.begin() + ptrdiff_t(pos + kStageHeaderSize + input_bytes));
        stage.events.assign(plain.begin() + ptrdiff_t(pos + kStageHeaderSize + input_bytes),
                            plain.begin() + ptrdiff_t(pos + kStageHeaderSize + stage.blob_size));
        u32 recorded = stage.frame_count;
        if (recorded > 0 && stage.frame(recorded - 1).eof()) --recorded;
        if (stage.events.size() != (recorded + 29) / 30) return fail("stage event stream length mismatch");
        stages.push_back(std::move(stage));
        pos += kStageHeaderSize + stage.blob_size;
    }

    user_blocks.clear();
    size_t user = user_offset;
    while (user + 12 <= size && std::memcmp(data + user, "USER", 4) == 0) {
        const u32 block = read_u32(data + user + 4);
        if (block < 12 || user + block > size) return fail("user block out of range");
        user_blocks.emplace_back(reinterpret_cast<const char*>(data + user + 12), std::size_t(block - 12));
        user += block;
    }
    return true;
}

}
