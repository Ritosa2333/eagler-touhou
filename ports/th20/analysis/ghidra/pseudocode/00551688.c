/* Entry: 0x00551688; symbol: FUN_00551688; body bytes: 36 */

void __cdecl FUN_00551688(uint param_1,int param_2)

{
  uint uVar1;
  
  *(undefined1 *)(param_2 + 0x24) = 1;
  *(uint *)(param_2 + 0x20) = param_1;
  uVar1 = FUN_00551606(param_1);
  *(undefined1 *)(param_2 + 0x1c) = 1;
  *(uint *)(param_2 + 0x18) = uVar1;
  return;
}

