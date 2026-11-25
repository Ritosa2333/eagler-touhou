# /// script
# requires-python = ">=3.10"
# dependencies = ["opencc-python-reimplemented==0.1.7"]
# ///
"""Build a local Chinese story pack from THBWiki, without copying game assets."""

import argparse
from functools import lru_cache
from html import unescape
import json
from pathlib import Path
import re
import unicodedata
from urllib.parse import urlencode
from urllib.request import urlopen

from stage_locale import CHINESE_STORY_COUNTS, CONTINUE_TOKEN, build_chinese_pack, need


# THBWiki contributors, CC BY-NC-SA 3.0; see README for the exact source pages.
REVISIONS = (626666, 626673)
PACK = "chinese-story-v1.pack"
SPEAKERS = {"靈夢": "灵梦", "亀": "玄爷", "？？": "？？", "明羅": "明罗",
            "魔梨沙": "魔梨沙", "魅魔": "魅魔", "里香": "里香"}
# Six longer Wiki lines need copy-fitting for a single original 15-character row.
SHORT_LINES = {
    (1, 13): "我要的是你的力量！！笨蛋！！",
    (1, 19): "你到底有没有在听我说话？！",
    (4, 16): "连龟爷爷也把我当傻瓜吗！！",
    (4, 23): "魅魔！让你见识我的力量！",
    (4, 35): "原来你想要的是这份力量・・",
    (5, 16): "2面要是这样，就太难了啊！！",
}


def fetch(revision):
    query = urlencode(dict(action="query", revids=revision, prop="revisions",
                           rvslots="main", rvprop="ids|content", format="json", formatversion=2))
    with urlopen("https://thwiki.cc/api.php?" + query, timeout=20) as response:
        data = json.load(response)
    item, = data["query"]["pages"][0]["revisions"]
    need(item["revid"] == revision, f"Unexpected THBWiki revision: {revision}")
    return item["slots"]["main"]["content"]


def clean(text):
    text = re.sub(r"<ref\b[^>]*?(?:/\s*>|>[\s\S]*?</ref>)", "", text, flags=re.I)
    text = re.sub(r"\[\[(?:[^|\]]+\|)?([^\]]+)\]\]", r"\1", text)
    return unescape(re.sub(r"<br\s*/?>", "\n", text, flags=re.I)).strip()


def sections(text):
    split = re.split(r"^==\s*([^=]+?)\s*==[ \t]*$", text, flags=re.M)
    return dict(zip(split[1::2], split[2::2], strict=True))


def pairs(text):
    pattern = r"^ja\n(.+?)\nzh\n(.*?)(?=\n(?:ja|char|bgm|status|remark|narrator|xx)\n|\Z)"
    return [(clean(j), clean(z)) for j, z in re.findall(pattern, text, flags=re.M | re.S)
            if not j.startswith("BGM:")]


def key(text):
    text = unicodedata.normalize("NFKC", text)
    text = re.sub(r"[\s、。!‘’]+", "", text)
    return text.replace("靈", "霊").replace("Σ", "∑").replace("修業", "修行")


def raw_rows(content, asset):
    count = CHINESE_STORY_COUNTS[asset]
    stage = asset.startswith("STAGE")
    stride, width = (40, 36) if stage else (46, 44)
    raw = (content / asset).read_bytes()
    size = count * stride
    need(len(raw) == size or (stage and len(raw) == size + 1 and raw[-1:] == b" "),
         f"{asset}: unexpected record count")
    return [raw[i * stride:i * stride + width].decode("cp932").strip() for i in range(count)]


def columns(text):
    return sum(1 if ord(c) < 128 or "\uff61" <= c <= "\uff9f" else 2
               for c in text.replace(CONTINUE_TOKEN, "12"))


def fit_rows(text, caps, where):
    """Keep the Wiki line breaks where they fit; otherwise reflow without truncating."""
    rows = text.splitlines()
    if len(rows) == len(caps) and all(row and columns(row) <= cap for row, cap in zip(rows, caps)):
        return rows
    # Each unit is indivisible; a runtime placeholder must never cross rows.
    units = re.findall(r"\{continues\}|[A-Za-z0-9]+|.", "".join(rows))

    @lru_cache(None)
    def solve(start, line):
        if line == len(caps):
            return (0, ()) if start == len(units) else None
        candidates = []
        for end in range(start + 1, len(units) + 1):
            row = "".join(units[start:end]).strip()
            w = columns(row)
            if w > caps[line]:
                break
            if not row or (end < len(units) and units[end] == " "):
                continue
            tail = solve(end, line + 1)
            if tail is not None:
                starts_with_punct = row[0] in "，。！？、；：）》」』…"
                held_open = row[-1] in "（《「『"
                cost = (sum(caps[line:]) * w - caps[line] * columns("".join(units[start:]))) ** 2
                candidates.append((tail[0] + cost + 40000 * (starts_with_punct + held_open),
                                   (row, *tail[1])))
        return min(candidates, default=None)

    result = solve(0, 0)
    need(result is not None, f"{where}: text cannot fit in {len(caps)} original rows")
    return list(result[1])


def dialogue(section, content, stage):
    asset = f"STAGE{stage}.TXT"
    original = raw_rows(content, asset)
    paragraphs = []
    for jp, zh in pairs(section)[1:]:  # skip the stage title
        paragraphs.extend(zip(re.split(r"\n\s*\n", jp), re.split(r"\n\s*\n", zh), strict=True))
    # Stage 3 also lists the demo. Stage 4 lists branches separately, and
    # Stage 5 repeats their common intro: the game's physical records share it.
    order = {
        2: range(11),
        3: [*range(39), *range(48, 56), *range(39, 44)],
        4: [*range(26), *range(36, 54), *range(27, 32), *range(54, 62)],
    }.get(stage, range(len(paragraphs)))
    need(len(order) * 2 == len(original), f"{asset}: THBWiki dialogue count differs")
    out = []
    for box, index in enumerate(order):
        first, second = original[box * 2:box * 2 + 2]
        speaker, body = first.split("：", 1)
        jp, zh = paragraphs[index]
        where = f"{asset}, box {box + 1}"
        if (stage, box) == (3, 42):
            jp = jp.removeprefix("ｘｘ")
            need(zh.count("ｘｘ") == 1, f"{where}: missing Continue counter")
            zh = zh.replace("ｘｘ", CONTINUE_TOKEN)
        need(key(body + second) == key(jp), f"{where}: Wiki Japanese does not match game record")
        zh = SHORT_LINES.get((stage, box), zh)
        prefix = SPEAKERS[speaker.strip()] + "："
        rows = fit_rows(zh, [36 - columns(prefix), 36] if second else [36 - columns(prefix)], where)
        out.extend((prefix + rows[0], rows[1] if second else ""))
    return out


def ending_variant(section, original, kind, shot):
    result = []
    for jp, zh in pairs(section):
        jrows = jp.splitlines()
        if jrows[0].startswith(("－ＢＡＤ", "－ＧＯＯＤ")):
            jrows = [" ".join(jrows)]
            zh = zh.replace("\n", " ")
        if kind == "Good" and shot > 1 and result and len(result) == 27:
            # The first sentence is shared with Good End 1 on disk, but
            # THBWiki starts each branch's new paragraph by repeating it.
            need(jrows[0].startswith("それはじゃ"), f"{kind} End {shot}: missing shared line")
            jrows = jrows[1:]
            zh = zh.split("\n", 1)[1]
        start = len(result)
        want = key("".join(jrows))
        # The Wiki occasionally wraps a Japanese sentence differently.
        end = next((end for end in range(start + 1, len(original) + 1)
                    if key("".join(original[start:end])) == want), None)
        where = f"{kind} End {shot}, record {start + 1}"
        need(end is not None, f"{where}: Wiki Japanese does not match game record")
        result.extend(fit_rows(zh, [44] * (end - start), where))
    need(len(result) == len(original), f"{kind} End {shot}: incomplete ending")
    return result


def ending(transcripts, content, kind, asset, offsets):
    original = raw_rows(content, asset)
    common = 27
    result = []
    for shot, (start, end) in enumerate(zip(offsets, (*offsets[1:], len(original)), strict=True), 1):
        section = transcripts[f"{kind} End No. {shot:02}"]
        branch = original[start:end]
        localized = ending_variant(section, original[:common] + branch if shot > 1 else branch, kind, shot)
        if shot > 1:
            need(localized[:common] == result[:common], f"{kind} End {shot}: branches disagree before the split")
            localized = localized[common:]
        result.extend(localized)
    return result


def story_pack(content, main, extra, to_simplified, to_traditional):
    transcripts = sections(main) | sections(extra)
    hans = {f"STAGE{i}.TXT": dialogue(transcripts[name], content, i)
            for i, name in enumerate([f"Stage {i}" for i in range(1, 6)] + ["Extra"])}
    hans["END1.TXT"] = ending(transcripts, content, "Bad", "END1.TXT", (0, 34, 42))
    hans["END2.TXT"] = ending(transcripts, content, "Good", "END2.TXT", (0, 48, 71))
    return build_chinese_pack({
        language: {asset: [convert(row) for row in rows] for asset, rows in hans.items()}
        for language, convert in (("zh-Hans", to_simplified), ("zh-Hant", to_traditional))
    }, story_only=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("content", type=Path, help="Game content directory with Japanese story files")
    args = parser.parse_args()
    from opencc import OpenCC

    source = args.content / "locale" / "ja" if (args.content / "locale").is_dir() else args.content
    pack = story_pack(source, *(fetch(rev) for rev in REVISIONS), OpenCC("t2s").convert, OpenCC("s2t").convert)
    target = args.content / PACK
    if target.exists():
        need(target.read_bytes() == pack, f"Refusing to overwrite a different pack: {target}")
    else:
        with target.open("xb") as stream:
            stream.write(pack)
    print(f"Chinese story pack ready: {target}")


if __name__ == "__main__":
    try:
        main()
    except (ValueError, OSError) as error:
        raise SystemExit(str(error)) from error
