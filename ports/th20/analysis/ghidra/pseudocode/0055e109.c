/* Entry: 0x0055e109; symbol: FUN_0055e109; body bytes: 18 */

uint FUN_0055e109(void)

{
  int unaff_FS_OFFSET;
  
  return *(uint *)(*(int *)(*(int *)(unaff_FS_OFFSET + 0x18) + 0x30) + 0x68) >> 8 & 0xffffff01;
}

