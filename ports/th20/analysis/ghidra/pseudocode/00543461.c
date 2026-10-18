/* Entry: 0x00543461; symbol: FUN_00543461; body bytes: 329 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */

UINT FUN_00543461(void)

{
  code *pcVar1;
  bool bVar2;
  undefined4 uVar3;
  int iVar4;
  int *piVar5;
  uint uVar6;
  UINT unaff_ESI;
  undefined4 *unaff_FS_OFFSET;
  int local_24;
  undefined4 local_14;
  
  uVar3 = ___scrt_initialize_crt(1);
  if ((char)uVar3 != '\0') {
    bVar2 = false;
    uVar3 = ___scrt_acquire_startup_lock();
    if (DAT_005e4f44 != 1) {
      if (DAT_005e4f44 == 0) {
        DAT_005e4f44 = 1;
        iVar4 = __initterm_e((undefined4 *)&DAT_0056c418,(undefined4 *)&DAT_0056c43c);
        if (iVar4 != 0) {
          unaff_ESI = 0xff;
          goto LAB_005435ba;
        }
        FUN_005559fe((undefined4 *)&DAT_0056c384,(undefined4 *)&DAT_0056c414);
        DAT_005e4f44 = 2;
      }
      else {
        bVar2 = true;
      }
      ___scrt_release_startup_lock((char)uVar3);
      piVar5 = (int *)FUN_00543d7c();
      if (*piVar5 != 0) {
        uVar3 = FUN_005428cf((int)piVar5);
        if ((char)uVar3 != '\0') {
          pcVar1 = (code *)*piVar5;
          (*(code *)PTR_guard_check_icall_0056c37c)(0,2,0);
          (*pcVar1)();
        }
      }
      piVar5 = (int *)FUN_00543d82();
      if (*piVar5 != 0) {
        uVar3 = FUN_005428cf((int)piVar5);
        if ((char)uVar3 != '\0') {
          __register_thread_local_exe_atexit_callback((_func_void_void_ptr_ulong_void_ptr *)*piVar5)
          ;
        }
      }
      FUN_00543b4e();
      FUN_005559a6();
      unaff_ESI = FUN_0041e7d0((HINSTANCE)&IMAGE_DOS_HEADER_00400000);
      uVar6 = FUN_00543b86();
      if ((char)uVar6 != '\0') {
        if (!bVar2) {
          FUN_00552dde();
        }
        ___scrt_uninitialize_crt('\x01','\0');
LAB_005435ba:
        *unaff_FS_OFFSET = local_14;
        return unaff_ESI;
      }
      goto LAB_005435d1;
    }
  }
  FUN_00543a39();
LAB_005435d1:
  FUN_00552e29(unaff_ESI);
                    /* WARNING: Subroutine does not return */
  __exit(local_24);
}

