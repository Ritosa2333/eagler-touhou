/* Entry: 0x005424bf; symbol: ~_Init_atexit; body bytes: 99 */

/* Library Function - Single Match
    public: __thiscall _Init_atexit::~_Init_atexit(void)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __thiscall _Init_atexit::~_Init_atexit(_Init_atexit *this)

{
  int iVar1;
  uint uVar2;
  code *pcVar3;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  uVar2 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  while (DAT_005b2584 < 10) {
    iVar1 = DAT_005b2584 * 4;
    DAT_005b2584 = DAT_005b2584 + 1;
    pcVar3 = (code *)DecodePointer(*(PVOID *)(iVar1 + 0x5e4ea4));
    if (pcVar3 != (code *)0x0) {
      (*(code *)PTR_guard_check_icall_0056c37c)(uVar2);
      (*pcVar3)();
    }
  }
  *unaff_FS_OFFSET = local_10;
  return;
}

