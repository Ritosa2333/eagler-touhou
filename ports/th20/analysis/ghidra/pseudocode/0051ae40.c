/* Entry: 0x0051ae40; symbol: FUN_0051ae40; body bytes: 190 */

void __fastcall FUN_0051ae40(int *param_1)

{
  bool bVar1;
  uint uVar2;
  int *piVar3;
  int iVar4;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  puStack_c = &LAB_0056ad20;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  local_8 = 0;
  bVar1 = FUN_0051ba80((int)param_1 + *(int *)(*param_1 + 4));
  if (bVar1) {
    uVar2 = FUN_0041ca90((int)param_1 + *(int *)(*param_1 + 4));
    if ((uVar2 & 2) != 0) {
      piVar3 = (int *)FUN_00438520((int)param_1 + *(int *)(*param_1 + 4));
      iVar4 = FUN_0051c130(piVar3);
      if (iVar4 == -1) {
        std::basic_ios<char,struct_std::char_traits<char>_>::setstate
                  ((basic_ios<char,struct_std::char_traits<char>_> *)
                   ((int)param_1 + *(int *)(*param_1 + 4)),4,false);
      }
    }
  }
  *unaff_FS_OFFSET = local_10;
  return;
}

