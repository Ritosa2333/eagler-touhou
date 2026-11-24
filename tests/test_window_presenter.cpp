#include "window_presenter.hpp"
#include <iostream>
#include <vector>
unsigned checks=0;void check(bool b){++checks;if(!b)throw std::runtime_error("Atomic presentation contract failed");}
int main(){try{
    HDC screen=GetDC(nullptr);if(!screen)throw std::runtime_error("No screen DC");HDC target=CreateCompatibleDC(screen);HBITMAP bitmap=CreateCompatibleBitmap(screen,800,600);auto original=SelectObject(target,bitmap);
    // GDI lazily initializes one process-cached stock object on first blit;
    // establish the leak baseline AFTER an identical warm-up lifecycle.
    {soew::WindowPresenter warmup;std::vector<std::uint32_t> fill(640*400,0x315779);warmup.prepare(target,800,600,fill,640,400,0,50,800,500);warmup.commit(target);GdiFlush();}
    {HBRUSH warm_brush=CreateSolidBrush(RGB(11,31,47));RECT full{0,0,800,600};FillRect(target,&full,warm_brush);DeleteObject(warm_brush);GdiFlush();}
    const DWORD baseline=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS);
    {
        soew::WindowPresenter presenter;std::vector<std::uint32_t> pixels(640*400);
        for(unsigned i=0;i<240;++i){
            const bool wide=i%2==0;const int x=wide?0:80,y=wide?50:0,w=wide?800:640,h=wide?500:600;
            const COLORREF before=RGB(11,31,47);RECT full{0,0,800,600};HBRUSH brush=CreateSolidBrush(before);FillRect(target,&full,brush);DeleteObject(brush);
            const std::uint32_t rgb=0x315779+(i%16);std::fill(pixels.begin(),pixels.end(),rgb);
            check(presenter.prepare(target,800,600,pixels,640,400,x,y,w,h));GdiFlush();
            // Critical regression: prepare/scaling MUST NOT change target pixels.
            check(GetPixel(target,400,300)==before);check(GetPixel(target,0,0)==before);
            presenter.commit(target);GdiFlush();check(GetPixel(target,400,300)==RGB((rgb>>16)&255,(rgb>>8)&255,rgb&255));check(GetPixel(target,0,0)==RGB(0,0,0));
        }
        check(!presenter.prepare(target,0,0,pixels,640,400,0,0,0,0));
        for(int size:{320,640,512,800}){check(presenter.prepare(target,size,size,pixels,640,400,0,0,size,size));presenter.commit(target);GdiFlush();check(GetPixel(target,size/2,size/2)!=CLR_INVALID);}
    }
    GdiFlush();const auto final_count=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS);std::cout<<"gdi_baseline="<<baseline<<" gdi_final="<<final_count<<'\n';check(final_count==baseline);
    SelectObject(target,original);DeleteObject(bitmap);DeleteDC(target);ReleaseDC(nullptr,screen);
    std::cout<<"presenter_checks="<<checks<<" failures=0 offscreen_prepare_target_unchanged=true\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
