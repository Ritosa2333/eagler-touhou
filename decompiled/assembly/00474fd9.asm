
// block 00474fd9
00474fd9  push       0x14
00474fdb  push       0x4aacf0
00474fe0  call       0x46fcec

// block 00474fe5
00474fe5  xor        ebx, ebx
00474fe7  mov        dword ptr [ebp - 0x20], ebx
00474fea  cmp        dword ptr [ebp + 8], 5
00474fee  jbe        0x47500f

// block 00474ff0
00474ff0  call       0x46e96f

// block 00474ff5
00474ff5  mov        dword ptr [eax], 0x16
00474ffb  push       ebx
00474ffc  push       ebx
00474ffd  push       ebx
00474ffe  push       ebx
00474fff  push       ebx
00475000  call       0x470f2d

// block 00475005
00475005  add        esp, 0x14
00475008  xor        eax, eax
0047500a  jmp        0x47513b

// block 0047500f
0047500f  call       0x475467

// block 00475014
00475014  mov        esi, eax
00475016  mov        dword ptr [ebp - 0x1c], esi
00475019  call       0x474181

// block 0047501e
0047501e  or         dword ptr [esi + 0x70], 0x10
00475022  mov        dword ptr [ebp - 4], ebx
00475025  push       1
00475027  push       0xd8
0047502c  call       0x477813

// block 00475031
00475031  pop        ecx
00475032  pop        ecx
00475033  mov        edi, eax
00475035  mov        dword ptr [ebp - 0x24], edi
00475038  cmp        edi, ebx
0047503a  je         0x47512c

// block 00475040
00475040  push       0xc
00475042  call       0x46ec9a

// block 00475047
00475047  pop        ecx
00475048  mov        dword ptr [ebp - 4], 1
0047504f  mov        ecx, dword ptr [esi + 0x6c]
00475052  mov        eax, edi
00475054  call       0x47411d

// block 00475059
00475059  mov        dword ptr [ebp - 4], ebx
0047505c  call       0x475107

// block 00475061
00475061  push       dword ptr [ebp + 8]
00475064  mov        ecx, dword ptr [ebp + 0xc]
00475067  mov        edx, edi
00475069  call       0x474cbe

// block 0047506e
0047506e  pop        ecx
0047506f  mov        dword ptr [ebp - 0x20], eax
00475072  cmp        eax, ebx
00475074  je         0x47511e

// block 0047507a
0047507a  cmp        dword ptr [ebp + 0xc], ebx
0047507d  je         0x47509c

// block 0047507f
0047507f  push       0x4ad9d8
00475084  push       dword ptr [ebp + 0xc]
00475087  call       0x472ac0

// block 0047508c
0047508c  pop        ecx
0047508d  pop        ecx
0047508e  test       eax, eax
00475090  je         0x47509c

// block 00475092
00475092  mov        dword ptr [0x4b40dc], 1

// block 0047509c
0047509c  push       0xc
0047509e  call       0x46ec9a

// block 004750a3
004750a3  pop        ecx
004750a4  mov        dword ptr [ebp - 4], 2
004750ab  lea        ebx, [esi + 0x6c]
004750ae  mov        eax, ebx
004750b0  call       0x474143

// block 004750b5
004750b5  push       edi
004750b6  call       0x474084

// block 004750bb
004750bb  pop        ecx
004750bc  test       byte ptr [esi + 0x70], 2
004750c0  jne        0x4750f4

// block 004750c2
004750c2  test       byte ptr [0x4ad9d4], 1
004750c9  jne        0x4750f4

// block 004750cb
004750cb  mov        edi, dword ptr [ebx]
004750cd  mov        eax, 0x4adab8
004750d2  call       0x474143

// block 004750d7
004750d7  push       0x18
004750d9  mov        eax, dword ptr [0x4adab8]
004750de  add        eax, 0xc
004750e1  push       eax
004750e2  push       0x4b40e0
004750e7  call       0x47a330

// block 004750ec
004750ec  add        esp, 0xc
004750ef  call       0x474261

// block 004750f4
004750f4  and        dword ptr [ebp - 4], 0
004750f8  call       0x475113

// block 004750fd
004750fd  jmp        0x47512c

// block 0047511e
0047511e  push       edi
0047511f  call       0x474084

// block 00475124
00475124  push       edi
00475125  call       0x473eac

// block 0047512a
0047512a  pop        ecx
0047512b  pop        ecx

// block 0047512c
0047512c  mov        dword ptr [ebp - 4], 0xfffffffe
00475133  call       0x475144

// block 00475138
00475138  mov        eax, dword ptr [ebp - 0x20]

// block 0047513b
0047513b  call       0x46fd31

// block 00475140
00475140  ret        
