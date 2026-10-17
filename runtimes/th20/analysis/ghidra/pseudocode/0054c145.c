/* Entry: 0x0054c145; symbol: FUN_0054c145; body bytes: 376 */

uint __thiscall FUN_0054c145(void *this,int param_1)

{
  undefined4 *puVar1;
  uint uVar2;
  uint uVar3;
  ushort uVar4;
  undefined4 uVar5;
  __acrt_ptd *p_Var6;
  int iVar7;
  bool bVar8;
  uint local_24;
  uint uStack_20;
  __acrt_ptd *local_1c;
  int local_8;
  
  p_Var6 = (__acrt_ptd *)0x0;
  if (*(char *)((int)this + 0x30) == '\0') {
    puVar1 = *(undefined4 **)((int)this + 0x54);
    *(undefined4 **)((int)this + 0x54) = puVar1 + 1;
    p_Var6 = (__acrt_ptd *)*puVar1;
    if (p_Var6 == (__acrt_ptd *)0x0) {
      p_Var6 = FUN_005516c1();
      *(undefined4 *)p_Var6 = 0x16;
      p_Var6 = (__acrt_ptd *)FUN_005480bc();
      goto LAB_0054c17a;
    }
                    /* WARNING: Load size is inaccurate */
    if ((*this & 1) == 0) goto LAB_0054c1be;
    *(undefined4 **)((int)this + 0x54) = puVar1 + 2;
    iVar7 = puVar1[1];
    if (iVar7 == 0) {
                    /* WARNING: Load size is inaccurate */
      if ((*this & 4) != 0) {
        __crt_stdio_input::string_input_adapter<wchar_t>::get
                  ((string_input_adapter<wchar_t> *)((int)this + 8));
        *(ushort *)p_Var6 = 0;
      }
LAB_0054c1b1:
      p_Var6 = FUN_005516c1();
      *(undefined4 *)p_Var6 = 0xc;
      goto LAB_0054c17a;
    }
  }
  else {
LAB_0054c1be:
    iVar7 = -1;
  }
  uVar2 = *(uint *)((int)this + 0x38);
  uVar3 = *(uint *)((int)this + 0x3c);
  local_8 = iVar7;
  if ((param_1 != 0) && (iVar7 != -1)) {
    local_8 = iVar7 + -1;
  }
  uStack_20 = 0;
  local_24 = 0;
  local_1c = p_Var6;
  while( true ) {
    if (((uVar2 != 0 || uVar3 != 0) && (local_24 == uVar2)) && (uStack_20 == uVar3))
    goto LAB_0054c27a;
    uVar4 = __crt_stdio_input::string_input_adapter<wchar_t>::get
                      ((string_input_adapter<wchar_t> *)((int)this + 8));
    uVar5 = FUN_0054ef3c(this,param_1,uVar4);
    if ((char)uVar5 == '\0') break;
    if (*(char *)((int)this + 0x30) == '\0') {
      if (local_8 == 0) {
        if (iVar7 != -1) {
          *(ushort *)p_Var6 = 0;
        }
        goto LAB_0054c1b1;
      }
      *(ushort *)local_1c = uVar4;
      local_1c = local_1c + 2;
      local_8 = local_8 + -1;
    }
    bVar8 = 0xfffffffe < local_24;
    local_24 = local_24 + 1;
    uStack_20 = uStack_20 + bVar8;
  }
  __crt_stdio_input::string_input_adapter<wchar_t>::unget
            ((string_input_adapter<wchar_t> *)((int)this + 8),uVar4);
LAB_0054c27a:
  p_Var6 = (__acrt_ptd *)(local_24 | uStack_20);
                    /* WARNING: Load size is inaccurate */
  if ((p_Var6 != (__acrt_ptd *)0x0) &&
     (((param_1 != 0 || ((local_24 == uVar2 && (uStack_20 == uVar3)))) ||
      (p_Var6 = (__acrt_ptd *)(*this & 4), p_Var6 != (__acrt_ptd *)0x0)))) {
    if ((*(char *)((int)this + 0x30) == '\0') && (param_1 != 0)) {
      *(ushort *)local_1c = 0;
      p_Var6 = local_1c;
    }
    return CONCAT31((int3)((uint)p_Var6 >> 8),1);
  }
LAB_0054c17a:
  return (uint)p_Var6 & 0xffffff00;
}

