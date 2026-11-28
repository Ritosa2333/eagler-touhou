using System;
using System.Runtime.InteropServices;

namespace Thsss.Browser
{
	public static unsafe partial class ThsssJs
	{
		public static void SetStatus(string text)
		{
			var value = WasmText.Alloc(text);
			try
			{
				thsss_set_status(value);
			}
			finally
			{
				WasmText.Free(value);
			}
		}

		public static void NotifyExit()
		{
			thsss_notify_exit();
		}

		public static string PageBase()
		{
			return WasmText.CopyFromJs(&thsss_copy_text, &thsss_page_base_len);
		}

		public static string ReadSaves()
		{
			return WasmText.CopyFromJs(&thsss_copy_text, &thsss_read_saves_len);
		}

		public static byte[] TakeArchive()
		{
			return WasmText.CopyBytes(&thsss_copy_bytes, thsss_take_archive_len());
		}

		public static void PutAudioFile(string path, byte[] bytes)
		{
			WithUtf8(path, (value) => WithBytes(bytes, (data, length) => thsss_put_audio_file(value, data, length)));
		}

		public static void WriteSave(string path, string data)
		{
			var pathBytes = WasmText.Alloc(path);
			var dataBytes = WasmText.Alloc(data);
			try
			{
				thsss_write_save(pathBytes, dataBytes);
			}
			finally
			{
				WasmText.Free(dataBytes);
				WasmText.Free(pathBytes);
			}
		}

		public static void Clear(float r, float g, float b, float a, bool color, bool depth)
		{
			thsss_clear(r, g, b, a, color ? 1 : 0, depth ? 1 : 0);
		}

		public static int CreateTexture(int width, int height, byte[] pixels)
		{
			var id = 0;
			WithBytes(pixels, (data, length) => id = thsss_create_texture(width, height, data, length));
			return id;
		}

		public static int BeginImage(byte[] bytes)
		{
			var id = 0;
			WithBytes(bytes, (data, length) => id = thsss_begin_image(data, length));
			return id;
		}

		public static int ImageState(int id)
		{
			return thsss_image_state(id);
		}

		public static int FinishImage(int id)
		{
			return thsss_finish_image(id);
		}

		public static int ImageWidth(int id)
		{
			return thsss_image_width(id);
		}

		public static int ImageHeight(int id)
		{
			return thsss_image_height(id);
		}

		public static int CreateRenderTarget(int width, int height)
		{
			return thsss_create_render_target(width, height);
		}

		public static void UpdateTexture(int id, int width, int height, byte[] pixels)
		{
			WithBytes(pixels, (data, length) => thsss_update_texture(id, width, height, data, length));
		}

		public static void DeleteTexture(int id)
		{
			thsss_delete_texture(id);
		}

		public static void BindTarget(int id)
		{
			thsss_bind_target(id);
		}

		public static void SetViewport(int x, int y, int width, int height, int canvasWidth, int canvasHeight)
		{
			thsss_set_viewport(x, y, width, height, canvasWidth, canvasHeight);
		}

		public static void SetBlend(bool enable, int src, int dst, int equation)
		{
			thsss_set_blend(enable ? 1 : 0, src, dst, equation);
		}

		public static void SetDepth(bool enable, bool write)
		{
			thsss_set_depth(enable ? 1 : 0, write ? 1 : 0);
		}

		public static void SetCull(int mode)
		{
			thsss_set_cull(mode);
		}

		public static void DrawSprites(int textureId, float flipV, byte[] vertices)
		{
			WithBytes(vertices, (data, length) => thsss_draw_sprites(textureId, flipV, data, length));
		}

		public static void DrawEffect(string name, int textureId, float flipV, byte[] vertices, float blurLevel, float x, float y, float time, float radius, int colorKey)
		{
			WithUtf8(name, (value) => WithBytes(vertices, (data, length) => thsss_draw_effect(value, textureId, flipV, data, length, blurLevel, x, y, time, radius, colorKey)));
		}

		public static void DrawFixed(int textureId, byte[] vertices, byte[] world, byte[] view, byte[] proj, float mr, float mg, float mb, float ma, float lighting, float fog, float fogStart, float fogEnd, float fr, float fg, float fb)
		{
			WithBytes(vertices, (data, length) => WithBytes(world, (worldData, worldLength) => WithBytes(view, (viewData, viewLength) => WithBytes(proj, (projData, projLength) => thsss_draw_fixed(textureId, data, length, worldData, worldLength, viewData, viewLength, projData, projLength, mr, mg, mb, ma, lighting, fog, fogStart, fogEnd, fr, fg, fb)))));
		}

		public static byte[] RasterText(string text, int width, int height, float sizePx, bool bold, bool italic)
		{
			var pixels = Array.Empty<byte>();
			WithUtf8(text, (value) => pixels = WasmText.CopyBytes(&thsss_copy_bytes, thsss_raster_text_len(value, width, height, sizePx, bold ? 1 : 0, italic ? 1 : 0)));
			return pixels;
		}

		public static byte[] RasterTextBox(string text, int width, int height, float sizePx, bool bold, bool italic, int argb, string family)
		{
			var pixels = Array.Empty<byte>();
			WithUtf8(text, (value) => WithUtf8(family, (face) => pixels = WasmText.CopyBytes(&thsss_copy_bytes, thsss_raster_text_box_len(value, width, height, sizePx, bold ? 1 : 0, italic ? 1 : 0, argb, face))));
			return pixels;
		}

		public static void PaintText(int id, string text, int width, int height, float sizePx, bool bold, bool italic, int argb, string family)
		{
			WithUtf8(text, (value) => WithUtf8(family, (face) => thsss_paint_text(id, value, width, height, sizePx, bold ? 1 : 0, italic ? 1 : 0, argb, face)));
		}

		public static int AudioPrepare(string path)
		{
			var id = 0;
			WithUtf8(path, (value) => id = thsss_audio_prepare(value));
			return id;
		}

		public static int AudioVoice()
		{
			return thsss_audio_voice();
		}

		public static void AudioDestroy(int voice)
		{
			thsss_audio_destroy(voice);
		}

		public static void AudioSubmit(int voice, int buffer, int playBegin, int playLength, int loopCount, int loopBegin, int loopLength)
		{
			thsss_audio_submit(voice, buffer, playBegin, playLength, loopCount, loopBegin, loopLength);
		}

		public static void AudioStart(int voice)
		{
			thsss_audio_start(voice);
		}

		public static void AudioStop(int voice)
		{
			thsss_audio_stop(voice);
		}

		public static void AudioVolume(int voice, float volume)
		{
			thsss_audio_volume(voice, volume);
		}

		public static void AudioPan(int voice, float left, float right)
		{
			thsss_audio_pan(voice, left, right);
		}

		public static void AudioFilter(int voice, bool highpass, float frequency)
		{
			thsss_audio_filter(voice, highpass ? 1 : 0, frequency);
		}

		public static int AudioSamples(int voice)
		{
			return thsss_audio_samples(voice);
		}

		public static int AudioChannels(int buffer)
		{
			return thsss_audio_channels(buffer);
		}

		private unsafe delegate void Utf8Action(byte* text);

		private unsafe delegate void BytesAction(byte* data, int length);

		private static void WithUtf8(string text, Utf8Action action)
		{
			var value = WasmText.Alloc(text);
			try
			{
				action(value);
			}
			finally
			{
				WasmText.Free(value);
			}
		}

		private static void WithBytes(byte[] bytes, BytesAction action)
		{
			if (bytes == null || bytes.Length == 0)
			{
				action(null, 0);
				return;
			}
			fixed (byte* data = bytes)
			{
				action(data, bytes.Length);
			}
		}

		[DllImport("env", EntryPoint = "thsss_set_status"), WasmImportLinkage]
		private static extern void thsss_set_status(byte* text);

		[DllImport("env", EntryPoint = "thsss_notify_exit"), WasmImportLinkage]
		private static extern void thsss_notify_exit();

		[DllImport("env", EntryPoint = "thsss_page_base_len"), WasmImportLinkage]
		private static extern int thsss_page_base_len();

		[DllImport("env", EntryPoint = "thsss_read_saves_len"), WasmImportLinkage]
		private static extern int thsss_read_saves_len();

		[DllImport("env", EntryPoint = "thsss_copy_text"), WasmImportLinkage]
		private static extern void thsss_copy_text(byte* dest, int destLen);

		[DllImport("env", EntryPoint = "thsss_write_save"), WasmImportLinkage]
		private static extern void thsss_write_save(byte* path, byte* data);

		[DllImport("env", EntryPoint = "thsss_take_archive_len"), WasmImportLinkage]
		private static extern int thsss_take_archive_len();

		[DllImport("env", EntryPoint = "thsss_put_audio_file"), WasmImportLinkage]
		private static extern void thsss_put_audio_file(byte* path, byte* data, int length);

		[DllImport("env", EntryPoint = "thsss_copy_bytes"), WasmImportLinkage]
		private static extern void thsss_copy_bytes(byte* dest, int length);

		[DllImport("env", EntryPoint = "thsss_clear"), WasmImportLinkage]
		private static extern void thsss_clear(float r, float g, float b, float a, int color, int depth);

		[DllImport("env", EntryPoint = "thsss_create_texture"), WasmImportLinkage]
		private static extern int thsss_create_texture(int width, int height, byte* pixels, int length);

		[DllImport("env", EntryPoint = "thsss_begin_image"), WasmImportLinkage]
		private static extern int thsss_begin_image(byte* data, int length);

		[DllImport("env", EntryPoint = "thsss_image_state"), WasmImportLinkage]
		private static extern int thsss_image_state(int id);

		[DllImport("env", EntryPoint = "thsss_finish_image"), WasmImportLinkage]
		private static extern int thsss_finish_image(int id);

		[DllImport("env", EntryPoint = "thsss_image_width"), WasmImportLinkage]
		private static extern int thsss_image_width(int id);

		[DllImport("env", EntryPoint = "thsss_image_height"), WasmImportLinkage]
		private static extern int thsss_image_height(int id);

		[DllImport("env", EntryPoint = "thsss_create_render_target"), WasmImportLinkage]
		private static extern int thsss_create_render_target(int width, int height);

		[DllImport("env", EntryPoint = "thsss_update_texture"), WasmImportLinkage]
		private static extern void thsss_update_texture(int id, int width, int height, byte* pixels, int length);

		[DllImport("env", EntryPoint = "thsss_delete_texture"), WasmImportLinkage]
		private static extern void thsss_delete_texture(int id);

		[DllImport("env", EntryPoint = "thsss_bind_target"), WasmImportLinkage]
		private static extern void thsss_bind_target(int id);

		[DllImport("env", EntryPoint = "thsss_set_viewport"), WasmImportLinkage]
		private static extern void thsss_set_viewport(int x, int y, int width, int height, int canvasWidth, int canvasHeight);

		[DllImport("env", EntryPoint = "thsss_set_blend"), WasmImportLinkage]
		private static extern void thsss_set_blend(int enable, int src, int dst, int equation);

		[DllImport("env", EntryPoint = "thsss_set_depth"), WasmImportLinkage]
		private static extern void thsss_set_depth(int enable, int write);

		[DllImport("env", EntryPoint = "thsss_set_cull"), WasmImportLinkage]
		private static extern void thsss_set_cull(int mode);

		[DllImport("env", EntryPoint = "thsss_draw_sprites"), WasmImportLinkage]
		private static extern void thsss_draw_sprites(int textureId, float flipV, byte* vertices, int length);

		[DllImport("env", EntryPoint = "thsss_draw_effect"), WasmImportLinkage]
		private static extern void thsss_draw_effect(byte* name, int textureId, float flipV, byte* vertices, int length, float blurLevel, float x, float y, float time, float radius, int colorKey);

		[DllImport("env", EntryPoint = "thsss_draw_fixed"), WasmImportLinkage]
		private static extern void thsss_draw_fixed(int textureId, byte* vertices, int length, byte* world, int worldLength, byte* view, int viewLength, byte* proj, int projLength, float mr, float mg, float mb, float ma, float lighting, float fog, float fogStart, float fogEnd, float fr, float fg, float fb);

		[DllImport("env", EntryPoint = "thsss_raster_text_len"), WasmImportLinkage]
		private static extern int thsss_raster_text_len(byte* text, int width, int height, float sizePx, int bold, int italic);

		[DllImport("env", EntryPoint = "thsss_raster_text_box_len"), WasmImportLinkage]
		private static extern int thsss_raster_text_box_len(byte* text, int width, int height, float sizePx, int bold, int italic, int argb, byte* family);

		[DllImport("env", EntryPoint = "thsss_paint_text"), WasmImportLinkage]
		private static extern void thsss_paint_text(int id, byte* text, int width, int height, float sizePx, int bold, int italic, int argb, byte* family);

		[DllImport("env", EntryPoint = "thsss_audio_prepare"), WasmImportLinkage]
		private static extern int thsss_audio_prepare(byte* path);

		[DllImport("env", EntryPoint = "thsss_audio_voice"), WasmImportLinkage]
		private static extern int thsss_audio_voice();

		[DllImport("env", EntryPoint = "thsss_audio_destroy"), WasmImportLinkage]
		private static extern void thsss_audio_destroy(int voice);

		[DllImport("env", EntryPoint = "thsss_audio_submit"), WasmImportLinkage]
		private static extern void thsss_audio_submit(int voice, int buffer, int playBegin, int playLength, int loopCount, int loopBegin, int loopLength);

		[DllImport("env", EntryPoint = "thsss_audio_start"), WasmImportLinkage]
		private static extern void thsss_audio_start(int voice);

		[DllImport("env", EntryPoint = "thsss_audio_stop"), WasmImportLinkage]
		private static extern void thsss_audio_stop(int voice);

		[DllImport("env", EntryPoint = "thsss_audio_volume"), WasmImportLinkage]
		private static extern void thsss_audio_volume(int voice, float volume);

		[DllImport("env", EntryPoint = "thsss_audio_pan"), WasmImportLinkage]
		private static extern void thsss_audio_pan(int voice, float left, float right);

		[DllImport("env", EntryPoint = "thsss_audio_filter"), WasmImportLinkage]
		private static extern void thsss_audio_filter(int voice, int highpass, float frequency);

		[DllImport("env", EntryPoint = "thsss_audio_samples"), WasmImportLinkage]
		private static extern int thsss_audio_samples(int voice);

		[DllImport("env", EntryPoint = "thsss_audio_channels"), WasmImportLinkage]
		private static extern int thsss_audio_channels(int buffer);
	}
}
