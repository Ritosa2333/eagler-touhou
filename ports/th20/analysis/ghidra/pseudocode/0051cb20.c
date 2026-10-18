/* Entry: 0x0051cb20; symbol: FUN_0051cb20; body bytes: 87 */

/* WARNING: Removing unreachable block (ram,0x0051cb3f) */

undefined1 __fastcall FUN_0051cb20(int *param_1)

{
  undefined1 *puVar1;
  longlong lVar2;
  undefined1 local_c;
  
  lVar2 = Gnavail();
  if (lVar2 < 1) {
    local_c = (**(code **)(*param_1 + 0x18))();
  }
  else {
    puVar1 = (undefined1 *)FUN_0051bab0((int)param_1);
    local_c = FUN_004ed5a0(*puVar1);
  }
  return local_c;
}

