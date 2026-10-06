
// block 00413080
00413080  mov        eax, dword ptr [esi + 0x8c]
00413086  mov        dword ptr [esi], 0x49fc28
0041308c  test       eax, eax
0041308e  je         0x4130a3

// block 00413090
00413090  push       eax
00413091  call       0x46c941

// block 00413096
00413096  add        esp, 4
00413099  mov        dword ptr [esi + 0x8c], 0

// block 004130a3
004130a3  push       esi
004130a4  call       0x46ca4f

// block 004130a9
004130a9  add        esp, 4
004130ac  mov        eax, esi
004130ae  ret        
