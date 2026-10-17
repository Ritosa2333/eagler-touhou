/* Entry: 0x0054ef27; symbol: FUN_0054ef27; body bytes: 21 */

void __fastcall FUN_0054ef27(int *param_1)

{
  byte *pbVar1;
  byte *pbVar2;
  
  pbVar2 = (byte *)FUN_0054e9a0(param_1);
  pbVar1 = pbVar2 + 0x2000;
  for (; pbVar2 != pbVar1; pbVar2 = pbVar2 + 1) {
    *pbVar2 = ~*pbVar2;
  }
  return;
}

