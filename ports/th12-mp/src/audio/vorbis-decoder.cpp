// A bounded, seekable pull decoder. Encoded input remains owned by the caller;
// no whole-track PCM allocation or whole-track decode entry point is exposed.
#define STB_VORBIS_NO_STDIO
#define STB_VORBIS_NO_PUSHDATA_API
#define STB_VORBIS_NO_INTEGER_CONVERSION
#include "../../third_party/stb_vorbis/stb_vorbis.h"
#include <cstdint>
#include <cstdlib>

namespace {
constexpr int max_channels = 8;
constexpr int max_frames = 16384;
constexpr int invalid_argument = -1;
constexpr int invalid_stream = -2;
constexpr int allocation_failed = -3;
int last_error = 0;

struct Decoder {
    stb_vorbis* stream;
    float* pcm;
    unsigned int frames;
    unsigned int sample_rate;
    int channels;
};
}

extern "C" {
int th12_vorbis_error() { return last_error; }
int th12_vorbis_max_frames() { return max_frames; }

Decoder* th12_vorbis_open(const unsigned char* encoded, int bytes) {
    last_error = 0;
    if (!encoded || bytes <= 0) { last_error = invalid_argument; return nullptr; }
    int error = 0;
    stb_vorbis* stream = stb_vorbis_open_memory(encoded, bytes, &error, nullptr);
    if (!stream) { last_error = error ? error : invalid_stream; return nullptr; }
    const auto info = stb_vorbis_get_info(stream);
    const auto frames = stb_vorbis_stream_length_in_samples(stream);
    error = stb_vorbis_get_error(stream);
    if (error || !info.sample_rate || info.channels < 1 || info.channels > max_channels ||
        !frames || frames >= 0xfffffffeu) {
        stb_vorbis_close(stream);
        last_error = error ? error : invalid_stream;
        return nullptr;
    }
    auto* decoder = static_cast<Decoder*>(std::malloc(sizeof(Decoder)));
    auto* pcm = static_cast<float*>(std::malloc(sizeof(float) * max_frames * info.channels));
    if (!decoder || !pcm) {
        std::free(decoder); std::free(pcm); stb_vorbis_close(stream);
        last_error = allocation_failed;
        return nullptr;
    }
    *decoder = {stream, pcm, frames, info.sample_rate, info.channels};
    return decoder;
}

void th12_vorbis_close(Decoder* decoder) {
    if (!decoder) return;
    stb_vorbis_close(decoder->stream);
    std::free(decoder->pcm);
    std::free(decoder);
}

unsigned int th12_vorbis_length(Decoder* decoder) { return decoder ? decoder->frames : 0; }
unsigned int th12_vorbis_rate(Decoder* decoder) { return decoder ? decoder->sample_rate : 0; }
int th12_vorbis_channels(Decoder* decoder) { return decoder ? decoder->channels : 0; }
float* th12_vorbis_pcm(Decoder* decoder) { return decoder ? decoder->pcm : nullptr; }

int th12_vorbis_seek(Decoder* decoder, unsigned int frame) {
    last_error = 0;
    if (!decoder || frame >= decoder->frames) { last_error = invalid_argument; return 0; }
    if (!stb_vorbis_seek(decoder->stream, frame)) {
        last_error = stb_vorbis_get_error(decoder->stream);
        if (!last_error) last_error = invalid_stream;
        return 0;
    }
    return 1;
}

int th12_vorbis_read(Decoder* decoder, int frames) {
    last_error = 0;
    if (!decoder || frames <= 0 || frames > max_frames) {
        last_error = invalid_argument;
        return -1;
    }
    float* channels[max_channels];
    for (int i = 0; i < decoder->channels; ++i) channels[i] = decoder->pcm + i * max_frames;
    const int read = stb_vorbis_get_samples_float(decoder->stream, decoder->channels, channels, frames);
    const int error = stb_vorbis_get_error(decoder->stream);
    if (error) { last_error = error; return -1; }
    return read;
}
}
