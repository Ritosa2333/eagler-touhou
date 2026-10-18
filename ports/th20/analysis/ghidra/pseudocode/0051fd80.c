/* Entry: 0x0051fd80; symbol: FUN_0051fd80; body bytes: 291 */

undefined4 __thiscall FUN_0051fd80(void *this,int param_1)

{
  bool bVar1;
  undefined4 *puVar2;
  undefined3 extraout_var;
  void *pvVar3;
  undefined3 extraout_var_00;
  int local_8;
  
  for (local_8 = 0; local_8 < 5; local_8 = local_8 + 1) {
    if (local_8 == param_1) {
      puVar2 = (undefined4 *)FUN_00414580((void *)((int)this + 0x394),local_8);
      bVar1 = FUN_00485a10(puVar2);
      if (CONCAT31(extraout_var,bVar1) != 0) {
        pvVar3 = (void *)FUN_00414580((void *)((int)this + 0x394),local_8);
        FUN_00486310(pvVar3);
      }
      if (-1 < *(int *)((int)this + local_8 * 4 + 0x58e4)) {
        FUN_0052cbf0(this,*(int *)(&DAT_005af058 + *(int *)((int)this + local_8 * 4 + 0x58e4) * 4) +
                          0x7f);
      }
    }
    else {
      puVar2 = (undefined4 *)FUN_00414580((void *)((int)this + 0x394),local_8);
      bVar1 = FUN_00485a10(puVar2);
      if (CONCAT31(extraout_var_00,bVar1) != 0) {
        pvVar3 = (void *)FUN_00414580((void *)((int)this + 0x394),local_8);
        FUN_00486350(pvVar3);
      }
      if (-1 < *(int *)((int)this + local_8 * 4 + 0x58e4)) {
        FUN_0052cc90(this,*(int *)(&DAT_005af058 + *(int *)((int)this + local_8 * 4 + 0x58e4) * 4) +
                          0x7f);
      }
    }
  }
  return 0;
}

