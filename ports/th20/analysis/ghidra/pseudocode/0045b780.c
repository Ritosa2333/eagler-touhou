/* Entry: 0x0045b780; symbol: FUN_0045b780; body bytes: 192 */

void __thiscall FUN_0045b780(void *this,int *param_1,undefined4 *param_2)

{
  int iVar1;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (param_1 != (int *)0x0) {
    if (param_2 != (undefined4 *)0x0) {
      *param_2 = 0;
    }
    iVar1 = (**(code **)(*param_1 + 0x24))(param_1,&local_c,this);
    if ((-1 < iVar1) && ((local_c & 2) != 0)) {
      do {
        iVar1 = (**(code **)(*param_1 + 0x50))(param_1);
        if (iVar1 == -0x7787ff6a) {
          Sleep(10);
        }
        iVar1 = (**(code **)(*param_1 + 0x50))(param_1);
      } while (iVar1 != 0);
      if (param_2 != (undefined4 *)0x0) {
        *param_2 = 1;
      }
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

