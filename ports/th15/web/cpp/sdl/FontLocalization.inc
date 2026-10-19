// Adapted from TH11 FontDevice.cpp, with TH15's 2x font sizes and point copy.
#if defined(TH_ENABLE_THCRAP)
namespace {
bool valid_utf8(const std::string& text){
 for(size_t p=0;p<text.size();){const u8 first=u8(text[p++]);if(first<0x80)continue;
  const int trailing=first>=0xc2&&first<=0xdf?1:first>=0xe0&&first<=0xef?2:first>=0xf0&&first<=0xf4?3:-1;
  if(trailing<0||p+size_t(trailing)>text.size())return false;
  const u8 next=u8(text[p]);if((first==0xe0&&next<0xa0)||(first==0xed&&next>=0xa0)||(first==0xf0&&next<0x90)||(first==0xf4&&next>=0x90))return false;
  for(int j=0;j<trailing;++j)if((u8(text[p++])&0xc0)!=0x80)return false;
 }return true;
}
std::string pack_font_file(){
 size_t size=0;auto* raw=static_cast<char*>(SDL_LoadFile("/thcrap/th15/localization/options.json",&size));if(!raw)return {};
 if(size>65536){SDL_free(raw);return {};}
 std::string options(raw,size);SDL_free(raw);const auto key=options.find("\"fontFile\"");if(key==std::string::npos)return {};
 const auto colon=options.find(':',key+10),open=colon==std::string::npos?colon:options.find('"',colon+1),close=open==std::string::npos?open:options.find('"',open+1);
 if(close==std::string::npos)return {};const auto name=options.substr(open+1,close-open-1);
 if(name.empty()||name.size()>128||name.find("..")!=std::string::npos||name.find_first_of("/\\:")!=std::string::npos)return {};
 return "/thcrap/th15/fonts/"+name;
}
u16 blend_pixel(u16 before,u32 color,u8 alpha){
 const u32 r=color&255,g=(color>>8)&255,b=(color>>16)&255;
 const u32 a=255-(before>>12)*17,old_r=((before>>8)&15)*17,old_g=((before>>4)&15)*17,old_b=(before&15)*17;
 const u32 out_a=alpha+(a*(255-alpha)+127)/255,out_r=(r*alpha+old_r*(255-alpha)+127)/255,out_g=(g*alpha+old_g*(255-alpha)+127)/255,out_b=(b*alpha+old_b*(255-alpha)+127)/255;
 return u16(((255-out_a+8)/17)<<12|((out_r+8)/17)<<8|((out_g+8)/17)<<4|((out_b+8)/17));
}
}
bool FontDevice::initialize_localized(){
 if(localization_checked)return true;const auto path=pack_font_file();
 if(!path.empty()){
  size_t locale_size=0;auto* locale=static_cast<char*>(SDL_LoadFile("/thcrap/th15/runtime-language.txt",&locale_size));
  english_details=locale&&locale_size==7&&!std::memcmp(locale,"lang_en",7);SDL_free(locale);
  if(!TTF_Init()){error="Unable to initialize THCRAP fonts";return false;}
  // Match TextRaster::font_index and the original 46f6a0 CreateFontA slots.
  // TextStyle::height is the scratch-row/outline size, not the font size.
  // Slot 8 is used by the localized spell-title request; keep other slots native.
  constexpr float heights[]={32,32,15,15,40,40,15,15,26};
  for(u32 i=0;i<localized.size();i++)if(!(localized[i]=TTF_OpenFont(path.c_str(),heights[i]))){error="Missing THCRAP font: "+path;return false;}
 }localization_checked=true;return true;
}
void FontDevice::font_style(int index,const std::string& commands){
 TTF_FontStyleFlags flags=TTF_STYLE_NORMAL;for(char c:commands){if(c=='b')flags|=TTF_STYLE_BOLD;else if(c=='i')flags|=TTF_STYLE_ITALIC;else if(c=='u')flags|=TTF_STYLE_UNDERLINE;}TTF_SetFontStyle(localized[index],flags);
}
LayoutLine FontDevice::layout(const std::string& text,int index){
 auto line=thcrap_layout(text,layout_tabs,1024,[&](const std::string& value,const std::string& commands){font_style(index,commands);int width=0,height=0;if(!value.empty())TTF_GetStringSize(localized[index],value.c_str(),value.size(),&width,&height);return width;});font_style(index,"");return line;
}
bool FontDevice::write_unicode(TextureImage& output,const TextRect& rect,const std::string& text,const TextStyle& style){
 const u32 rows=u32(std::max(17,style.height)*2+12);const int index=style.font;
 if(index<0||index>=9||rows>raster.scratch.height||text.size()>4096||output.format!=raster.scratch.format||rect.left<0||rect.top<0||rect.right<=rect.left||rect.bottom<=rect.top||rect.right>i32(output.width)||rect.bottom>i32(output.height))return false;
 auto& scratch=raster.scratch;auto* pixels=reinterpret_cast<u16*>(scratch.pixels.data());std::fill(scratch.pixels.begin(),scratch.pixels.end(),0);TextRaster::invert_alpha(scratch,rows);
 const auto line=layout(text,index);
 for(const auto& run:line.runs){
  font_style(index,run.commands);auto* rendered=TTF_RenderText_Blended(localized[index],run.text.c_str(),run.text.size(),SDL_Color{255,255,255,255});font_style(index,"");if(!rendered)return false;
  auto* rgba=SDL_ConvertSurface(rendered,SDL_PIXELFORMAT_RGBA32);SDL_DestroySurface(rendered);if(!rgba)return false;
  auto stamp=[&](i32 ox,i32 oy,u32 color){for(i32 y=0;y<rgba->h;y++){const i32 dy=y+oy;if(dy<0||dy>=i32(rows))continue;const auto* row=static_cast<const u8*>(rgba->pixels)+size_t(y)*rgba->pitch;for(i32 x=0;x<rgba->w;x++){const i32 dx=x+ox;if(dx<0||dx>=1024)continue;const u8 alpha=row[x*4+3];if(alpha)pixels[size_t(dy)*1024+dx]=blend_pixel(pixels[size_t(dy)*1024+dx],color,alpha);}}};
  const i32 x=style.offset+2+run.x;
  if(style.shadow)for(const auto delta:{Vec2{1,1},Vec2{-1,1},Vec2{1,3},Vec2{-1,3},Vec2{2,2},Vec2{-2,2},Vec2{0,0},Vec2{0,4}})stamp(x+i32(delta.x),i32(delta.y),style.outline);
  stamp(x,2,style.color);SDL_DestroySurface(rgba);
 }
 TextRaster::invert_alpha(scratch,rows);TextRaster::expand_alpha(scratch,rows);TextRaster::bleed(scratch,rows);
 const u32 width=rect.right-rect.left,height=rect.bottom-rect.top,source_width=std::min(1024u,width);if(height>scratch.height)return false;
 for(u32 y=0;y<height;y++)for(u32 x=0;x<width;x++){const u32 sx=u32((u64(x)*2+1)*source_width/(u64(width)*2));std::memcpy(output.pixels.data()+(rect.top+y)*output.pitch+(rect.left+x)*2,scratch.pixels.data()+y*scratch.pitch+sx*2,2);}return true;
}
#endif
FontDevice::~FontDevice(){
#if defined(TH_ENABLE_THCRAP)
 for(auto* font:localized)if(font)TTF_CloseFont(font);
#endif
}
