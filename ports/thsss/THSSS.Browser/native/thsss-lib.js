var LibraryThsss = {
  $ThsssState: {
    text: "",
    bytes: null
  },
  $bytes: function (ptr, len) {
    return ptr && len > 0 ? HEAPU8.subarray(ptr, ptr + len) : new Uint8Array(0);
  },

  thsss_set_status: function (ptr) {
    Module["thsss"].setStatus(UTF8ToString(ptr));
  },
  thsss_notify_exit: function () {
    Module["thsss"].notifyExit();
  },
  thsss_page_base_len: function () {
    ThsssState.text = String(Module["thsss"].pageBase() || "");
    return lengthBytesUTF8(ThsssState.text) + 1;
  },
  thsss_read_saves_len: function () {
    ThsssState.text = String(Module["thsss"].readSaves() || "");
    return lengthBytesUTF8(ThsssState.text) + 1;
  },
  thsss_copy_text: function (dest, destLen) {
    stringToUTF8(ThsssState.text, dest, destLen);
    ThsssState.text = "";
  },
  thsss_write_save: function (pathPtr, dataPtr) {
    Module["thsss"].writeSave(UTF8ToString(pathPtr), UTF8ToString(dataPtr));
  },
  thsss_take_archive_len: function () {
    const bytes = globalThis.__thsssArchive;
    ThsssState.bytes = bytes && bytes.byteLength ? (bytes instanceof Uint8Array ? bytes : new Uint8Array(bytes)) : new Uint8Array(0);
    return ThsssState.bytes.length;
  },
  thsss_put_audio_file: function (pathPtr, ptr, len) {
    Module["thsss"].putAudioFile(UTF8ToString(pathPtr), ptr && len > 0 ? HEAPU8.slice(ptr, ptr + len) : new Uint8Array(0));
  },
  thsss_copy_bytes: function (dest, len) {
    const bytes = ThsssState.bytes || new Uint8Array(0);
    HEAPU8.set(bytes.subarray(0, len), dest);
    ThsssState.bytes = new Uint8Array(0);
    globalThis.__thsssArchive = null;
  },
  thsss_clear: function (r, g, b, a, color, depth) {
    Module["thsss"].clear(r, g, b, a, !!color, !!depth);
  },
  thsss_create_texture: function (width, height, ptr, len) {
    return Module["thsss"].createTexture(width, height, bytes(ptr, len)) | 0;
  },
  thsss_begin_image: function (ptr, len) {
    return Module["thsss"].beginImage(bytes(ptr, len)) | 0;
  },
  thsss_image_state: function (id) {
    return Module["thsss"].imageState(id) | 0;
  },
  thsss_finish_image: function (id) {
    return Module["thsss"].finishImage(id) | 0;
  },
  thsss_image_width: function (id) {
    return Module["thsss"].imageWidth(id) | 0;
  },
  thsss_image_height: function (id) {
    return Module["thsss"].imageHeight(id) | 0;
  },
  thsss_create_render_target: function (width, height) {
    return Module["thsss"].createRenderTarget(width, height) | 0;
  },
  thsss_update_texture: function (id, width, height, ptr, len) {
    Module["thsss"].updateTexture(id, width, height, bytes(ptr, len));
  },
  thsss_delete_texture: function (id) {
    Module["thsss"].deleteTexture(id);
  },
  thsss_bind_target: function (id) {
    Module["thsss"].bindTarget(id);
  },
  thsss_set_viewport: function (x, y, width, height, canvasWidth, canvasHeight) {
    Module["thsss"].setViewport(x, y, width, height, canvasWidth, canvasHeight);
  },
  thsss_set_blend: function (enable, src, dst, equation) {
    Module["thsss"].setBlend(!!enable, src, dst, equation);
  },
  thsss_set_depth: function (enable, write) {
    Module["thsss"].setDepth(!!enable, !!write);
  },
  thsss_set_cull: function (mode) {
    Module["thsss"].setCull(mode);
  },
  thsss_draw_sprites: function (textureId, flipV, ptr, len) {
    Module["thsss"].drawSprites(textureId, flipV, bytes(ptr, len));
  },
  thsss_draw_effect: function (namePtr, textureId, flipV, ptr, len, blurLevel, x, y, time, radius, colorKey) {
    Module["thsss"].drawEffect(UTF8ToString(namePtr), textureId, flipV, bytes(ptr, len), blurLevel, x, y, time, radius, colorKey);
  },
  thsss_draw_fixed: function (textureId, ptr, len, worldPtr, worldLen, viewPtr, viewLen, projPtr, projLen, mr, mg, mb, ma, lighting, fog, fogStart, fogEnd, fr, fg, fb) {
    Module["thsss"].drawFixed(textureId, bytes(ptr, len), bytes(worldPtr, worldLen), bytes(viewPtr, viewLen), bytes(projPtr, projLen), mr, mg, mb, ma, lighting, fog, fogStart, fogEnd, fr, fg, fb);
  },
  thsss_raster_text_len: function (textPtr, width, height, sizePx, bold, italic) {
    ThsssState.bytes = Module["thsss"].rasterText(UTF8ToString(textPtr), width, height, sizePx, !!bold, !!italic) || new Uint8Array(0);
    return ThsssState.bytes.length;
  },
  thsss_raster_text_box_len: function (textPtr, width, height, sizePx, bold, italic, argb, familyPtr) {
    ThsssState.bytes = Module["thsss"].rasterTextBox(UTF8ToString(textPtr), width, height, sizePx, !!bold, !!italic, argb, UTF8ToString(familyPtr)) || new Uint8Array(0);
    return ThsssState.bytes.length;
  },
  thsss_paint_text: function (id, textPtr, width, height, sizePx, bold, italic, argb, familyPtr) {
    Module["thsss"].paintText(id, UTF8ToString(textPtr), width, height, sizePx, !!bold, !!italic, argb, UTF8ToString(familyPtr));
  },
  thsss_audio_prepare: function (pathPtr) {
    return Module["thsss"].audioPrepare(UTF8ToString(pathPtr)) | 0;
  },
  thsss_audio_voice: function () {
    return Module["thsss"].audioVoice() | 0;
  },
  thsss_audio_destroy: function (voice) {
    Module["thsss"].audioDestroy(voice);
  },
  thsss_audio_submit: function (voice, buffer, playBegin, playLength, loopCount, loopBegin, loopLength) {
    Module["thsss"].audioSubmit(voice, buffer, playBegin, playLength, loopCount, loopBegin, loopLength);
  },
  thsss_audio_start: function (voice) {
    Module["thsss"].audioStart(voice);
  },
  thsss_audio_stop: function (voice) {
    Module["thsss"].audioStop(voice);
  },
  thsss_audio_volume: function (voice, volume) {
    Module["thsss"].audioVolume(voice, volume);
  },
  thsss_audio_pan: function (voice, left, right) {
    Module["thsss"].audioPan(voice, left, right);
  },
  thsss_audio_filter: function (voice, highpass, frequency) {
    Module["thsss"].audioFilter(voice, !!highpass, frequency);
  },
  thsss_audio_samples: function (voice) {
    return Module["thsss"].audioSamples(voice) | 0;
  },
  thsss_audio_channels: function (buffer) {
    return Module["thsss"].audioChannels(buffer) | 0;
  }
};

autoAddDeps(LibraryThsss, "$ThsssState");
autoAddDeps(LibraryThsss, "$bytes");
mergeInto(LibraryManager.library, LibraryThsss);
