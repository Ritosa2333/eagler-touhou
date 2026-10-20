
// block 004652f0
004652f0  fld        dword ptr [esp + 4]
004652f4  sub        esp, 8
004652f7  fstp       dword ptr [esp + 4]
004652fb  fld        dword ptr [eax]
004652fd  fstp       dword ptr [esp]
00465300  call       0x465280

// block 00465305
00465305  push       ecx
00465306  fstp       dword ptr [esp]
00465309  call       0x4646e0

// block 0046530e
0046530e  fstp       dword ptr [esi]
00465310  mov        eax, esi
00465312  ret        4
