/* Entry: 0x005427dd; symbol: ___scrt_acquire_startup_lock; body bytes: 50 */

/* Library Function - Single Match
    ___scrt_acquire_startup_lock
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

uint ___scrt_acquire_startup_lock(void)

{
  uint uVar1;
  uint uVar2;
  bool bVar3;
  undefined3 extraout_var;
  uint uVar4;
  int unaff_FS_OFFSET;
  
  bVar3 = ___scrt_is_ucrt_dll_in_use();
  uVar4 = CONCAT31(extraout_var,bVar3);
  if (uVar4 != 0) {
    uVar1 = *(uint *)(*(int *)(unaff_FS_OFFSET + 0x18) + 4);
    while( true ) {
      uVar4 = 0;
      LOCK();
      uVar2 = uVar1;
      if (DAT_005e4f48 != 0) {
        uVar4 = DAT_005e4f48;
        uVar2 = DAT_005e4f48;
      }
      DAT_005e4f48 = uVar2;
      UNLOCK();
      if (uVar4 == 0) break;
      if (uVar1 == uVar4) {
        return CONCAT31((int3)(uVar4 >> 8),1);
      }
    }
  }
  return uVar4 & 0xffffff00;
}

