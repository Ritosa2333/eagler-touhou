/* Entry: 0x005403d0; symbol: FUN_005403d0; body bytes: 80 */

int __fastcall FUN_005403d0(int param_1)

{
  undefined4 local_c;
  
  if ((*(int *)(param_1 + 8) == -1) || (*(int *)(param_1 + 4) == -1)) {
    local_c = 0;
  }
  else {
    local_c = FUN_0053e8e0(*(void **)(*(int *)(param_1 + 0x28) + 0x58),*(int *)(param_1 + 4),
                           *(int *)(param_1 + 8));
  }
  return local_c;
}

