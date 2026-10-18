/* Entry: 0x00447c60; symbol: FUN_00447c60; body bytes: 86 */

void * __fastcall FUN_00447c60(void *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  _eh_vector_constructor_iterator_(param_1,0x600,0x10000,FUN_004489f0,FUN_004492f0);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

