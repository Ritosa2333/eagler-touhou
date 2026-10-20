
// block 00453e20
00453e20  fld        dword ptr [esp + 4]
00453e24  push       ebx
00453e25  fmul       qword ptr [0x4a3d30]
00453e2b  push       esi
00453e2c  mov        esi, eax
00453e2e  lea        eax, [esi + esi*4]
00453e31  fdiv       qword ptr [0x4a3d28]
00453e37  movsx      ebx, word ptr [eax*4 + 0x4ae5aa]
00453e3f  push       edi
00453e40  call       0x4931e0

// block 00453e45
00453e45  xor        ecx, ecx
00453e47  mov        edx, 0x4cf508
00453e4c  lea        esp, [esp]

// block 00453e50
00453e50  mov        edi, dword ptr [edx]
00453e52  test       edi, edi
00453e54  jl         0x453e96

// block 00453e56
00453e56  cmp        edi, esi
00453e58  je         0x453e6c

// block 00453e5a
00453e5a  add        edx, 4
00453e5d  inc        ecx
00453e5e  cmp        edx, 0x4cf538
00453e64  jl         0x453e50

// block 00453e66
00453e66  pop        edi
00453e67  pop        esi
00453e68  pop        ebx
00453e69  ret        4

// block 00453e6c
00453e6c  mov        edx, dword ptr [ecx*4 + 0x4cf538]
00453e73  cmp        edx, 0x80
00453e79  jge        0x453ebe

// block 00453e7b
00453e7b  mov        esi, ecx
00453e7d  shl        esi, 7
00453e80  add        esi, edx
00453e82  pop        edi
00453e83  mov        dword ptr [esi*4 + 0x4cf568], eax
00453e8a  inc        dword ptr [ecx*4 + 0x4cf538]
00453e91  pop        esi
00453e92  pop        ebx
00453e93  ret        4

// block 00453e96
00453e96  cmp        ecx, 0xc
00453e99  jge        0x453ebe

// block 00453e9b
00453e9b  mov        edx, ecx
00453e9d  shl        edx, 9
00453ea0  mov        dword ptr [ecx*4 + 0x4cf508], esi
00453ea7  mov        dword ptr [edx + 0x4cf568], eax
00453ead  inc        dword ptr [ecx*4 + 0x4cf538]
00453eb4  lea        eax, [esi + esi*2]
00453eb7  mov        dword ptr [eax*8 + 0x4d0e74], ebx

// block 00453ebe
00453ebe  pop        edi
00453ebf  pop        esi
00453ec0  pop        ebx
00453ec1  ret        4
