/* Entry: 0x004840e0; symbol: FUN_004840e0; body bytes: 267 */

bool __fastcall FUN_004840e0(int param_1)

{
  float fVar1;
  bool bVar2;
  void *this;
  int iVar3;
  undefined4 *puVar4;
  float10 fVar5;
  float fVar6;
  
  this = (void *)FUN_0047c680((void *)(param_1 + 0xa0),0);
  bVar2 = FUN_00423590(this,0x10);
  if (!bVar2) {
    *(uint *)(param_1 + 0x90) = *(uint *)(param_1 + 0x90) & 0xfffffffe;
  }
  else {
    iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),0);
    fVar5 = FUN_00423bd0(iVar3);
    fVar6 = DAT_0056edd4 - ((float)fVar5 * DAT_0056edd4) / DAT_0056cd90;
    fVar1 = *(float *)(param_1 + 0x20);
    fVar5 = FUN_004292e0((float *)(param_1 + 0x84));
    FUN_00439330((void *)(param_1 + 0x70),(float *)(param_1 + 0x70),(float)fVar5,fVar6 + fVar1);
    puVar4 = (undefined4 *)FUN_0047c680((void *)(param_1 + 0xa0),0);
    FUN_00423540(puVar4);
  }
  return !bVar2;
}

