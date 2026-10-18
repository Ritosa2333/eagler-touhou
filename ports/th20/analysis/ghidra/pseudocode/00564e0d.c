/* Entry: 0x00564e0d; symbol: FUN_00564e0d; body bytes: 259 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */

int __cdecl FUN_00564e0d(char *param_1,uint *param_2,uint param_3,int *param_4)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  undefined4 *unaff_FS_OFFSET;
  undefined4 local_14;
  
  if (param_1 == (char *)0xfffffffe) {
    *(undefined1 *)(param_4 + 9) = 1;
    param_4[8] = 0;
    *(undefined1 *)(param_4 + 7) = 1;
    param_4[6] = 9;
  }
  else {
    if (((int)param_1 < 0) || (DAT_005e5890 <= param_1)) {
      bVar1 = false;
    }
    else {
      bVar1 = true;
    }
    if (bVar1) {
      iVar3 = ((uint)param_1 & 0x3f) * 0x38;
      if ((*(byte *)((&DAT_005e5690)[(int)param_1 >> 6] + 0x28 + iVar3) & 1) != 0) {
        FUN_0055fab5((uint)param_1);
        iVar2 = -1;
        if ((*(byte *)(iVar3 + 0x28 + (&DAT_005e5690)[(int)param_1 >> 6]) & 1) == 0) {
          *(undefined1 *)(param_4 + 7) = 1;
          param_4[6] = 9;
          *(undefined1 *)(param_4 + 9) = 1;
          param_4[8] = 0;
        }
        else {
          iVar2 = FUN_00564f1e(param_1,param_2,param_3,param_4);
        }
        FUN_00564f16();
        goto LAB_00564f00;
      }
    }
    *(undefined1 *)(param_4 + 9) = 1;
    param_4[8] = 0;
    *(undefined1 *)(param_4 + 7) = 1;
    param_4[6] = 9;
    FUN_0054803f((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0,param_4);
  }
  iVar2 = -1;
LAB_00564f00:
  *unaff_FS_OFFSET = local_14;
  return iVar2;
}

