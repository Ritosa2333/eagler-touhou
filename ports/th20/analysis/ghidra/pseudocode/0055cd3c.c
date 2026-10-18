/* Entry: 0x0055cd3c; symbol: FUN_0055cd3c; body bytes: 35 */

uint __cdecl FUN_0055cd3c(uint param_1)

{
  undefined3 uVar1;
  
  uVar1 = (undefined3)((param_1 & 0xffffff03) >> 8);
  if (((char)(param_1 & 0xffffff03) == '\x02') && ((param_1 & 0xc0) != 0)) {
    return CONCAT31(uVar1,1);
  }
  return CONCAT31(uVar1,(char)(param_1 >> 0xb)) & 0xffffff01;
}

