/* Entry: 0x0053e7c0; symbol: FUN_0053e7c0; body bytes: 138 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __thiscall FUN_0053e7c0(void *this,int param_1,int param_2,int param_3)

{
  code *pcVar1;
  undefined4 uVar2;
  
  if (((uint)*(ushort *)(param_1 + 8) & 1 << ((byte)param_3 & 0x1f)) == 0) {
    uVar2 = 0;
  }
  else if (*(float *)(param_1 + 0x10 + param_3 * 4) < _DAT_0056e0e8) {
    pcVar1 = *(code **)(**(int **)((int)this + 0x28) + 0x14);
    uVar2 = (*pcVar1)((int)*(float *)(param_1 + 0x10 + param_3 * 4),pcVar1,
                      *(undefined4 *)((int)this + 0x28));
  }
  else {
    uVar2 = FUN_0053e630((void *)((int)this + 0xc),
                         (int)*(float *)(param_1 + 0x10 + param_3 * 4) + param_2);
  }
  return uVar2;
}

