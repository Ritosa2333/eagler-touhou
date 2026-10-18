/* Entry: 0x00542340; symbol: FUN_00542340; body bytes: 124 */

void __fastcall FUN_00542340(ios_base *param_1)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005679a0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  std::ios_base::_Callfns(param_1,0);
  puVar2 = *(undefined4 **)(param_1 + 0x28);
  while (puVar2 != (undefined4 *)0x0) {
    puVar1 = (undefined4 *)*puVar2;
    FUN_0054278d(puVar2);
    puVar2 = puVar1;
  }
  *(undefined4 *)(param_1 + 0x28) = 0;
  puVar2 = *(undefined4 **)(param_1 + 0x2c);
  while (puVar2 != (undefined4 *)0x0) {
    puVar1 = (undefined4 *)*puVar2;
    FUN_0054278d(puVar2);
    puVar2 = puVar1;
  }
  *(undefined4 *)(param_1 + 0x2c) = 0;
  *unaff_FS_OFFSET = local_10;
  return;
}

