/* Entry: 0x00505e40; symbol: FUN_00505e40; body bytes: 308 */

undefined4 __thiscall
FUN_00505e40(void *this,int param_1,int param_2,int param_3,undefined4 *param_4)

{
  void *this_00;
  bool bVar1;
  char *local_20;
  uint local_10;
  char *local_8;
  
  local_10 = 0;
  local_8 = *(char **)(*(int *)((int)this + 0x12558) + 0x5d4 + param_1 * 4);
  do {
    if (*local_8 < '\0') {
      return 0;
    }
    if (*local_8 == '\0') {
LAB_00505ef2:
      if (local_8[0x28] == '\0') {
        local_20 = (char *)0x0;
      }
      else {
        this_00 = (void *)FUN_0040ff90(*(int *)((int)this + 0x12594));
        local_20 = (char *)FUN_005060a0(this_00,((int)local_8[0x28] & 0xfU) - 1);
      }
      FUN_00505d90(this,(char *)(param_1 << 8 | local_10),param_2,param_4,local_20);
    }
    else {
      if (local_8[0x32] == '\0') {
        bVar1 = param_2 % (int)*local_8 != (int)local_8[1];
      }
      else {
        bVar1 = param_3 % (int)local_8[0x32] != (int)local_8[0x33];
      }
      if (!bVar1) goto LAB_00505ef2;
    }
    local_10 = local_10 + 1;
    local_8 = local_8 + 0x78;
  } while( true );
}

