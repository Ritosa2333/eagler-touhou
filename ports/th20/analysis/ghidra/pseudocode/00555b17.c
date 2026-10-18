/* Entry: 0x00555b17; symbol: FUN_00555b17; body bytes: 15 */

void FUN_00555b17(void)

{
  uint *puVar1;
  int unaff_EBP;
  
  puVar1 = (uint *)(*(int *)**(undefined4 **)(unaff_EBP + 0x10) + 0x350);
  *puVar1 = *puVar1 & 0xffffffef;
  return;
}

