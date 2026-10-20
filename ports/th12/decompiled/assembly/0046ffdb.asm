
// block 0046ffdb
0046ffdb  mov        edi, edi
0046ffdd  push       ebp
0046ffde  mov        ebp, esp
0046ffe0  push       ecx
0046ffe1  push       esi
0046ffe2  mov        esi, dword ptr [ebp + 0xc]
0046ffe5  push       esi
0046ffe6  call       0x47c1da

// block 0046ffeb
0046ffeb  mov        dword ptr [ebp + 0xc], eax
0046ffee  mov        eax, dword ptr [esi + 0xc]
0046fff1  pop        ecx
0046fff2  test       al, 0x82
0046fff4  jne        0x47000d

// block 0046fff6
0046fff6  call       0x46e96f

// block 0046fffb
0046fffb  mov        dword ptr [eax], 9

// block 00470001
00470001  or         dword ptr [esi + 0xc], 0x20
00470005  or         eax, 0xffffffff
00470008  jmp        0x47013c

// block 0047000d
0047000d  test       al, 0x40
0047000f  je         0x47001e

// block 00470011
00470011  call       0x46e96f

// block 00470016
00470016  mov        dword ptr [eax], 0x22
0047001c  jmp        0x470001

// block 0047001e
0047001e  push       ebx
0047001f  xor        ebx, ebx
00470021  test       al, 1
00470023  je         0x47003b

// block 00470025
00470025  mov        dword ptr [esi + 4], ebx
00470028  test       al, 0x10
0047002a  je         0x4700b7

// block 00470030
00470030  mov        ecx, dword ptr [esi + 8]
00470033  and        eax, 0xfffffffe
00470036  mov        dword ptr [esi], ecx
00470038  mov        dword ptr [esi + 0xc], eax

// block 0047003b
0047003b  mov        eax, dword ptr [esi + 0xc]
0047003e  and        eax, 0xffffffef
00470041  or         eax, 2
00470044  mov        dword ptr [esi + 0xc], eax
00470047  mov        dword ptr [esi + 4], ebx
0047004a  mov        dword ptr [ebp - 4], ebx
0047004d  test       eax, 0x10c
00470052  jne        0x470080

// block 00470054
00470054  call       0x47c025

// block 00470059
00470059  add        eax, 0x20
0047005c  cmp        esi, eax
0047005e  je         0x47006c

// block 00470060
00470060  call       0x47c025

// block 00470065
00470065  add        eax, 0x40
00470068  cmp        esi, eax
0047006a  jne        0x470079

// block 0047006c
0047006c  push       dword ptr [ebp + 0xc]
0047006f  call       0x47bfc1

// block 00470074
00470074  pop        ecx
00470075  test       eax, eax
00470077  jne        0x470080

// block 00470079
00470079  push       esi
0047007a  call       0x47bf78

// block 0047007f
0047007f  pop        ecx

// block 00470080
00470080  test       dword ptr [esi + 0xc], 0x108
00470087  push       edi
00470088  je         0x47010e

// block 0047008e
0047008e  mov        eax, dword ptr [esi + 8]
00470091  mov        edi, dword ptr [esi]
00470093  lea        ecx, [eax + 1]
00470096  mov        dword ptr [esi], ecx
00470098  mov        ecx, dword ptr [esi + 0x18]
0047009b  sub        edi, eax
0047009d  dec        ecx
0047009e  cmp        edi, ebx
004700a0  mov        dword ptr [esi + 4], ecx
004700a3  jle        0x4700c2

// block 004700a5
004700a5  push       edi
004700a6  push       eax
004700a7  push       dword ptr [ebp + 0xc]
004700aa  call       0x47be9c

// block 004700af
004700af  add        esp, 0xc
004700b2  mov        dword ptr [ebp - 4], eax
004700b5  jmp        0x470104

// block 004700b7
004700b7  or         eax, 0x20
004700ba  mov        dword ptr [esi + 0xc], eax
004700bd  or         eax, 0xffffffff
004700c0  jmp        0x47013b

// block 004700c2
004700c2  mov        ecx, dword ptr [ebp + 0xc]
004700c5  cmp        ecx, -1
004700c8  je         0x4700e5

// block 004700ca
004700ca  cmp        ecx, -2
004700cd  je         0x4700e5

// block 004700cf
004700cf  mov        eax, ecx
004700d1  and        eax, 0x1f
004700d4  mov        edx, ecx
004700d6  sar        edx, 5
004700d9  shl        eax, 6
004700dc  add        eax, dword ptr [edx*4 + 0x4d6320]
004700e3  jmp        0x4700ea

// block 004700e5
004700e5  mov        eax, 0x4adbb0

// block 004700ea
004700ea  test       byte ptr [eax + 4], 0x20
004700ee  je         0x470104

// block 004700f0
004700f0  push       2
004700f2  push       ebx
004700f3  push       ebx
004700f4  push       ecx
004700f5  call       0x47b650

// block 004700fa
004700fa  and        eax, edx
004700fc  add        esp, 0x10
004700ff  cmp        eax, -1
00470102  je         0x470129

// block 00470104
00470104  mov        eax, dword ptr [esi + 8]
00470107  mov        cl, byte ptr [ebp + 8]
0047010a  mov        byte ptr [eax], cl
0047010c  jmp        0x470124

// block 0047010e
0047010e  xor        edi, edi
00470110  inc        edi
00470111  push       edi
00470112  lea        eax, [ebp + 8]
00470115  push       eax
00470116  push       dword ptr [ebp + 0xc]
00470119  call       0x47be9c

// block 0047011e
0047011e  add        esp, 0xc
00470121  mov        dword ptr [ebp - 4], eax

// block 00470124
00470124  cmp        dword ptr [ebp - 4], edi
00470127  je         0x470132

// block 00470129
00470129  or         dword ptr [esi + 0xc], 0x20
0047012d  or         eax, 0xffffffff
00470130  jmp        0x47013a

// block 00470132
00470132  mov        eax, dword ptr [ebp + 8]
00470135  and        eax, 0xff

// block 0047013a
0047013a  pop        edi

// block 0047013b
0047013b  pop        ebx

// block 0047013c
0047013c  pop        esi
0047013d  leave      
0047013e  ret        
