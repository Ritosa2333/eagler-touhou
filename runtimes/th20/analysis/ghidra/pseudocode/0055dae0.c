/* Entry: 0x0055dae0; symbol: FUN_0055dae0; body bytes: 146 */

void __cdecl FUN_0055dae0(uint param_1)

{
  int iVar1;
  int *unaff_FS_OFFSET;
  undefined1 local_14 [16];
  
  iVar1 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)local_14;
  if (0 < DAT_005e5288) {
    if (((param_1 & 0x40) != 0) && (DAT_005b29fc != 0)) {
      MXCSR = param_1;
      *unaff_FS_OFFSET = iVar1;
      return;
    }
    MXCSR = param_1 & 0xffffffbf;
  }
  *unaff_FS_OFFSET = iVar1;
  return;
}

