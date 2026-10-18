/* Entry: 0x004f5540; symbol: ~_Locinfo; body bytes: 140 */

/* Library Function - Single Match
    public: __thiscall std::_Locinfo::~_Locinfo(void)
   
   Library: Visual Studio 2019 Release */

void __thiscall std::_Locinfo::~_Locinfo(_Locinfo *this)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  _Locinfo_dtor(this);
  FUN_004f5240((int *)(this + 0x2c));
  FUN_004f5240((int *)(this + 0x24));
  FUN_004f5240((int *)(this + 0x1c));
  FUN_004f5240((int *)(this + 0x14));
  FUN_004f5240((int *)(this + 0xc));
  FUN_004f5240((int *)(this + 4));
  FUN_005419ac((int *)this);
  *unaff_FS_OFFSET = local_10;
  return;
}

