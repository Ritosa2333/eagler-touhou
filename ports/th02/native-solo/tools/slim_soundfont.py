#!/usr/bin/env python3
"""Build a lossless TH02-only subset of GeneralUser GS 1.44 for TinySoundFont."""
import argparse
from collections import defaultdict
from dataclasses import dataclass
import hashlib
from pathlib import Path
import struct


SONGS = (
    "OP", "STAGE0", "STAGE1", "STAGE2", "STAGE3", "STAGE4", "BOSS1", "BOSS4",
    "BOSS2", "BOSS3", "MIMA", "END1", "ENDING", "STAGE5", "BOSS5",
)
SOUNDFONT_SHA = "00725296486f652ba42bdbf2098254762ca80239353d04b55a27454fc8edb8a8"
RECORDS = {
    b"phdr": "<20sHHHIII", b"pbag": "<HH", b"pmod": "<5H", b"pgen": "<HH",
    b"inst": "<20sH", b"ibag": "<HH", b"imod": "<5H", b"igen": "<HH",
    b"shdr": "<20s5IBbHH",
}
MMD_COMMANDS = frozenset((*range(128), 0xE7, 0xEB, 0xEC, 0xEE, 0xF8, 0xF9))


def signed(value, bits):
    return value - (1 << bits) if value & (1 << (bits - 1)) else value


def float32(value):
    return struct.unpack("<f", struct.pack("<f", value))[0]


def mmd_events(data, start):
    if not 0x50 < start < len(data):
        raise ValueError("Invalid MMD track start")
    cache = [0] * 4
    cursor = start
    while cursor < len(data):
        command = data[cursor]
        cursor += 1
        if command == 0xFE:
            return
        if 0x80 <= command <= 0x8F:
            for column in range(4):
                if command & (8 >> column):
                    cache[column] = data[cursor]
                    cursor += 1
        else:
            cache = [command, *data[cursor:cursor + 3]]
            cursor += 3
        if len(cache) < 4 or cache[0] not in MMD_COMMANDS:
            raise ValueError("Invalid or unsupported MMD event")
        yield tuple(cache)
    raise ValueError("Missing MMD track terminator")


def resolve_preset(presets, channel, bank, program):
    bank &= 0x7FFF
    requested = [(bank, program), (0, program)]
    if channel == 9:
        requested[:0] = [(128 | bank, program), (128, program), (128, 0)]
    for patch in requested:
        if patch in presets:
            return patch
    raise ValueError(f"Missing source preset for channel {channel}, bank {bank}, program {program}")


def song_usage(data, presets):
    # TH02's songs have at most one active event stream per MIDI channel.
    # Delays cannot affect the channel's bank/program state in this case.
    if len(data) <= 0x50 or not data[0]:
        raise ValueError("Invalid MMD header")
    patches = defaultdict(set)
    channels = set()
    for track in range(18):
        start, transpose, channel = struct.unpack_from("<HBB", data, 2 + 4 * track)
        events = tuple(mmd_events(data, start))
        if channel == 255 or not events:
            continue
        if channel >= 16 or channel in channels:
            raise ValueError("Unsupported channel sharing in TH02 MMD")
        channels.add(channel)
        shift = 0 if channel == 9 or transpose & 128 else signed(data[1], 8) + signed(transpose, 8)
        bank = 0
        patch = resolve_preset(presets, channel, bank, 0)
        patches[patch]
        cursor, stack, seen = 0, [], set()
        while cursor < len(events):
            state = cursor, tuple(stack), bank, patch
            if state in seen:
                break
            seen.add(state)
            command, delay, a, b = events[cursor]
            cursor += 1
            if command <= 127 and a and b:
                key = max(0, min(127, command + shift))
                # TinySoundFont converts the normalized float to a MIDI velocity for zone matching.
                velocity = int(float32(float32(b / 127) * 127))
                patches[patch].add((key, velocity))
            elif command == 0xEB:
                if a == 0:
                    bank = 0x8000 | b
                elif a == 32:
                    bank = (((bank & 127) << 7) if bank & 0x8000 else 0) | b
                elif a == 121:
                    bank = 0
            elif command == 0xEC:
                patch = resolve_preset(presets, channel, bank, a)
                patches[patch]
            elif command == 0xF9:
                if len(stack) == 8:
                    raise ValueError("MMD loop stack overflow")
                stack.append((cursor, 1))
            elif command == 0xF8:
                begin, count = stack[-1]
                if delay == 0 or count < delay:
                    stack[-1] = begin, (count + 1 if delay else count)
                    cursor = begin
                else:
                    stack.pop()
    return patches


def chunks(data):
    cursor = 0
    while cursor < len(data):
        tag, size = struct.unpack_from("<4sI", data, cursor)
        begin = cursor + 8
        cursor = begin + size + (size & 1)
        if cursor > len(data):
            raise ValueError(f"Invalid RIFF chunk length for {tag!r}")
        yield tag, data[begin:begin + size]


def chunk(tag, data):
    return struct.pack("<4sI", tag, len(data)) + data + (b"\0" if len(data) & 1 else b"")


@dataclass(frozen=True)
class Zone:
    generators: tuple
    modulators: tuple
    reference: int | None
    keys: tuple
    velocities: tuple

    def admits(self, key, velocity):
        return self.keys[0] <= key <= self.keys[1] and self.velocities[0] <= velocity <= self.velocities[1]


def zone_lists(headers, bag_column, bags, generators, modulators, ref_operator):
    for head, tail in zip(headers, headers[1:]):
        zones = []
        for index in range(head[bag_column], tail[bag_column]):
            gen_start, mod_start = bags[index]
            gen_end, mod_end = bags[index + 1]
            gens = tuple(generators[gen_start:gen_end])
            values = dict(gens)
            reference = values.get(ref_operator)
            if reference is None:
                if zones or 43 in values or 44 in values:
                    raise ValueError("Unexpected SoundFont global zone layout")
            elif not gens or gens[-1][0] != ref_operator:
                raise ValueError("SoundFont zone reference is not its final generator")
            keys, velocities = values.get(43, 0x7F00), values.get(44, 0x7F00)
            zones.append(Zone(gens, tuple(modulators[mod_start:mod_end]), reference,
                              (keys & 255, keys >> 8), (velocities & 255, velocities >> 8)))
        yield zones


def write_zones(tables, old, headers_key, bag_column, prefix, selections, ref_operator, remap):
    bag_key, gen_key, mod_key = (prefix + suffix for suffix in (b"bag", b"gen", b"mod"))
    headers, bags, generators, modulators = [], [], [], []
    for source_index, zones in [*selections.items(), (-1, ())]:
        header = list(old[headers_key][source_index])
        header[bag_column] = len(bags)
        headers.append(header)
        for zone in zones:
            bags.append((len(generators), len(modulators)))
            generators.extend((operator, remap[value] if operator == ref_operator else value)
                              for operator, value in zone.generators)
            modulators.extend(zone.modulators)
    bags.append((len(generators), len(modulators)))
    generators.append(old[gen_key][-1])
    modulators.append(old[mod_key][-1])
    tables.update({headers_key: headers, bag_key: bags, gen_key: generators, mod_key: modulators})


def address_change(operator, zones):
    coarse = {0: 4, 1: 12, 2: 45, 3: 50}[operator]
    return sum(signed(amount, 16) * (32768 if op == coarse else 1)
               for zone in zones for op, amount in zone.generators if op in (operator, coarse))


def slim_soundfont(data, content):
    """Return a subset SF2 and coverage details using original PCM coordinates."""
    if hashlib.sha256(data).hexdigest() != SOUNDFONT_SHA:
        raise ValueError("TH02 subsetting expects the audited GeneralUser GS 1.44 input")
    if data[:4] != b"RIFF" or data[8:12] != b"sfbk" or struct.unpack_from("<I", data, 4)[0] != len(data) - 8:
        raise ValueError("Invalid SoundFont RIFF header")
    lists = {value[:4]: value[4:] for key, value in chunks(data[12:]) if key == b"LIST"}
    sdta = dict(chunks(lists[b"sdta"]))
    if sdta.keys() != {b"smpl"}:
        raise ValueError("TH02 subsetting requires 16-bit PCM with no auxiliary sample table")
    pcm = sdta[b"smpl"]
    source = dict(chunks(lists[b"pdta"]))
    tables = {tag: list(struct.iter_unpack(fmt, source[tag])) for tag, fmt in RECORDS.items()}
    presets = {(row[2], row[1]): index for index, row in enumerate(tables[b"phdr"][:-1])}
    preset_zones = list(zone_lists(tables[b"phdr"], 3, tables[b"pbag"], tables[b"pgen"], tables[b"pmod"], 41))
    instruments = list(zone_lists(tables[b"inst"], 1, tables[b"ibag"], tables[b"igen"], tables[b"imod"], 53))
    usage = defaultdict(set)
    per_song = {}
    for name in SONGS:
        path = content / (name + ".MMD")
        data = path.read_bytes()
        per_song[path.name] = hashlib.sha256(data).hexdigest()
        for patch, notes in song_usage(data, presets).items():
            usage[patch].update(notes)
    selected_presets = {}
    selected_instruments = defaultdict(set)
    samples = tables[b"shdr"]
    sample_spans = defaultdict(list)
    for patch in sorted(usage, key=presets.__getitem__):
        index = presets[patch]
        notes = usage[patch]
        global_preset = [zone for zone in preset_zones[index] if zone.reference is None]
        active_zones = list(global_preset)
        for preset_zone in preset_zones[index]:
            instrument = preset_zone.reference
            if instrument is None:
                continue
            preset_notes = {(key, vel) for key, vel in notes if preset_zone.admits(key, vel)}
            if notes and not preset_notes:
                continue
            global_instrument = [zone for zone in instruments[instrument] if zone.reference is None]
            active = False
            for zone_index, zone in enumerate(instruments[instrument]):
                sample = zone.reference
                if sample is None or (notes and not any(zone.admits(key, vel) for key, vel in preset_notes)):
                    continue
                active = True
                selected_instruments[instrument].add(zone_index)
                _, start, end, loop_start, loop_end, *_ = samples[sample]
                layers = [*global_preset, preset_zone, *global_instrument, zone]
                effective = [base + address_change(op, layers)
                             for op, base in enumerate((start, end, loop_start, loop_end))]
                # Preserve the full waveform and the engine's interpolation guard samples.
                sample_spans[sample].append((min(start, *effective), max(end + 46, max(effective) + 2)))
            if active:
                active_zones.append(preset_zone)
        if not any(zone.reference is not None for zone in active_zones):
            raise ValueError(f"No playable SoundFont zones selected for {patch}")
        selected_presets[index] = active_zones

    queue = list(sample_spans)
    for index in queue:
        link, kind = samples[index][-2:]
        if kind & 0x7FFF in (2, 4, 8) and link not in sample_spans:
            if not 0 <= link < len(samples) - 1:
                raise ValueError(f"Invalid stereo sample link: {index} -> {link}")
            linked = samples[link]
            sample_spans[link].append((linked[1], linked[2] + 46))
            queue.append(link)
    sample_map = {index: new for new, index in enumerate(sorted(sample_spans))}
    instrument_map = {index: new for new, index in enumerate(sorted(selected_instruments))}
    chosen_instruments = {
        index: [zone for i, zone in enumerate(instruments[index]) if i in selected_instruments[index] or zone.reference is None]
        for index in instrument_map
    }
    result = {}
    write_zones(result, tables, b"phdr", 3, b"p", selected_presets, 41, instrument_map)
    write_zones(result, tables, b"inst", 1, b"i", chosen_instruments, 53, sample_map)
    result[b"shdr"] = []
    # TinySoundFont accumulates the absolute PCM index in double precision. Moving
    # live samples changes interpolator rounding, so zero the gaps and let the
    # existing package compressor remove them without shifting any sample.
    sparse_pcm = bytearray(len(pcm))
    merged = []
    for start, end in sorted((start, end) for spans in sample_spans.values() for start, end in spans):
        if start < 0 or start > len(pcm) // 2:
            raise ValueError("Sample region lies outside the SoundFont PCM table")
        end = min(end, len(pcm) // 2)
        if merged and start <= merged[-1][1]:
            merged[-1][1] = max(end, merged[-1][1])
        else:
            merged.append([start, end])
    for start, end in merged:
        sparse_pcm[2 * start:2 * end] = pcm[2 * start:2 * end]
    for index in sample_map:
        row = list(samples[index])
        if row[-1] & 0x7FFF in (2, 4, 8):
            row[-2] = sample_map[row[-2]]
        result[b"shdr"].append(row)
    result[b"shdr"].append(samples[-1])
    pdta = b"".join(chunk(tag, b"".join(struct.pack(fmt, *row) for row in result[tag]))
                    for tag, fmt in RECORDS.items())
    font = chunk(b"RIFF", b"sfbk" + chunk(b"LIST", b"INFO" + lists[b"INFO"])
                 + chunk(b"LIST", b"sdta" + chunk(b"smpl", sparse_pcm)) + chunk(b"LIST", b"pdta" + pdta))
    return font, {
        "source_sha256": SOUNDFONT_SHA,
        "subset_sha256": hashlib.sha256(font).hexdigest(),
        "songs": per_song,
        "presets": {f"{bank}:{program}": len(usage[bank, program]) for bank, program in sorted(usage)},
        "instruments": len(instrument_map), "samples": len(sample_map),
        "retained_pcm_bytes": 2 * sum(end - start for start, end in merged),
    }


def main():
    import json

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", type=Path)
    parser.add_argument("content", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    font, report = slim_soundfont(args.source.read_bytes(), args.content)
    with args.output.open("xb") as output:
        output.write(font)
    print(json.dumps(report, indent=2))


if __name__ == "__main__":
    main()
