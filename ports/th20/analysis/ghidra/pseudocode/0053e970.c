/* Entry: 0x0053e970; symbol: FUN_0053e970; body bytes: 278 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_0053e970(void *this,int param_1)

{
  code *pcVar1;
  int iVar2;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar2 = FUN_005403d0((int)this);
  if (((uint)*(ushort *)(iVar2 + 8) & 1 << ((byte)param_1 & 0x1f)) != 0) {
    if (*(float *)(iVar2 + 0x10 + param_1 * 4) < _DAT_0056e0e8) {
      if ((DAT_0056e0f4 < *(float *)(iVar2 + 0x10 + param_1 * 4)) ||
         (*(float *)(iVar2 + 0x10 + param_1 * 4) < DAT_005732dc)) {
        pcVar1 = *(code **)(**(int **)((int)this + 0x28) + 0x10);
        (*pcVar1)((int)*(float *)(iVar2 + 0x10 + param_1 * 4),pcVar1,
                  *(undefined4 *)((int)this + 0x28));
      }
      else {
        FUN_00540450((void *)((int)this + 0xc),
                     (int)(*(float *)(iVar2 + 0x10 + param_1 * 4) * DAT_0056d7bc),&local_c,'f');
      }
    }
    else {
      FUN_0053e6a0((void *)((int)this + 0xc),(int)*(float *)(iVar2 + 0x10 + param_1 * 4));
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

