/* Entry: 0x0041a200; symbol: ReleaseDocSite; body bytes: 50 */

/* Library Function - Single Match
    public: void __thiscall CDocObjectServer::ReleaseDocSite(void)
   
   Libraries: Visual Studio 2005 Debug, Visual Studio 2008 Debug, Visual Studio 2010 Debug */

void __thiscall CDocObjectServer::ReleaseDocSite(CDocObjectServer *this)

{
  if (*(int *)(this + 4) != 0) {
    (**(code **)(**(int **)(this + 4) + 8))(*(undefined4 *)(this + 4));
    *(undefined4 *)(this + 4) = 0;
  }
  return;
}

