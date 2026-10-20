
// block 0043bbe0
0043bbe0  push       esi
0043bbe1  mov        esi, dword ptr [0x4b4518]
0043bbe7  mov        eax, dword ptr [esi + 0x1c]
0043bbea  add        eax, 0xc
0043bbed  push       eax
0043bbee  call       0x46d5b7

// block 0043bbf3
0043bbf3  add        esp, 4
0043bbf6  lea        eax, [edi + 7]
0043bbf9  test       edi, edi
0043bbfb  jne        0x43bc02

// block 0043bbfd
0043bbfd  mov        eax, dword ptr [0x4b0cb0]

// block 0043bc02
0043bc02  mov        ecx, dword ptr [esi + 0x1c]
0043bc05  mov        dword ptr [ecx + 0x68], eax
0043bc08  xor        eax, eax
0043bc0a  pop        esi
0043bc0b  ret        
