
// block 004308b0
004308b0  or         dword ptr [eax + 0x590], 0x8000
004308ba  push       ebx
004308bb  mov        ebx, dword ptr [0x498090]
004308c1  push       esi
004308c2  push       edi
004308c3  lea        esi, [eax + 0x810]
004308c9  mov        edi, 0xc
004308ce  mov        edi, edi

// block 004308d0
004308d0  push       esi
004308d1  call       ebx

// block 004308d3
004308d3  add        esi, 0x18
004308d6  sub        edi, 1
004308d9  jne        0x4308d0

// block 004308db
004308db  pop        edi
004308dc  pop        esi
004308dd  pop        ebx
004308de  ret        
