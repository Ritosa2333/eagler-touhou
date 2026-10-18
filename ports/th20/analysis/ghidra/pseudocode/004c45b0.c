/* Entry: 0x004c45b0; symbol: FUN_004c45b0; body bytes: 147 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_004c45b0(void *this,float *param_1,int param_2,int param_3)

{
  float10 fVar1;
  float fVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  
  for (; 0 < param_2; param_2 = param_2 + -1) {
    uVar5 = 0xffffffff;
    uVar4 = 0;
    iVar3 = 0;
    fVar2 = DAT_0056c8d0;
    fVar1 = FUN_004298b0(0x5ba4a8,(DAT_0056e0f0 / _DAT_00570db0) * DAT_0056e734);
    FUN_004c3c90(this,param_3,param_1,0xffffffff,(float)fVar1 - DAT_0056e0f0 / DAT_0056c8d0,fVar2,
                 iVar3,uVar4,uVar5);
  }
  return;
}

