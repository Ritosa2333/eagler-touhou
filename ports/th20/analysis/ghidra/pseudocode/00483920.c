/* Entry: 0x00483920; symbol: FUN_00483920; body bytes: 419 */

bool __fastcall FUN_00483920(int param_1)

{
  float fVar1;
  bool bVar2;
  void *this;
  int iVar3;
  undefined4 *puVar4;
  float10 fVar5;
  float10 fVar6;
  
  this = (void *)FUN_0047c680((void *)(param_1 + 0xa0),2);
  iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),2);
  bVar2 = FUN_004235c0(this,*(int *)(iVar3 + 0x30));
  if (!bVar2) {
    iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),2);
    fVar5 = FUN_004292e0((float *)&DAT_005aefe4);
    fVar1 = *(float *)(iVar3 + 0x14);
    fVar6 = FUN_004292e0((float *)(param_1 + 0x84));
    fVar5 = (float10)FUN_00438600((float)fVar6,(float)fVar5 * fVar1);
    FUN_00452f20((void *)(param_1 + 0x84),(float)fVar5);
    iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),2);
    fVar5 = FUN_004292e0((float *)&DAT_005aefe4);
    *(float *)(param_1 + 0x20) =
         *(float *)(param_1 + 0x20) + (float)fVar5 * *(float *)(iVar3 + 0x10);
    fVar1 = *(float *)(param_1 + 0x20);
    fVar5 = FUN_004292e0((float *)(param_1 + 0x84));
    FUN_00439330((void *)(param_1 + 0x70),(float *)(param_1 + 0x70),(float)fVar5,fVar1);
    puVar4 = (undefined4 *)FUN_0047c680((void *)(param_1 + 0xa0),2);
    FUN_00423540(puVar4);
  }
  else {
    *(uint *)(param_1 + 0x90) = *(uint *)(param_1 + 0x90) & 0xfffffff7;
  }
  return bVar2;
}

