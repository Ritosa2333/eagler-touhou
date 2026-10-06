
// block 004390f0
004390f0  push       ebp
004390f1  mov        ebp, dword ptr [esp + 8]
004390f5  push       esi
004390f6  mov        esi, eax
004390f8  add        esi, 0x8988
004390fe  xor        eax, eax

// block 00439100
00439100  test       byte ptr [esi + 0x70], 1
00439104  je         0x439118

// block 00439106
00439106  inc        eax
00439107  add        esi, 0x74
0043910a  cmp        eax, 0x80
0043910f  jl         0x439100

// block 00439111
00439111  mov        eax, esi
00439113  pop        esi
00439114  pop        ebp
00439115  ret        0x14

// block 00439118
00439118  push       edi
00439119  push       0x74
0043911b  push       0
0043911d  push       esi
0043911e  call       0x477420

// block 00439123
00439123  or         dword ptr [esi + 0x70], 3
00439127  push       0x34
00439129  lea        edi, [esi + 0x18]
0043912c  push       0
0043912e  push       edi
0043912f  call       0x477420

// block 00439134
00439134  fld        dword ptr [esp + 0x2c]
00439138  mov        eax, dword ptr [ebp]
0043913b  mov        dword ptr [edi], eax
0043913d  mov        ecx, dword ptr [ebp + 4]
00439140  mov        dword ptr [edi + 4], ecx
00439143  mov        edx, dword ptr [ebp + 8]
00439146  fstp       dword ptr [esi]
00439148  fld        dword ptr [esp + 0x30]
0043914c  mov        dword ptr [edi + 8], edx
0043914f  fstp       dword ptr [esi + 4]
00439152  mov        eax, dword ptr [esi + 0x5c]
00439155  add        esp, 0x18
00439158  pop        edi
00439159  test       al, 1
0043915b  jne        0x43917d

// block 0043915d
0043915d  fldz       
0043915f  or         eax, 1
00439162  fstp       dword ptr [esi + 0x54]
00439165  mov        dword ptr [esi + 0x50], 0
0043916c  mov        dword ptr [esi + 0x4c], 0xfff0bdc1
00439173  mov        dword ptr [esi + 0x58], 0x4b2ed0
0043917a  mov        dword ptr [esi + 0x5c], eax

// block 0043917d
0043917d  mov        eax, dword ptr [esp + 0x18]
00439181  fild       dword ptr [esp + 0x18]
00439185  mov        dword ptr [esi + 0x50], eax
00439188  dec        eax
00439189  mov        dword ptr [esi + 0x4c], eax
0043918c  fstp       dword ptr [esi + 0x54]
0043918f  mov        eax, dword ptr [esp + 0x1c]
00439193  mov        dword ptr [esi + 0x60], eax
00439196  mov        dword ptr [esi + 0x64], 0
0043919d  mov        dword ptr [esi + 0x68], 0xf423f
004391a4  mov        dword ptr [esi + 0x6c], 4
004391ab  mov        eax, esi
004391ad  pop        esi
004391ae  pop        ebp
004391af  ret        0x14
