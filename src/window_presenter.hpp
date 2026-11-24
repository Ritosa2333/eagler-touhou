#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <span>
#include <cstdint>
#include <stdexcept>
#include <string_view>
namespace soew {
// Window DC is touched ONLY by commit(). All clears/scaling happen offscreen.
// Reuse the client-sized bitmap; never expose a black intermediate paint.
class WindowPresenter {
    HDC memory_=nullptr;
    HBITMAP bitmap_=nullptr;
    HGDIOBJ original_=nullptr;
    int width_=0,height_=0;
public:
    WindowPresenter()=default;
    WindowPresenter(const WindowPresenter&)=delete;
    WindowPresenter& operator=(const WindowPresenter&)=delete;
    ~WindowPresenter(){reset();}
    void reset(){
        if(memory_&&original_)SelectObject(memory_,original_);
        if(bitmap_)DeleteObject(bitmap_);
        if(memory_)DeleteDC(memory_);
        memory_=nullptr;bitmap_=nullptr;original_=nullptr;width_=height_=0;
    }
    bool prepare(HDC reference,int width,int height,std::span<const std::uint32_t> pixels,int frame_w,int frame_h,int x,int y,int view_w,int view_h){
        if(width<=0||height<=0)return false;
        if(frame_w<=0||frame_h<=0||pixels.size()!=std::size_t(frame_w)*frame_h||x<0||y<0||view_w<0||view_h<0||x+view_w>width||y+view_h>height)throw std::runtime_error("Invalid presentation dimensions");
        if(width!=width_||height!=height_||!memory_){
            HDC next=CreateCompatibleDC(reference);if(!next)throw std::runtime_error("Presentation DC allocation failed");
            HBITMAP next_bitmap=CreateCompatibleBitmap(reference,width,height);
            if(!next_bitmap){DeleteDC(next);throw std::runtime_error("Presentation bitmap allocation failed");}
            HGDIOBJ next_original=SelectObject(next,next_bitmap);
            if(!next_original||next_original==HGDI_ERROR){DeleteObject(next_bitmap);DeleteDC(next);throw std::runtime_error("Presentation bitmap selection failed");}
            reset();memory_=next;bitmap_=next_bitmap;original_=next_original;width_=width;height_=height;
        }
        RECT full{0,0,width,height};if(!FillRect(memory_,&full,static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH))))throw std::runtime_error("Offscreen clear failed");
        BITMAPINFO info{};info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=frame_w;info.bmiHeader.biHeight=-frame_h;info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;
        SetStretchBltMode(memory_,COLORONCOLOR);
        if(view_w&&view_h){const auto result=StretchDIBits(memory_,x,y,view_w,view_h,0,0,frame_w,frame_h,pixels.data(),&info,DIB_RGB_COLORS,SRCCOPY);if(result==static_cast<int>(GDI_ERROR)||result==0)throw std::runtime_error("Offscreen image scaling failed");}
        return true;
    }
    void append(std::span<const std::uint32_t> pixels,int frame_w,int frame_h,int x,int y,int view_w,int view_h){
        if(!memory_||frame_w<=0||frame_h<=0||pixels.size()!=std::size_t(frame_w)*frame_h||x<0||y<0||view_w<0||view_h<0||x+view_w>width_||y+view_h>height_)throw std::runtime_error("Invalid extra panel dimensions");
        BITMAPINFO info{};info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=frame_w;info.bmiHeader.biHeight=-frame_h;info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;
        if(view_w&&view_h){const auto result=StretchDIBits(memory_,x,y,view_w,view_h,0,0,frame_w,frame_h,pixels.data(),&info,DIB_RGB_COLORS,SRCCOPY);if(result==int(GDI_ERROR)||result==0)throw std::runtime_error("Extra panel scaling failed");}
    }
    void corner_label(std::wstring_view label,RECT bounds){
        const auto font=SelectObject(memory_,GetStockObject(DEFAULT_GUI_FONT));
        const auto mode=SetBkMode(memory_,OPAQUE);
        const auto background=SetBkColor(memory_,RGB(0,0,0));
        const auto foreground=SetTextColor(memory_,RGB(255,255,255));
        DrawTextW(memory_,label.data(),static_cast<int>(label.size()),&bounds,DT_SINGLELINE|DT_RIGHT|DT_BOTTOM|DT_NOPREFIX);
        SetTextColor(memory_,foreground);
        SetBkColor(memory_,background);
        SetBkMode(memory_,mode);
        SelectObject(memory_,font);
    }
    void commit(HDC target){
        if(!memory_||!BitBlt(target,0,0,width_,height_,memory_,0,0,SRCCOPY))throw std::runtime_error("Frame presentation failed");
    }
};
}
