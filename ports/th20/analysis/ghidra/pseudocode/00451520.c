/* Entry: 0x00451520; symbol: FUN_00451520; body bytes: 88 */

void FUN_00451520(int param_1)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  undefined1 local_10 [8];
  int local_8;
  
  puVar2 = (undefined4 *)FUN_0044dde0(local_10);
  uVar1 = puVar2[1];
  *(undefined4 *)(local_8 + 0x90 + param_1 * 8) = *puVar2;
  *(undefined4 *)(local_8 + 0x94 + param_1 * 8) = uVar1;
  *(undefined4 *)(local_8 + 0x50 + param_1 * 8) = *(undefined4 *)(local_8 + 0x90 + param_1 * 8);
  *(undefined4 *)(local_8 + 0x54 + param_1 * 8) = *(undefined4 *)(local_8 + 0x94 + param_1 * 8);
  return;
}

