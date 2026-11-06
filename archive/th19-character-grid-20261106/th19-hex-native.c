// Program SHA256: 8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9
// Address 4cb8b0

int __thiscall FUN_004cb8b0(int param_1,uint param_2)

{
  int *piVar1;
  undefined4 *puVar2;
  int iVar3;
  
  if (param_2 != 0) {
    if ((param_2 & 0xffff) == 0xffff) {
      iVar3 = 0;
      puVar2 = (undefined4 *)(param_1 + 0x71c);
      do {
        for (piVar1 = (int *)puVar2[-2]; piVar1 != (int *)0x0; piVar1 = (int *)piVar1[1]) {
          if (*(uint *)(*piVar1 + 0x4d8) == param_2) {
            return *piVar1;
          }
        }
        for (piVar1 = (int *)*puVar2; piVar1 != (int *)0x0; piVar1 = (int *)piVar1[1]) {
          if (*(uint *)(*piVar1 + 0x4d8) == param_2) {
            return *piVar1;
          }
        }
        iVar3 = iVar3 + 1;
        puVar2 = puVar2 + 8;
      } while (iVar3 < 2);
      return 0;
    }
    param_1 = (param_2 & 0xffff) * 0x64c + param_1;
    if (*(char *)(param_1 + 0xd98) != '\0') {
      iVar3 = param_1 + 0x754;
      if (*(uint *)(param_1 + 0xc2c) != param_2) {
        iVar3 = 0;
      }
      return iVar3;
    }
  }
  return 0;
}


// Reference 004bed6a
// Reference 004d099b
// Reference 004bef7d
// Reference 004de3e4
// Reference 004de434
// Reference 004cb3de
// Reference 0054ed9c
// Reference 004cb7b3
// Reference 00409e94
// Reference 00409f64
// Reference 00409fad
// Reference 0040a024
// Reference 0040a052
// Reference 0040a07f
// Reference 0040a0b6
// Reference 0040a0d5
// Reference 0040a134
// Reference 0040a174
// Reference 0040a1a6
// Reference 0040a1c5
// Reference 0040a1f6
// Reference 0040a215
// Reference 0040a274
// Reference 0040a2b4
// Reference 0040a2e6
// Reference 0040c9ee
// Reference 00506728
// Reference 005068ca
// Reference 00534894
// Reference 00534955
// Reference 0040e22b
// Reference 0040ffca
// Reference 00507e84
// Reference 004cb83d
// Reference 0050aacb
// Reference 004128e5
// Reference 0041291d
// Reference 00412966
// Reference 004129a7
// Reference 004129df
// Reference 00412a29
// Reference 00412a68
// Reference 00513ba8
// Reference 00513ce9
// Reference 00513d2f
// Reference 00513d72
// Reference 00513e34
// Reference 00513e84
// Reference 00513ed4
// Reference 00513f24
// Reference 004dd160
// Reference 004dd1a8
// Reference 004dd1ee
// Reference 004dd2de
// Reference 004dd33e
// Reference 004caee1
// Reference 004ddfc6
// Reference 004de014
// Reference 00413b76
// Reference 00415da7
// Reference 00416c34
// Reference 00416c6a
// Reference 00416e97
// Reference 00416ecf
// Reference 00417997
// Reference 004179d8
// Reference 00418302
// Reference 0041834b
// Reference 0041836c
// Reference 00418614
// Reference 00418656
// Reference 00508f58
// Reference 00508fd8
// Reference 00508a4b
// Reference 00508aa3
// Reference 00508e68
// Reference 00509997
// Reference 0050a2bc
// Reference 004cb1f6
// Reference 0050a760
// Reference 0050a81b
// Reference 0051cd1c
// Reference 00419172
// Reference 004191ae
// Reference 004191ca
// Reference 004194e4
// Reference 00419524
// Reference 0054851a
// Reference 0054887c
// Reference 0041a4e3
// Reference 0041a502
// Reference 0041a553
// Reference 0041ba94
// Reference 0041bae8
// Reference 0041c3c3
// Reference 0041c408
// Reference 0041c42c
// Reference 0041c45a
// Reference 0041c916
// Reference 0041c963
// Reference 0041c98c
// Reference 0041c9e3
// Reference 0041ca0a
// Reference 0041cee3
// Reference 0041cfea
// Reference 0041da1a
// Reference 0041da75
// Reference 0041da9d
// Reference 0041dac9
// Reference 0041e084
// Reference 0041e0bf
// Reference 0041e0e8
// Reference 0041ea28
// Reference 0041f1fd
// Reference 0041f248
// Reference 0041f267
// Reference 0041f286
// Reference 0041f2a2
// Reference 0041f2d5
// Reference 0041f308
// Reference 0041fb9e
// Reference 0041fbcf
// Reference 0041fc00
// Reference 0041fc2e
// Reference 0041fc53
// Reference 0041fc88
// Reference 00420226
// Reference 0042109a
// Reference 004210fa
// Reference 00421122
// Reference 00421151
// Reference 004216e9
// Reference 00421712
// Reference 00421799
// Reference 00421eaa
// Reference 00423072
// Reference 00423098
// Reference 004230b8
// Reference 004230da
// Reference 00423851
// Reference 00423877
// Reference 00423897
// Reference 004238b9
// Reference 00423e0a
// Reference 00424851
// Reference 0042487d
// Reference 00424e11
// Reference 00424e3a
// Reference 00424e5c
// Reference 0042572f
// Reference 00425aaa
// Reference 00425e5e
// Reference 00425e8a
// Reference 00425f09
// Reference 00425f3a
// Reference 004261ba
// Reference 004263a2
// Reference 004263d1
// Reference 00426449
// Reference 0042647d
// Reference 00427585
// Reference 004275b0
// Reference 004275d2
// Reference 00427c55
// Reference 00427c83
// Reference 00427d20
// Reference 0042960b
// Reference 00429631
// Reference 0042965e
// Reference 0042968f
// Reference 004296b6
// Reference 004296e0
// Reference 0042a0be
// Reference 0042a0e8
// Reference 0042a105
// Reference 0042a132
// Reference 0042a14f
// Reference 0042a17c
// Reference 0042afd8
// Reference 0042b004
// Reference 0042b026
// Reference 0042b29f
// Reference 0042b2c6
// Reference 0042bb97
// Reference 0042c41e
// Reference 0042c44a
// Reference 0042c46c
// Reference 0042caab
// Reference 0042cad9
// Reference 0042caff
// Reference 0042d659
// Reference 0042d67d
// Reference 0042d69c
// Reference 0042d717
// Reference 0042e1de
// Reference 0042e206
// Reference 0042e229
// Reference 0042e2b5
// Reference 0042e769
// Reference 0042ec38
// Reference 00430f77
// Reference 00430fd8
// Reference 00431693
// Reference 00561f88
// Reference 005621d2
// Reference 004cac4e
// Reference 004cac7e
// Reference 004cacb1
// Reference 004cae6b
// Reference 004cae9e
// Reference 004cafb1
// Reference 004cb2be
// Reference 004cb32b
// Reference 004cb35b
// Reference 004cb3ab
// Reference 004cb5dc
// Reference 004cb60c
// Reference 004cb63c
// Reference 004cb67c
// Reference 004cb6ac
// Reference 004cb6dc
// Reference 004cb760
// Reference 004d9c2c
// Reference 004d9c66
// Reference 004d9ca9
// Reference 004d9d04
// Reference 004d9d5c
// Reference 004da864
// Reference 004dcf86
// Reference 004dd083
// Reference 004dd71a
// Reference 004de114
// Reference 004de176
// Reference 004de1a6
// Reference 004de1e9
// Reference 004deb6c
// Reference 004e0b5a
// Reference 004e0a17
// Reference 004e41bc
// Reference 004e41ea
// Reference 004e423e
// Reference 004e4260
// Reference 004e4284
// Reference 004e42ad
// Reference 004e50ac
// Reference 004e50da
// Reference 004e512e
// Reference 004e5150
// Reference 004e5174
// Reference 004e519d
// Reference 004e0d1a
// Reference 004e394a
// Reference 004e43b8
// Reference 004e458b
// Reference 004e48db
// Reference 004e4959
// Reference 004e4984
// Reference 004e49c3
// Reference 004e4c8a
// Reference 004e4cb6
// Reference 004e4d08
// Reference 004e8ce5
// Reference 004e94ab
// Reference 004f4d1d
// Reference 004f5604
// Reference 004f6938
// Reference 004f696a
// Reference 004f6a9a
// Reference 004f7100
// Reference 004f9f66
// Reference 004fe3bb
// Reference 004fea5d
// Reference 004cb488
// Reference 00505d18
// Reference 00505dca
// Reference 00410464
// Reference 0050cbb4
// Reference 0050cdb4
// Reference 0050cfa7
// Reference 0050f7a1
// Reference 0050fae5
// Reference 0050fb12
// Reference 0050fb4e
// Reference 0050fb85
// Reference 0050fbb2
// Reference 0050fbf1
// Reference 0050fc26
// Reference 0050fc56
// Reference 0050fcb5
// Reference 0050fce2
// Reference 0050fd1e
// Reference 0050fd55
// Reference 0050fd82
// Reference 0050fdc1
// Reference 0050fdf6
// Reference 0050fe26
// Reference 00512209
// Reference 005124fb
// Reference 00512521
// Reference 0051253d
// Reference 00512559
// Reference 00512799
// Reference 005127b1
// Reference 005127c9
// Reference 00512811
// Reference 00512827
// Reference 0051283e
// Reference 00512855
// Reference 0051286b
// Reference 00513587
// Reference 005135b4
// Reference 004cb4c8
// Reference 0053ab35
// Reference 0053965b
// Reference 0053979f
// Reference 0051473d
// Reference 0051484b
// Reference 00514059
// Reference 0051441a
// Reference 00516768
// Reference 00516798
// Reference 005167dc
// Reference 00519795
// Reference 005197dd
// Reference 00519825
// Reference 0051986a
// Reference 005198af
// Reference 005198f7
// Reference 0051993f
// Reference 00519988
// Reference 00519a12
// Reference 0051a98b
// Reference 0051b96f
// Reference 0054fbba
// Reference 0052148c
// Reference 0052a6c3
// Reference 0052a754
// Reference 0052d067
// Reference 0052d0f8
// Reference 0053940b
// Reference 0053943b
// Reference 00539536
// Reference 005398fc
// Reference 0053b2a4
// Reference 0053b71e
// Reference 0053c697
// Reference 0053c6e9
// Reference 0053c727
// Reference 0053db32
// Reference 0053db4c
// Reference 0053fc4d
// Reference 0053fca2
// Reference 0053fccd
// Reference 0053fd07
// Reference 0053fd43
// Reference 0054050a
// Reference 00541c82
// Reference 00541cd8
// Reference 00541cf5
// Reference 00544eef
// Reference 0054e062
// Reference 00546e6c
// Reference 00546e97
// Reference 00547565
// Reference 005475e3
// Reference 0054ba19
// Reference 0054beb6
// Reference 0054cbb2
// Reference 0054d2e6
// Reference 0054d7b2
// Reference 0054dbce
// Reference 005537f8
// Reference 00553839
// Reference 00553984
// Reference 0055bbf2
// Reference 0055c09e
// Reference 0055c0d0
// Reference 00562f9a
// Reference 005103d9
// Reference 00510450
// Reference 00510581
// Reference 005105d3
// Reference 00510693
// Reference 005106b8
// Reference 005106e8
// Reference 00510718
// Reference 00510815
// Reference 00510858
// Reference 005108ac
// Reference 005108dc
// Reference 00510906
// Reference 00510936
// Reference 0051094d
// Reference 005109ee
// Reference 00510a21
// Reference 00510a94
// Reference 00510b9c
// Reference 00510bf8
// Reference 00510c58
// Reference 00510c90
// Reference 00510cba
// Reference 00510ce4
// Reference 00510cfc
// Reference 00510ed1
// Reference 00510ef8
// Reference 00510f1f
// Reference 005111f0
// Reference 005112b7
// Reference 005112d2
// Reference 005113fd
// Reference 005115c0
// Reference 005115db
// Reference 005115f6
// Reference 00511611
// Reference 004e110e
// Reference 004e1148
// Reference 004e1368
// Reference 004e1f1a
// Reference 004e1f45
// Reference 004e416c
// Reference 004e2c68
// Reference 004e33f8
// Reference 00554f1a
// Reference 00554fe6
// Reference 00555076
// Reference 0055509d
// Reference 00519b19
// Reference 00519b4a
// Reference 00519b9b
// Reference 00519bbe
// Reference 00519bf4
// Reference 00519f8e
// Reference 00519fab
// Reference 00519fe1
// Reference 0051a11f
// Reference 0051a146
// Reference 0051a170
// Reference 0051a363
// Reference 0051a38a
// Reference 0051a3b4
// Reference 0051a4df
// Reference 0051a4fc
// Reference 0051a532
// Reference 004e0864
// Reference 004e0896
// Reference 004e08dc
// Reference 005554ac
// Reference 00555928
// Reference 0055a311
// Reference 00539068
// Reference 00539098
// Reference 00553a80
// Reference 00553b18
// Reference 00553b48
// Reference 00556d41
// Reference 00556d81
// Reference 005577da
// Reference 0055788c
// Reference 00557b0c
// Reference 0041ad31
// Reference 0041af08
// Reference 00540dd4
// Reference 00540e2b
// Reference 00540e4c
// Reference 00506c08
// Reference 00506c70
// Reference 00506cdf
// Address 4c6910

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __thiscall FUN_004c6910(int param_1,int param_2)

{
  float *pfVar1;
  char cVar2;
  code *pcVar3;
  int *piVar4;
  byte bVar5;
  undefined8 *puVar6;
  undefined4 uVar7;
  uint uVar8;
  uint uVar9;
  int iVar10;
  float fVar11;
  float local_e0;
  float fStack_dc;
  undefined4 local_d4;
  float local_d0;
  undefined4 uStack_cc;
  undefined4 uStack_c8;
  undefined4 uStack_c4;
  undefined4 local_c0;
  float fStack_bc;
  undefined4 uStack_b8;
  undefined4 uStack_b4;
  float local_b0;
  float fStack_ac;
  undefined4 uStack_a8;
  undefined4 uStack_a4;
  undefined4 local_a0;
  undefined4 uStack_9c;
  undefined4 uStack_98;
  undefined4 uStack_94;
  undefined4 local_90;
  undefined4 uStack_8c;
  undefined4 uStack_88;
  undefined4 uStack_84;
  undefined4 local_80;
  undefined4 uStack_7c;
  undefined4 uStack_78;
  undefined4 uStack_74;
  undefined4 local_70;
  undefined4 uStack_6c;
  undefined4 uStack_68;
  undefined4 uStack_64;
  float local_60;
  float fStack_5c;
  float fStack_58;
  undefined4 uStack_54;
  undefined1 local_50 [68];
  uint local_c;
  
  local_c = DAT_005c3d80 ^ (uint)&stack0xfffffffc;
  if (((*(char *)(param_2 + 0x4b4) == '\0') || (*(char *)(param_2 + 0x4b5) == '\0')) ||
     (*(char *)(param_2 + 0x4af) == '\0')) {
    return 0xffffffff;
  }
  if (*(int *)(param_1 + 0x64c0f64) != 0) {
    FUN_004ca360();
  }
  if ((*(byte *)(param_2 + 0x4bc) & 1) == 0) {
    FUN_00534cb0();
  }
  else {
    FUN_00534c70();
  }
  if ((*(uint *)(param_2 + 0x4bc) & 0x400000) != 0) goto LAB_004c6e03;
  pfVar1 = (float *)(param_2 + 0x5b4);
  *pfVar1 = *(float *)(param_2 + 0x3d4);
  *(undefined4 *)(param_2 + 0x5b8) = *(undefined4 *)(param_2 + 0x3d8);
  *(undefined4 *)(param_2 + 0x5bc) = *(undefined4 *)(param_2 + 0x3dc);
  *(undefined4 *)(param_2 + 0x5c0) = *(undefined4 *)(param_2 + 0x3e0);
  *(undefined4 *)(param_2 + 0x5c4) = *(undefined4 *)(param_2 + 0x3e4);
  *(undefined4 *)(param_2 + 0x5c8) = *(undefined4 *)(param_2 + 1000);
  *(undefined4 *)(param_2 + 0x5cc) = *(undefined4 *)(param_2 + 0x3ec);
  *(undefined4 *)(param_2 + 0x5d0) = *(undefined4 *)(param_2 + 0x3f0);
  *(undefined4 *)(param_2 + 0x5d4) = *(undefined4 *)(param_2 + 0x3f4);
  *(undefined4 *)(param_2 + 0x5d8) = *(undefined4 *)(param_2 + 0x3f8);
  *(undefined4 *)(param_2 + 0x5dc) = *(undefined4 *)(param_2 + 0x3fc);
  *(undefined4 *)(param_2 + 0x5e0) = *(undefined4 *)(param_2 + 0x400);
  *(undefined4 *)(param_2 + 0x5e4) = *(undefined4 *)(param_2 + 0x404);
  *(undefined4 *)(param_2 + 0x5e8) = *(undefined4 *)(param_2 + 0x408);
  *(undefined4 *)(param_2 + 0x5ec) = *(undefined4 *)(param_2 + 0x40c);
  *(undefined4 *)(param_2 + 0x5f0) = *(undefined4 *)(param_2 + 0x410);
  *pfVar1 = *(float *)(param_2 + 0x5c) * *(float *)(param_2 + 0x54) * *pfVar1;
  *(float *)(param_2 + 0x5c8) =
       *(float *)(param_2 + 0x60) * *(float *)(param_2 + 0x58) * *(float *)(param_2 + 0x5c8);
  cVar2 = *(char *)(param_2 + 0x4ce);
  *(undefined1 *)(param_2 + 0x4b9) = 0;
  if ((cVar2 == '\x01') || (cVar2 == '\x05')) {
    *pfVar1 = *pfVar1 * DAT_0062eeb0;
    fVar11 = *(float *)(param_2 + 0x5c8) * DAT_0062eeb0;
LAB_004c6a5c:
    *(float *)(param_2 + 0x5c8) = fVar11;
  }
  else if ((cVar2 == '\x02') || (cVar2 == '\x06')) {
    *pfVar1 = DAT_0062eeb0 * _DAT_005a7320 * *pfVar1;
    fVar11 = DAT_0062eeb0 * _DAT_005a7320 * *(float *)(param_2 + 0x5c8);
    goto LAB_004c6a5c;
  }
  puVar6 = (undefined8 *)FUN_00404ae0();
  fVar11 = *(float *)(puVar6 + 1);
  local_e0 = (float)*puVar6;
  fStack_dc = (float)((ulonglong)*puVar6 >> 0x20);
  switch(*(undefined1 *)(param_2 + 0x4cb)) {
  case 0:
    if (local_e0 != 0.0) {
      D3DXMatrixRotationX(local_50,local_e0);
      D3DXMatrixMultiply(pfVar1,pfVar1,local_50);
    }
    if (fStack_dc != 0.0) {
      D3DXMatrixRotationY(local_50,fStack_dc);
      D3DXMatrixMultiply(pfVar1,pfVar1,local_50);
    }
    if (fVar11 == 0.0) break;
    D3DXMatrixRotationZ(local_50,fVar11);
    goto LAB_004c6df0;
  case 1:
    if (local_e0 != 0.0) {
      D3DXMatrixRotationX(local_50,local_e0);
      D3DXMatrixMultiply(pfVar1,pfVar1,local_50);
    }
    if (fVar11 != 0.0) {
      D3DXMatrixRotationZ(local_50,fVar11);
      D3DXMatrixMultiply(pfVar1,pfVar1,local_50);
    }
    if (fStack_dc != 0.0) {
      D3DXMatrixRotationY(local_50,fStack_dc);
      goto LAB_004c6df0;
    }
    break;
  case 2:
    if (fStack_dc != 0.0) {
      D3DXMatrixRotationY(local_50,fStack_dc);
      D3DXMatrixMultiply(pfVar1,pfVar1,local_50);
    }
    if (local_e0 != 0.0) {
      D3DXMatrixRotationX(local_50,local_e0);
      D3DXMatrixMultiply(pfVar1,pfVar1,local_50);
    }
    if (fVar11 != 0.0) {
      D3DXMatrixRotationZ(local_50,fVar11);
      goto LAB_004c6df0;
    }
    break;
  case 3:
    if (fStack_dc != 0.0) {
      D3DXMatrixRotationY(local_50,fStack_dc);
      D3DXMatrixMultiply(pfVar1,pfVar1,local_50);
    }
    if (fVar11 != 0.0) {
      D3DXMatrixRotationZ(local_50,fVar11);
LAB_004c6dbc:
      D3DXMatrixMultiply(pfVar1,pfVar1,local_50);
    }
    goto LAB_004c6dcb;
  case 4:
    if (fVar11 != 0.0) {
      D3DXMatrixRotationZ(local_50,fVar11);
      D3DXMatrixMultiply(pfVar1,pfVar1,local_50);
    }
    if (local_e0 != 0.0) {
      D3DXMatrixRotationX(local_50,local_e0);
      D3DXMatrixMultiply(pfVar1,pfVar1,local_50);
    }
    if (fStack_dc != 0.0) {
      D3DXMatrixRotationY(local_50,fStack_dc);
      goto LAB_004c6df0;
    }
    break;
  case 5:
    if (fVar11 != 0.0) {
      D3DXMatrixRotationZ(local_50,fVar11);
      D3DXMatrixMultiply(pfVar1,pfVar1,local_50);
    }
    if (fStack_dc != 0.0) {
      D3DXMatrixRotationY(local_50,fStack_dc);
      goto LAB_004c6dbc;
    }
LAB_004c6dcb:
    if (local_e0 != 0.0) {
      D3DXMatrixRotationX(local_50,local_e0);
LAB_004c6df0:
      D3DXMatrixMultiply(pfVar1,pfVar1,local_50);
    }
  }
  *(undefined1 *)(param_2 + 0x4b8) = 0;
LAB_004c6e03:
  local_90 = *(undefined4 *)(param_2 + 0x5b4);
  uStack_8c = *(undefined4 *)(param_2 + 0x5b8);
  uStack_88 = *(undefined4 *)(param_2 + 0x5bc);
  uStack_84 = *(undefined4 *)(param_2 + 0x5c0);
  local_80 = *(undefined4 *)(param_2 + 0x5c4);
  uStack_7c = *(undefined4 *)(param_2 + 0x5c8);
  uStack_78 = *(undefined4 *)(param_2 + 0x5cc);
  uStack_74 = *(undefined4 *)(param_2 + 0x5d0);
  local_70 = *(undefined4 *)(param_2 + 0x5d4);
  uStack_6c = *(undefined4 *)(param_2 + 0x5d8);
  uStack_68 = *(undefined4 *)(param_2 + 0x5dc);
  uStack_64 = *(undefined4 *)(param_2 + 0x5e0);
  fStack_58 = *(float *)(param_2 + 0x5ec);
  uStack_54 = *(undefined4 *)(param_2 + 0x5f0);
  local_60 = (*(float *)(param_2 + 0x614) + *(float *)(param_2 + 0x30) + *(float *)(param_2 + 0x4a0)
             ) - *(float *)(param_2 + 0x84) * *(float *)(param_2 + 0x54) *
                 *(float *)(param_2 + 0x5c);
  fStack_5c = (*(float *)(param_2 + 0x618) + *(float *)(param_2 + 0x34) +
              *(float *)(param_2 + 0x4a4)) -
              *(float *)(param_2 + 0x88) * *(float *)(param_2 + 0x58) * *(float *)(param_2 + 0x60);
  FUN_00404050(&local_60);
  FUN_004c9dc0(param_2);
  if (*(char *)(param_2 + 0x4c6) == '\0') {
    uVar9 = *(uint *)(param_2 + 0x4ac);
  }
  else {
    uVar9 = *(uint *)(param_2 + 0x4b0);
  }
  local_d4 = uVar9;
  if (*(int *)(param_1 + 0x8200f84) != 0) {
    uVar8 = (uint)*(byte *)(param_1 + 0x8200f82) * (uVar9 >> 0x10 & 0xff) >> 7;
    if (0xff < uVar8) {
      uVar8 = 0xff;
    }
    local_d4._3_1_ = (byte)(uVar9 >> 0x18);
    bVar5 = local_d4._3_1_;
    local_d4._0_3_ = CONCAT12((char)uVar8,(short)uVar9);
    uVar8 = (uint)(uint3)local_d4;
    uVar9 = (uint)*(byte *)(param_1 + 0x8200f81) * (uVar9 >> 8 & 0xff) >> 7;
    if (0xff < uVar9) {
      uVar9 = 0xff;
    }
    uVar8 = (uint)*(byte *)(param_1 + 0x8200f80) * (uVar8 & 0xff) >> 7;
    if (0xff < uVar8) {
      uVar8 = 0xff;
    }
    local_d4 = CONCAT31(CONCAT21(local_d4._2_2_,(char)uVar9),(char)uVar8);
    uVar9 = (uint)*(byte *)(param_1 + 0x8200f83) * (uint)bVar5 >> 7;
    if (0xff < uVar9) {
      uVar9 = 0xff;
    }
    local_d4 = CONCAT13((char)uVar9,(uint3)local_d4);
  }
  if (*(uint *)(param_1 + 0x64c0ef8) != local_d4) {
    FUN_004ca360();
    *(uint *)(param_1 + 0x64c0ef8) = local_d4;
    (**(code **)(*DAT_0062bb48 + 0xe4))(DAT_0062bb48,0x3c,local_d4);
  }
  fStack_58 = *(float *)(param_2 + 0x61c) + *(float *)(param_2 + 0x38) + *(float *)(param_2 + 0x4a8)
  ;
  (**(code **)(*DAT_0062bb48 + 0xb0))(DAT_0062bb48,0x100,&local_90);
  iVar10 = *(int *)(param_2 + 0x24) * 0x58 +
           *(int *)(*(int *)(DAT_005d19bc + 0x64c0780 + *(int *)(param_2 + 0x20) * 4) + 0x220);
  if (*(int *)(param_1 + 0x64c0efc) != *(int *)(iVar10 + 8)) {
    *(int *)(param_1 + 0x64c0efc) = *(int *)(iVar10 + 8);
    FUN_004ca360();
    piVar4 = DAT_0062bb48;
    pcVar3 = *(code **)(*DAT_0062bb48 + 0x104);
    uVar7 = FUN_00405030(*(undefined4 *)(param_1 + 0x64c0efc));
    (*pcVar3)(piVar4,0,uVar7);
  }
  if ((((*(int *)(param_1 + 0x64c0f0c) != iVar10) || (*(float *)(param_2 + 0x7c) != 0.0)) ||
      (*(float *)(param_2 + 0x7c) != 0.0)) ||
     ((*(float *)(param_2 + 0x6c) != DAT_005aa230 || (*(float *)(param_2 + 0x70) != DAT_005aa230))))
  {
    *(int *)(param_1 + 0x64c0f0c) = iVar10;
    uStack_a8 = *(undefined4 *)(param_2 + 0x43c);
    uStack_a4 = *(undefined4 *)(param_2 + 0x440);
    local_c0 = *(undefined4 *)(param_2 + 0x424);
    uStack_b8 = *(undefined4 *)(param_2 + 0x42c);
    uStack_b4 = *(undefined4 *)(param_2 + 0x430);
    uStack_cc = *(undefined4 *)(param_2 + 0x418);
    uStack_c8 = *(undefined4 *)(param_2 + 0x41c);
    uStack_c4 = *(undefined4 *)(param_2 + 0x420);
    local_a0 = *(undefined4 *)(param_2 + 0x444);
    uStack_9c = *(undefined4 *)(param_2 + 0x448);
    uStack_98 = *(undefined4 *)(param_2 + 0x44c);
    uStack_94 = *(undefined4 *)(param_2 + 0x450);
    local_b0 = *(float *)(param_2 + 0x3ac) + *(float *)(param_2 + 0x7c);
    local_d0 = *(float *)(param_2 + 0x414) * *(float *)(param_2 + 0x6c);
    fStack_ac = *(float *)(param_2 + 0x3b0) + *(float *)(param_2 + 0x80);
    fStack_bc = *(float *)(param_2 + 0x428) * *(float *)(param_2 + 0x70);
    (**(code **)(*DAT_0062bb48 + 0xb0))(DAT_0062bb48,0x10,&local_d0);
  }
  if (*(char *)(param_1 + 0x64c0f02) != '\x02') {
    (**(code **)(*DAT_0062bb48 + 400))(DAT_0062bb48,0,*(undefined4 *)(param_1 + 0x64c0f10),0,0x14);
    (**(code **)(*DAT_0062bb48 + 0x164))(DAT_0062bb48,0x102);
    *(undefined1 *)(param_1 + 0x64c0f02) = 2;
  }
  FUN_004ca0e0(1);
  (**(code **)(*DAT_0062bb48 + 0x144))
            (DAT_0062bb48,5,
             ((uint)*(byte *)(param_2 + 0x4c8) * 3 + (uint)*(byte *)(param_2 + 0x4c7)) * 4,2);
  return 0;
}


// Reference 004c52c6
// Reference 004c52f6
