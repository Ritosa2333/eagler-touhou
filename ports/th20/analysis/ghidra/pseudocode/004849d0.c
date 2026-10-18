/* Entry: 0x004849d0; symbol: FUN_004849d0; body bytes: 65 */

undefined4 __cdecl FUN_004849d0(int param_1)

{
  uint uVar1;
  undefined4 uVar2;
  
  if ((DAT_005ba828 != 0) && (uVar1 = FUN_00424030(DAT_005ba828), uVar1 != 0)) {
    return 1;
  }
  *(int *)(param_1 + 0x286d7c) = *(int *)(param_1 + 0x286d7c) + 1;
  uVar2 = FUN_0047d600(param_1);
  return uVar2;
}

