/* Entry: 0x00542f0f; symbol: FUN_00542f0f; body bytes: 20 */

void FUN_00542f0f(void)

{
  char in_AL;
  uint unaff_EBX;
  int unaff_EBP;
  
  if (in_AL == '\0') {
    __ArrayUnwind(*(void **)(unaff_EBP + 8),*(uint *)(unaff_EBP + 0xc),unaff_EBX,
                  *(_func_void_void_ptr **)(unaff_EBP + 0x18));
  }
  return;
}

