/* Entry: 0x0054c44c; symbol: FUN_0054c44c; body bytes: 130 */

int * __thiscall FUN_0054c44c(void *this,int *param_1)

{
  int *piVar1;
  uint uVar2;
  undefined *puVar3;
  int iVar4;
  undefined *puVar5;
  
  *(undefined1 *)((int)this + 0xc) = 0;
  piVar1 = (int *)((int)this + 4);
  if (param_1 == (int *)0x0) {
    puVar3 = PTR_PTR_005b2840;
    puVar5 = PTR_DAT_005b2844;
    if (DAT_005e55dc != 0) {
      iVar4 = FUN_005579c2();
      *(int *)this = iVar4;
      *piVar1 = *(int *)(iVar4 + 0x4c);
      *(int *)((int)this + 8) = *(int *)(iVar4 + 0x48);
      ___acrt_update_locale_info(iVar4,piVar1);
                    /* WARNING: Load size is inaccurate */
      FUN_0055856f(*this,(int *)((int)this + 8));
                    /* WARNING: Load size is inaccurate */
      uVar2 = *(uint *)(*this + 0x350);
      if ((uVar2 & 2) != 0) {
        return (int *)this;
      }
      *(uint *)(*this + 0x350) = uVar2 | 2;
      *(undefined1 *)((int)this + 0xc) = 1;
      return (int *)this;
    }
  }
  else {
    puVar3 = (undefined *)*param_1;
    puVar5 = (undefined *)param_1[1];
  }
  *piVar1 = (int)puVar3;
  *(undefined **)((int)this + 8) = puVar5;
  return (int *)this;
}

