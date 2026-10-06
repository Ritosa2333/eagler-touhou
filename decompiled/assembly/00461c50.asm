
// block 00461c50
00461c50  mov        eax, dword ptr [esi]
00461c52  mov        edx, dword ptr [0x4ce8cc]
00461c58  push       eax
00461c59  call       0x461920

// block 00461c5e
00461c5e  test       eax, eax
00461c60  jne        0x461c64

// block 00461c62
00461c62  mov        dword ptr [esi], eax

// block 00461c64
00461c64  ret        
