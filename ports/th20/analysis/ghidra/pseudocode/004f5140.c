/* Entry: 0x004f5140; symbol: locale; body bytes: 72 */

/* Library Function - Single Match
    public: __thiscall std::locale::locale(void)
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

locale * __thiscall std::locale::locale(locale *this)

{
  _Locimp *p_Var1;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  p_Var1 = FUN_00541b24('\x01');
  *(_Locimp **)(this + 4) = p_Var1;
  *unaff_FS_OFFSET = local_10;
  return this;
}

