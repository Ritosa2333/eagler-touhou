
// block 004606b0
004606b0  push       ebp
004606b1  mov        ebp, esp
004606b3  and        esp, 0xfffffff8
004606b6  sub        esp, 0x14
004606b9  push       esi
004606ba  mov        esi, eax
004606bc  test       esi, esi
004606be  jg         0x4606c7

// block 004606c0
004606c0  mov        esi, 0x11
004606c5  jmp        0x4606d0

// block 004606c7
004606c7  cmp        esi, 8
004606ca  jle        0x460755

// block 004606d0
004606d0  mov        eax, dword ptr [ebp + 0xc]
004606d3  fld        dword ptr [eax + 0xc]
004606d6  call       0x4931e0

// block 004606db
004606db  mov        ecx, dword ptr [ebp + 0xc]
004606de  fld        dword ptr [ecx + 0x10]
004606e1  mov        dword ptr [esp + 8], eax
004606e5  call       0x4931e0

// block 004606ea
004606ea  mov        edx, dword ptr [ebp + 0xc]
004606ed  fld        dword ptr [edx + 0x14]
004606f0  mov        dword ptr [esp + 0xc], eax
004606f4  call       0x4931e0

// block 004606f9
004606f9  mov        dword ptr [esp + 0x10], eax
004606fd  mov        eax, dword ptr [ebp + 0xc]
00460700  fld        dword ptr [eax + 0x18]
00460703  call       0x4931e0

// block 00460708
00460708  cmp        dword ptr [ebp + 0x1c], 0
0046070c  mov        dword ptr [esp + 0x14], eax
00460710  jne        0x46073b

// block 00460712
00460712  mov        ecx, dword ptr [ebp + 0x20]
00460715  mov        edx, dword ptr [ebp + 8]
00460718  mov        eax, dword ptr [ebp + 0x18]
0046071b  push       ecx
0046071c  mov        ecx, dword ptr [ebp + 0x14]
0046071f  push       edx
00460720  push       eax
00460721  push       ecx
00460722  push       esi
00460723  lea        edx, [esp + 0x1c]
00460727  push       edx
00460728  mov        edx, dword ptr [ebp + 0x10]
0046072b  mov        eax, edi
0046072d  mov        ecx, ebx
0046072f  call       0x44e660

// block 00460734
00460734  pop        esi
00460735  mov        esp, ebp
00460737  pop        ebp
00460738  ret        0x1c

// block 0046073b
0046073b  mov        eax, dword ptr [ebp + 8]
0046073e  mov        ecx, dword ptr [ebp + 0x14]
00460741  mov        edx, dword ptr [ebp + 0x10]
00460744  push       eax
00460745  push       ebx
00460746  push       ecx
00460747  push       esi
00460748  push       edx
00460749  lea        eax, [esp + 0x1c]
0046074d  push       eax
0046074e  mov        eax, edi
00460750  call       0x44e8e0

// block 00460755
00460755  pop        esi
00460756  mov        esp, ebp
00460758  pop        ebp
00460759  ret        0x1c
