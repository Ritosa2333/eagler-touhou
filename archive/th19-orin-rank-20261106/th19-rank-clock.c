// Program SHA256: 8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9
// Address 4026c0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004026c0(int *param_1)

{
  float fVar1;
  int iVar2;
  undefined *puVar3;
  float fVar4;
  
  if (param_1[3] != 0) {
    param_1[3] = 0;
  }
  puVar3 = PTR_DAT_00596da0;
  iVar2 = param_1[1];
  *param_1 = iVar2;
  if (puVar3 != (undefined *)0x0) {
    if ((_DAT_005aa22c < *(float *)puVar3) &&
       (*(float *)puVar3 <= DAT_005aa234 && DAT_005aa234 != *(float *)puVar3)) {
      param_1[2] = (int)((float)param_1[2] + DAT_005aa230);
      param_1[1] = iVar2 + 1;
      return;
    }
    fVar4 = *(float *)puVar3;
    fVar1 = (float)param_1[2];
    param_1[2] = (int)(fVar4 + fVar1);
    param_1[1] = (int)(fVar4 + fVar1);
    return;
  }
  fVar4 = (float)param_1[2] + DAT_005aa230;
  param_1[1] = iVar2 + 1;
  param_1[2] = (int)fVar4;
  return;
}


// Reference 00402c40
// Reference 00402de3
// Reference 00403016
// Reference 00403260
// Reference 0040343f
// Reference 004037f6
// Reference 0040bf06
// Reference 004108d6
// Reference 00416efa
// Reference 00417a03
// Reference 0041819f
// Reference 00418679
// Reference 00418ba0
// Reference 00508e9e
// Reference 00508ed4
// Reference 00508ee0
// Reference 0041900f
// Reference 00419547
// Reference 00419fd6
// Reference 0041a70f
// Reference 0041a81d
// Reference 0041b407
// Reference 0041bb0b
// Reference 0041c21f
// Reference 0041c9a5
// Reference 0041d00c
// Reference 0041d400
// Reference 0041d483
// Reference 0041d8e6
// Reference 0041e102
// Reference 0041eb0e
// Reference 0041f019
// Reference 0041fcd3
// Reference 00420248
// Reference 00420a68
// Reference 00420f66
// Reference 004217cf
// Reference 00421ecc
// Reference 00422589
// Reference 004226a5
// Reference 00422cde
// Reference 004238fe
// Reference 00423e2c
// Reference 00424228
// Reference 004242b5
// Reference 00424708
// Reference 00424e92
// Reference 00425800
// Reference 00425cf8
// Reference 004264b9
// Reference 00426dc8
// Reference 00426e57
// Reference 00427368
// Reference 00427d57
// Reference 00428b0d
// Reference 00428b9c
// Reference 0042912e
// Reference 0042a19d
// Reference 0042a6ab
// Reference 0042a745
// Reference 0042ac5e
// Reference 0042b321
// Reference 0042b9ff
// Reference 0042bbe4
// Reference 0042c09e
// Reference 0042cb36
// Reference 0042cd78
// Reference 0042ce07
// Reference 0042d38c
// Reference 0042e2f8
// Reference 0042ebc0
// Reference 0042ec56
// Reference 004ba056
// Reference 004ba269
// Reference 004ba580
// Reference 004ba5da
// Reference 004ba6f0
// Reference 004ba73b
// Reference 004ba774
// Reference 004ba865
// Reference 004babb1
// Reference 004da817
// Reference 004daee5
// Reference 004db442
// Reference 004dc426
// Reference 004dc9f9
// Reference 004dd813
// Reference 004e35c1
// Reference 004e54a3
// Reference 004e7a89
// Reference 004e7cb6
// Reference 004e8587
// Reference 004eb207
// Reference 004edae1
// Reference 004ef6bb
// Reference 004ef8cc
// Reference 004f0ac8
// Reference 004f0c69
// Reference 004f0e95
// Reference 004f10c5
// Reference 004f116d
// Reference 004f52ed
// Reference 004f54f4
// Reference 004f5500
// Reference 004f5699
// Reference 004f6f27
// Reference 0050b408
// Reference 0050cc49
// Reference 0050ce49
// Reference 0050d03c
// Reference 0050e0c4
// Reference 005121f6
// Reference 00512687
// Reference 00512a29
// Reference 00512b26
// Reference 00513868
// Reference 00513966
// Reference 005169a9
// Reference 00516cad
// Reference 00519c41
// Reference 0051aed2
// Reference 0051aff3
// Reference 0051b373
// Reference 0051b398
// Reference 0051b3bd
// Reference 0051b48f
// Reference 0051c125
// Reference 0051cc55
// Reference 0051dbe1
// Reference 0051e061
// Reference 0051fbef
// Reference 00522369
// Reference 005224b3
// Reference 005258e8
// Reference 00525a6d
// Reference 00525b78
// Reference 00525d39
// Reference 00527e2d
// Reference 0052b418
// Reference 0052df7d
// Reference 0052e0ae
// Reference 0052e6b0
// Reference 0052e82d
// Reference 0052e9e9
// Reference 005308dc
// Reference 00536497
// Reference 00537153
// Reference 0053a341
// Reference 0053a34a
// Reference 0053aeaa
// Reference 0053b847
// Reference 0053b8f3
// Reference 0053bd70
// Reference 0053c51f
// Reference 0053ce3e
// Reference 0053d270
// Reference 0053d9bf
// Reference 0053e292
// Reference 0053e2d2
// Reference 0053edb1
// Reference 0053edf1
// Reference 0053f2b0
// Reference 0053facf
// Reference 00540562
// Reference 00541581
// Reference 005415c3
// Reference 00541b1f
// Reference 00544f20
// Reference 00546cb4
// Reference 00546cc0
// Reference 0054f983
// Reference 0054e461
// Reference 0054e895
// Reference 0054e8ff
// Reference 00546f12
// Reference 00553baf
// Reference 0055b0f1
// Reference 0055c124
// Reference 004eb433
// Reference 004eb4d5
// Reference 004eb715
// Reference 004eb71e
// Reference 004eb882
// Reference 004eb91c
// Reference 004eb9b6
// Reference 004f5951
// Reference 004f5b2e
// Reference 00522fc1
// Reference 0051dde3
// Reference 0051df59
// Reference 0051e046
// Reference 0053613b
// Reference 005363af
// Reference 005362a2
// Reference 0053646a
// Reference 005236f8
// Reference 0041af2a
// Reference 00540ac1
// Reference 00507ac0
// Reference 005079c9
// Address 40e4d0

void __fastcall FUN_0040e4d0(int param_1)

{
  int *piVar1;
  int *piVar2;
  int *piVar3;
  int local_c [2];
  
  piVar1 = (int *)(param_1 + 400);
  local_c[1] = 0;
  piVar3 = local_c + 1;
  if (-1 < *piVar1) {
    piVar3 = piVar1;
  }
  piVar2 = local_c;
  if (*piVar1 < 100000) {
    piVar2 = piVar3;
  }
  *piVar1 = *piVar2;
  return;
}


// Reference 00521dbb
// Reference 0051b160
// Reference 004fbcf9
// Reference 00502ff9
// Reference 00503012
// Reference 00503029
// Reference 004f8010
// Reference 004f8297
// Reference 004f88a1
// Reference 004f7d2b
// Reference 004f7990
// Reference 004f7a2a
// Reference 004f85d5
// Reference 00517a5a
// Reference 00517a66
