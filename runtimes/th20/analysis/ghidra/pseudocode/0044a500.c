/* Entry: 0x0044a500; symbol: FUN_0044a500; body bytes: 159 */

int __fastcall FUN_0044a500(int param_1)

{
  char cVar1;
  int *unaff_FS_OFFSET;
  int local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  puStack_c = &LAB_0056844d;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  local_8 = 0;
  local_14 = param_1;
  do {
    if (local_14 == 0) {
      local_8 = 0xffffffff;
      FUN_00414400(&stack0x00000004);
      local_14 = 0;
LAB_0044a58e:
      *unaff_FS_OFFSET = local_10;
      return local_14;
    }
    cVar1 = FUN_00449630(&stack0x00000004,local_14);
    if (cVar1 != '\0') {
      local_8 = 0xffffffff;
      FUN_00414400(&stack0x00000004);
      goto LAB_0044a58e;
    }
    local_14 = FUN_0040ff90(local_14);
  } while( true );
}

