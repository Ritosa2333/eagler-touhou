/* Entry: 0x004c7dd0; symbol: FUN_004c7dd0; body bytes: 301 */

void __thiscall FUN_004c7dd0(void *this,int param_1,int param_2)

{
  bool bVar1;
  int *piVar2;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  DName *this_00;
  int iVar3;
  int local_14;
  int local_10;
  int local_8;
  
  piVar2 = (int *)FUN_004c5e20(DAT_005b8898,*(int *)((int)this + 0x10c));
  bVar1 = FUN_004c5e50(piVar2);
  if (CONCAT31(extraout_var,bVar1) == 0) {
    piVar2 = (int *)FUN_004c5e20(DAT_005b8898,*(int *)((int)this + 0x10c));
    bVar1 = FUN_004c5eb0(piVar2);
    if (CONCAT31(extraout_var_00,bVar1) == 0) {
      local_8 = (int)this + 0xa0;
    }
    else {
      local_8 = (int)this + 0x80;
    }
  }
  else {
    local_8 = (int)this + 0x60;
  }
  if (*(short *)(local_8 + param_1 * 2) != param_2) {
    local_10 = 0;
    while( true ) {
      this_00 = (DName *)FUN_004c5e20(DAT_005b8898,*(int *)((int)this + 0x10c));
      iVar3 = DName::isEmpty(this_00);
      if (iVar3 == 0) {
        local_14 = 4;
      }
      else {
        local_14 = 8;
      }
      if (local_14 <= local_10) break;
      if ((local_10 != param_1) && (*(short *)(local_8 + local_10 * 2) == param_2)) {
        *(undefined2 *)(local_8 + local_10 * 2) = *(undefined2 *)(local_8 + param_1 * 2);
      }
      local_10 = local_10 + 1;
    }
    *(undefined2 *)(local_8 + param_1 * 2) = (undefined2)param_2;
    FUN_00426d70(&DAT_005ba830,7,0);
    FUN_00423520((void *)((int)this + 0xec),8);
  }
  return;
}

