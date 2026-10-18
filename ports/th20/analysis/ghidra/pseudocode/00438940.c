/* Entry: 0x00438940; symbol: FUN_00438940; body bytes: 226 */

undefined4 __thiscall FUN_00438940(void *this,void *param_1,int param_2)

{
  undefined4 uVar1;
  
  if (*(int *)(*(int *)((int)this + 0x54) + param_2 * 4) == 0) {
    _memset(param_1,0,0x5e4);
    uVar1 = 0xffffffff;
  }
  else {
    FUN_004299d0((int)param_1);
    FUN_00429e30(param_1);
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
    uVar1 = 0;
  }
  return uVar1;
}

