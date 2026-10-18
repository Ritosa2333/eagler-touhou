/* Entry: 0x004d5580; symbol: FUN_004d5580; body bytes: 381 */

bool __fastcall FUN_004d5580(int param_1)

{
  bool bVar1;
  void *this;
  int iVar2;
  undefined4 *puVar3;
  float10 fVar4;
  
  this = (void *)FUN_0047c680((void *)(param_1 + 0x90),2);
  iVar2 = FUN_0047c680((void *)(param_1 + 0x90),2);
  bVar1 = FUN_004235c0(this,*(int *)(iVar2 + 0x30));
  if (!bVar1) {
    iVar2 = FUN_0047c680((void *)(param_1 + 0x90),2);
    fVar4 = FUN_004292e0((float *)&DAT_005aefe4);
    fVar4 = (float10)FUN_00438600(*(float *)(param_1 + 0x70),(float)fVar4 * *(float *)(iVar2 + 0x14)
                                 );
    *(float *)(param_1 + 0x70) = (float)fVar4;
    iVar2 = FUN_0047c680((void *)(param_1 + 0x90),2);
    fVar4 = FUN_004292e0((float *)&DAT_005aefe4);
    *(float *)(param_1 + 0x7c) =
         *(float *)(param_1 + 0x7c) + (float)fVar4 * *(float *)(iVar2 + 0x10);
    FUN_00439330((void *)(param_1 + 100),(float *)(param_1 + 100),*(float *)(param_1 + 0x70),
                 *(float *)(param_1 + 0x7c));
    puVar3 = (undefined4 *)FUN_0047c680((void *)(param_1 + 0x90),2);
    FUN_00423540(puVar3);
  }
  else {
    *(uint *)(param_1 + 0x698) = *(uint *)(param_1 + 0x698) & 0xfffffff7;
  }
  return bVar1;
}

