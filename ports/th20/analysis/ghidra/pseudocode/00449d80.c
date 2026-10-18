/* Entry: 0x00449d80; symbol: FUN_00449d80; body bytes: 188 */

void __fastcall FUN_00449d80(undefined4 *param_1)

{
  int local_34;
  void *local_30;
  int *local_2c;
  undefined1 local_28 [28];
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  (**(code **)(*(int *)*param_1 + 0x48))(*param_1,0,&local_2c);
  if (local_2c != (int *)0x0) {
    (**(code **)(*local_2c + 0x30))(local_2c,local_28);
    (**(code **)(*local_2c + 0x34))(local_2c,&local_34,0,0);
    _memset(local_30,0,local_34 * local_c);
    (**(code **)(*local_2c + 0x38))(local_2c);
    if (local_2c != (int *)0x0) {
      (**(code **)(*local_2c + 8))(local_2c);
      local_2c = (int *)0x0;
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

