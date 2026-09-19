#!/usr/bin/env python3
"""Guard TH07's software texture cache against out-of-band GL mutations."""

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ANM = (ROOT / "src" / "AnmManager.cpp").read_text(encoding="utf-8")
TEXT = (ROOT / "src" / "TextHelper.cpp").read_text(encoding="utf-8")


release_start = ANM.index("void AnmManager::ReleaseTexture(i32 textureIdx)")
release_end = ANM.index("void AnmManager::LoadSprite", release_start)
release = ANM[release_start:release_end]

cache_guard = "if (this->currentTexture == this->textures[textureIdx])"
cache_clear = "this->currentTexture = 0;"
delete_main = "g_Supervisor.gfxDevice->DeleteTexture(this->textures[textureIdx]);"
assert cache_guard in release
guard_pos = release.index(cache_guard)
clear_pos = release.index(cache_clear, guard_pos)
delete_pos = release.index(delete_main)
assert guard_pos < clear_pos < delete_pos, (
    "ReleaseTexture must invalidate the cached main-texture binding before deletion"
)

copy_start = TEXT.index("bool TextHelper::CopyTextToTexture(")
copy_end = TEXT.index("ZunResult TextHelper::CreateTextBuffer()", copy_start)
copy = TEXT[copy_start:copy_end]

flush = "g_AnmManager->Flush();"
bind = "g_Supervisor.gfxDevice->BindTexture(outTexture);"
sync = "g_AnmManager->SetTexture(outTexture);"
upload = "g_Supervisor.gfxDevice->SetTextureSubImage("
assert flush in copy
assert bind in copy
assert sync in copy
assert upload in copy
assert "SetCurrentTexture(outTexture)" not in copy, (
    "out-of-band font uploads must not use the cache-short-circuiting bind helper"
)
assert copy.index(flush) < copy.index(bind) < copy.index(sync) < copy.index(upload), (
    "font texture uploads must flush, bind the real target, synchronize the cache, then upload"
)

# Model the concrete regression that the old 5f80a0d-era source contract could
# not see: an out-of-band loader binds the title texture while AnmManager still
# caches the text texture. SetCurrentTexture(text) then short-circuits and the
# following glTexSubImage2D mutates the title texture. The current upload path
# always performs the real bind before synchronizing the cache.
text_texture = 7
title_texture = 32

cached = text_texture
actual = title_texture
if cached != text_texture:  # old SetCurrentTexture(text_texture)
    actual = text_texture
    cached = text_texture
assert actual == title_texture, "fixture must reproduce the old stale-cache misbind"

cached = text_texture
actual = title_texture
actual = text_texture       # unconditional BindTexture(text_texture)
cached = text_texture       # SetTexture(text_texture)
assert actual == text_texture and cached == text_texture

print("TH07 texture binding cache contract: PASS")
