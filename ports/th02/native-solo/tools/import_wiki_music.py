# /// script
# requires-python = ">=3.10"
# dependencies = ["opencc-python-reimplemented==0.1.7"]
# ///
"""Import the pinned TH02 Music Room translations into a local content tree."""

import argparse
import json
from pathlib import Path
import re
import textwrap
from urllib.parse import urlencode
from urllib.request import Request, urlopen

from opencc import OpenCC
from import_wiki_story import clean


EN_WIKI, ZH_WIKI = "https://touhou.fandom.com", "https://thwiki.cc"
EN_REV, ZH_REV = 155789, 712594
FILES = ("OP.M", "STAGE0.M", "STAGE1.M", "STAGE2.M", "STAGE3.M", "STAGE4.M",
         "BOSS1.M", "BOSS4.M", "BOSS2.M", "BOSS3.M", "MIMA.M", "END1.M",
         "ENDING.M", "STAGE5.M", "BOSS5.M")
ALBUM_ORDER = (1, 2, 4, 6, 8, 10, 3, 5, 7, 9, 11, 14, 15, 12, 13)
COMMENT_CELLS, COMMENT_ROWS = 41, 38


def wiki(site, **params):
    request = Request(site + "/api.php?" + urlencode({"format": "json", **params}),
                      headers={"User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) PowerShell/7.6.9"})
    with urlopen(request, timeout=25) as response:
        return json.load(response)


def english():
    data = wiki(EN_WIKI, action="parse", oldid=EN_REV, prop="wikitext|revid")["parse"]
    if data["revid"] != EN_REV:
        raise ValueError("Unexpected English Music Room revision")
    sections = re.split(r"^;([^\n]+)\n", data["wikitext"]["*"], flags=re.M)
    songs = []
    for heading, section in zip(sections[1::2], sections[2::2], strict=True):
        if heading == "Unused":
            continue
        titles = re.findall(r"^:([^\n]+)", section, re.M)
        _, text = re.findall(r"<blockquote>(.*?)</blockquote>", section, re.S)
        text = clean(re.sub(r"<br\s*/?>[ \t]*\n", "<br/>", text, flags=re.I))
        text = re.sub(r"'{2,5}", "", text)
        fm, midi = re.split(r"(?m)^MIDI version\s*$", text.removeprefix("FM version"), maxsplit=1)
        songs.append((clean(titles[-1]), fm.strip(), midi.strip()))
    if len(songs) != len(FILES):
        raise ValueError("Unexpected English Music Room track list")
    return [songs[index - 1] for index in ALBUM_ORDER]


def chinese():
    data = wiki(ZH_WIKI, action="query", revids=ZH_REV, prop="revisions",
                rvprop="ids|content", rvslots="main", formatversion=2)
    revision = data["query"]["pages"][0]["revisions"][0]
    text = revision["slots"]["main"]["content"]
    titles = wiki(ZH_WIKI, action="expandtemplates", title="东方封魔录/Music", prop="wikitext",
                  text="§".join("{{封魔录音乐名|1|%d}}" % i for i in range(1, 16)))
    titles = titles["expandtemplates"]["wikitext"].split("§")
    tracks = {}
    for section in re.split(r"^category\s*$", text, flags=re.M)[1:]:
        index = int(re.search(r"^titleZH = .*\|1\|(\d+)}}", section, re.M)[1])
        fm, midi, native = re.split(r"^source = ", section, flags=re.M)[1:4]
        file = native.splitlines()[0]
        if index != ALBUM_ORDER[FILES.index(file)]:
            raise ValueError(f"Unexpected title index for {file}")
        tracks[file] = (titles[index - 1], *(clean(s.split("\nzh\n", 1)[1]) for s in (fm, midi)))
    return [tracks[file] for file in FILES]


def wrap(text, chinese_text):
    if not chinese_text:
        return [line for p in re.split(r"\n\s*\n", text) for line in textwrap.wrap(" ".join(p.split()), COMMENT_CELLS)]
    # The fixed bitmap renderer gives ASCII one cell and CJK two.
    text = "".join(line.strip() for line in text.splitlines())
    rows = []
    while text:
        end, width = 0, 0
        for c in text:
            width += 1 if c.isascii() else 2
            if width > COMMENT_CELLS:
                break
            end += 1
        while end > 1 and end < len(text) and (text[end] in "，。！？：；、）”」…．" or text[end - 1] in "（“「"):
            end -= 1
        rows.append(text[:end])
        text = text[end:]
    return rows


def stage(content, lang, songs, labels):
    rows = []
    for title, fm, midi in songs:
        block = [labels[0], *wrap(fm, lang != "en"), "", labels[1], *wrap(midi, lang != "en")]
        if len(block) > COMMENT_ROWS:
            raise ValueError(f"{lang} {title} needs {len(block)} rows, have {COMMENT_ROWS}")
        rows.extend([title, *block, *([""] * (COMMENT_ROWS - len(block)))])
    (content / f"music-room.{lang}.pack").write_text("\n".join(rows) + "\n", encoding="utf-8", newline="\n")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("content", type=Path, help="TH02 content directory")
    args = parser.parse_args()
    hans = chinese()
    converter = OpenCC("s2t")
    hant = [tuple(converter.convert(row) for row in song) for song in hans]
    stage(args.content, "en", english(), ("FM version", "MIDI version"))
    stage(args.content, "zh-Hans", hans, ("FM版", "MIDI版"))
    stage(args.content, "zh-Hant", hant, ("FM版", "MIDI版"))
    files = args.content / "full-files.json"
    if files.is_file():
        names = set(json.loads(files.read_text(encoding="utf-8")))
        names.update(f"music-room.{lang}.pack" for lang in ("en", "zh-Hans", "zh-Hant"))
        files.write_text(json.dumps(sorted(names), ensure_ascii=False) + "\n", encoding="utf-8", newline="\n")


if __name__ == "__main__":
    main()
