/* Entry: 0x004e6600; symbol: FUN_004e6600; body bytes: 149 */

void __thiscall
FUN_004e6600(void *this,undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
            undefined4 param_5)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  int iVar5;
  int iVar6;
  undefined4 uVar7;
  
  iVar5 = FUN_0044ced0(&param_1);
  iVar6 = FUN_00437cd0(iVar5);
  fVar1 = *(float *)(iVar6 + 0x48);
  fVar2 = *(float *)(iVar6 + 0x4c);
  fVar3 = *(float *)(iVar6 + 0x10);
  fVar4 = *(float *)(iVar6 + 0xc);
  uVar7 = FUN_004e6740(iVar5);
  FUN_004e6530(this,*(undefined4 *)(iVar5 + 0x1c),uVar7,param_2,param_3,param_4,param_5,(int)fVar4,
               (int)fVar3,(int)fVar2,(int)fVar1);
  return;
}

