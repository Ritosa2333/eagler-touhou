/* Entry: 0x004beb60; symbol: FUN_004beb60; body bytes: 161 */

void __fastcall FUN_004beb60(int param_1)

{
  char *pcVar1;
  undefined1 uVar2;
  uint uVar3;
  
  uVar2 = FUN_00463f20(param_1);
  *(undefined1 *)(param_1 + 0x1d1) = uVar2;
  uVar3 = FUN_00423ee0(0x5ba4c4);
  pcVar1 = (char *)(param_1 + 0x1b0 + uVar3 % 0x20);
  *pcVar1 = *pcVar1 + '\x01';
  *(char *)(param_1 + 0x1d1) = *(char *)(param_1 + 0x1d1) + '\x01';
  *(char *)(param_1 + 0x1d0) = *(char *)(param_1 + 0x1d0) + '\x02';
  uVar3 = FUN_00423ee0(0x5ba4c4);
  pcVar1 = (char *)(param_1 + 0x1d2 + uVar3 % 0x20);
  *pcVar1 = *pcVar1 + '\x01';
  return;
}

