/* Entry: 0x00474e70; symbol: FUN_00474e70; body bytes: 555 */

undefined4 __thiscall FUN_00474e70(void *this,int param_1)

{
  undefined1 (*pauVar1) [16];
  undefined4 uVar2;
  SIZE_T *pSVar3;
  int iVar4;
  int local_c;
  
  if (*(int *)((int)this + 0x3360) == 0) {
    iVar4 = 0;
    pSVar3 = (SIZE_T *)((int)this + 0x3364);
    pauVar1 = (undefined1 (*) [16])FUN_004bd4a0(param_1);
    uVar2 = FUN_00410aa0(pauVar1,pSVar3,iVar4);
    *(undefined4 *)((int)this + 0x3360) = uVar2;
    if (*(int *)((int)this + 0x3360) == 0) {
      return 0xffffffff;
    }
  }
  uVar2 = FUN_0041f610(*(SIZE_T *)((int)this + 0x3364));
  *(undefined4 *)((int)this + 0x3324) = uVar2;
  FUN_00543e20(*(uint **)((int)this + 0x3324),*(uint **)((int)this + 0x3360),
               *(uint *)((int)this + 0x3364));
  iVar4 = FUN_0044eb30(DAT_005c0028,(*(uint *)((int)this + 0x3358) & 1) + 3,
                       (char **)(*(int *)((int)this + 0x3324) + 0x10));
  *(int *)((int)this + 0x3334) = iVar4;
  if (iVar4 == 0) {
    FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056f91c);
    uVar2 = 0xffffffff;
  }
  else {
    uVar2 = move<>(*(undefined4 *)((int)this + 0x3328),*(int *)((int)this + 0x3324) + 0x90);
    *(undefined4 *)((int)this + 0x3328) = uVar2;
    uVar2 = move<>(*(undefined4 *)((int)this + 0x332c),
                   *(int *)(*(int *)((int)this + 0x3324) + 4) + *(int *)((int)this + 0x3324));
    *(undefined4 *)((int)this + 0x332c) = uVar2;
    uVar2 = move<>(*(undefined4 *)((int)this + 0x3330),
                   *(int *)(*(int *)((int)this + 0x3324) + 8) + *(int *)((int)this + 0x3324));
    *(undefined4 *)((int)this + 0x3330) = uVar2;
    for (local_c = 0; local_c < **(short **)((int)this + 0x3324); local_c = local_c + 1) {
      *(int *)(*(int *)((int)this + 0x3328) + local_c * 4) =
           *(int *)(*(int *)((int)this + 0x3328) + local_c * 4) + *(int *)((int)this + 0x3324);
    }
    uVar2 = FUN_0041f610(*(short *)(*(int *)((int)this + 0x3324) + 2) * 0x5e4);
    *(undefined4 *)((int)this + 0x3320) = uVar2;
    _memset(*(void **)((int)this + 0x3320),0,*(short *)(*(int *)((int)this + 0x3324) + 2) * 0x5e4);
    uVar2 = 0;
  }
  return uVar2;
}

