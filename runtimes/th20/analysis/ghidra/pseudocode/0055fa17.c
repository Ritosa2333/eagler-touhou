/* Entry: 0x0055fa17; symbol: ___acrt_lowio_ensure_fh_exists; body bytes: 146 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */
/* Library Function - Single Match
    ___acrt_lowio_ensure_fh_exists
   
   Library: Visual Studio 2019 Release */

undefined4 __cdecl ___acrt_lowio_ensure_fh_exists(uint param_1)

{
  __acrt_ptd *p_Var1;
  undefined4 *puVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  undefined4 *unaff_FS_OFFSET;
  undefined4 local_14;
  
  if (param_1 < 0x2000) {
    uVar4 = 0;
    ___acrt_lock(7);
    iVar5 = 0;
    iVar3 = DAT_005e5890;
    while (iVar3 <= (int)param_1) {
      if ((&DAT_005e5690)[iVar5] == 0) {
        puVar2 = ___acrt_lowio_create_handle_array();
        (&DAT_005e5690)[iVar5] = puVar2;
        if (puVar2 == (undefined4 *)0x0) {
          uVar4 = 0xc;
          break;
        }
        iVar3 = DAT_005e5890 + 0x40;
        DAT_005e5890 = iVar3;
      }
      iVar5 = iVar5 + 1;
    }
    FUN_0055faac();
  }
  else {
    p_Var1 = FUN_005516c1();
    uVar4 = 9;
    *(undefined4 *)p_Var1 = 9;
    FUN_005480bc();
  }
  *unaff_FS_OFFSET = local_14;
  return uVar4;
}

