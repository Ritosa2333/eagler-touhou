/* Entry: 0x00470030; symbol: FUN_00470030; body bytes: 271 */

uint __cdecl FUN_00470030(uint param_1,uint param_2,uint param_3,uint param_4,int *param_5)

{
  uint uVar1;
  ulonglong uVar2;
  ulonglong uVar3;
  ulonglong uVar4;
  longlong lVar5;
  uint local_5c;
  int local_58;
  uint local_34;
  uint local_30;
  
  uVar2 = __aullshr(0x20,param_2);
  uVar3 = __aullshr(0x20,param_4);
  lVar5 = (uVar2 & 0xffffffff) * (uVar3 & 0xffffffff);
  local_34 = (uint)((ulonglong)param_1 * (ulonglong)param_3);
  local_30 = (uint)((ulonglong)param_1 * (ulonglong)param_3 >> 0x20);
  uVar4 = __aullshr(0x20,local_30);
  uVar2 = (uVar2 & 0xffffffff) * (ulonglong)param_3 + (uVar4 & 0xffffffff);
  uVar4 = __aullshr(0x20,(uint)(uVar2 >> 0x20));
  uVar2 = __aullshr(0x20,(uint)((ulonglong)param_1 * (uVar3 & 0xffffffff) + (uVar2 & 0xffffffff) >>
                               0x20));
  local_5c = (uint)lVar5;
  uVar1 = (uint)uVar4 + local_5c;
  local_58 = (int)((ulonglong)lVar5 >> 0x20);
  lVar5 = __allshl(0x20,0);
  *param_5 = uVar1 + (uint)uVar2;
  param_5[1] = local_58 + (uint)CARRY4((uint)uVar4,local_5c) + (uint)CARRY4(uVar1,(uint)uVar2);
  return (uint)lVar5 | local_34;
}

