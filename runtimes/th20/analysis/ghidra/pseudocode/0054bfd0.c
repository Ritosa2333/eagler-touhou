/* Entry: 0x0054bfd0; symbol: FUN_0054bfd0; body bytes: 373 */

uint __thiscall FUN_0054bfd0(void *this,int param_1)

{
  undefined4 *puVar1;
  ushort uVar2;
  __acrt_ptd *p_Var3;
  undefined4 uVar4;
  undefined1 *puVar5;
  uint uVar6;
  bool bVar7;
  WCHAR WVar8;
  undefined1 *local_24;
  undefined8 local_20;
  uint local_18;
  uint local_14;
  uint local_10;
  uint local_c;
  uint local_8;
  
  puVar5 = (undefined1 *)0x0;
  if (*(char *)((int)this + 0x30) == '\0') {
    puVar1 = *(undefined4 **)((int)this + 0x54);
    *(undefined4 **)((int)this + 0x54) = puVar1 + 1;
    puVar5 = (undefined1 *)*puVar1;
    if (puVar5 == (undefined1 *)0x0) {
      p_Var3 = FUN_005516c1();
      *(undefined4 *)p_Var3 = 0x16;
      p_Var3 = (__acrt_ptd *)FUN_005480bc();
      goto LAB_0054c005;
    }
                    /* WARNING: Load size is inaccurate */
    if ((*this & 1) == 0) goto LAB_0054c047;
    *(undefined4 **)((int)this + 0x54) = puVar1 + 2;
    uVar6 = puVar1[1];
    if (uVar6 == 0) {
                    /* WARNING: Load size is inaccurate */
      if ((*this & 4) != 0) {
        __crt_stdio_input::string_input_adapter<wchar_t>::get
                  ((string_input_adapter<wchar_t> *)((int)this + 8));
        *puVar5 = 0;
      }
LAB_0054c03a:
      p_Var3 = FUN_005516c1();
      *(undefined4 *)p_Var3 = 0xc;
      goto LAB_0054c005;
    }
  }
  else {
LAB_0054c047:
    uVar6 = 0xffffffff;
  }
  local_14 = *(uint *)((int)this + 0x38);
  local_18 = *(uint *)((int)this + 0x3c);
  local_10 = uVar6;
  if ((param_1 != 0) && (uVar6 != 0xffffffff)) {
    local_10 = uVar6 - 1;
  }
  local_20 = 0;
  local_20._4_4_ = 0;
  local_20._0_4_ = 0;
  local_8 = local_20._4_4_;
  local_c = (uint)local_20;
  local_24 = puVar5;
  while( true ) {
    if (((local_14 != 0 || local_18 != 0) && (local_c == local_14)) && (local_8 == local_18))
    goto LAB_0054c104;
    uVar2 = __crt_stdio_input::string_input_adapter<wchar_t>::get
                      ((string_input_adapter<wchar_t> *)((int)this + 8));
    local_20 = (ulonglong)CONCAT24(uVar2,(uint)local_20);
    uVar4 = FUN_0054ef3c(this,param_1,uVar2);
    WVar8 = (WCHAR)(local_20 >> 0x20);
    if ((char)uVar4 == '\0') break;
    if (*(char *)((int)this + 0x30) == '\0') {
      if (local_10 == 0) {
        if (uVar6 != 0xffffffff) {
          *puVar5 = 0;
        }
        goto LAB_0054c03a;
      }
      uVar4 = write_character(puVar5,uVar6,(int *)&local_24,&local_10,WVar8);
      if ((char)uVar4 == '\0') goto LAB_0054c104;
    }
    bVar7 = 0xfffffffe < local_c;
    local_c = local_c + 1;
    local_8 = local_8 + bVar7;
  }
  __crt_stdio_input::string_input_adapter<wchar_t>::unget
            ((string_input_adapter<wchar_t> *)((int)this + 8),WVar8);
LAB_0054c104:
  p_Var3 = (__acrt_ptd *)(local_c | local_8);
                    /* WARNING: Load size is inaccurate */
  if ((p_Var3 != (__acrt_ptd *)0x0) &&
     (((param_1 != 0 || ((local_c == local_14 && (local_8 == local_18)))) ||
      (p_Var3 = (__acrt_ptd *)(*this & 4), p_Var3 != (__acrt_ptd *)0x0)))) {
    if ((*(char *)((int)this + 0x30) == '\0') && (param_1 != 0)) {
      *local_24 = 0;
    }
    return CONCAT31((int3)((uint)p_Var3 >> 8),1);
  }
LAB_0054c005:
  return (uint)p_Var3 & 0xffffff00;
}

