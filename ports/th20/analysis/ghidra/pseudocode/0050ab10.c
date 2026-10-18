/* Entry: 0x0050ab10; symbol: FUN_0050ab10; body bytes: 189 */

void FUN_0050ab10(undefined4 param_1,undefined4 param_2,undefined4 *param_3)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  int *unaff_FS_OFFSET;
  undefined1 local_38 [12];
  undefined4 local_2c [3];
  undefined4 *local_20;
  undefined4 *local_1c;
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056a85d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = FUN_0040c310(&DAT_005c0240,6);
  FUN_0040b730(&local_18,uVar1);
  local_8 = 0;
  FUN_0040bc60(local_1c);
  FUN_0040baa0(local_1c + 3,0);
  puVar2 = FUN_0046a570(local_38,&param_1,&param_2,param_3);
  local_20 = FUN_0050aa60(local_2c,puVar2);
  FUN_0040bac0(local_1c,(int)local_20);
  FUN_0040b9d0(local_2c);
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

