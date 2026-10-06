
// block 00472f8d
00472f8d  mov        edi, edi
00472f8f  push       ebp
00472f90  mov        ebp, esp
00472f92  push       ecx
00472f93  push       ecx
00472f94  push       ebx
00472f95  mov        ebx, dword ptr [ebp + 8]
00472f98  push       esi
00472f99  push       edi
00472f9a  xor        esi, esi
00472f9c  xor        edi, edi
00472f9e  mov        dword ptr [ebp - 4], edi

// block 00472fa1
00472fa1  cmp        ebx, dword ptr [edi*8 + 0x4ad3f8]
00472fa8  je         0x472fb3

// block 00472faa
00472faa  inc        edi
00472fab  mov        dword ptr [ebp - 4], edi
00472fae  cmp        edi, 0x17
00472fb1  jb         0x472fa1

// block 00472fb3
00472fb3  cmp        edi, 0x17
00472fb6  jae        0x473133

// block 00472fbc
00472fbc  push       3
00472fbe  call       0x4831b2

// block 00472fc3
00472fc3  pop        ecx
00472fc4  cmp        eax, 1
00472fc7  je         0x473101

// block 00472fcd
00472fcd  push       3
00472fcf  call       0x4831b2

// block 00472fd4
00472fd4  pop        ecx
00472fd5  test       eax, eax
00472fd7  jne        0x472fe6

// block 00472fd9
00472fd9  cmp        dword ptr [0x4ad134], 1
00472fe0  je         0x473101

// block 00472fe6
00472fe6  cmp        ebx, 0xfc
00472fec  je         0x473133

// block 00472ff2
00472ff2  push       0x49d3d0
00472ff7  mov        ebx, 0x314
00472ffc  push       ebx
00472ffd  mov        edi, 0x4b3da8
00473002  push       edi
00473003  call       0x47a2b9

// block 00473008
00473008  add        esp, 0xc
0047300b  test       eax, eax
0047300d  je         0x47301c

// block 0047300f
0047300f  push       esi
00473010  push       esi
00473011  push       esi
00473012  push       esi
00473013  push       esi
00473014  call       0x470dc6

// block 00473019
00473019  add        esp, 0x14

// block 0047301c
0047301c  push       0x104
00473021  mov        esi, 0x4b3dc1
00473026  push       esi
00473027  push       0
00473029  mov        byte ptr [0x4b3ec5], 0
00473030  call       dword ptr [0x4980b4]

// block 00473036
00473036  test       eax, eax
00473038  jne        0x473060

// block 0047303a
0047303a  push       0x49d3b8
0047303f  push       0x2fb
00473044  push       esi
00473045  call       0x47a2b9

// block 0047304a
0047304a  add        esp, 0xc
0047304d  test       eax, eax
0047304f  je         0x473060

// block 00473051
00473051  xor        eax, eax
00473053  push       eax
00473054  push       eax
00473055  push       eax
00473056  push       eax
00473057  push       eax
00473058  call       0x470dc6

// block 0047305d
0047305d  add        esp, 0x14

// block 00473060
00473060  push       esi
00473061  call       0x475860

// block 00473066
00473066  inc        eax
00473067  pop        ecx
00473068  cmp        eax, 0x3c
0047306b  jbe        0x4730a5

// block 0047306d
0047306d  push       esi
0047306e  call       0x475860

// block 00473073
00473073  sub        esi, 0x3b
00473076  add        eax, esi
00473078  push       3
0047307a  mov        ecx, 0x4b40bc
0047307f  push       0x49d3b4
00473084  sub        ecx, eax
00473086  push       ecx
00473087  push       eax
00473088  call       0x4830fd

// block 0047308d
0047308d  add        esp, 0x14
00473090  test       eax, eax
00473092  je         0x4730a5

// block 00473094
00473094  xor        esi, esi
00473096  push       esi
00473097  push       esi
00473098  push       esi
00473099  push       esi
0047309a  push       esi
0047309b  call       0x470dc6

// block 004730a0
004730a0  add        esp, 0x14
004730a3  jmp        0x4730a7

// block 004730a5
004730a5  xor        esi, esi

// block 004730a7
004730a7  push       0x49d3b0
004730ac  push       ebx
004730ad  push       edi
004730ae  call       0x483089

// block 004730b3
004730b3  add        esp, 0xc
004730b6  test       eax, eax
004730b8  je         0x4730c7

// block 004730ba
004730ba  push       esi
004730bb  push       esi
004730bc  push       esi
004730bd  push       esi
004730be  push       esi
004730bf  call       0x470dc6

// block 004730c4
004730c4  add        esp, 0x14

// block 004730c7
004730c7  mov        eax, dword ptr [ebp - 4]
004730ca  push       dword ptr [eax*8 + 0x4ad3fc]
004730d1  push       ebx
004730d2  push       edi
004730d3  call       0x483089

// block 004730d8
004730d8  add        esp, 0xc
004730db  test       eax, eax
004730dd  je         0x4730ec

// block 004730df
004730df  push       esi
004730e0  push       esi
004730e1  push       esi
004730e2  push       esi
004730e3  push       esi
004730e4  call       0x470dc6

// block 004730e9
004730e9  add        esp, 0x14

// block 004730ec
004730ec  push       0x12010
004730f1  push       0x49d388
004730f6  push       edi
004730f7  call       0x482f20

// block 004730fc
004730fc  add        esp, 0xc
004730ff  jmp        0x473133

// block 00473101
00473101  push       -0xc
00473103  call       dword ptr [0x498168]

// block 00473109
00473109  mov        ebx, eax
0047310b  cmp        ebx, esi
0047310d  je         0x473133

// block 0047310f
0047310f  cmp        ebx, -1
00473112  je         0x473133

// block 00473114
00473114  push       0
00473116  lea        eax, [ebp - 8]
00473119  push       eax
0047311a  lea        esi, [edi*8 + 0x4ad3fc]
00473121  push       dword ptr [esi]
00473123  call       0x475860

// block 00473128
00473128  pop        ecx
00473129  push       eax
0047312a  push       dword ptr [esi]
0047312c  push       ebx
0047312d  call       dword ptr [0x4980ac]

// block 00473133
00473133  pop        edi
00473134  pop        esi
00473135  pop        ebx
00473136  leave      
00473137  ret        
