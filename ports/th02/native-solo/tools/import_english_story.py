"""Align the English Touhou Wiki stage dialogue with TH02's original boxes."""

import argparse
from difflib import SequenceMatcher
import json
from pathlib import Path
import re
import unicodedata
from functools import lru_cache
from urllib.parse import quote
from urllib.request import Request, urlopen

from import_wiki_story import clean, need, raw_rows, sections


WIKI = "https://touhou.fandom.com"
REVISIONS = {"Regular Stages": 191034, "Extra Stage": 190875}
SPEAKERS = {"霊夢": "Reimu", "亀": "Genjii", "？？": "??", "明羅": "Meira",
            "魔梨沙": "Marisa", "魅魔": "Mima", "里香": "Rika"}


def fetch(topic):
    revision = REVISIONS[topic]
    page = quote("Story of Eastern Wonderland: " + topic)
    url = f"{WIKI}/api.php?action=parse&page={page}&prop=wikitext%7Crevid&format=json"
    request = Request(url, headers={"User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) PowerShell/7.6.9"})
    with urlopen(request, timeout=20) as response:
        data = json.load(response)["parse"]
        if data["revid"] != revision:
            raise ValueError(f"Unexpected English wiki revision for {topic}: {data['revid']}")
        return data["wikitext"]["*"]


def plain(text):
    text = re.sub(r"\s*<(/?center|/?i|/?b)\s*>", "", text, flags=re.I)
    text = re.sub(r"(?m)^\s*:", "", text)
    text = text.replace("'''", "").replace("''", "")
    return clean(text)


def key(text):
    text = unicodedata.normalize("NFKC", text)
    text = text.replace("靈", "霊").replace("Σ", "∑").replace("修業", "修行")
    text = text.replace("魔理沙", "魔梨沙").replace("ヶ", "ケ").replace("ヶ月", "ケ月")
    return "".join(c.lower() for c in text if c.isalnum() or c in "∑！？!?")


def rows(wikitext):
    for part in re.split(r"(?m)^\|-(?!})[^\n]*\n", wikitext):
        cols = re.split(r"(?m)^\|[ \t]*(?:\n|width[^\n]*?\|)", part.strip())
        if len(cols) != 3 or not re.match(r"^!\s*\n", cols[0]):
            continue
        jp, en = (plain(col.split("\n|}", 1)[0]) for col in cols[1:])
        if jp and en:
            yield jp, en


def ascii_text(text):
    translated = unicodedata.normalize("NFKC", text).translate(str.maketrans({
        "’": "'", "‘": "'", "“": '"', "”": '"', "—": "-", "–": "-", "·": ".", "〜": "~", "・": ".",
    }))
    if not translated.isascii():
        raise ValueError(f"Non-ASCII translation: {translated!r}")
    return " ".join(translated.split())


def classify_dialogue(content, stage, wiki):
    original = raw_rows(content, f"STAGE{stage}.TXT")
    boxes = []
    for first, second in zip(original[::2], original[1::2], strict=True):
        speaker, body = first.split("：", 1)
        boxes.append((SPEAKERS[speaker.strip().replace("靈", "霊")], key(body + second)))
    entries = list(rows(wiki))
    if stage == 2:
        entries = entries[:8] + entries[-2:]  # Drop the trial-version epilogue.
    groups = []
    for j, (jp, en) in enumerate(entries):
        want = key(jp)
        candidates = []
        for i in range(len(boxes)):
            for n in range(1, 6):
                group = boxes[i:i+n]
                if len(group) != n or len({s for s, _ in group}) != 1:
                    break
                raw = "".join(body for _, body in group)
                candidates.append((SequenceMatcher(None, want, raw).ratio(), i, n))
        score, start, count = max(candidates, default=(0, 0, 0), key=lambda c: (c[0], -abs(c[1] - j)))
        if (stage, j) == (3, 38):
            start = 50  # Identical Genjii line on both post-boss routes.
        groups.append((score, start, count, jp, en))
    return boxes, groups


# The repeat at Stage 5 box 43 is shared by the transcript's branch headings.
# Copy fit only where the English transcript exceeds the original 2×36 columns.
COMPACT = {
    (0, 12): "Now, face my greatest creation: the Flower Tank! So just be a good girl and blow up, thank you.",
    (1, 20): "Sure, sure. Fight, fight.",
    (1, 33): "Excuse me? Were you even paying attention?",
    (2, 1): "This is the Dreamworld, between the demon world and our own. It only looks like space, so you can breathe here.",
    (2, 4): "This is the quickest route to Reimaden.",
    (3, 39): "I'm worried you're not yet a match for Mima.",
    (3, 47): "Mima wields immense power... You must go on alone, Reimu, painful as it is. But please don't forget your old Gramps...",
    (4, 21): "Lady Reimu, retreat, train, and come back...",
    (4, 30): "Only a member of the Hakurei bloodline can use the Yin-Yang Orbs. They absorb their user's power. When full, they release it all at once: the power of both positive and negative. Then the cycle begins again, restoring power through absorption...",
}


def fit_box(text, speaker, count):
    # Keep words and the Continue counter indivisible, and give a whole line
    # to the last box where possible. Original buffers always reveal 72 bytes.
    words = text.split()
    limits = [36-len(speaker)-2, 36] * count
    if count == 1 and len(text.replace("{continues}", "12")) <= limits[0]:
        return [(speaker + ": " + text, "")]

    @lru_cache(None)
    def solve(start, line):
        if line == len(limits):
            return (0, ()) if start == len(words) else None
        candidates = []
        for end in range(start + (line % 2 == 0), len(words) + 1):
            chunk = " ".join(words[start:end])
            width = len(chunk.replace("{continues}", "12"))
            if width > limits[line]:
                break
            tail = solve(end, line + 1)
            if tail is not None:
                remaining = len(" ".join(words[start:]).replace("{continues}", "12"))
                average = min(limits[line], remaining / (len(limits)-line))
                cost = (width-average)**2
                candidates.append((tail[0]+cost, (chunk, *tail[1])))
        return min(candidates, default=None)

    result = solve(0, 0)
    if result is None:
        return None
    fitted = result[1]
    return [(speaker + ": " + first, second) for first, second in zip(fitted[::2], fitted[1::2], strict=True)]


def dialogue(content, stage, wiki):
    boxes, groups = classify_dialogue(content, stage, wiki)
    translated = [None] * len(boxes)
    for j, (score, start, count, jp, en) in enumerate(groups):
        need(score >= 0.94, f"Stage {stage} Wiki row {j}: Source mismatch {jp!r}")
        if (stage, j) == (4, 35):
            continue  # Duplicate of the line already written for box 43.
        text = COMPACT.get((stage, start), ascii_text(en))
        if stage == 3 and start <= 42 < start+count:
            need(start == 41 and count == 2 and "xx times already" in text.lower(),
                 f"Continue placeholder changed: {text}")
            fitted = [("Marisa: Kyahaha, he's right!", ""),
                      ("Marisa: You've continued", "{continues} times already!!")]
        else:
            fitted = fit_box(text, boxes[start][0], count)
        need(fitted is not None, f"Stage {stage} box {start}, count {count}: {text!r} does not fit")
        need(all(not row for row in translated[start:start+count]), f"Stage {stage} box {start}: duplicate Wiki row")
        translated[start:start+count] = fitted
    need(all(translated), f"Stage {stage}: missing boxes {[i for i, row in enumerate(translated) if not row]}")
    return translated


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("content", type=Path)
    args = parser.parse_args()
    regular = sections(fetch("Regular Stages"))
    extra = sections(fetch("Extra Stage"))["Extra Stage"]
    for stage, wiki in enumerate([regular[f"Stage {i}"] for i in range(1, 5)] + [regular["Final Stage"], extra]):
        translated = dialogue(args.content, stage, wiki)
        print(f"inline constexpr std::array<DialogueBox, {len(translated)}> stage{stage}{{{{")
        for first, second in translated:
            print(f"    {{{json.dumps(first)}, {json.dumps(second)}}},")
        print("}};")


if __name__ == "__main__":
    main()
