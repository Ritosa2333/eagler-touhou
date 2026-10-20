
// block 004301c0
004301c0  sub        esp, 0x104
004301c6  mov        eax, dword ptr [0x4ad138]
004301cb  xor        eax, esp
004301cd  mov        dword ptr [esp + 0x100], eax
004301d4  mov        eax, dword ptr [esp + 0x108]
004301db  lea        edx, [esp]
004301de  sub        edx, eax

// block 004301e0
004301e0  mov        cl, byte ptr [eax]
004301e2  mov        byte ptr [edx + eax], cl
004301e5  inc        eax
004301e6  test       cl, cl
004301e8  jne        0x4301e0

// block 004301ea
004301ea  push       edi
004301eb  lea        eax, [esp + 4]
004301ef  push       0x2e
004301f1  push       eax
004301f2  call       0x46d260

// block 004301f7
004301f7  mov        ecx, dword ptr [0x4b451c]
004301fd  add        esp, 8
00430200  mov        byte ptr [eax + 1], 0x77
00430204  mov        byte ptr [eax + 2], 0x61
00430208  mov        byte ptr [eax + 3], 0x76
0043020c  push       -1
0043020e  push       2
00430210  lea        edi, [esp + 0xc]
00430214  mov        eax, 0x4cf4e8
00430219  mov        byte ptr [ecx + esi + 0x1e9da], 1
00430221  call       0x454960

// block 00430226
00430226  mov        ecx, dword ptr [esp + 0x104]
0043022d  pop        edi
0043022e  xor        ecx, esp
00430230  xor        eax, eax
00430232  call       0x46c932

// block 00430237
00430237  add        esp, 0x104
0043023d  ret        4
