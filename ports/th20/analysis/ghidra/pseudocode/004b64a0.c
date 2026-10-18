/* Entry: 0x004b64a0; symbol: FUN_004b64a0; body bytes: 183 */

void __fastcall FUN_004b64a0(int param_1)

{
  bool bVar1;
  undefined3 extraout_var;
  uint uVar2;
  
  bVar1 = FUN_00478160(param_1);
  if (CONCAT31(extraout_var,bVar1) != 0) {
    FUN_004ae830(*(void **)(param_1 + 0x1bc));
    *(undefined4 *)(param_1 + 0x1bc) = 0;
    *(undefined4 *)(param_1 + 0x1bc) = 0;
  }
  uVar2 = FUN_00474e00(0x5ba568);
  if (uVar2 == 0) {
    uVar2 = FUN_004b8370(0x5ba568);
    if (uVar2 == 0) {
      FUN_0044c430(DAT_005c0028,6);
      *(undefined4 *)(param_1 + 0x170) = 0;
      if (*(int *)(param_1 + 0x1c0) != 0) {
        FUN_0041f670(*(LPVOID *)(param_1 + 0x1c0));
        *(undefined4 *)(param_1 + 0x1c0) = 0;
      }
      *(undefined4 *)(param_1 + 0x1c0) = 0;
    }
  }
  return;
}

