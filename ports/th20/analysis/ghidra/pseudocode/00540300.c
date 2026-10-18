/* Entry: 0x00540300; symbol: FUN_00540300; body bytes: 52 */

undefined4 __fastcall FUN_00540300(void *param_1)

{
  undefined4 uVar1;
  
  uVar1 = *(undefined4 *)((int)param_1 + 0x14);
  FUN_0053f0b0(param_1,4,(float *)((int)param_1 + 0x14),'\0');
  *(undefined4 *)((int)param_1 + 0x10) = uVar1;
  return 0;
}

