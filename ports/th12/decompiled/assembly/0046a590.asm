
// block 0046a590
0046a590  fld        dword ptr [esi]
0046a592  sub        esp, 8
0046a595  fstp       dword ptr [esp + 4]
0046a599  fld        dword ptr [esp + 0xc]
0046a59d  fstp       dword ptr [esp]
0046a5a0  call       0x46a520

// block 0046a5a5
0046a5a5  fadd       dword ptr [esp + 4]
0046a5a9  push       ecx
0046a5aa  fstp       dword ptr [esp + 8]
0046a5ae  fld        dword ptr [esp + 8]
0046a5b2  fstp       dword ptr [esp]
0046a5b5  call       0x4646e0

// block 0046a5ba
0046a5ba  fstp       dword ptr [esi]
0046a5bc  ret        4
