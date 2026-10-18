/* Entry: 0x0051e1c0; symbol: FUN_0051e1c0; body bytes: 505 */

void __fastcall FUN_0051e1c0(void *param_1)

{
  bool bVar1;
  undefined4 uVar2;
  int iVar3;
  void *this;
  int iVar4;
  int iVar5;
  int *unaff_FS_OFFSET;
  int local_3c;
  int local_38;
  int local_34;
  undefined4 local_30;
  int local_2c [3];
  undefined4 local_20;
  int local_1c;
  int local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056ae7d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar2 = FUN_0040c310(&DAT_005c0240,0x14);
  FUN_0040b730(&local_30,uVar2);
  local_8 = 0;
  for (local_34 = 0; local_34 < 0x71; local_34 = local_34 + 1) {
    iVar3 = FUN_00488700((int)param_1);
    this = (void *)FUN_00486e10((void *)(iVar3 + 0xb08),local_34);
    FUN_00488590(this,0);
  }
  local_20 = 0;
  FUN_00461510(&local_18);
  FUN_0050ae60(local_2c + 2);
  while( true ) {
    bVar1 = FUN_00449460(&local_18,local_2c + 2);
    if (!bVar1) break;
    iVar3 = FUN_0040c300(&local_18);
    local_2c[1] = 0;
    FUN_00461510(&local_1c);
    FUN_0050ae80(local_2c);
    while( true ) {
      bVar1 = FUN_00449460(&local_1c,local_2c);
      if (!bVar1) break;
      iVar4 = FUN_0040c300(&local_1c);
      iVar5 = FUN_004bd460(param_1,iVar3,iVar4);
      *(undefined4 *)(iVar5 + 0x76f0) = 1;
      for (local_38 = 0; local_38 < 6; local_38 = local_38 + 1) {
        for (local_3c = 0; local_3c < 8; local_3c = local_3c + 1) {
          iVar5 = FUN_004bd460(param_1,iVar3,iVar4);
          *(undefined1 *)(iVar5 + local_38 * 0x90 + 0x7701 + local_3c * 0x10) = 1;
        }
      }
      FUN_00461860(&local_1c);
    }
    FUN_00461860(&local_18);
  }
  FUN_004beb40((int)param_1);
  local_8 = 0xffffffff;
  FUN_0040b900(&local_30);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

