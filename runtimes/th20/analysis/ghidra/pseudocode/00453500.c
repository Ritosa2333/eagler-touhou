/* Entry: 0x00453500; symbol: FUN_00453500; body bytes: 1462 */

void __cdecl FUN_00453500(char *param_1,int param_2,longlong param_3)

{
  undefined8 uVar1;
  longlong lVar2;
  longlong lVar3;
  longlong lVar4;
  longlong lVar5;
  longlong lVar6;
  undefined1 *local_2c;
  undefined1 *local_24;
  undefined1 *local_1c;
  undefined1 *local_14;
  undefined1 *local_c;
  
  if ((param_3 < 0x100000000) && (param_3 < 0)) {
    param_3 = __allmul((uint)param_3,param_3._4_4_,0xffffffff,-1);
    *param_1 = '-';
    param_1 = param_1 + 1;
    param_2 = param_2 + -1;
  }
  if (((int)param_3._4_4_ < 1) && ((param_3 < 0 || ((uint)param_3 < 1000)))) {
    uVar1 = FUN_00459240(param_1,param_1 + param_2,(uint)param_3,param_3._4_4_,10);
    local_2c = (undefined1 *)uVar1;
    *local_2c = 0;
  }
  else if (((int)param_3._4_4_ < 1) && ((param_3 < 0 || ((uint)param_3 < 1000000)))) {
    uVar1 = __alldiv((uint)param_3,param_3._4_4_,1000,0);
    lVar2 = __allmul((uint)uVar1,(int)((ulonglong)uVar1 >> 0x20),10,0);
    lVar2 = __allmul((uint)lVar2,(int)((ulonglong)lVar2 >> 0x20),1000,0);
    uVar1 = __allrem((uint)param_3,param_3._4_4_,1000,0);
    param_3._0_4_ = (int)lVar2 + (int)uVar1;
    uVar1 = FUN_00459210(param_1,param_1 + param_2,(uint)param_3,10);
    local_24 = (undefined1 *)uVar1;
    *local_24 = 0;
    local_24[-4] = 0x2c;
  }
  else if (((int)param_3._4_4_ < 1) && ((param_3 < 0 || ((uint)param_3 < 1000000000)))) {
    uVar1 = __alldiv((uint)param_3,param_3._4_4_,1000000,0);
    lVar2 = __allmul((uint)uVar1,(int)((ulonglong)uVar1 >> 0x20),100,0);
    lVar2 = __allmul((uint)lVar2,(int)((ulonglong)lVar2 >> 0x20),1000,0);
    lVar3 = __allmul((uint)lVar2,(int)((ulonglong)lVar2 >> 0x20),1000,0);
    uVar1 = __alldiv((uint)param_3,param_3._4_4_,1000,0);
    uVar1 = __allrem((uint)uVar1,(uint)((ulonglong)uVar1 >> 0x20),1000,0);
    lVar2 = __allmul((uint)uVar1,(int)((ulonglong)uVar1 >> 0x20),10,0);
    lVar4 = __allmul((uint)lVar2,(int)((ulonglong)lVar2 >> 0x20),1000,0);
    lVar2 = __allrem((uint)param_3,param_3._4_4_,1000,0);
    lVar2 = lVar3 + lVar4 + lVar2;
    param_3._4_4_ = (uint)((ulonglong)lVar2 >> 0x20);
    uVar1 = FUN_00459240(param_1,param_1 + param_2,(int)lVar2,param_3._4_4_,10);
    local_1c = (undefined1 *)uVar1;
    *local_1c = 0;
    local_1c[-4] = 0x2c;
    local_1c[-8] = 0x2c;
  }
  else if (((int)param_3._4_4_ < 0xe9) &&
          (((int)param_3._4_4_ < 0xe8 || ((uint)param_3 < 0xd4a51000)))) {
    uVar1 = __alldiv((uint)param_3,param_3._4_4_,1000000000,0);
    lVar2 = __allmul((uint)uVar1,(int)((ulonglong)uVar1 >> 0x20),1000,0);
    lVar3 = __allmul((uint)lVar2,(int)((ulonglong)lVar2 >> 0x20),1000000000,0);
    uVar1 = __alldiv((uint)param_3,param_3._4_4_,1000000,0);
    uVar1 = __allrem((uint)uVar1,(uint)((ulonglong)uVar1 >> 0x20),1000,0);
    lVar2 = __allmul((uint)uVar1,(int)((ulonglong)uVar1 >> 0x20),100,0);
    lVar4 = __allmul((uint)lVar2,(int)((ulonglong)lVar2 >> 0x20),1000000,0);
    uVar1 = __alldiv((uint)param_3,param_3._4_4_,1000,0);
    uVar1 = __allrem((uint)uVar1,(uint)((ulonglong)uVar1 >> 0x20),1000,0);
    lVar2 = __allmul((uint)uVar1,(int)((ulonglong)uVar1 >> 0x20),10,0);
    lVar5 = __allmul((uint)lVar2,(int)((ulonglong)lVar2 >> 0x20),1000,0);
    lVar2 = __allrem((uint)param_3,param_3._4_4_,1000,0);
    lVar2 = lVar3 + lVar4 + lVar5 + lVar2;
    param_3._4_4_ = (uint)((ulonglong)lVar2 >> 0x20);
    uVar1 = FUN_00459240(param_1,param_1 + param_2,(int)lVar2,param_3._4_4_,10);
    local_14 = (undefined1 *)uVar1;
    *local_14 = 0;
    local_14[-4] = 0x2c;
    local_14[-8] = 0x2c;
    local_14[-0xc] = 0x2c;
  }
  else {
    uVar1 = __alldiv((uint)param_3,param_3._4_4_,0xd4a51000,0xe8);
    uVar1 = __allrem((uint)uVar1,(uint)((ulonglong)uVar1 >> 0x20),1000,0);
    lVar2 = __allmul((uint)uVar1,(int)((ulonglong)uVar1 >> 0x20),10000,0);
    lVar3 = __allmul((uint)lVar2,(int)((ulonglong)lVar2 >> 0x20),0xd4a51000,0xe8);
    uVar1 = __alldiv((uint)param_3,param_3._4_4_,1000000000,0);
    uVar1 = __allrem((uint)uVar1,(uint)((ulonglong)uVar1 >> 0x20),1000,0);
    lVar2 = __allmul((uint)uVar1,(int)((ulonglong)uVar1 >> 0x20),1000,0);
    lVar4 = __allmul((uint)lVar2,(int)((ulonglong)lVar2 >> 0x20),1000000000,0);
    uVar1 = __alldiv((uint)param_3,param_3._4_4_,1000000,0);
    uVar1 = __allrem((uint)uVar1,(uint)((ulonglong)uVar1 >> 0x20),1000,0);
    lVar2 = __allmul((uint)uVar1,(int)((ulonglong)uVar1 >> 0x20),100,0);
    lVar5 = __allmul((uint)lVar2,(int)((ulonglong)lVar2 >> 0x20),1000000,0);
    uVar1 = __alldiv((uint)param_3,param_3._4_4_,1000,0);
    uVar1 = __allrem((uint)uVar1,(uint)((ulonglong)uVar1 >> 0x20),1000,0);
    lVar2 = __allmul((uint)uVar1,(int)((ulonglong)uVar1 >> 0x20),10,0);
    lVar6 = __allmul((uint)lVar2,(int)((ulonglong)lVar2 >> 0x20),1000,0);
    lVar2 = __allrem((uint)param_3,param_3._4_4_,1000,0);
    lVar2 = lVar3 + lVar4 + lVar5 + lVar6 + lVar2;
    param_3._4_4_ = (uint)((ulonglong)lVar2 >> 0x20);
    uVar1 = FUN_00459240(param_1,param_1 + param_2,(int)lVar2,param_3._4_4_,10);
    local_c = (undefined1 *)uVar1;
    *local_c = 0;
    local_c[-4] = 0x2c;
    local_c[-8] = 0x2c;
    local_c[-0xc] = 0x2c;
    local_c[-0x10] = 0x2c;
  }
  return;
}

