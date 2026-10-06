
// block 004399d0
004399d0  push       ecx
004399d1  push       ebx
004399d2  mov        ebx, eax
004399d4  mov        eax, dword ptr [0x4b0c48]
004399d9  cdq        
004399da  idiv       dword ptr [0x4b0cd4]
004399e0  cmp        dword ptr [ebx + 0xc598], 0
004399e7  push       esi
004399e8  je         0x4399f7

// block 004399ea
004399ea  mov        ecx, dword ptr [ebx + 0xa2c]
004399f0  mov        edx, dword ptr [ecx + 0x20]
004399f3  lea        eax, [eax + edx + 1]

// block 004399f7
004399f7  mov        ecx, dword ptr [ebx + 0xa2c]
004399fd  mov        esi, dword ptr [ecx + eax*8 + 0x268]
00439a04  mov        al, byte ptr [esi]
00439a06  test       al, al
00439a08  jl         0x439a37

// block 00439a0a
00439a0a  lea        ebx, [ebx]

// block 00439a10
00439a10  movsx      ecx, al
00439a13  mov        eax, edi
00439a15  cdq        
00439a16  idiv       ecx
00439a18  movsx      eax, byte ptr [esi + 1]
00439a1c  cmp        edx, eax
00439a1e  jne        0x439a2d

// block 00439a20
00439a20  push       edi
00439a21  lea        ecx, [ebx + 0x97c]
00439a27  push       esi
00439a28  call       0x439630

// block 00439a2d
00439a2d  mov        al, byte ptr [esi + 0x34]
00439a30  add        esi, 0x34
00439a33  test       al, al
00439a35  jge        0x439a10

// block 00439a37
00439a37  pop        esi
00439a38  xor        eax, eax
00439a3a  pop        ebx
00439a3b  pop        ecx
00439a3c  ret        
