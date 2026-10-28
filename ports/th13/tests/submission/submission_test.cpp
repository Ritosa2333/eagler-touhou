#include "../../web/cpp/sdl/GraphicsDevice.hpp"
#include <cassert>
#include <cerrno>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>

using namespace th13;
using namespace touhou::graphics;
using Submission = th13::sdl::RenderSubmission;
static std::string rendered;
static std::string load(const std::string& path) {
    std::ifstream f(path); return {std::istreambuf_iterator<char>(f), {}};
}
static std::string hex(const void* data, size_t n) {
    const auto* p = static_cast<const unsigned char*>(data); std::string out;
    for (size_t i=0;i<n;++i) { char b[3]; std::snprintf(b,3,"%02x",p[i]); out += b; }
    return out;
}
// This is deliberately an operation recorder, NOT an oracle for GLES pixels.
// It implements the real Renderer interface to compile/test GraphicsDevice.cpp.
namespace touhou::sdl {
State::State() { for (auto& m:matrix) m[0]=m[5]=m[10]=m[15]=1; }
Renderer::Renderer(int v, Resolve r, void* o):version(v),resolve(r),owner(o) {}
Renderer::~Renderer() = default;
bool Renderer::initialize() { return true; }
void Renderer::flush() { rendered += "flush;"; }
void Renderer::draw(Topology t,u32 n,const void* data,u32 stride,const void*,IndexType) {
    rendered += "draw:"+std::to_string(state.target)+":"+std::to_string(state.texture)+":"+
        std::to_string(state.pipeline.textureFactor)+":"+hex(data,size_t(vertex_count(t,n))*stride)+";";
}
void Renderer::draw_batch(u32 n,const void* data,u32 stride) { draw(Topology::Triangles,n,data,stride); }
void Renderer::clear(u32 flags,u32 color,float,u32,const i32* rect,u32) {
    rendered += "clear:"+std::to_string(state.target)+":"+std::to_string(flags)+":"+std::to_string(color)+(rect?hex(rect,16):"")+";";
}
void Renderer::changed(u32 id,u32 previous,const std::array<u32,4>&) { rendered += "update:"+std::to_string(id)+":"+std::to_string(previous)+";"; }
void Renderer::prepare(u32 id) { rendered += "prepare:"+std::to_string(id)+";"; }
void Renderer::release(u32 id) { rendered += "release:"+std::to_string(id)+";"; }
void Renderer::present(u32 id) { rendered += "present:"+std::to_string(id)+";"; }
}
struct Vertex { float x,y,z,w; uint32_t color; float u,v; };
static std::string capture(const std::string& path, bool enabled, int mutation=0) {
    rendered.clear();
    th13::sdl::GraphicsDevice d; assert(d.initialize());
    AnmResource file; AnmTexture t; t.kind=AnmTexture::Kind::Blank; t.name="synthetic";t.width=t.height=2;
    file.textures.push_back(t);
    if (enabled) assert(d.capture_begin(path.c_str()));
    assert(d.preload(file)); auto handle=d.texture(file,0);d.bind_texture(handle);
    d.set_layout(VertexLayout::ScreenColorUv);
    d.pipeline().textureFactor=0xff123456;
    Vertex v[6]{};for (auto& a:v) { a.w=1;a.color=0xffffffff; }
    if (mutation==1) v[5].x=123;
    if (mutation==2) v[5].v=.75f;
    if (mutation==3) d.pipeline().colorMask=7;
    if (mutation==4) assert(d.select_target(&file,0));
    if (mutation==5) d.set_layout(VertexLayout::ScreenColor);
    if (mutation==6) d.backend.state.viewport.width=123;
    if (mutation==7) d.backend.state.matrix[3][15]=-0.f;
    d.triangles(2,v,sizeof(Vertex));
    if (mutation==8) {
        auto* image=d.pixels(handle);image->pixels.back()=17;d.changed(handle);
    }
    d.present();
    if (enabled) { assert(d.capture_finish());assert(d.submission_trace.frames_out()==1); }
    return rendered;
}
struct CloseFailure { bool closed=false; };
static ssize_t accept_write(void*,const char*,size_t n) { return ssize_t(n); }
static int fail_close(void* cookie) { static_cast<CloseFailure*>(cookie)->closed=true;errno=EIO;return -1; }
int main(int argc,char** argv) {
    assert(argc==2);const std::string dir=argv[1];
    const auto off=capture(dir+"/off.jsonl",false);
    const auto on=capture(dir+"/base.jsonl",true);
    assert(off==on);
    const auto base=load(dir+"/base.jsonl");
    assert(base.find("\"kind\":\"complete\"")!=std::string::npos);
    for (int m=1;m<=8;++m) {
        const auto path=dir+"/mutation"+std::to_string(m)+".jsonl";
        capture(path,true,m);assert(load(path)!=base);
    }
    std::cout << "ok capture toggle and complete geometry/state/layout/viewport/target/update mutations\n";
    // Snapshot is owned even if the live state is overwritten before dispatch.
    touhou::sdl::State state; state.pipeline.textureFactor=77;
    Submission snapshot{Submission::Kind::Flush,state};state.pipeline.textureFactor=88;
    assert(snapshot.state.pipeline.textureFactor==77);
    SubmissionTrace trace;
    assert(trace.open((dir+"/unknown.jsonl").c_str()));
    Submission unknown{Submission::Kind::Draw,state};
    unknown.state.texture=999; Vertex v[3]{};unknown.vertices=v;unknown.stride=sizeof(Vertex);unknown.count=1;
    auto missing=+[](void*,uint32_t,SubmissionTrace::Resource&){return false;};
    th13::sdl::GraphicsDevice d;assert(d.initialize());rendered.clear();
    assert(!unknown.dispatch(d.backend,trace,missing,nullptr));
    assert(rendered.find("draw:")!=std::string::npos); // error cannot suppress production
    assert(trace.error().find("unknown resource")!=std::string::npos);assert(!trace.close());
    assert(load(dir+"/unknown.jsonl").find("\"complete\":false")!=std::string::npos);
    std::cout << "ok owned snapshot, unknown is not null, trace failure preserves production\n";
    SubmissionTrace incomplete;assert(incomplete.open((dir+"/incomplete.jsonl").c_str()));
    assert(incomplete.consume(snapshot,missing,nullptr));assert(!incomplete.close());
    SubmissionTrace count;assert(count.open((dir+"/count.jsonl").c_str()));assert(!count.close(1));
    SubmissionTrace full;assert(!full.open("/dev/full"));assert(!full.close());
    CloseFailure cookie;cookie_io_functions_t io{};io.write=accept_write;io.close=fail_close;
    auto* stream=fopencookie(&cookie,"w",io);assert(stream);
    SubmissionTrace closing;assert(closing.open_stream(stream));assert(!closing.close());
    assert(cookie.closed);assert(closing.error()=="trace close failed");
    std::cout << "ok incomplete frame, frame count, real write and close failures\n";
    assert(SubmissionTrace::sha256(nullptr,0)=="e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    assert(SubmissionTrace::sha256("abc",3)=="ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
    SubmissionTrace::ContentHash cache;unsigned char bytes[65]{};
    SubmissionTrace::Surface s;s.data=bytes;s.size=sizeof bytes;
    const auto a=cache.get(s,0);bytes[64]=1;
    assert(cache.get(s,1)!=a);
    cache.invalidate();assert(cache.get(s,1)==SubmissionTrace::sha256(bytes,sizeof bytes));
    for (size_t n : {size_t(55),size_t(56),size_t(63),size_t(64),size_t(65)})
        std::cout << "hash " << n << " " << SubmissionTrace::sha256(bytes,n) << "\n";
    std::cout << "ok SHA256 known vectors and revision cache invalidation\n";
}
