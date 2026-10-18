/* Entry: 0x00461140; symbol: FUN_00461140; body bytes: 462 */

void __fastcall FUN_00461140(int param_1)

{
  void *pvVar1;
  float fVar2;
  float10 fVar3;
  float10 fVar4;
  float fVar5;
  float fVar6;
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  fVar3 = FUN_004379c0(*(int *)(param_1 + 4));
  fVar2 = *(float *)(param_1 + 0x14);
  fVar4 = FUN_00438290(0x5b6758);
  fVar5 = (float)fVar3 * fVar2 * (float)fVar4 * DAT_0056e0ec;
  fVar3 = FUN_004379a0(*(int *)(param_1 + 4));
  fVar2 = *(float *)(param_1 + 0x18);
  fVar4 = FUN_00438290(0x5b6758);
  fVar6 = (float)fVar3 * fVar2 * (float)fVar4 * DAT_0056e0ec;
  FUN_00422e10(&local_14);
  FUN_00445bc0(DAT_005c0028,*(int *)(param_1 + 4));
  FUN_004376e0(*(void **)(param_1 + 4),&local_14);
  pvVar1 = (void *)FUN_0040c300((undefined4 *)(param_1 + 0x20));
  fVar2 = (float)FUN_0040c300((undefined4 *)(param_1 + 0x20));
  FUN_0043a2b0(DAT_005c0028,local_14,local_10,fVar5,fVar6,DAT_0056d7bc,0.0,(void *)0x40,fVar2,pvVar1
              );
  FUN_0043ab20(DAT_005c0028,local_14,local_10,fVar5 - DAT_0056d7bc,fVar6 - DAT_0056d7bc,DAT_0056d7bc
               ,DAT_0056f00c,0.0,0x20,-NAN,-NAN);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

