/* Entry: 0x004ff4e0; symbol: FUN_004ff4e0; body bytes: 241 */

undefined4 __fastcall FUN_004ff4e0(int param_1)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  undefined4 local_10;
  undefined4 local_c;
  
  iVar2 = FUN_00464230(0);
  bVar1 = FUN_004ff840(iVar2);
  if (bVar1) {
    iVar2 = *(int *)(param_1 + 0x2208);
    iVar3 = FUN_00464080(0);
    iVar3 = FUN_00411700(iVar3);
    local_10 = *(undefined4 *)(iVar2 + iVar3 * 0xc + 0x30);
  }
  else {
    bVar1 = FUN_004ff5e0(param_1);
    if (bVar1) {
      iVar2 = *(int *)(param_1 + 0x2208);
      iVar3 = FUN_00464080(0);
      iVar3 = FUN_00411700(iVar3);
      local_c = *(undefined4 *)(iVar2 + iVar3 * 0xc + 0x2c);
    }
    else {
      iVar2 = *(int *)(param_1 + 0x2208);
      iVar3 = FUN_00464080(0);
      iVar3 = FUN_0041ca90(iVar3);
      local_c = *(undefined4 *)(iVar2 + 0x28 + iVar3 * 0xc);
    }
    local_10 = local_c;
  }
  return local_10;
}

