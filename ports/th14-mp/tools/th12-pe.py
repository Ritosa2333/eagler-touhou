"""TH12 PE image helper.

WHY THIS FILE EXISTS
--------------------
Three separate rounds produced wrong conclusions purely from address
arithmetic, and every one of them was a *different* flavour of the same
mistake:

  * r87  - a single VA/offset constant derived from ``.text`` was applied to
           ``.rdata``, making every ``.rdata`` address 0x400 too low.  Reverse
           lookups "found nothing" (a false negative).
  * r88  - the same wrong mapping read a string region as a pointer table
           (a false positive).
  * r146 - a scanner sliced the raw bytes with ``data[0x400:0x97000]`` and
           then mapped ``match.start()`` (a *slice-relative* index) as if it
           were already a file offset, putting every reported hit 0x400 too
           low (a false positive).

The lesson is not "be careful": it is that the mapping has to live in one
place, and that place has to refuse to answer when it cannot prove the
answer.  So this module

  1. parses the real section table instead of assuming a constant, and
  2. carries a self-test (``selftest``) that checks known anchors with a
     positive control, so a broken mapping fails loudly instead of producing
     confident nonsense.

USAGE
-----
    import sys
    sys.path.insert(0, r"H:\\code\\thp\\tools")
    from importlib import import_module
    Image = import_module("th12-pe").Image

    img = Image(r"H:\\Games\\th\\12\\th12.exe")
    img.selftest()                  # raises on a broken mapping
    img.offset_of(0x4692fa)         # VA -> file offset
    img.scan_dword(0x4154c0)        # [VA, ...] of every little-endian dword
    img.scan_code(b"\\xff\\x50\\x04")  # VAs in .text; offsets handled inside

``scan_code`` returns virtual addresses and never leaks slice offsets, which
removes the entire class of mistake that produced r146.  Callers that need the
following bytes read them from ``read(va, n)`` at a VA.
"""

from __future__ import annotations

import struct

DEFAULT_EXE = r"H:\Games\th\12\th12.exe"


class Image:
    """A mapped PE image with proven VA <-> file offset conversion."""

    def __init__(self, path: str = DEFAULT_EXE):
        with open(path, "rb") as handle:
            self.data = handle.read()
        self.path = path

        pe = struct.unpack_from("<I", self.data, 0x3C)[0]
        if self.data[pe:pe + 4] != b"PE\0\0":
            raise ValueError("not a PE image")
        self.image_base = struct.unpack_from("<I", self.data, pe + 0x34)[0]
        count = struct.unpack_from("<H", self.data, pe + 6)[0]
        opt_size = struct.unpack_from("<H", self.data, pe + 0x14)[0]
        table = pe + 0x18 + opt_size

        # (name, virtual_base, virtual_size, raw_offset, raw_size)
        self.sections: list[tuple[str, int, int, int, int]] = []
        for index in range(count):
            at = table + index * 40
            name = self.data[at:at + 8].rstrip(b"\0").decode("latin1")
            virt_size, virt_rva, raw_size, raw_off = struct.unpack_from(
                "<IIII", self.data, at + 8
            )
            self.sections.append(
                (
                    name,
                    self.image_base + virt_rva,
                    max(virt_size, raw_size),
                    raw_off,
                    raw_size,
                )
            )

    # -- mapping ---------------------------------------------------------

    def section_of(self, va: int):
        for section in self.sections:
            _, base, size, _, _ = section
            if base <= va < base + size:
                return section
        return None

    def offset_of(self, va: int) -> int:
        """File offset for a virtual address.  Raises when unmapped.

        The bound is the *raw* size, not the virtual size: a section can be
        larger in memory than in the file (`.data` here is 0x29460 virtual
        against 0x6a00 raw).  Using the virtual size would let a VA in that
        zero-initialised tail resolve to a file offset inside the *next*
        section's raw data, which is precisely the silent-wrong-answer class
        this module exists to prevent.  The selftest dropped into that hole on
        its first run.
        """
        section = self.section_of(va)
        if section is None:
            raise ValueError(f"VA 0x{va:08x} is not in any section")
        _, base, _, raw_off, raw_size = section
        if va >= base + raw_size:
            raise ValueError(
                f"VA 0x{va:08x} is in a virtual tail with no file backing"
            )
        offset = raw_off + (va - base)
        if offset >= len(self.data):
            raise ValueError(f"VA 0x{va:08x} maps past end of file")
        return offset

    def va_of(self, offset: int) -> int:
        """Virtual address for a file offset.  Raises when unmapped."""
        for _, base, _, raw_off, raw_size in self.sections:
            if raw_off <= offset < raw_off + raw_size:
                return base + (offset - raw_off)
        raise ValueError(f"file offset 0x{offset:x} is not in any section")

    def section_name_of(self, va: int) -> str:
        section = self.section_of(va)
        return section[0] if section else ""

    def is_code(self, va: int) -> bool:
        return self.section_name_of(va) == ".text"

    # -- reads -----------------------------------------------------------

    def read(self, va: int, size: int) -> bytes:
        offset = self.offset_of(va)
        return self.data[offset:offset + size]

    def u32(self, va: int) -> int:
        return struct.unpack_from("<I", self.data, self.offset_of(va))[0]

    def cstring(self, va: int, limit: int = 96) -> str:
        offset = self.offset_of(va)
        end = self.data.index(b"\0", offset, offset + limit)
        return self.data[offset:end].decode("latin1", "replace")

    # -- scans -----------------------------------------------------------

    def scan_dword(self, value: int) -> list[int]:
        """Every VA holding *value* as a little-endian dword."""
        pattern = struct.pack("<I", value & 0xFFFFFFFF)
        hits, at = [], self.data.find(pattern)
        while at != -1:
            try:
                hits.append(self.va_of(at))
            except ValueError:
                pass
            at = self.data.find(pattern, at + 1)
        return hits

    def scan_code(self, pattern: bytes) -> list[int]:
        """Every VA in .text where *pattern* matches.

        The slice offset is handled here and never escapes: callers get VAs.
        """
        name, base, _, raw_off, raw_size = self.sections[0]
        assert name == ".text", "first section is expected to be .text"
        window = self.data[raw_off:raw_off + raw_size]
        hits, at = [], window.find(pattern)
        while at != -1:
            # `at` is window-relative; the file offset is raw_off + at, and the
            # virtual address is base + at.  Applying va_of(raw_off + at) is
            # the same thing but goes through the proven path.
            hits.append(self.va_of(raw_off + at))
            at = window.find(pattern, at + 1)
        return hits

    def scan_ascii(self, needle: bytes) -> list[int]:
        hits, at = [], self.data.find(needle)
        while at != -1:
            try:
                hits.append(self.va_of(at))
            except ValueError:
                pass
            at = self.data.find(needle, at + 1)
        return hits

    # -- proof -----------------------------------------------------------

    def selftest(self) -> None:
        """Positive-control the mapping against anchors proven elsewhere.

        Each anchor was established by an independent route: a magic comparison
        in disassembled code, or an ASCII match whose address was then confirmed
        by disassembling the referring site.  If any anchor moves, the mapping
        is wrong and every scan built on it is void.
        """
        # .text: the SCPT magic is compared at 0x4692f8; scan_code must place
        # the same bytes at the same VA.  (r146 got this wrong by 0x400.)
        text_magic = self.scan_code(struct.pack("<I", 0x54504353))
        assert text_magic == [0x4692FA], f"SCPT magic anchor moved: {text_magic}"

        # .rdata: the enemy source line string, found by ASCII search and then
        # used as a reference anchor.  (r87 got this wrong by 0x400.)
        enemy_line = self.scan_ascii(b"enemy.cpp:202")
        assert enemy_line == [0x49FB8F], f"enemy.cpp:202 anchor moved: {enemy_line}"

        not_loaded = self.scan_ascii(b"Ecl not load")
        assert not_loaded == [0x4A1778], f"Ecl not load anchor moved: {not_loaded}"

        # Same bytes, two directions, every section.  Probe only the region
        # that actually exists in the file: a section's virtual size can exceed
        # its raw size (`.data` here is 0x29460 virtual against 0x6a00 raw, the
        # usual zero-initialised tail), and probing that tail is expected to
        # raise rather than round-trip.
        for name, base, size, raw_off, raw_size in self.sections:
            last = base + raw_size - 1 if raw_size else base
            for probe in (base, base + 1, last):
                if self.offset_of(probe) - raw_off != probe - base:
                    raise AssertionError(f"round trip broken in {name}")
            if self.va_of(raw_off) != base:
                raise AssertionError(f"raw base mismatch in {name}")
            if size > raw_size:
                # The virtual tail must be classified but must not claim a file
                # offset; a silent mapping there is how r87/r88 happened.
                tail = base + raw_size
                if self.section_of(tail) is None:
                    raise AssertionError(f"virtual tail of {name} unclassified")
                try:
                    self.offset_of(tail)
                except ValueError:
                    pass
                else:
                    raise AssertionError(f"virtual tail of {name} faked an offset")


if __name__ == "__main__":
    image = Image()
    for name, base, size, raw_off, raw_size in image.sections:
        print(
            f"{name:8} VA=0x{base:08x} vsize=0x{size:06x} "
            f"raw=0x{raw_off:06x} rsize=0x{raw_size:06x}"
        )
    image.selftest()
    print("th12-pe: selftest OK (SCPT, enemy.cpp:202, Ecl not load, round trips)")
