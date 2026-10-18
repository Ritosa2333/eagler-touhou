/* Entry: 0x0041a240; symbol: ReleaseDocSite; body bytes: 50 */

/* Library Function - Single Match
    public: void __thiscall CDocObjectServer::ReleaseDocSite(void)
   
   Libraries: Visual Studio 2005 Debug, Visual Studio 2008 Debug, Visual Studio 2010 Debug */

void __thiscall CDocObjectServer::ReleaseDocSite(CDocObjectServer *this)

{
  if (*(int *)(this + 8) != 0) {
    (**(code **)(**(int **)(this + 8) + 8))(*(undefined4 *)(this + 8));
    *(undefined4 *)(this + 8) = 0;
  }
  return;
}

