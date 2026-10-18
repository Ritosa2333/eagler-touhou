/* Entry: 0x0055845c; symbol: FUN_0055845c; body bytes: 58 */

void FUN_0055845c(void)

{
  uint uVar1;
  void *this;
  int local_8;
  
  if (DAT_005e5660 == 0) {
    local_8 = 0;
    uVar1 = FUN_0055e11b();
    if ((char)uVar1 == '\0') {
      FUN_00557fd2(this,&local_8);
    }
    DAT_005e5660 = (local_8 == 1) + 1;
    LOCK();
    UNLOCK();
  }
  return;
}

