/* Entry: 0x00564100; symbol: FUN_00564100; body bytes: 277 */

void __cdecl FUN_00564100(int param_1,int *param_2,ushort *param_3)

{
  bool bVar1;
  undefined3 extraout_var;
  int iVar2;
  uint uVar3;
  uint local_94;
  uint local_90 [12];
  undefined8 local_60;
  uint local_50;
  uint local_14;
  
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffff0;
  local_94 = (uint)*param_3;
  switch(*param_2) {
  case 1:
  case 5:
    uVar3 = 8;
    break;
  case 2:
    uVar3 = 4;
    break;
  case 3:
    uVar3 = 0x11;
    break;
  case 4:
    uVar3 = 0x12;
    break;
  default:
    goto switchD_00564145_caseD_6;
  case 8:
    uVar3 = 0x10;
  }
  bVar1 = FUN_00553b40(uVar3,(double *)(param_2 + 6),local_94);
  if (CONCAT31(extraout_var,bVar1) == 0) {
    if (((param_1 == 0x10) || (param_1 == 0x16)) || (param_1 == 0x1d)) {
      local_60 = *(undefined8 *)(param_2 + 4);
      local_50 = local_50 & 0xffffffe3 | 3;
    }
    else {
      local_50 = local_50 & 0xfffffffe;
    }
    FUN_00553e70(local_90,&local_94,uVar3,param_1,(uint *)(param_2 + 2),(uint *)(param_2 + 6));
  }
switchD_00564145_caseD_6:
  FUN_0055dbe0();
  if (((*param_2 == 8) || (bVar1 = FUN_00555910(), !bVar1)) ||
     (iVar2 = FUN_00555940(param_2), iVar2 == 0)) {
    FUN_005541d0(*param_2);
  }
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffff0);
  return;
}

