/* Entry: 0x0052dd70; symbol: iterator::operator++; body bytes: 29 */

/* Library Function - Multiple Matches With Same Base Name
    public: class __FrameHandler3::HandlerMap::iterator & __thiscall
   __FrameHandler3::HandlerMap::iterator::operator++(void)
    public: class __FrameHandler3::TryBlockMap::iterator & __thiscall
   __FrameHandler3::TryBlockMap::iterator::operator++(void)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2019 Debug */

int __fastcall iterator__operator__(int param_1)

{
  *(int *)(param_1 + 4) = *(int *)(param_1 + 4) + 1;
  return param_1;
}

