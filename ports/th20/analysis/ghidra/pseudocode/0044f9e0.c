/* Entry: 0x0044f9e0; symbol: OnStatusTextChange; body bytes: 45 */

/* Library Function - Multiple Matches With Same Base Name
    public: virtual void __thiscall CHtmlView::OnStatusTextChange(char const *)
    public: virtual void __thiscall CHtmlView::OnStatusTextChange(wchar_t const *)
   
   Libraries: Visual Studio 2003 Debug, Visual Studio 2005 Debug, Visual Studio 2008 Debug, Visual
   Studio 2010 Debug */

void __thiscall OnStatusTextChange(void *this,undefined4 param_1)

{
  void *this_00;
  
  this_00 = (void *)FUN_0044ced0((undefined4 *)this);
  if (this_00 != (void *)0x0) {
    FUN_0044fa10(this_00,param_1);
  }
  return;
}

