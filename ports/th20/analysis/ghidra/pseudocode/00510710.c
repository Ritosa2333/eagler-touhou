/* Entry: 0x00510710; symbol: FUN_00510710; body bytes: 255 */

void __thiscall FUN_00510710(void *this,undefined4 *param_1,int param_2,undefined4 param_3)

{
  undefined1 *puVar1;
  int local_8;
  
  if (9 < *(int *)((int)this + 0x14)) {
    *(undefined4 *)((int)this + 0x14) = 0;
  }
  puVar1 = (undefined1 *)((int)this + *(int *)((int)this + 0x14) * 0x44 + 0x600);
  puVar1[0x38] = 1;
  local_8 = 0;
  if (param_2 < 0) {
    *puVar1 = 10;
    local_8 = 1;
  }
  else {
    for (; param_2 != 0; param_2 = param_2 / 10) {
      puVar1[local_8] = (char)(param_2 % 10);
      local_8 = local_8 + 1;
    }
  }
  if (local_8 == 0) {
    *puVar1 = 0;
    local_8 = 1;
  }
  puVar1[0x39] = (undefined1)local_8;
  *(undefined4 *)(puVar1 + 0x1c) = param_3;
  FUN_00423520(puVar1 + 0x20,0);
  *(undefined4 *)(puVar1 + 0xc) = *param_1;
  *(undefined4 *)(puVar1 + 0x10) = param_1[1];
  *(undefined4 *)(puVar1 + 0x14) = param_1[2];
  *(undefined4 *)(puVar1 + 0x18) = DAT_0056c8cc;
  *(int *)((int)this + 0x14) = *(int *)((int)this + 0x14) + 1;
  return;
}

