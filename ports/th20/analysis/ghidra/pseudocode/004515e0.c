/* Entry: 0x004515e0; symbol: FUN_004515e0; body bytes: 54 */

void FUN_004515e0(int param_1)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  undefined1 local_10 [8];
  int local_8;
  
  puVar2 = (undefined4 *)FUN_0044dde0(local_10);
  uVar1 = puVar2[1];
  *(undefined4 *)(local_8 + 0x90 + param_1 * 8) = *puVar2;
  *(undefined4 *)(local_8 + 0x94 + param_1 * 8) = uVar1;
  return;
}

