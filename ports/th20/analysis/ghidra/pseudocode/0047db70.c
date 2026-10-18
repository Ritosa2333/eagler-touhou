/* Entry: 0x0047db70; symbol: FUN_0047db70; body bytes: 273 */

undefined4 __thiscall FUN_0047db70(void *this,int param_1)

{
  int iVar1;
  void *this_00;
  float10 fVar2;
  float fVar3;
  int local_8;
  
  for (local_8 = *(int *)((int)this + param_1 * 4 + 0x14); local_8 != 0;
      local_8 = *(int *)(local_8 + 0x60)) {
    iVar1 = *(int *)(local_8 + 0x1c);
    *(undefined4 *)(iVar1 + 0x5bc) = *(undefined4 *)(local_8 + 100);
    *(undefined4 *)(iVar1 + 0x5c0) = *(undefined4 *)(local_8 + 0x68);
    *(undefined4 *)(iVar1 + 0x5c4) = *(undefined4 *)(local_8 + 0x6c);
    if (*(byte *)(*(int *)(local_8 + 0x1c) + 0x4a2) >> 5 != 0) {
      this_00 = *(void **)(local_8 + 0x1c);
      fVar3 = DAT_0056e0f0 / DAT_0056c8d0;
      fVar2 = FUN_004292e0((float *)(local_8 + 0x84));
      fVar2 = (float10)FUN_00438600((float)fVar2,fVar3);
      FUN_00450590(this_00,(float)fVar2);
    }
    if ((*(uint *)(local_8 + 0x14) >> 6 & 1) != 0) {
      FUN_004864f0(*(void **)(local_8 + 0x1c),*(undefined4 *)(local_8 + 0x48),
                   *(undefined4 *)(local_8 + 0x48));
    }
    FUN_00443880(DAT_005c0028,*(void **)(local_8 + 0x1c));
  }
  return 1;
}

