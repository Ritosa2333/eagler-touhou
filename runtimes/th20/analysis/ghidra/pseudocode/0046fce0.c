/* Entry: 0x0046fce0; symbol: FUN_0046fce0; body bytes: 585 */

void __cdecl FUN_0046fce0(uint param_1,uint param_2,uint *param_3,int param_4)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint extraout_EDX;
  uint extraout_EDX_00;
  ulonglong uVar4;
  ulonglong uVar5;
  longlong lVar6;
  uint local_84;
  uint local_30;
  uint local_2c;
  uint local_20;
  int local_1c;
  uint local_18;
  int local_14;
  uint local_10;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00470030(param_1,param_2,*param_3,param_3[1],(int *)&local_10);
  uVar1 = FUN_00470030(param_1,param_2,param_3[2],param_3[3],(int *)&local_18);
  uVar2 = FUN_00470030(param_1,param_2,param_3[4],param_3[5],(int *)&local_20);
  uVar3 = extraout_EDX + local_c + (uint)CARRY4(uVar1,local_10);
  if ((extraout_EDX < uVar3) || ((extraout_EDX <= uVar3 && (uVar1 <= uVar1 + local_10)))) {
    local_2c = 0;
  }
  else {
    local_2c = 1;
  }
  uVar1 = extraout_EDX_00 + local_14 + (uint)CARRY4(uVar2,local_18) +
          (uint)CARRY4(uVar2 + local_18,local_2c);
  if ((extraout_EDX_00 < uVar1) ||
     ((extraout_EDX_00 <= uVar1 && (uVar2 <= uVar2 + local_18 + local_2c)))) {
    local_30 = 0;
  }
  else {
    local_30 = 1;
  }
  uVar2 = local_1c + (uint)CARRY4(local_30,local_20);
  if (param_4 < 0xa0) {
    FUN_0046fcb0(local_30 + local_20,uVar2);
    uVar4 = __allshl(0x20,0);
    uVar5 = __aullshr(0x20,uVar1);
    FUN_0046fcb0((int)(uVar4 | uVar5),(uint)((uVar4 | uVar5) >> 0x20));
    lVar6 = __allshl(0x20,0);
    uVar4 = __aullshr((char)param_4 + 0x80,(uint)((ulonglong)lVar6 >> 0x20));
    FUN_0046fcb0((int)uVar4,(uint)(uVar4 >> 0x20));
  }
  else {
    FUN_0046fcb0(local_30 + local_20,uVar2);
    lVar6 = __allshl(0x20,0);
    uVar4 = __aullshr(0x20,uVar1);
    local_84 = (uint)((ulonglong)lVar6 >> 0x20) | (uint)(uVar4 >> 0x20);
    uVar4 = __aullshr((char)param_4 + 0x60,local_84);
    FUN_0046fcb0((int)uVar4,(uint)(uVar4 >> 0x20));
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

