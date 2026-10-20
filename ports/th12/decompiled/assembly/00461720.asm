
// block 00461720
00461720  sub        esp, 0xc
00461723  test       dword ptr [0x4cee78], 0x8000
0046172d  push       ebx
0046172e  push       ebp
0046172f  mov        ebp, dword ptr [esp + 0x20]
00461733  push       esi
00461734  mov        ebx, eax
00461736  je         0x461749

// block 00461738
00461738  push       0x4cf1d0
0046173d  call       dword ptr [0x498088]

// block 00461743
00461743  inc        byte ptr [0x4cf221]

// block 00461749
00461749  inc        dword ptr [edi + 0x130]
0046174f  call       0x4621c0

// block 00461754
00461754  fldz       
00461756  mov        esi, eax
00461758  fst        dword ptr [esp + 0xc]
0046175c  mov        eax, dword ptr [ebp + 0x20]
0046175f  fst        dword ptr [esp + 0x10]
00461763  mov        ecx, dword ptr [esp + 0xc]
00461767  fstp       dword ptr [esp + 0x14]
0046176b  mov        edx, dword ptr [esp + 0x10]
0046176f  or         dword ptr [esi + 0x480], 1
00461776  mov        dword ptr [esi + 0x430], ecx
0046177c  mov        ecx, dword ptr [esp + 0x20]
00461780  push       ecx
00461781  mov        dword ptr [esi + 0x20], eax
00461784  mov        eax, dword ptr [esp + 0x18]
00461788  mov        dword ptr [esi + 0x434], edx
0046178e  push       esi
0046178f  mov        ecx, edi
00461791  mov        dword ptr [esi + 0x438], eax
00461797  call       0x454d10

// block 0046179c
0046179c  mov        eax, ebx
0046179e  and        eax, 4
004617a1  mov        dword ptr [esi + 0x3c], ebp
004617a4  je         0x4617b8

// block 004617a6
004617a6  test       bl, 2
004617a9  je         0x4617b8

// block 004617ab
004617ab  mov        ebx, esi
004617ad  lea        eax, [esp + 0x24]
004617b1  call       0x4613d0

// block 004617b6
004617b6  jmp        0x4617ec

// block 004617b8
004617b8  test       eax, eax
004617ba  lea        eax, [esp + 0x24]
004617be  je         0x4617d1

// block 004617c0
004617c0  mov        ebx, esi
004617c2  call       0x461350

// block 004617c7
004617c7  mov        ecx, dword ptr [eax]
004617c9  mov        edx, dword ptr [esp + 0x1c]
004617cd  mov        dword ptr [edx], ecx
004617cf  jmp        0x4617f4

// block 004617d1
004617d1  test       bl, 2
004617d4  mov        ebx, esi
004617d6  je         0x4617e7

// block 004617d8
004617d8  call       0x4612d0

// block 004617dd
004617dd  mov        eax, dword ptr [eax]
004617df  mov        ecx, dword ptr [esp + 0x1c]
004617e3  mov        dword ptr [ecx], eax
004617e5  jmp        0x4617f4

// block 004617e7
004617e7  call       0x461250

// block 004617ec
004617ec  mov        edx, dword ptr [eax]
004617ee  mov        eax, dword ptr [esp + 0x1c]
004617f2  mov        dword ptr [eax], edx

// block 004617f4
004617f4  mov        edx, dword ptr [ebp + 0x14]
004617f7  lea        eax, [ebp + 0x10]
004617fa  lea        ecx, [esi + 0x10]
004617fd  pop        esi
004617fe  pop        ebp
004617ff  pop        ebx
00461800  test       edx, edx
00461802  je         0x46180d

// block 00461804
00461804  mov        dword ptr [ecx + 4], edx
00461807  mov        edx, dword ptr [eax + 4]
0046180a  mov        dword ptr [edx + 8], ecx

// block 0046180d
0046180d  mov        dword ptr [eax + 4], ecx
00461810  mov        dword ptr [ecx + 8], eax
00461813  test       dword ptr [0x4cee78], 0x8000
0046181d  je         0x461830

// block 0046181f
0046181f  push       0x4cf1d0
00461824  call       dword ptr [0x49808c]

// block 0046182a
0046182a  dec        byte ptr [0x4cf221]

// block 00461830
00461830  mov        eax, dword ptr [esp + 0x10]
00461834  add        esp, 0xc
00461837  ret        0xc
