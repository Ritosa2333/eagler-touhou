
// block 00422290
00422290  push       ebx
00422291  push       ebp
00422292  push       esi
00422293  push       edi
00422294  call       0x43d2e0

// block 00422299
00422299  fld1       
0042229b  mov        eax, dword ptr [0x4cee40]
004222a0  fstp       dword ptr [0x4b2ed0]
004222a6  and        dword ptr [0x4b0ce0], 0xfffffffc
004222ad  xor        ebp, ebp
004222af  mov        dword ptr [0x4b450c], ebp
004222b5  mov        dword ptr [0x4b4508], ebp
004222bb  lea        ebx, [ebp + 2]
004222be  cmp        eax, 0xa
004222c1  je         0x422362

// block 004222c7
004222c7  cmp        eax, 0xb
004222ca  je         0x422362

// block 004222d0
004222d0  cmp        eax, 0xc
004222d3  jne        0x4222fb

// block 004222d5
004222d5  fld        dword ptr [0x4a4260]

// block 004222fb
004222fb  cmp        eax, 4
004222fe  jne        0x42231d

// block 00422300
00422300  fld        dword ptr [0x4a4260]

// block 0042231d
0042231d  cmp        eax, 0x10
00422320  je         0x422300

// block 00422322
00422322  cmp        eax, 0xe
00422325  jne        0x422397

// block 00422327
00422327  fld        dword ptr [0x4a4260]

// block 00422362
00422362  fld        dword ptr [0x4a4260]
