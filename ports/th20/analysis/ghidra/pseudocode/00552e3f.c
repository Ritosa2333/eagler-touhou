/* Entry: 0x00552e3f; symbol: FUN_00552e3f; body bytes: 317 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl FUN_00552e3f(int param_1)

{
  int *piVar1;
  int iVar2;
  __acrt_ptd *p_Var3;
  int *piVar4;
  byte *pbVar5;
  uint local_10;
  int *local_c;
  uint local_8;
  
  if (param_1 == 0) {
    iVar2 = 0;
  }
  else if ((param_1 == 2) || (param_1 == 1)) {
    ___acrt_initialize_multibyte();
    ___acrt_GetModuleFileNameA((HMODULE)0x0,&DAT_005e54a0,0x104);
    _DAT_005e5978 = &DAT_005e54a0;
    if ((DAT_005e5988 == (byte *)0x0) || (pbVar5 = DAT_005e5988, *DAT_005e5988 == 0)) {
      pbVar5 = &DAT_005e54a0;
    }
    local_8 = 0;
    local_10 = 0;
    FUN_00552f7c(pbVar5,(undefined4 *)0x0,(byte *)0x0,(int *)&local_8,(int *)&local_10);
    piVar4 = (int *)___acrt_allocate_buffer_for_argv(local_8,local_10,1);
    if (piVar4 == (int *)0x0) {
      p_Var3 = FUN_005516c1();
      iVar2 = 0xc;
      *(undefined4 *)p_Var3 = 0xc;
    }
    else {
      FUN_00552f7c(pbVar5,piVar4,(byte *)(piVar4 + local_8),(int *)&local_8,(int *)&local_10);
      if (param_1 != 1) {
        local_c = (int *)0x0;
        iVar2 = FUN_0055e7ae(piVar4,&local_c);
        piVar1 = local_c;
        if (iVar2 == 0) {
          _DAT_005e597c = 0;
          iVar2 = *local_c;
          while (iVar2 != 0) {
            local_c = local_c + 1;
            _DAT_005e597c = _DAT_005e597c + 1;
            iVar2 = *local_c;
          }
          local_c = (int *)0x0;
          _DAT_005e5980 = piVar1;
          FUN_00557ba0((LPVOID)0x0);
          iVar2 = 0;
        }
        else {
          FUN_00557ba0(local_c);
        }
        local_c = (int *)0x0;
        FUN_00557ba0(piVar4);
        return iVar2;
      }
      _DAT_005e597c = local_8 - 1;
      iVar2 = 0;
      _DAT_005e5980 = piVar4;
    }
    FUN_00557ba0((LPVOID)0x0);
  }
  else {
    p_Var3 = FUN_005516c1();
    iVar2 = 0x16;
    *(undefined4 *)p_Var3 = 0x16;
    FUN_005480bc();
  }
  return iVar2;
}

