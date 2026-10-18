/* Entry: 0x004708e0; symbol: FUN_004708e0; body bytes: 56 */

void __thiscall FUN_004708e0(void *this,void *param_1,int param_2)

{
  FUN_004299d0((int)param_1);
  FUN_00429e30(param_1);
                    /* WARNING: Load size is inaccurate */
  *(undefined4 *)((int)param_1 + 0x1c) = *this;
  FUN_00438620(this,(int)param_1,param_2);
  return;
}

