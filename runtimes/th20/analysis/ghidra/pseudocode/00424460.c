/* Entry: 0x00424460; symbol: FUN_00424460; body bytes: 82 */

undefined4 __cdecl FUN_00424460(int param_1)

{
  bool bVar1;
  undefined4 uVar2;
  
  *(undefined4 *)(param_1 + 0x18) = 0xff;
  bVar1 = FUN_004235c0((void *)(param_1 + 0x30),*(int *)(param_1 + 0x1c));
  if (bVar1) {
    uVar2 = 7;
  }
  else {
    FUN_00423540((undefined4 *)(param_1 + 0x30));
    uVar2 = 1;
  }
  return uVar2;
}

