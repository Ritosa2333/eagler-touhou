/* Entry: 0x00565b6d; symbol: FUN_00565b6d; body bytes: 178 */

undefined4 __cdecl FUN_00565b6d(uint param_1,short *param_2,uint param_3,uint param_4,char param_5)

{
  short sVar1;
  ulonglong uVar2;
  short *psVar3;
  __acrt_ptd *p_Var4;
  undefined4 uVar5;
  short *psVar6;
  uint local_c;
  short *local_8;
  
  local_8 = param_2;
  if (param_5 != '\0') {
    *param_2 = 0x2d;
    local_8 = param_2 + 1;
    param_1 = -param_1;
  }
  local_c = (uint)(param_5 != '\0');
  psVar3 = local_8;
  do {
    psVar6 = psVar3;
    uVar2 = (ulonglong)param_1;
    param_1 = param_1 / param_4;
    local_c = local_c + 1;
    *psVar6 = (-(ushort)(9 < (uint)(uVar2 % (ulonglong)param_4)) & 0x27) + 0x30 +
              (short)(uVar2 % (ulonglong)param_4);
    if (param_1 == 0) break;
    psVar3 = psVar6 + 1;
  } while (local_c < param_3);
  if (local_c < param_3) {
    psVar6[1] = 0;
    do {
      sVar1 = *psVar6;
      *psVar6 = *local_8;
      psVar6 = psVar6 + -1;
      *local_8 = sVar1;
      local_8 = local_8 + 1;
    } while (local_8 < psVar6);
    uVar5 = 0;
  }
  else {
    *param_2 = 0;
    p_Var4 = FUN_005516c1();
    uVar5 = 0x22;
    *(undefined4 *)p_Var4 = 0x22;
    FUN_005480bc();
  }
  return uVar5;
}

