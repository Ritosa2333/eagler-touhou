/* Entry: 0x005322c0; symbol: FUN_005322c0; body bytes: 80 */

void __fastcall FUN_005322c0(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined1 local_14 [8];
  void *local_c;
  int local_8;
  
  local_8 = param_1;
  iVar2 = FUN_0040ff90(*(int *)(param_1 + 0x128));
  local_c = (void *)(iVar2 + 0x630);
  puVar3 = (undefined4 *)FUN_004f58f0(local_c,local_14,(int *)(local_8 + 0x88));
  uVar1 = puVar3[1];
  *(undefined4 *)(local_8 + 0x70) = *puVar3;
  *(undefined4 *)(local_8 + 0x74) = uVar1;
  return;
}

