/* Entry: 0x00542848; symbol: ___scrt_initialize_onexit_tables; body bytes: 135 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Library Function - Single Match
    ___scrt_initialize_onexit_tables
   
   Library: Visual Studio 2019 Release */

undefined4 __cdecl ___scrt_initialize_onexit_tables(int param_1)

{
  code *pcVar1;
  bool bVar2;
  undefined4 in_EAX;
  undefined3 extraout_var;
  uint uVar3;
  undefined4 uVar4;
  
  if (DAT_005e4f4d != '\0') {
    return CONCAT31((int3)((uint)in_EAX >> 8),1);
  }
  if ((param_1 != 0) && (param_1 != 1)) {
    FUN_00543a39();
    pcVar1 = (code *)swi(3);
    uVar4 = (*pcVar1)();
    return uVar4;
  }
  bVar2 = ___scrt_is_ucrt_dll_in_use();
  uVar3 = CONCAT31(extraout_var,bVar2);
  if ((uVar3 == 0) || (param_1 != 0)) {
    DAT_005e4f50 = 0xffffffff;
    _DAT_005e4f54 = 0xffffffff;
    _DAT_005e4f58 = 0xffffffff;
    _DAT_005e4f5c = 0xffffffff;
    _DAT_005e4f60 = 0xffffffff;
    _DAT_005e4f64 = 0xffffffff;
LAB_005428bb:
    DAT_005e4f4d = '\x01';
    uVar3 = CONCAT31((int3)(uVar3 >> 8),1);
  }
  else {
    uVar3 = __initialize_onexit_table(&DAT_005e4f50);
    if (uVar3 == 0) {
      uVar3 = __initialize_onexit_table((int *)&DAT_005e4f5c);
      if (uVar3 == 0) goto LAB_005428bb;
    }
    uVar3 = uVar3 & 0xffffff00;
  }
  return uVar3;
}

