/* Entry: 0x00504d40; symbol: FUN_00504d40; body bytes: 326 */

undefined4 __thiscall FUN_00504d40(void *this,int param_1,int param_2,int param_3)

{
  void *this_00;
  int iVar1;
  undefined4 *puVar2;
  bool bVar3;
  char *local_20;
  uint local_10;
  char *local_8;
  
  local_10 = 0;
  local_8 = *(char **)(*(int *)((int)this + 0x12558) + 0x5d4 + param_3 * 4);
  do {
    if (*local_8 < '\0') {
      return 0;
    }
    if (*local_8 == '\0') {
LAB_00504df2:
      if (local_8[0x28] == '\0') {
        local_20 = (char *)0x0;
      }
      else {
        this_00 = (void *)FUN_0040ff90(*(int *)((int)this + 0x12594));
        local_20 = (char *)FUN_005060a0(this_00,((int)local_8[0x28] & 0xfU) - 1);
      }
      iVar1 = FUN_0040ff90(*(int *)((int)this + 0x12594));
      puVar2 = (undefined4 *)FUN_00460850(iVar1);
      FUN_00505d90(this,(char *)(param_3 << 8 | local_10),param_1,puVar2,local_20);
    }
    else {
      if (local_8[0x32] == '\0') {
        bVar3 = param_1 % (int)*local_8 != (int)local_8[1];
      }
      else {
        bVar3 = param_2 % (int)local_8[0x32] != (int)local_8[0x33];
      }
      if (!bVar3) goto LAB_00504df2;
    }
    local_10 = local_10 + 1;
    local_8 = local_8 + 0x78;
  } while( true );
}

