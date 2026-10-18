/* Entry: 0x004ed910; symbol: FUN_004ed910; body bytes: 494 */

void __thiscall FUN_004ed910(void *this,char param_1)

{
  char cVar1;
  int local_c;
  
  if ((param_1 < '\0') || (cVar1 = FUN_004ed630(), cVar1 < param_1)) {
    func_0x004fcd20("Invalid type specification.");
  }
  local_c = 0;
  switch(param_1) {
  case '\0':
    break;
  default:
    func_0x004fcd20("Invalid presentation type specifier");
    break;
  case 'A':
  case 'E':
  case 'F':
  case 'G':
  case 'a':
  case 'e':
  case 'f':
  case 'g':
    local_c = 2;
    break;
  case 'B':
  case 'X':
  case 'b':
  case 'd':
  case 'o':
  case 'x':
    local_c = 1;
    break;
  case 'P':
  case 'p':
    local_c = 4;
    break;
  case 'c':
    local_c = 5;
    break;
  case 's':
    local_c = 3;
  }
  switch(*(undefined1 *)((int)this + 0xc)) {
  case 1:
  case 2:
  case 3:
  case 4:
    if (local_c == 0) {
      local_c = 1;
    }
    if ((local_c != 1) && (local_c != 5)) {
      func_0x004fcd20("Invalid presentation type for integer");
    }
    break;
  case 5:
    if (local_c == 0) {
      local_c = 3;
    }
    if ((local_c != 3) && (local_c != 1)) {
      func_0x004fcd20("Invalid presentation type for bool");
    }
    break;
  case 6:
    if (local_c == 0) {
      local_c = 5;
    }
    if ((local_c != 5) && (local_c != 1)) {
      func_0x004fcd20("Invalid presentation type for char");
    }
    break;
  case 7:
  case 8:
  case 9:
    if (local_c == 0) {
      local_c = 2;
    }
    if (local_c != 2) {
      func_0x004fcd20("Invalid presentation type for floating-point");
    }
    break;
  case 10:
    if (local_c == 0) {
      local_c = 4;
    }
    if (local_c != 4) {
      func_0x004fcd20("Invalid presentation type for pointer");
    }
    break;
  case 0xb:
  case 0xc:
    if (local_c == 0) {
      local_c = 3;
    }
    if (local_c != 3) {
      func_0x004fcd20("Invalid presentation type for string");
    }
  }
  if (((*(char *)((int)this + 0xd) != '\0') && (local_c != 1)) && (local_c != 2)) {
    func_0x004fcd20("Hash/sign modifier requires an arithmetic presentation type");
  }
  if (((*(char *)((int)this + 0xe) != '\0') && (local_c != 1)) && ((local_c != 2 && (local_c != 4)))
     ) {
    func_0x004fcd20("Zero modifier requires an arithmetic or pointer presentation type");
  }
  FUN_004fcba0(this,param_1);
  return;
}

