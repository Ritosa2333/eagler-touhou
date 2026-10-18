/* Entry: 0x00441c00; symbol: FUN_00441c00; body bytes: 753 */

void __thiscall FUN_00441c00(void *this,int param_1)

{
  float fVar1;
  float fVar2;
  float10 fVar3;
  float fVar4;
  uint local_68;
  int local_5c;
  float local_58;
  float local_54;
  float local_50;
  undefined4 local_4c;
  float local_48 [16];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00441430(this,param_1);
  FUN_0040bc20(local_48,0x10,4,FUN_004398d0);
  FUN_00422e10(&local_58);
  fVar1 = DAT_005c5840[0x54];
  fVar2 = DAT_005c5840[0x55];
  if ((*(byte *)(param_1 + 0x4a1) >> 2 & 7) == 0) {
    local_68 = *(uint *)(param_1 + 0x490);
  }
  else {
    local_68 = *(uint *)(param_1 + 0x494);
  }
  local_4c = local_68;
  for (local_5c = 0; local_5c < 4; local_5c = local_5c + 1) {
    D3DXVec3Transform(local_48 + local_5c * 4,(int)this + local_5c * 0x14 + 0x6000e20,
                      (int)this + 0x60007d8);
    local_58 = local_48[local_5c * 4] - *DAT_005c5840;
    local_54 = local_48[local_5c * 4 + 1] - DAT_005c5840[1];
    local_50 = local_48[local_5c * 4 + 2] - DAT_005c5840[2];
    fVar3 = (float10)FUN_00445700(&local_58);
    fVar4 = (float)fVar3;
    if (fVar4 < DAT_005c5840[0x54] || fVar4 == DAT_005c5840[0x54]) {
      (&DAT_005aef20)[local_5c * 7] = local_4c;
    }
    else {
      fVar4 = (DAT_005c5840[0x54] - fVar4) / (fVar1 - fVar2);
      if (fVar4 < DAT_0056c8cc) {
        *(char *)(&DAT_005aef20 + local_5c * 7) =
             (char)local_4c - (char)(int)(((float)(local_4c & 0xff) - DAT_005c5840[0x56]) * fVar4);
        *(byte *)((int)&DAT_005aef20 + local_5c * 0x1c + 1) =
             local_4c._1_1_ - (char)(int)(((float)local_4c._1_1_ - DAT_005c5840[0x57]) * fVar4);
        *(byte *)((int)&DAT_005aef20 + local_5c * 0x1c + 2) =
             local_4c._2_1_ - (char)(int)(((float)local_4c._2_1_ - DAT_005c5840[0x58]) * fVar4);
        *(undefined1 *)((int)&DAT_005aef20 + local_5c * 0x1c + 3) = local_4c._3_1_;
      }
      else {
        (&DAT_005aef20)[local_5c * 7] = DAT_005c5840[0x5a];
        *(undefined1 *)((int)&DAT_005aef20 + local_5c * 0x1c + 3) = local_4c._3_1_;
      }
    }
  }
  FUN_0043f6a0(this,param_1,2);
  DAT_005aef70 = DAT_0056c8cc;
  DAT_005aef54 = DAT_0056c8cc;
  DAT_005aef38 = DAT_0056c8cc;
  DAT_005aef1c = DAT_0056c8cc;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

