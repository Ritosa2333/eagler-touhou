/* Entry: 0x0044cf50; symbol: FUN_0044cf50; body bytes: 95 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

float10 __thiscall FUN_0044cf50(int param_1,int param_2)

{
  undefined8 *puVar1;
  double in_XMM0_Qa;
  undefined8 uVar2;
  undefined1 local_20 [8];
  undefined1 local_18 [16];
  int local_8;
  
  local_8 = param_1;
  puVar1 = (undefined8 *)
           FUN_00446e00(local_18,(COleCurrency *)(param_1 + 0x90 + param_2 * 8),
                        (COleCurrency *)(param_1 + 0x50 + param_2 * 8));
  puVar1 = (undefined8 *)FUN_00447870(local_20,puVar1);
  uVar2 = FUN_0044c090(puVar1);
  FUN_00542e00((uint)uVar2,(int)((ulonglong)uVar2 >> 0x20));
  return (float10)(in_XMM0_Qa / _DAT_0056e640);
}

