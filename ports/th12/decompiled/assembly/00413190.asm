
// block 00413190
00413190  mov        eax, dword ptr [esp + 4]
00413194  push       esi
00413195  mov        esi, dword ptr [eax + 0x68]
00413198  test       esi, esi
0041319a  je         0x4131e1

// block 0041319c
0041319c  push       edi
0041319d  lea        ecx, [ecx]

// block 004131a0
004131a0  mov        eax, dword ptr [esi]
004131a2  test       byte ptr [eax + 0x26fb], 1
004131a9  mov        edi, dword ptr [esi + 4]
004131ac  jne        0x4131cb

// block 004131ae
004131ae  add        eax, 0x1040
004131b3  push       eax
004131b4  call       0x413840

// block 004131b9
004131b9  test       eax, eax
004131bb  jne        0x4131cb

// block 004131bd
004131bd  mov        esi, dword ptr [esi]
004131bf  and        dword ptr [esi + 0x26f8], 0xfffdffff
004131c9  jmp        0x4131da

// block 004131cb
004131cb  mov        ecx, dword ptr [esi]
004131cd  test       ecx, ecx
004131cf  je         0x4131da

// block 004131d1
004131d1  mov        edx, dword ptr [ecx]
004131d3  mov        eax, dword ptr [edx + 0x14]
004131d6  push       1
004131d8  call       eax

// block 004131da
004131da  mov        esi, edi
004131dc  test       edi, edi
004131de  jne        0x4131a0

// block 004131e0
004131e0  pop        edi

// block 004131e1
004131e1  mov        esi, dword ptr [esp + 8]
004131e5  add        esi, 0x50
004131e8  call       0x464a80

// block 004131ed
004131ed  mov        eax, 1
004131f2  pop        esi
004131f3  ret        4
