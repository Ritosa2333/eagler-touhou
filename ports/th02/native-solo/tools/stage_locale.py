"""Stage only the verified SoEW language assets from the local extraction evidence."""
import argparse
import hashlib
import json
import struct
from pathlib import Path

from hdi_baseline import inspect as inspect_hdi
from pf_extract import decode

STAGES = {f"STAGE{i}.TXT": boxes for i, boxes in enumerate((24, 35, 11, 52, 57, 31))}
ENDINGS = {f"END{i}.TXT": lines for i, lines in enumerate((51, 94, 12), 1)}
LANGUAGE_ASSETS = (*STAGES, *ENDINGS, "MUSIC.TXT")
CHINESE_PACK = "chinese-v1.txt"
CHINESE_MAGIC = "SOEW-CHINESE-V1"
CHINESE_LOCALES = ("zh-Hans", "zh-Hant")
CHINESE_COUNTS = {**{name: count * 2 for name, count in STAGES.items()}, **ENDINGS, "MUSIC.TXT": 300}
CHINESE_STORY_COUNTS = {name: count for name, count in CHINESE_COUNTS.items() if name not in ("END3.TXT", "MUSIC.TXT")}
CHINESE_STORY_MAGIC = "SOEW-CHINESE-STORY-V1"
CHINESE_ROWS = sum(CHINESE_COUNTS.values()) * len(CHINESE_LOCALES)
CHINESE_MAX_SIZE = 256 * 1024
CHINESE_REVIEW_FORMAT = "soew-th02-reviewed-chinese"
CONTINUE_TOKEN = "{continues}"
SOURCES = {
    "ja": ("patched/partition_0/東方封魔.録", "patched-assets", "dfad9522b1b63fd977d20cf01bf49c58a0d0674ae92274a557f5057a2ddd20ed"),
    "en": ("original/partition_0/東方封魔.録", "original-assets", "40dc303c54377eca034e1dbb62de90947be209bc19abbc891d8d28f8b84b033d"),
    "patch": ("patched/partition_0/T2EN.DAT", None, "142727e5da29c97174297c82687d3768b653f1967118777f9b26e5884850513e"),
}
SHARED = {"OP2.PI": "5b265d3d814ca3e4ad69864ffd0604a1951d78fe71faa5254907e096dc84287a",
          "OP.RGB": "940636cf77c1789e9bfafbd8d43589fa5f824345dd8c63014ddd107306f1c795"}
TH02_MEMBER = Path("partition_0/HUMAROKU/東方封魔.録")
CN_HDI_SHA256 = "1e45044b37bce861ca090d9bf5190aa002316204fb32e2de409429492305fb94"
CN_PF_SHA256 = "a3ab334843689012f18973b1373d39bed0131563385e59c4d244672d5d17425d"
CN_FONT_SHA256 = "93ea316452052391f56d062547b67b3df979d41d8a1a8d4e76b0ba419e70f82d"
JP_FONT_SHA256 = "41535bd7242c07d69ec5427584d31f068ca4eb996ca95246caed4e3573a494e6"
# Source glyph transcription, not copyediting or Chinese regional normalization.
# Local review compared the actual 16×16 glyphs and surrounding records.
# e89a and e9fc intentionally retain the shapes 焔 and 挡 as drawn.
CN_GLYPHS = dict(zip(
    "e89a e89e e98b e994 e995 e996 e998 e99a e99c e99d e99e e9f5 e9f7 e9f8 e9f9 e9fa "
    "e9fc ea65 ea67 ea68 ea6a ea6b ea6c ea6e ea6f ea71 ea76 ea77 ea78 ea7b ea82 ea85 "
    "ea89 ea8a ea8b ea8c ea8d ea8e ea90 ea91 ea92 ea93 ea95 ea96 ea97 ea98 ea99 ea9a "
    "ea9b ea9c ea9d ea9e".split(),
    "焔聽糕奶帕唔吵扯燙拽趴唉咱煩傻夠挡咧徵靚懂瞧阱喂閒您狀捏偷幫喲跑趕搞值哼份辦伙傢她嗯哪哎啦啊吧妳你呢嗎呃",
    strict=True,
))


def digest(data):
    return hashlib.sha256(data).hexdigest()


def need(condition, message):
    if not condition:
        raise ValueError(message)


def inspect_archive(source, language):
    path, extraction, expected = SOURCES[language]
    raw = (source / path).read_bytes()
    need(digest(raw) == expected, f"Unexpected archive SHA-256: {path}")
    folder = source / extraction if extraction else None
    index = {row["name"]: row for row in json.loads((folder / "pf-index.json").read_text(encoding="utf-8"))} if folder else {}
    names = (*LANGUAGE_ASSETS, *SHARED) if folder else (*LANGUAGE_ASSETS, "OP2.PI")
    return read_pf(raw, path, expected, names, source=source, extraction=extraction, index=index)


def read_pf(raw, path, expected, names, *, source=None, extraction=None, index=None):
    folder = source / extraction if extraction else None
    results = {}
    first_payload = len(raw)
    for at in range(0, 255 * 32, 32):
        if at >= first_payload:
            break
        kind, _, obfuscated, packed_size, recorded_size, offset, _ = struct.unpack_from("<HB13sIII4s", raw, at)
        if not kind:
            break
        need(kind in (0x9595, 0xf388) and offset >= at + 32 and offset + packed_size <= len(raw),
             f"{path}: invalid PF entry at {at}")
        first_payload = min(first_payload, offset)
        name = bytes(byte ^ 255 for byte in obfuscated.split(b"\0", 1)[0]).decode("cp932")
        if name not in names:
            continue
        packed = bytes(byte ^ 0x12 for byte in raw[offset:offset + packed_size])
        need(kind in (0x9595, 0xf388), f"{name}: unknown PF entry kind")
        data = decode(packed, kind == 0x9595)
        sha = digest(data)
        if folder:
            row = index[name]
            need((row["sha256"], row["offset"], row["packed_size"], row["size"]) ==
                 (sha, offset, packed_size, recorded_size), f"{extraction}/{name}: extraction index differs")
            need(data == (folder / name).read_bytes(), f"{extraction}/{name}: actual bytes differ")
        need(name not in results, f"{path}/{name}: duplicate PF entry")
        results[name] = (data, {"asset": name, "archive": path, "archive_sha256": expected,
                               "extraction": extraction, "offset": offset, "packed_size": packed_size,
                               "recorded_size": recorded_size, "size": len(data), "sha256": sha})
    need(len(results) == len(names), f"{path}: missing allowlisted resource")
    return results


def check_text(name, data, language):
    if name in STAGES:
        size = STAGES[name] * 80
        trailer = b" " if language == "en" and name != "STAGE5.TXT" else b""
        count, width, stride = STAGES[name] * 2, 36, 40
    elif name in ENDINGS:
        count, width, stride = ENDINGS[name], 44, 46
        size, trailer = count * stride, b""
    else:
        count, width, stride = 300, 40, 42
        size, trailer = count * stride, b"  " if language == "en" else b""
    need(len(data) == size + len(trailer) and data[size:] == trailer, f"{language}/{name}: record size or trailer")
    for index in range(count):
        row = data[index * stride:(index + 1) * stride]
        row[:width].decode("cp932", errors="strict")
        if name not in STAGES:
            need(row[width:] == b"\r\n", f"{language}/{name}: record {index} terminator")
    if name == "STAGE3.TXT":
        line = data[42 * 80:42 * 80 + 36]
        need(line[8:12] == b"\x81\x40\x81\x40", f"{language}/{name}: Continue digit positions differ")
        expected = b" times" if language == "en" else "回も".encode("cp932")
        need(line[12:].startswith(expected), f"{language}/{name}: Continue counter context differs")
    return {"display_records": count, "record_bytes": stride, "text_bytes": width, "trailer_bytes": len(trailer)}


def checked_bytes(path, expected):
    data = path.read_bytes()
    need(digest(data) == expected, f"Unexpected SHA-256: {path}")
    return data


def chinese_source(game):
    cn = checked_bytes(game / "cn.hdi", CN_HDI_SHA256)
    raw = inspect_hdi(cn, select={TH02_MEMBER})[TH02_MEMBER]
    need(digest(raw) == CN_PF_SHA256, "Unexpected Chinese TH02 PF SHA-256")
    ja = inspect_hdi(game / "jp.hdi", select={TH02_MEMBER})[TH02_MEMBER]
    need(digest(ja) == SOURCES["ja"][2], "Unexpected Japanese TH02 PF SHA-256")
    return (read_pf(raw, str(TH02_MEMBER), CN_PF_SHA256, LANGUAGE_ASSETS),
            read_pf(ja, str(TH02_MEMBER), SOURCES["ja"][2], LANGUAGE_ASSETS),
            checked_bytes(game / "font_cn.bmp", CN_FONT_SHA256),
            checked_bytes(game / "font_jp.bmp", JP_FONT_SHA256))


def chinese_records(name, data, japanese):
    baseline = check_text(name, japanese, "ja")
    count, width, stride = (baseline[key] for key in ("display_records", "text_bytes", "record_bytes"))
    # Neither source anomaly changes a visible record or its index.
    if name == "STAGE5.TXT":
        need(len(data) == count * stride - 1 and data[-4:] == b"\x40\x81\x40\r",
             "STAGE5.TXT: unexpected final record suffix")
        data += b"\n"
    if name == "END1.TXT":
        need(len(data) == count * stride + 1 and data.endswith(b"\r\n "),
             "END1.TXT: unexpected trailing byte")
        data = data[:-1]
    need(len(data) == count * stride, f"{name}: Chinese record count differs")
    for at in range(count):
        row = data[at * stride:(at + 1) * stride]
        need(row[-2:] == b"\r\n", f"{name}: record {at} terminator differs")
        if name in STAGES:
            # The translation sometimes uses TH02's two ignored padding bytes.
            # Keep them in the audit for review, never change the simulated width.
            width_with_padding = stride - 2
        else:
            width_with_padding = width
        yield row[:width_with_padding]


def sjis_font_cell(pair):
    # master.lib TEXT_PUTSA writes (JIS row-32) in the low byte and
    # the JIS column in the high byte; the emulator bitmap uses that high byte
    # for a 16-scanline band and the low byte for the glyph within the band.
    first, second = pair
    x = (first - (0x81 if first < 0xA0 else 0xC1)) * 2 + 1
    if second >= 0x9F:
        return x + 1, second - 0x7E
    return x, second - (0x1F if second < 0x7F else 0x20)


def bitmap_glyph(font, token):
    off = struct.unpack_from("<I", font, 10)[0]
    if len(token) == 1:
        x, y, width = token[0], 0, 1
    else:
        x, y = sjis_font_cell(token)
        x *= 2
        width = 2
    need(0 <= x <= 256 - width and 0 <= y < 128, f"No ANEX86 font cell for {token.hex()}")
    return bytes(byte ^ 255 for row in range(16)
                 for pos in [off + (2047 - y * 16 - row) * 256 + x]
                 for byte in font[pos:pos + width])


def record_cells(row):
    at = 0
    while at < len(row):
        first = row[at]
        if 0x81 <= first <= 0x9F or 0xE0 <= first <= 0xFC:
            need(at + 1 < len(row) and 0x40 <= row[at + 1] <= 0xFC and row[at + 1] != 0x7F,
                 f"Invalid two-byte display cell at byte {at}")
            token = row[at:at + 2]
        else:
            need(0x20 <= first <= 0x7E or 0xA1 <= first <= 0xDF,
                 f"Invalid single-byte display cell at byte {at}")
            token = row[at:at + 1]
        yield at, token
        at += len(token)


def audit_chinese(game):
    variant, japanese, cn_font, jp_font = chinese_source(game)
    replacements = {}
    records = {}
    for name in LANGUAGE_ASSETS:
        data, metadata = variant[name]
        rows = list(chinese_records(name, data, japanese[name][0]))
        if name == "STAGE3.TXT":
            need(rows[84][8:12] == b"\x81\x40\x81\x40" and rows[84][12:16] == b"\x89\xf1\x97\xb9",
                 "STAGE3.TXT: Continue counter template positions differ")
        if name == "END3.TXT":
            for index, row in enumerate(rows):
                need(row.startswith(b"  \xea\x79\x81\x44\x81\x44\x81\x44"),
                     f"END3.TXT: rating template differs at index {index}")
        for index, row in enumerate(rows):
            for at, cell in record_cells(row):
                cn_glyph = bitmap_glyph(cn_font, cell)
                jp_glyph = bitmap_glyph(jp_font, cell)
                try:
                    conventional = cell.decode("cp932", errors="strict")
                except UnicodeDecodeError:
                    conventional = None
                if cn_glyph != jp_glyph or conventional is None:
                    entry = replacements.setdefault(cell.hex(), {
                        "cp932": conventional, "glyph_sha256": digest(cn_glyph),
                        "changed_bitmap": cn_glyph != jp_glyph, "occurrences": 0,
                        "first_record": {"asset": name, "record": index, "byte": at},
                    })
                    entry["occurrences"] += 1
        records[name] = {"sha256": metadata["sha256"], "size": metadata["size"],
                         "pf_recorded_size": metadata["recorded_size"], "display_records": len(rows)}
        if name in STAGES:
            records[name]["nonblank_padding_records"] = [index for index, row in enumerate(rows)
                                                        if row[36:] not in (b"  ", b"\x81\x40")]
    return {"version": 1, "publishable": False, "hdi_sha256": CN_HDI_SHA256, "pf_sha256": CN_PF_SHA256,
            "cn_font_sha256": CN_FONT_SHA256, "jp_font_sha256": JP_FONT_SHA256,
            "credits": {"programming": "258921", "translation": ["asdfghj135", "絶対無視"],
                        "data_entry": "洛城東華"}, "assets": records, "replacement_glyphs": replacements,
            "untranscribed_glyphs": sorted(replacements.keys() - CN_GLYPHS.keys()),
            "transcribed_glyphs": {code: CN_GLYPHS[code] for code in sorted(replacements.keys() & CN_GLYPHS.keys())}}


def review_chinese(game, code):
    # Local review only: retain the original bitmap and show every occurrence in
    # context, marking all substituted cells instead of guessing their readings.
    audited = audit_chinese(game)["replacement_glyphs"]
    need(code in audited, f"Not an audited substitution: {code}")
    variant, japanese, font, _ = chinese_source(game)
    print(f"{code} ({audited[code]['occurrences']} occurrences)")
    pixels = bitmap_glyph(font, bytes.fromhex(code))
    for row in range(16):
        bits = int.from_bytes(pixels[row * 2:row * 2 + 2], "big")
        print("".join("##" if bits & (0x8000 >> col) else "  " for col in range(16)))
    for name in LANGUAGE_ASSETS:
        rows = list(chinese_records(name, variant[name][0], japanese[name][0]))
        for i, row in enumerate(rows):
            if any(token.hex() == code for _, token in record_cells(row)):
                print(f"{name}:{i}:")
                for at in range(max(0, i - 1), min(len(rows), i + 2)):
                    text = "".join(f"[{token.hex()}]" if token.hex() in audited else token.decode("cp932")
                                   for _, token in record_cells(rows[at]))
                    print(f"  {at}: {text.strip()}")


def transcribe_chinese_record(name, index, row):
    parts = []
    for at, cell in record_cells(row):
        if name == "STAGE3.TXT" and index == 84:
            if at == 8:
                parts.append("{continues}")
                continue
            if at == 10:
                continue
        code = cell.hex()
        parts.append(CN_GLYPHS[code] if code in CN_GLYPHS else cell.decode("cp932", errors="strict"))
    return "".join(parts).rstrip(" \u3000")


def chinese_draft(game):
    audit = audit_chinese(game)
    need(set(audit["replacement_glyphs"]) == CN_GLYPHS.keys(), "Chinese glyph transcription coverage differs")
    variant, japanese, _, _ = chinese_source(game)
    return {"format": "soew-th02-glyph-transcription-draft", "version": 1, "publishable": False,
            "provenance": audit,
            "records": {name: [transcribe_chinese_record(name, index, row)
                               for index, row in enumerate(chinese_records(name, variant[name][0], japanese[name][0]))]
                        for name in LANGUAGE_ASSETS}}


def strict_json(data):
    def unique_keys(pairs):
        obj = {}
        for key, value in pairs:
            need(key not in obj, f"Duplicate JSON key: {key}")
            obj[key] = value
        return obj

    def invalid_constant(value):
        raise ValueError(f"Invalid JSON number: {value}")

    return json.loads(data.decode("utf-8"), object_pairs_hook=unique_keys, parse_constant=invalid_constant)


def bounded_file(path, cap):
    with path.open("rb") as stream:
        data = stream.read(cap + 1)
    need(0 < len(data) <= cap, f"File exceeds format bounds: {path}")
    return data


def chinese_character(cp):
    return (0x20 <= cp <= 0x7E or cp in (0x00B7, 0x2014, 0x2018, 0x2019, 0x201C, 0x201D,
                                          0x2022, 0x2026, 0x2211, 0x2212, 0x2605, 0x2606, 0x30FB) or
            0x3000 <= cp <= 0x303F or 0x3041 <= cp <= 0x3096 or 0x30A1 <= cp <= 0x30FA or
            0x3400 <= cp <= 0x4DBF or 0x4E00 <= cp <= 0x9FFF or 0xFF01 <= cp <= 0xFF9F or
            0xFFE0 <= cp <= 0xFFE6)


def check_chinese_line(lang, asset, index, text):
    label = f"{lang}/{asset}:{index}"
    need(isinstance(text, str), f"{label}: expected display text")
    expected = int(asset == "STAGE3.TXT" and index == 84)
    need(text.count(CONTINUE_TOKEN) == expected, f"{label}: wrong Continue placeholder count")
    visible = text.replace(CONTINUE_TOKEN, "12")
    need("{" not in visible and "}" not in visible, f"{label}: unknown placeholder")
    need(all(chinese_character(ord(char)) for char in visible), f"{label}: unsupported display character")
    columns = sum(1 if ord(char) <= 0x7E or 0xFF61 <= ord(char) <= 0xFF9F else 2 for char in visible)
    cap = 36 if asset in STAGES else 44 if asset in ENDINGS else 40
    need(columns <= cap, f"{label}: display text exceeds {cap} columns ({columns})")


def parse_chinese_pack(data, *, story_only=False):
    need(0 < len(data) <= CHINESE_MAX_SIZE, "Chinese pack size is outside bounds")
    need(data.endswith(b"\n") and b"\r" not in data, "Chinese pack requires canonical LF endings")
    lines = iter(data[:-1].decode("utf-8", errors="strict").split("\n"))
    magic, counts = (CHINESE_STORY_MAGIC, CHINESE_STORY_COUNTS) if story_only else (CHINESE_MAGIC, CHINESE_COUNTS)
    need(next(lines, None) == magic, "Unsupported Chinese pack header")
    variants = {}
    for lang in CHINESE_LOCALES:
        need(next(lines, None) == f"locale\t{lang}", f"Missing or out-of-order Chinese locale: {lang}")
        records = variants[lang] = {}
        for asset, count in counts.items():
            need(next(lines, None) == f"asset\t{asset}\t{count}", f"{lang}: wrong asset header or count for {asset}")
            rows = records[asset] = []
            for index in range(count):
                head, sep, text = next(lines, "").partition("\t")
                need(head == str(index) and sep, f"{lang}/{asset}: wrong record index {index}")
                check_chinese_line(lang, asset, index, text)
                rows.append(text)
    need(next(lines, None) is None, "Unexpected trailing Chinese pack records")
    return variants


def build_chinese_pack(variants, *, story_only=False):
    need(isinstance(variants, dict) and variants.keys() == set(CHINESE_LOCALES), "Expected exactly zh-Hans and zh-Hant")
    magic, counts = (CHINESE_STORY_MAGIC, CHINESE_STORY_COUNTS) if story_only else (CHINESE_MAGIC, CHINESE_COUNTS)
    lines = [magic]
    for lang in CHINESE_LOCALES:
        records = variants[lang]
        need(isinstance(records, dict) and records.keys() == counts.keys(), f"{lang}: wrong asset names")
        lines.append(f"locale\t{lang}")
        for asset, count in counts.items():
            rows = records[asset]
            need(isinstance(rows, list) and len(rows) == count, f"{lang}/{asset}: wrong record count")
            lines.append(f"asset\t{asset}\t{count}")
            for index, text in enumerate(rows):
                check_chinese_line(lang, asset, index, text)
                lines.append(f"{index}\t{text}")
    data = ("\n".join(lines) + "\n").encode("utf-8")
    parse_chinese_pack(data, story_only=story_only)
    return data


def chinese_pack_metadata(data):
    return {"format": CHINESE_MAGIC, "size": len(data), "sha256": digest(data), "display_records": CHINESE_ROWS}


def validate_chinese_overlay(manifest_data, pack):
    manifest = strict_json(manifest_data)
    need(isinstance(manifest, dict) and type(manifest.get("version")) is int and manifest["version"] == 1 and
         isinstance(manifest.get("assets"), dict), "Invalid locale manifest")
    assets = manifest["assets"]
    need(not any(name.casefold() == CHINESE_PACK and name != CHINESE_PACK for name in assets),
         "Unexpected Chinese pack entry spelling")
    if pack is None:
        need(CHINESE_PACK not in assets, "Chinese pack is declared but missing")
        return
    entry = assets.get(CHINESE_PACK)
    need(isinstance(entry, dict) and type(entry.get("size")) is int and type(entry.get("display_records")) is int and
         entry == chinese_pack_metadata(pack), "Chinese pack manifest entry or SHA-256 mismatch")
    parse_chinese_pack(pack)


def chinese_draft_hash(draft):
    # Canonical source identity, independent of how the private draft was exported.
    return digest(json.dumps(draft, ensure_ascii=True, sort_keys=True, separators=(",", ":")).encode("ascii"))


def convert_chinese_review(game, review_path):
    draft = chinese_draft(game)
    review = strict_json(bounded_file(review_path, 1024 * 1024))
    need(isinstance(review, dict) and review.keys() == {"format", "version", "draft_sha256", "variants"} and
         review["format"] == CHINESE_REVIEW_FORMAT and type(review["version"]) is int and review["version"] == 1,
         "Expected an independently reviewed Chinese source, not a glyph transcription draft")
    need(review["draft_sha256"] == chinese_draft_hash(draft), "Reviewed source does not match the verified glyph draft")
    pack = build_chinese_pack(review["variants"])
    for lang, records in review["variants"].items():
        for asset, rows in records.items():
            for index, (original, text) in enumerate(zip(draft["records"][asset], rows, strict=True)):
                need(bool(original.strip(" \u3000")) == bool(text.strip(" \u3000")),
                     f"{lang}/{asset}:{index}: blank structure changed")
        for name in ("258921", "asdfghj135", "絶対無視", "洛城東華"):
            source_slots = [(asset, index) for asset, rows in draft["records"].items()
                            for index, original in enumerate(rows) if name in original]
            need(all(name in records[asset][index] for asset, index in source_slots), f"{lang}: patch credit changed: {name}")
    need(review["variants"]["zh-Hans"] != review["variants"]["zh-Hant"],
         "Reviewed Chinese locales must be distinct adaptations")
    return pack


def stage(source, output, pack=None):
    variants = {lang: inspect_archive(source, lang) for lang in SOURCES}
    chosen = {}
    for lang in ("ja", "en"):
        for name in LANGUAGE_ASSETS:
            data, metadata = variants[lang][name]
            if lang == "en":
                patch, patch_meta = variants["patch"][name]
                need(data == patch, f"{name}: English extraction does not match T2EN.DAT")
                metadata = {**metadata, "matching_patch_entry": patch_meta}
            chosen[f"{lang}/{name}"] = (data, {**metadata, **check_text(name, data, lang)})
    need(variants["en"]["OP2.PI"][0] == variants["patch"]["OP2.PI"][0], "Patched title file identity differs")
    for name, expected in SHARED.items():
        data, metadata = variants["ja"][name]
        need(digest(data) == expected, f"{name}: unexpected shared asset")
        if name == "OP.RGB":
            need(data == variants["en"][name][0], "OP.RGB palettes differ")
        chosen[f"common/{name}"] = (data, metadata)
    if pack is not None:
        chosen[CHINESE_PACK] = (pack, chinese_pack_metadata(pack))
    else:
        need(not (output / CHINESE_PACK).exists(), "Existing Chinese pack requires explicit reviewed input")
    manifest = {"version": 1, "assets": {path: meta for path, (_, meta) in chosen.items()}}
    manifest_data = json.dumps(manifest, indent=2, ensure_ascii=False).encode("utf-8") + b"\n"
    validate_chinese_overlay(manifest_data, pack)
    chosen["manifest.json"] = (manifest_data, {})
    # Check the complete destination before writing any files.
    for rel, (data, _) in chosen.items():
        path = output / rel
        if path.exists():
            need(path.is_file() and path.read_bytes() == data, f"Refusing to overwrite different staged file: {path}")
    for rel, (data, _) in chosen.items():
        path = output / rel
        if not path.exists():
            path.parent.mkdir(parents=True, exist_ok=True)
            with path.open("xb") as stream:
                stream.write(data)
    print(f"Verified {len(manifest['assets'])} allowlisted files in {output}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", type=Path, nargs="?", help="Local original/patched extraction evidence root")
    parser.add_argument("output", type=Path, nargs="?", help="Candidate content's locale directory (not the installed content)")
    parser.add_argument("--audit-chinese", type=Path, metavar="GAME", help="Audit a local collection's game directory without writing assets")
    parser.add_argument("--chinese-game", type=Path, metavar="GAME", help="Verified HDI and font source for compiling private reviewed Chinese text")
    parser.add_argument("--chinese-review", type=Path, metavar="REVIEW", help="Private reviewed Chinese JSON; requires --chinese-game and staging arguments")
    parser.add_argument("--validate-chinese", type=Path, metavar="LOCALE", help="Validate an existing locale directory's optional Chinese pack")
    review = parser.add_mutually_exclusive_group()
    review.add_argument("--review-glyph", metavar="HEX", help="Show the audited cell's bitmap and every source context locally")
    review.add_argument("--draft-stdout", action="store_true", help="Emit a private Unicode transcription for review, never a runtime pack")
    args = parser.parse_args()
    if bool(args.chinese_game) != bool(args.chinese_review):
        parser.error("--chinese-game and --chinese-review must be used together")
    if args.validate_chinese is not None:
        if any((args.audit_chinese, args.chinese_game, args.review_glyph, args.draft_stdout, args.source, args.output)):
            parser.error("Chinese validation cannot be combined with auditing or staging")
        path = args.validate_chinese / CHINESE_PACK
        pack = bounded_file(path, CHINESE_MAX_SIZE) if path.exists() else None
        validate_chinese_overlay(bounded_file(args.validate_chinese / "manifest.json", 256 * 1024), pack)
        print(f"Chinese pack verified: {digest(pack)}" if pack is not None else "No optional Chinese pack declared")
    elif args.audit_chinese is not None:
        if any((args.source, args.output, args.chinese_game)):
            parser.error("Chinese audit is read-only and takes no staging arguments")
        if args.review_glyph is not None:
            review_chinese(args.audit_chinese, args.review_glyph.lower())
        else:
            data = chinese_draft(args.audit_chinese) if args.draft_stdout else audit_chinese(args.audit_chinese)
            print(json.dumps(data, indent=2, ensure_ascii=True))
    else:
        if args.review_glyph is not None or args.draft_stdout:
            parser.error("Glyph review and draft transcription require --audit-chinese")
        if args.source is None or args.output is None:
            parser.error("Staging requires SOURCE and OUTPUT")
        pack = convert_chinese_review(args.chinese_game, args.chinese_review) if args.chinese_game else None
        stage(args.source, args.output, pack)
