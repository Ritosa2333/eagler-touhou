/* Entry: 0x00470620; symbol: FUN_00470620; body bytes: 78 */

int __fastcall FUN_00470620(int param_1)

{
  undefined4 local_c;
  
  if (*(int *)(param_1 + 0x1a1bc) < 0x140) {
    local_c = FUN_0046b210((void *)(param_1 + 0x11bc),*(int *)(param_1 + 0x1a1bc));
  }
  else {
    local_c = param_1 + 0x1a260;
  }
  return local_c;
}

