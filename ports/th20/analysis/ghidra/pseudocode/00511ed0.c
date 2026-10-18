/* Entry: 0x00511ed0; symbol: FUN_00511ed0; body bytes: 45 */

undefined4 FUN_00511ed0(void *param_1)

{
  int *piVar1;
  undefined4 *puVar2;
  
  piVar1 = (int *)FUN_004a0da0(*(int *)((int)param_1 + 0x5c8));
  puVar2 = (undefined4 *)FUN_004aadb0(piVar1);
  FUN_0045dd60(param_1,puVar2);
  return 0;
}

