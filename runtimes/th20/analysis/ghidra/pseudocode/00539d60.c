/* Entry: 0x00539d60; symbol: ~shared_ptr<class___ExceptionPtr>; body bytes: 62 */

/* Library Function - Single Match
    public: __thiscall std::shared_ptr<class __ExceptionPtr>::~shared_ptr<class
   __ExceptionPtr>(void)
   
   Libraries: Visual Studio 2015 Debug, Visual Studio 2015 Release */

void __thiscall
std::shared_ptr<class___ExceptionPtr>::~shared_ptr<class___ExceptionPtr>
          (shared_ptr<class___ExceptionPtr> *this)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0053a170((undefined4 *)this);
  *unaff_FS_OFFSET = local_10;
  return;
}

