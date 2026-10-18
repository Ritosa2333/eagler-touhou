/* Entry: 0x004ab140; symbol: FUN_004ab140; body bytes: 98 */

int __fastcall FUN_004ab140(int param_1)

{
  int iVar1;
  
  iVar1 = DAT_005c49f0;
  DAT_005c49ec = DAT_005c49f0;
  DAT_005c49f0 = DAT_005c49f0 + 1 & 0xffff;
  if (DAT_005c49f0 == 0) {
    DAT_005c49f0 = 1;
  }
  DAT_005c49f0 = *(int *)(param_1 + 300) << 0x10 | DAT_005c49f0;
  return iVar1;
}

