/* Entry: 0x004fe210; symbol: FUN_004fe210; body bytes: 168 */

uint __cdecl FUN_004fe210(uint param_1,uint param_2,uint param_3,char param_4)

{
  longlong lVar1;
  uint uVar2;
  ulonglong uVar3;
  ulonglong uVar4;
  uint local_2c;
  int local_28;
  uint local_24;
  uint local_14;
  
  uVar3 = __aullshr(0x20,param_3);
  lVar1 = (ulonglong)param_1 * (uVar3 & 0xffffffff);
  local_2c = (uint)((ulonglong)param_1 * (ulonglong)param_2 >> 0x20);
  uVar3 = __aullshr(0x20,local_2c);
  local_28 = (int)lVar1;
  local_24 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar4 = __aullshr(0x20,local_24);
  uVar2 = local_28 + (uint)uVar3;
  local_14 = (uint)(uVar2 < (uint)uVar3);
  return (int)uVar4 + local_14 << (0x20 - (param_4 - 0x20U) & 0x1f) |
         uVar2 >> (param_4 - 0x20U & 0x1f);
}

