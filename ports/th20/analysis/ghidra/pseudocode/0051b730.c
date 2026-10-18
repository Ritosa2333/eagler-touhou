/* Entry: 0x0051b730; symbol: FUN_0051b730; body bytes: 239 */

void __fastcall FUN_0051b730(int *param_1)

{
  char cVar1;
  int *piVar2;
  int iVar3;
  int *unaff_FS_OFFSET;
  uint uStack_48;
  int local_28;
  int local_20 [2];
  uint local_18;
  undefined1 *local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056ad4d;
  local_10 = *unaff_FS_OFFSET;
  uStack_48 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_14 = (undefined1 *)&uStack_48;
  *unaff_FS_OFFSET = (int)&local_10;
  local_18 = uStack_48;
  piVar2 = (int *)FUN_00438520((int)param_1 + *(int *)(*param_1 + 4));
  if (piVar2 != (int *)0x0) {
    FUN_00515ec0(local_20,param_1);
    local_8 = 0;
    cVar1 = FUN_00516450((int)local_20);
    if (cVar1 != '\0') {
      local_28 = 0;
      local_8 = CONCAT31(local_8._1_3_,1);
      iVar3 = FUN_0051c130(piVar2);
      if (iVar3 == -1) {
        local_28 = 4;
      }
      local_8 = 0;
      std::basic_ios<char,struct_std::char_traits<char>_>::setstate
                ((basic_ios<char,struct_std::char_traits<char>_> *)
                 ((int)param_1 + *(int *)(*param_1 + 4)),local_28,false);
    }
    local_8 = 0xffffffff;
    FUN_00516370(local_20);
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_18 ^ (uint)&stack0xfffffffc);
  return;
}

