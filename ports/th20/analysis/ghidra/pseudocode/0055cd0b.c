/* Entry: 0x0055cd0b; symbol: FUN_0055cd0b; body bytes: 49 */

uint __cdecl FUN_0055cd0b(int param_1,int *param_2)

{
  uint *puVar1;
  uint uVar2;
  
  if (param_1 != 0) {
    puVar1 = (uint *)(param_1 + 0xc);
    param_1 = *puVar1 >> 0xd;
    if ((param_1 & 1U) != 0) {
      uVar2 = FUN_0055cd3c(*puVar1);
      if ((char)uVar2 != '\0') {
        return CONCAT31((int3)(uVar2 >> 8),1);
      }
      *param_2 = *param_2 + 1;
      param_1 = (int)param_2;
    }
  }
  return param_1 & 0xffffff00;
}

