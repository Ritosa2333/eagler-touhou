/* Entry: 0x005671f0; symbol: __IsNonwritableInCurrentImage; body bytes: 164 */

/* Library Function - Single Match
    __IsNonwritableInCurrentImage
   
   Library: Visual Studio 2019 Release */

BOOL __cdecl __IsNonwritableInCurrentImage(PBYTE pTarget)

{
  uint uVar1;
  bool bVar2;
  undefined3 extraout_var;
  PIMAGE_SECTION_HEADER p_Var3;
  int *unaff_FS_OFFSET;
  int local_14;
  code *pcStack_10;
  uint local_c;
  undefined4 local_8;
  
  pcStack_10 = __except_handler4;
  local_14 = *unaff_FS_OFFSET;
  local_c = DAT_005b25c0 ^ 0x5ac720;
  *unaff_FS_OFFSET = (int)&local_14;
  local_8 = 0;
  bVar2 = FUN_005672b0((short *)&IMAGE_DOS_HEADER_00400000);
  if (CONCAT31(extraout_var,bVar2) != 0) {
    p_Var3 = __FindPESection((PBYTE)&IMAGE_DOS_HEADER_00400000,(DWORD_PTR)(pTarget + -0x400000));
    if (p_Var3 != (PIMAGE_SECTION_HEADER)0x0) {
      uVar1 = p_Var3->Characteristics;
      *unaff_FS_OFFSET = local_14;
      return ~(uVar1 >> 0x1f) & 1;
    }
  }
  *unaff_FS_OFFSET = local_14;
  return 0;
}

