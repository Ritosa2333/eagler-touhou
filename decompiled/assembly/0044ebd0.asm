
// block 0044ebd0
0044ebd0  push       esi
0044ebd1  mov        esi, dword ptr [esp + 8]
0044ebd5  mov        eax, esi
0044ebd7  push       edi
0044ebd8  lea        edi, [eax + 1]
0044ebdb  jmp        0x44ebe0

// block 0044ebe0
0044ebe0  mov        dl, byte ptr [eax]
0044ebe2  inc        eax
0044ebe3  test       dl, dl
0044ebe5  jne        0x44ebe0

// block 0044ebe7
0044ebe7  sub        eax, edi
0044ebe9  push       eax
0044ebea  push       esi
0044ebeb  call       0x44ec80

// block 0044ebf0
0044ebf0  pop        edi
0044ebf1  pop        esi
0044ebf2  ret        4
