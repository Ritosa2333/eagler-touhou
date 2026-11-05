"""Capture only the debugger-owned game client's visible pixels."""
import ctypes as c
from ctypes import wintypes as w
import struct
import zlib
from pathlib import Path

def encode_png(width,height,pixels):
    # Lossless format encoding of captured bytes; no resampling or pixel edits.
    rows=[]
    for y in range(height):
        source=pixels[y*width*4:(y+1)*width*4]
        rgb=bytearray(width*3);rgb[0::3]=source[2::4];rgb[1::3]=source[1::4];rgb[2::3]=source[0::4]
        rows.append(b'\0'+rgb)
    def chunk(name,body):return struct.pack('>I',len(body))+name+body+struct.pack('>I',zlib.crc32(name+body)&0xffffffff)
    return b'\x89PNG\r\n\x1a\n'+chunk(b'IHDR',struct.pack('>IIBBBBB',width,height,8,2,0,0,0))+chunk(b'IDAT',zlib.compress(b''.join(rows)))+chunk(b'IEND',b'')

def capture_client(pid, destination):
    user=c.WinDLL('user32',use_last_error=True); gdi=c.WinDLL('gdi32',use_last_error=True)
    callback=c.WINFUNCTYPE(w.BOOL,w.HWND,w.LPARAM); windows=[]
    user.GetWindowThreadProcessId.argtypes=[w.HWND,c.POINTER(w.DWORD)]
    user.GetClientRect.argtypes=[w.HWND,c.POINTER(w.RECT)]
    user.GetDC.argtypes=[w.HWND]; user.GetDC.restype=w.HDC
    user.ReleaseDC.argtypes=[w.HWND,w.HDC]
    user.IsWindowVisible.argtypes=[w.HWND]
    user.EnumWindows.argtypes=[callback,w.LPARAM]
    @callback
    def visit(hwnd,_):
        owner=w.DWORD();user.GetWindowThreadProcessId(hwnd,c.byref(owner))
        if owner.value==pid and user.IsWindowVisible(hwnd):windows.append(hwnd)
        return True
    user.EnumWindows(visit,0)
    if not windows:raise RuntimeError('找不到检查用原版窗口。')
    hwnd=windows[0]; rect=w.RECT();user.GetClientRect(hwnd,c.byref(rect))
    width,height=rect.right,rect.bottom
    if not 1<=width<=4096 or not 1<=height<=4096:raise RuntimeError('游戏窗口尺寸无效。')
    for name,args,result in [
        ('CreateCompatibleDC',[w.HDC],w.HDC),
        ('CreateCompatibleBitmap',[w.HDC,c.c_int,c.c_int],w.HBITMAP),
        ('SelectObject',[w.HDC,w.HANDLE],w.HANDLE),
        ('BitBlt',[w.HDC,c.c_int,c.c_int,c.c_int,c.c_int,w.HDC,c.c_int,c.c_int,w.DWORD],w.BOOL),
        ('GetDIBits',[w.HDC,w.HBITMAP,w.UINT,w.UINT,c.c_void_p,c.c_void_p,w.UINT],c.c_int),
        ('DeleteObject',[w.HANDLE],w.BOOL),('DeleteDC',[w.HDC],w.BOOL)]:
        fn=getattr(gdi,name);fn.argtypes=args;fn.restype=result
    source=user.GetDC(hwnd);target=gdi.CreateCompatibleDC(source)
    bitmap=gdi.CreateCompatibleBitmap(source,width,height);old=gdi.SelectObject(target,bitmap)
    try:
        if not gdi.BitBlt(target,0,0,width,height,source,0,0,0x00cc0020):raise c.WinError(c.get_last_error())
        gdi.SelectObject(target,old)
        info=c.create_string_buffer(40)
        struct.pack_into('<IiiHHIIiiII',info,0,40,width,-height,1,32,0,width*height*4,0,0,0,0)
        pixels=c.create_string_buffer(width*height*4)
        if gdi.GetDIBits(target,bitmap,0,height,pixels,info,0)!=height:raise RuntimeError('读取画面失败。')
        temporary=Path(destination).with_suffix('.tmp.png');temporary.write_bytes(encode_png(width,height,pixels.raw));temporary.replace(destination)
    finally:
        gdi.SelectObject(target,old);gdi.DeleteObject(bitmap);gdi.DeleteDC(target);user.ReleaseDC(hwnd,source)
