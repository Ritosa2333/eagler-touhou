
// block 00411060
00411060  push       ebx
00411061  push       0x28
00411063  call       0x46c9ea

// block 00411068
00411068  add        esp, 4
0041106b  test       eax, eax
0041106d  je         0x41109a

// block 0041106f
0041106f  xor        ecx, ecx
00411071  mov        dword ptr [eax], ecx
00411073  mov        dword ptr [eax + 4], ecx
00411076  mov        dword ptr [eax + 8], ecx
00411079  mov        dword ptr [eax + 0xc], ecx
0041107c  mov        dword ptr [eax + 0x10], ecx
0041107f  mov        dword ptr [eax + 0x14], ecx
00411082  mov        dword ptr [eax + 0x18], ecx
00411085  mov        dword ptr [eax + 0x1c], ecx
00411088  mov        dword ptr [eax + 0x20], ecx
0041108b  mov        dword ptr [eax + 0x24], ecx
0041108e  or         dword ptr [eax], 2
00411091  mov        dword ptr [0x4b43d8], eax
00411096  mov        ebx, eax
00411098  jmp        0x41109c

// block 0041109a
0041109a  xor        ebx, ebx

// block 0041109c
0041109c  push       ebx
0041109d  call       0x410c60

// block 004110a2
004110a2  test       eax, eax
004110a4  je         0x4110bc

// block 004110a6
004110a6  test       ebx, ebx
004110a8  je         0x4110b8

// block 004110aa
004110aa  call       0x410ea0

// block 004110af
004110af  push       ebx
004110b0  call       0x46ca4f

// block 004110b5
004110b5  add        esp, 4

// block 004110b8
004110b8  xor        eax, eax
004110ba  pop        ebx
004110bb  ret        

// block 004110bc
004110bc  mov        eax, ebx
004110be  pop        ebx
004110bf  ret        
