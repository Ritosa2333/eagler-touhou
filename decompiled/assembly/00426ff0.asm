
// block 00426ff0
00426ff0  push       ecx
00426ff1  push       ebx
00426ff2  push       esi
00426ff3  mov        esi, eax
00426ff5  mov        eax, dword ptr [0x4b0c48]
00426ffa  cmp        eax, dword ptr [0x4b0cd0]
00427000  jl         0x427035

// block 00427002
00427002  mov        eax, dword ptr [0x4b0c44]
00427007  inc        eax
00427008  cmp        eax, 0x3b9aca00
0042700d  mov        dword ptr [0x4b0c44], eax
00427012  jl         0x42701e

// block 00427014
00427014  mov        dword ptr [0x4b0c44], 0x3b9ac9ff

// block 0042701e
0042701e  push       -1
00427020  add        esi, 0x96c
00427026  push       esi
00427027  mov        eax, 0x64
0042702c  call       0x43e250

// block 00427031
00427031  pop        esi
00427032  pop        ebx
00427033  pop        ecx
00427034  ret        

// block 00427035
00427035  mov        ebx, 1
0042703a  mov        eax, 0x4b0c40
0042703f  call       0x422d70

// block 00427044
00427044  test       eax, eax
00427046  je         0x4270a9

// block 00427048
00427048  mov        ecx, dword ptr [0x4b4514]
0042704e  push       ecx
0042704f  call       0x4385b0

// block 00427054
00427054  push       0xffffff40
00427059  add        esi, 0x96c
0042705f  push       esi
00427060  or         eax, 0xffffffff
00427063  call       0x43e250

// block 00427068
00427068  fld        dword ptr [esi]
0042706a  push       ecx
0042706b  lea        eax, [ebx + 0xc]
0042706e  fstp       dword ptr [esp]
00427071  call       0x453e20

// block 00427076
00427076  mov        eax, dword ptr [0x4b0ccc]
0042707b  add        eax, 0xc
0042707e  cmp        eax, 0x400
00427083  mov        dword ptr [0x4b0ccc], eax
00427088  jle        0x427098

// block 0042708a
0042708a  mov        dword ptr [0x4b0ccc], 0x400
00427094  pop        esi
00427095  pop        ebx
00427096  pop        ecx
00427097  ret        

// block 00427098
00427098  cmp        eax, 0xfffffc00
0042709d  jge        0x4270a9

// block 0042709f
0042709f  mov        dword ptr [0x4b0ccc], 0xfffffc00

// block 004270a9
004270a9  pop        esi
004270aa  pop        ebx
004270ab  pop        ecx
004270ac  ret        
