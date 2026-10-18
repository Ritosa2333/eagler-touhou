/* Entry: 0x004383a0; symbol: FUN_004383a0; body bytes: 338 */

void __thiscall FUN_004383a0(void *this,void *param_1,int param_2)

{
  int iVar1;
  
  if ((*(int *)(*(int *)((int)this + 0x54) + param_2 * 4) == 0) ||
     (iVar1 = FUN_004292c0((int)this + 0x5c), iVar1 != 0)) {
    _memset(param_1,0,0x5e4);
  }
  else {
    *(undefined2 *)((int)param_1 + 0x440) = (undefined2)param_2;
                    /* WARNING: Load size is inaccurate */
    *(undefined4 *)((int)param_1 + 0x18) = *this;
                    /* WARNING: Load size is inaccurate */
    *(undefined4 *)((int)param_1 + 0x1c) = *this;
    *(byte *)((int)param_1 + 0x4a0) = *(byte *)((int)param_1 + 0x4a0) & 0x3f;
    *(byte *)((int)param_1 + 0x4a0) = *(byte *)((int)param_1 + 0x4a0) & 0xfc | 2;
    *(int *)((int)param_1 + 0x24) = param_2;
    *(undefined4 *)((int)param_1 + 0x28) = 0;
    FUN_00423520((void *)((int)param_1 + 0x4d8),0);
    FUN_00423520((void *)((int)param_1 + 0x4c8),0);
    *(byte *)((int)param_1 + 0x49a) = *(byte *)((int)param_1 + 0x49a) & 0xfe;
    *(undefined4 *)((int)param_1 + 0x5dc) = 0;
    FUN_00435500(param_1);
    *(int *)(DAT_005c0028 + 0xb8) = *(int *)(DAT_005c0028 + 0xb8) + 1;
    if ((*(byte *)((int)param_1 + 0x4a0) & 3) == 2) {
      *(byte *)((int)param_1 + 0x4a0) = *(byte *)((int)param_1 + 0x4a0) & 0xfc | 1;
    }
  }
  return;
}

