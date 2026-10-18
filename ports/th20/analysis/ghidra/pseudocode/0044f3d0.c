/* Entry: 0x0044f3d0; symbol: FUN_0044f3d0; body bytes: 806 */

void __thiscall FUN_0044f3d0(void *this,int param_1,int param_2)

{
  if (param_1 == 0x29) {
    FUN_0044f720(&DAT_005c4d40,3);
    FUN_004dda60(0x5c4d40);
    *(int *)((int)this + 0x6c0) = param_2 + 1;
  }
  else if ((param_1 == 0x2b) || (param_1 == 0x2c)) {
    FUN_0041dce0(&DAT_005c4d40,3);
    FUN_004dda60(0x5c4d40);
    *(undefined4 *)((int)this + 0x6c0) = 0xd;
  }
  else if ((param_1 < 3) || ((0x26 < param_1 && (param_1 < 0x2b)))) {
    *(undefined4 *)((int)this + 0x6c0) = 0;
  }
  else if (param_1 < 6) {
    if (*(int *)((int)this + 0x6c0) != param_2 + 1) {
      FUN_0044f720(&DAT_005c4d40,0);
      FUN_004dda60(0x5c4d40);
      *(int *)((int)this + 0x6c0) = param_2 + 1;
    }
  }
  else if (param_1 < 0xc) {
    if (*(int *)((int)this + 0x6c0) != param_2 + 3) {
      FUN_0044f720(&DAT_005c4d40,0);
      FUN_004dda60(0x5c4d40);
      *(int *)((int)this + 0x6c0) = param_2 + 3;
    }
  }
  else if (param_1 < 0x14) {
    if (*(int *)((int)this + 0x6c0) != param_2 + 5) {
      FUN_0044f720(&DAT_005c4d40,0);
      FUN_004dda60(0x5c4d40);
      *(int *)((int)this + 0x6c0) = param_2 + 5;
    }
  }
  else if (param_1 < 0x18) {
    if (*(int *)((int)this + 0x6c0) != param_2 + 7) {
      FUN_0044f700(&DAT_005c4d40,1);
      FUN_004ddf80(0x5c4d40);
      FUN_004d9db0(&DAT_005c4d40,0x17,8);
    }
  }
  else if (param_1 < 0x1a) {
    if (*(int *)((int)this + 0x6c0) != 9) {
      FUN_0041dce0(&DAT_005c4d40,4);
      FUN_004ddf80(0x5c4d40);
      FUN_004d9db0(&DAT_005c4d40,0x17,8);
      *(undefined4 *)(DAT_005c0028 + 200) = 0;
      *(undefined4 *)(DAT_005c0028 + 0xcc) = 0;
      *(undefined4 *)((int)this + 0x6c0) = 9;
    }
  }
  else if ((((param_1 < 0x20) || ((0x21 < param_1 && (param_1 < 0x25)))) ||
           ((0x2c < param_1 && (param_1 < 0x31)))) || ((0x32 < param_1 && (param_1 < 0x36)))) {
    if (*(int *)((int)this + 0x6c0) != 10) {
      FUN_0041dce0(&DAT_005c4d40,2);
      FUN_004ddf80(0x5c4d40);
      FUN_004d9db0(&DAT_005c4d40,0x17,8);
      *(undefined4 *)(DAT_005c0028 + 200) = 0;
      *(undefined4 *)(DAT_005c0028 + 0xcc) = 0;
      *(undefined4 *)((int)this + 0x6c0) = 10;
    }
  }
  else if ((((0x1f < param_1) && (param_1 < 0x22)) || ((0x30 < param_1 && (param_1 < 0x33)))) &&
          (*(int *)((int)this + 0x6c0) != param_2 + 0xb)) {
    FUN_0044f700(&DAT_005c4d40,5);
    FUN_004ddf80(0x5c4d40);
    FUN_004d9db0(&DAT_005c4d40,0x17,8);
    *(undefined4 *)(DAT_005c0028 + 200) = 0;
    *(undefined4 *)(DAT_005c0028 + 0xcc) = 0;
    *(int *)((int)this + 0x6c0) = param_2 + 0xb;
  }
  return;
}

