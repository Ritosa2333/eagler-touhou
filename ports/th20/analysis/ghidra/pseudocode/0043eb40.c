/* Entry: 0x0043eb40; symbol: FUN_0043eb40; body bytes: 985 */

void FUN_0043eb40(void *param_1,float *param_2,float *param_3,float *param_4,float *param_5)

{
  int iVar1;
  int iVar2;
  int iVar3;
  float fVar4;
  float fVar5;
  float local_10;
  float local_8;
  
  iVar1 = *(int *)((int)param_1 + 0x4a8) * 0x10;
  iVar3 = *(int *)((int)param_1 + 0x4ac) * 0x10;
  iVar2 = FUN_00437cd0((int)param_1);
  fVar4 = *(float *)(iVar2 + 0x24) - *(float *)((int)param_1 + 0x80);
  fVar5 = *(float *)(iVar2 + 0x28) - *(float *)((int)param_1 + 0x84);
  local_8 = *(float *)((int)param_1 + 0x50) * *(float *)((int)param_1 + 0x58);
  local_10 = *(float *)((int)param_1 + 0x54) * *(float *)((int)param_1 + 0x5c);
  if ((*(int *)((int)param_1 + 0x55c) != 0) && ((*(uint *)((int)param_1 + 0x49c) >> 0xc & 1) == 0))
  {
    local_8 = *(float *)(*(int *)((int)param_1 + 0x55c) + 0x50) *
              *(float *)(*(int *)((int)param_1 + 0x55c) + 0x58) * local_8;
    local_10 = *(float *)(*(int *)((int)param_1 + 0x55c) + 0x54) *
               *(float *)(*(int *)((int)param_1 + 0x55c) + 0x5c) * local_10;
  }
  if ((*(char *)((int)param_1 + 0x4a4) == '\x01') || (*(char *)((int)param_1 + 0x4a4) == '\x05')) {
    local_8 = local_8 * DAT_005b8818;
    local_10 = local_10 * DAT_005b8818;
  }
  else if ((*(char *)((int)param_1 + 0x4a4) == '\x02') ||
          (*(char *)((int)param_1 + 0x4a4) == '\x06')) {
    local_8 = DAT_005b8818 * DAT_0056e0ec * local_8;
    local_10 = DAT_005b8818 * DAT_0056e0ec * local_10;
  }
  *param_2 = (*(float *)(&DAT_0056e140 + iVar1) * *(float *)((int)param_1 + 0x70) - fVar4) *
             *(float *)(iVar2 + 0x2c) * local_8;
  param_2[1] = (*(float *)(&DAT_0056e170 + iVar3) * *(float *)((int)param_1 + 0x74) - fVar5) *
               *(float *)(iVar2 + 0x30) * local_10;
  *param_3 = (*(float *)(&DAT_0056e144 + iVar1) * *(float *)((int)param_1 + 0x70) - fVar4) *
             *(float *)(iVar2 + 0x2c) * local_8;
  param_3[1] = (*(float *)(&DAT_0056e174 + iVar3) * *(float *)((int)param_1 + 0x74) - fVar5) *
               *(float *)(iVar2 + 0x30) * local_10;
  *param_4 = (*(float *)(&DAT_0056e148 + iVar1) * *(float *)((int)param_1 + 0x70) - fVar4) *
             *(float *)(iVar2 + 0x2c) * local_8;
  param_4[1] = (*(float *)(&DAT_0056e178 + iVar3) * *(float *)((int)param_1 + 0x74) - fVar5) *
               *(float *)(iVar2 + 0x30) * local_10;
  *param_5 = (*(float *)(&DAT_0056e14c + iVar1) * *(float *)((int)param_1 + 0x70) - fVar4) *
             *(float *)(iVar2 + 0x2c) * local_8;
  param_5[1] = (*(float *)(&DAT_0056e17c + iVar3) * *(float *)((int)param_1 + 0x74) - fVar5) *
               *(float *)(iVar2 + 0x30) * local_10;
  FUN_004376e0(param_1,(float *)&DAT_005c0018);
  FUN_004296e0(param_2,(float *)&DAT_005c0018);
  FUN_004296e0(param_3,(float *)&DAT_005c0018);
  FUN_004296e0(param_4,(float *)&DAT_005c0018);
  FUN_004296e0(param_5,(float *)&DAT_005c0018);
  fVar4 = *(float *)((int)param_1 + 0x34) + *(float *)((int)param_1 + 0x5c4) +
          *(float *)((int)param_1 + 0x48c);
  param_5[2] = fVar4;
  param_4[2] = fVar4;
  param_3[2] = fVar4;
  param_2[2] = fVar4;
  return;
}

