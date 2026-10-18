/* Entry: 0x0044d020; symbol: FUN_0044d020; body bytes: 2143 */

void __fastcall FUN_0044d020(int param_1)

{
  int *piVar1;
  int local_3c;
  int local_38;
  int local_34;
  int local_30;
  int local_2c;
  int local_28;
  int local_24;
  int local_20;
  int local_1c;
  int local_18;
  uint *local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *(undefined4 *)(param_1 + 0x6000e48) = DAT_0056cdbc;
  *(undefined4 *)(param_1 + 0x6000e20) = DAT_0056cdbc;
  *(float *)(param_1 + 0x6000e5c) = DAT_0056cd98;
  *(float *)(param_1 + 0x6000e34) = DAT_0056cd98;
  *(undefined4 *)(param_1 + 0x6000e38) = DAT_0056cdbc;
  *(undefined4 *)(param_1 + 0x6000e24) = DAT_0056cdbc;
  *(float *)(param_1 + 0x6000e60) = DAT_0056cd98;
  *(float *)(param_1 + 0x6000e4c) = DAT_0056cd98;
  *(undefined4 *)(param_1 + 0x6000e64) = 0;
  *(undefined4 *)(param_1 + 0x6000e50) = 0;
  *(undefined4 *)(param_1 + 0x6000e3c) = 0;
  *(undefined4 *)(param_1 + 0x6000e28) = 0;
  *(undefined4 *)(param_1 + 0x6000e54) = 0;
  *(undefined4 *)(param_1 + 0x6000e2c) = 0;
  *(undefined4 *)(param_1 + 0x6000e68) = DAT_0056c8cc;
  *(undefined4 *)(param_1 + 0x6000e40) = DAT_0056c8cc;
  *(undefined4 *)(param_1 + 0x6000e44) = 0;
  *(undefined4 *)(param_1 + 0x6000e30) = 0;
  *(undefined4 *)(param_1 + 0x6000e6c) = DAT_0056c8cc;
  *(undefined4 *)(param_1 + 0x6000e58) = DAT_0056c8cc;
  DAT_005aef80 = *(undefined4 *)(param_1 + 0x6000e20);
  DAT_005aef84 = *(undefined4 *)(param_1 + 0x6000e24);
  DAT_005aef88 = *(undefined4 *)(param_1 + 0x6000e28);
  DAT_005aef98 = *(undefined4 *)(param_1 + 0x6000e34);
  DAT_005aef9c = *(undefined4 *)(param_1 + 0x6000e38);
  DAT_005aefa0 = *(undefined4 *)(param_1 + 0x6000e3c);
  DAT_005aefb0 = *(undefined4 *)(param_1 + 0x6000e48);
  DAT_005aefb4 = *(undefined4 *)(param_1 + 0x6000e4c);
  DAT_005aefb8 = *(undefined4 *)(param_1 + 0x6000e50);
  DAT_005aefc8 = *(undefined4 *)(param_1 + 0x6000e5c);
  DAT_005aefcc = *(undefined4 *)(param_1 + 0x6000e60);
  DAT_005aefd0 = *(undefined4 *)(param_1 + 0x6000e64);
  DAT_005aef90 = *(undefined4 *)(param_1 + 0x6000e2c);
  DAT_005aef94 = *(undefined4 *)(param_1 + 0x6000e30);
  DAT_005aefa8 = *(undefined4 *)(param_1 + 0x6000e40);
  DAT_005aefac = *(undefined4 *)(param_1 + 0x6000e44);
  DAT_005aefc0 = *(undefined4 *)(param_1 + 0x6000e54);
  DAT_005aefc4 = *(undefined4 *)(param_1 + 0x6000e58);
  DAT_005aefd8 = *(undefined4 *)(param_1 + 0x6000e68);
  DAT_005aefdc = *(undefined4 *)(param_1 + 0x6000e6c);
  piVar1 = (int *)FUN_00412730(0x5c4d40);
  (**(code **)(*piVar1 + 0x68))(piVar1,0x2d0,0,0x102,1,param_1 + 0x6000e1c,0);
  (**(code **)(**(int **)(param_1 + 0x6000e1c) + 0x2c))
            (*(undefined4 *)(param_1 + 0x6000e1c),0,0,&local_c,0);
  if (local_c != (uint *)0x0) {
    FUN_00543e20(local_c,(uint *)(param_1 + 0x6000e20),0x50);
    for (local_18 = 0; local_18 < 4; local_18 = local_18 + 1) {
      *(float *)(param_1 + 0x6000e24 + local_18 * 0x14) =
           *(float *)(param_1 + 0x6000e24 + local_18 * 0x14) + DAT_0056cd98;
    }
    FUN_00543e20(local_c + 0x3c,(uint *)(param_1 + 0x6000e20),0x50);
    for (local_1c = 0; local_1c < 4; local_1c = local_1c + 1) {
      *(float *)(param_1 + 0x6000e24 + local_1c * 0x14) =
           *(float *)(param_1 + 0x6000e24 + local_1c * 0x14) - DAT_0056cda0;
    }
    FUN_00543e20(local_c + 0x78,(uint *)(param_1 + 0x6000e20),0x50);
    for (local_20 = 0; local_20 < 4; local_20 = local_20 + 1) {
      *(float *)(param_1 + 0x6000e20 + local_20 * 0x14) =
           *(float *)(param_1 + 0x6000e20 + local_20 * 0x14) + DAT_0056cd98;
    }
    for (local_24 = 0; local_24 < 4; local_24 = local_24 + 1) {
      *(float *)(param_1 + 0x6000e24 + local_24 * 0x14) =
           *(float *)(param_1 + 0x6000e24 + local_24 * 0x14) + DAT_0056cd98;
    }
    FUN_00543e20(local_c + 0x14,(uint *)(param_1 + 0x6000e20),0x50);
    for (local_28 = 0; local_28 < 4; local_28 = local_28 + 1) {
      *(float *)(param_1 + 0x6000e24 + local_28 * 0x14) =
           *(float *)(param_1 + 0x6000e24 + local_28 * 0x14) + DAT_0056cd98;
    }
    FUN_00543e20(local_c + 0x50,(uint *)(param_1 + 0x6000e20),0x50);
    for (local_2c = 0; local_2c < 4; local_2c = local_2c + 1) {
      *(float *)(param_1 + 0x6000e24 + local_2c * 0x14) =
           *(float *)(param_1 + 0x6000e24 + local_2c * 0x14) - DAT_0056cda0;
    }
    FUN_00543e20(local_c + 0x8c,(uint *)(param_1 + 0x6000e20),0x50);
    for (local_30 = 0; local_30 < 4; local_30 = local_30 + 1) {
      *(float *)(param_1 + 0x6000e20 + local_30 * 0x14) =
           *(float *)(param_1 + 0x6000e20 + local_30 * 0x14) - DAT_0056cda0;
    }
    for (local_34 = 0; local_34 < 4; local_34 = local_34 + 1) {
      *(float *)(param_1 + 0x6000e24 + local_34 * 0x14) =
           *(float *)(param_1 + 0x6000e24 + local_34 * 0x14) + DAT_0056cd98;
    }
    FUN_00543e20(local_c + 0x28,(uint *)(param_1 + 0x6000e20),0x50);
    for (local_38 = 0; local_38 < 4; local_38 = local_38 + 1) {
      *(float *)(param_1 + 0x6000e24 + local_38 * 0x14) =
           *(float *)(param_1 + 0x6000e24 + local_38 * 0x14) + DAT_0056cd98;
    }
    FUN_00543e20(local_c + 100,(uint *)(param_1 + 0x6000e20),0x50);
    for (local_3c = 0; local_3c < 4; local_3c = local_3c + 1) {
      *(float *)(param_1 + 0x6000e24 + local_3c * 0x14) =
           *(float *)(param_1 + 0x6000e24 + local_3c * 0x14) - DAT_0056cda0;
    }
    FUN_00543e20(local_c + 0xa0,(uint *)(param_1 + 0x6000e20),0x50);
    (**(code **)(**(int **)(param_1 + 0x6000e1c) + 0x30))(*(undefined4 *)(param_1 + 0x6000e1c));
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 400))(piVar1,0,*(undefined4 *)(param_1 + 0x6000e1c),0,0x14);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

