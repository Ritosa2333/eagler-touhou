/* Entry: 0x004ec490; symbol: FUN_004ec490; body bytes: 98 */

uint FUN_004ec490(void)

{
  char cVar1;
  undefined8 uVar2;
  undefined1 local_5;
  
  local_5 = 0;
  uVar2 = FUN_004f3b70(&local_5);
  cVar1 = FUN_004f36a0((uint)uVar2,(int)((ulonglong)uVar2 >> 0x20));
  if (cVar1 == '\0') {
    func_0x004fcd20("Number is too big.");
  }
  return (uint)uVar2;
}

