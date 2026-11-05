// ImGui GPU adapter reused from TH11; only render-scale handling differs.
#if TH_ENABLE_THPRAC
namespace {
constexpr const char* imguiVertexSource=R"(#version 300 es
precision mediump float;
uniform mat4 u_ProjMtx;
layout(location=0) in vec2 Position;
layout(location=1) in vec2 UV;
layout(location=2) in vec4 Color;
out vec2 Frag_UV;
out vec4 Frag_Color;
void main(){Frag_UV=UV;Frag_Color=Color.bgra;gl_Position=u_ProjMtx*vec4(Position.xy,0.0,1.0);}
)";
constexpr const char* imguiFragmentSource=R"(#version 300 es
precision mediump float;
in vec2 Frag_UV;
in vec4 Frag_Color;
uniform sampler2D Texture;
out vec4 Out_Color;
void main(){Out_Color=Frag_Color*texture(Texture,Frag_UV.st);}
)";

}
void Renderer::render_imgui(const ImDrawData* data,u32 targetHandle){
 if(!data||data->CmdListsCount<=0||data->TotalVtxCount<=0||data->TotalIdxCount<=0)return;
 // TH11's backbuffer is handle 1 and depth buffer is handle 2. The TH10
 // sentinel 0xffffffff is not a valid TH11 surface; ImGui needs no depth.
 flush();auto& output=target(targetHandle,0);
 if(!imguiProgram){
  const auto vs=shader(GL_VERTEX_SHADER,imguiVertexSource),fs=shader(GL_FRAGMENT_SHADER,imguiFragmentSource);imguiProgram=glCreateProgram();glAttachShader(imguiProgram,vs);glAttachShader(imguiProgram,fs);glLinkProgram(imguiProgram);glDeleteShader(vs);glDeleteShader(fs);
  GLint linked=0;glGetProgramiv(imguiProgram,GL_LINK_STATUS,&linked);if(!linked){char log[1024]{};glGetProgramInfoLog(imguiProgram,sizeof(log),nullptr,log);failure=std::string("ImGui program: ")+log;return;}
  imguiProjMtx=glGetUniformLocation(imguiProgram,"u_ProjMtx");imguiTexture=glGetUniformLocation(imguiProgram,"Texture");
  glGenVertexArrays(1,&imguiVao);glGenBuffers(1,&imguiVbo);glGenBuffers(1,&imguiEbo);glBindVertexArray(imguiVao);glBindBuffer(GL_ARRAY_BUFFER,imguiVbo);glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,imguiEbo);
  glEnableVertexAttribArray(0);glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(ImDrawVert),reinterpret_cast<void*>(offsetof(ImDrawVert,pos)));
  glEnableVertexAttribArray(1);glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(ImDrawVert),reinterpret_cast<void*>(offsetof(ImDrawVert,uv)));
  glEnableVertexAttribArray(2);glVertexAttribPointer(2,4,GL_UNSIGNED_BYTE,GL_TRUE,sizeof(ImDrawVert),reinterpret_cast<void*>(offsetof(ImDrawVert,col)));
 }
 auto& io=ImGui::GetIO();if(imguiFontTexture&&io.Fonts->TexID==nullptr){glDeleteTextures(1,&imguiFontTexture);imguiFontTexture=0;}
 if(!imguiFontTexture){unsigned char* pixels=nullptr;int width=0,height=0;io.Fonts->GetTexDataAsRGBA32(&pixels,&width,&height);if(!pixels||width<=0||height<=0)return;
  glGenTextures(1,&imguiFontTexture);glActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D,imguiFontTexture);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);glPixelStorei(GL_UNPACK_ALIGNMENT,1);glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);io.Fonts->TexID=reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(imguiFontTexture));io.Fonts->ClearTexData();
 }
 bind_framebuffer(GL_FRAMEBUFFER,output.framebuffer);const auto scale=resolve(owner,targetHandle).renderScale;glViewport(0,0,640*scale,480*scale);glEnable(GL_BLEND);glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);glBlendFuncSeparate(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA);glDisable(GL_CULL_FACE);glDisable(GL_DEPTH_TEST);glDepthMask(GL_FALSE);glEnable(GL_SCISSOR_TEST);glUseProgram(imguiProgram);glActiveTexture(GL_TEXTURE0);glBindVertexArray(imguiVao);glBindBuffer(GL_ARRAY_BUFFER,imguiVbo);glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,imguiEbo);glUniform1i(imguiTexture,0);
 const ImVec2 clipOffset=data->DisplayPos,clipScale=data->FramebufferScale;const float displayWidth=data->DisplaySize.x*clipScale.x,displayHeight=data->DisplaySize.y*clipScale.y,left=data->DisplayPos.x,right=left+data->DisplaySize.x,top=data->DisplayPos.y,bottom=top+data->DisplaySize.y;
 // The backbuffer is presented with a vertical flip (see commit()), so draw
 // ImGui pre-flipped: mirror the projection Y and skip the GL-origin scissor
 // conversion below. The two flips compose to identity for the overlay.
 const float projection[4][4]={{2.0f/(right-left),0,0,0},{0,2.0f/(bottom-top),0,0},{0,0,-1,0},{(right+left)/(left-right),(top+bottom)/(top-bottom),0,1}};glUniformMatrix4fv(imguiProjMtx,1,GL_FALSE,&projection[0][0]);
 glBufferData(GL_ARRAY_BUFFER,GLsizeiptr(data->TotalVtxCount*sizeof(ImDrawVert)),nullptr,GL_STREAM_DRAW);glBufferData(GL_ELEMENT_ARRAY_BUFFER,GLsizeiptr(data->TotalIdxCount*sizeof(ImDrawIdx)),nullptr,GL_STREAM_DRAW);GLsizeiptr vertexOffset=0,indexOffset=0;
 for(int listIndex=0;listIndex<data->CmdListsCount;++listIndex){const auto* list=data->CmdLists[listIndex];const GLsizeiptr vertexBytes=GLsizeiptr(list->VtxBuffer.Size*sizeof(ImDrawVert)),indexBytes=GLsizeiptr(list->IdxBuffer.Size*sizeof(ImDrawIdx));glBufferSubData(GL_ARRAY_BUFFER,vertexOffset,vertexBytes,list->VtxBuffer.Data);glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,indexOffset,indexBytes,list->IdxBuffer.Data);
  for(int commandIndex=0;commandIndex<list->CmdBuffer.Size;++commandIndex){const auto& command=list->CmdBuffer[commandIndex];if(command.UserCallback){if(command.UserCallback==ImDrawCallback_ResetRenderState){glUseProgram(imguiProgram);glBindVertexArray(imguiVao);glUniformMatrix4fv(imguiProjMtx,1,GL_FALSE,&projection[0][0]);}else command.UserCallback(list,&command);continue;}
   ImVec4 clip{(command.ClipRect.x-clipOffset.x)*clipScale.x,(command.ClipRect.y-clipOffset.y)*clipScale.y,(command.ClipRect.z-clipOffset.x)*clipScale.x,(command.ClipRect.w-clipOffset.y)*clipScale.y};if(clip.x>=clip.z||clip.y>=clip.w||clip.z<=0||clip.w<=0||clip.x>=displayWidth||clip.y>=displayHeight)continue;
   const GLint x=GLint(std::floor(std::max(clip.x,0.0f))),y=GLint(std::floor(std::max(clip.y,0.0f)));const GLsizei width=GLsizei(std::ceil(std::min(clip.z,displayWidth))-x),height=GLsizei(std::ceil(std::min(clip.w,displayHeight))-y);if(width<=0||height<=0)continue;glScissor(x*scale,y*scale,width*scale,height*scale);
   const GLuint texture=command.TextureId?GLuint(reinterpret_cast<uintptr_t>(command.TextureId)):imguiFontTexture;glBindTexture(GL_TEXTURE_2D,texture);const auto base=vertexOffset+GLsizeiptr(command.VtxOffset*sizeof(ImDrawVert));glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(ImDrawVert),reinterpret_cast<void*>(base+offsetof(ImDrawVert,pos)));glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(ImDrawVert),reinterpret_cast<void*>(base+offsetof(ImDrawVert,uv)));glVertexAttribPointer(2,4,GL_UNSIGNED_BYTE,GL_TRUE,sizeof(ImDrawVert),reinterpret_cast<void*>(base+offsetof(ImDrawVert,col)));glDrawElements(GL_TRIANGLES,GLsizei(command.ElemCount),sizeof(ImDrawIdx)==2?GL_UNSIGNED_SHORT:GL_UNSIGNED_INT,reinterpret_cast<void*>(indexOffset+GLsizeiptr(command.IdxOffset*sizeof(ImDrawIdx))));
  }vertexOffset+=vertexBytes;indexOffset+=indexBytes;
 }
 // ImGui is an end-of-frame overlay on the existing GPU backbuffer.  Keep the
 // GPU copy synchronized with the current CPU revision: advancing the private
 // GPU version here makes surface() treat the unchanged CPU pixels as newer on
 // the next paused/menu frame and upload them over the overlay/game image.
 output.rendered=true;drawState=DrawStateCache{};currentProgram=0;currentLayout=0;boundTexture=readFramebuffer=drawFramebuffer=~0u;glDepthMask(GL_TRUE);
}
#endif
