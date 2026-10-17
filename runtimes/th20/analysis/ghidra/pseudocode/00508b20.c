/* Entry: 0x00508b20; symbol: ClearCancel; body bytes: 51 */

/* Library Function - Single Match
    public: void __thiscall Concurrency::details::ContextBase::ClearCancel(void)
   
   Libraries: Visual Studio 2012, Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

void __thiscall Concurrency::details::ContextBase::ClearCancel(ContextBase *this)

{
  *(undefined4 *)(this + 8) = 0;
  *(undefined4 *)(this + 4) = 0;
  *(undefined4 *)(this + 0xc) = 0;
  *(undefined4 *)(this + 0x10) = 0;
  return;
}

