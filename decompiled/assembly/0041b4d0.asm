
// block 0041b4d0
0041b4d0  mov        edx, dword ptr [0x4ce8cc]
0041b4d6  sub        esp, 0x10
0041b4d9  push       ebx
0041b4da  push       esi
0041b4db  mov        esi, ecx
0041b4dd  mov        eax, dword ptr [esi + 0xe0]
0041b4e3  push       edi
0041b4e4  push       eax
0041b4e5  call       0x461920

// block 0041b4ea
0041b4ea  mov        ebx, eax
0041b4ec  test       ebx, ebx
0041b4ee  jne        0x41b4f6

// block 0041b4f0
0041b4f0  mov        dword ptr [esi + 0xe0], eax

// block 0041b4f6
0041b4f6  fld        dword ptr [0x4a3e00]
0041b4fc  sub        esp, 8
0041b4ff  fstp       dword ptr [esp + 4]
0041b503  lea        ecx, [esp + 0x18]
0041b507  fld        dword ptr [ebx + 0x2c]
0041b50a  fstp       dword ptr [esp]
0041b50d  call       0x41c580

// block 0041b512
0041b512  fld        dword ptr [esi + 0x34]
0041b515  lea        edi, [esi + 0x34]
0041b518  fadd       dword ptr [esp + 0x10]
0041b51c  push       ecx
0041b51d  lea        eax, [esp + 0x14]
0041b521  fstp       dword ptr [esp + 0x14]
0041b525  fld        dword ptr [edi + 4]
0041b528  fadd       dword ptr [esp + 0x18]
0041b52c  fstp       dword ptr [esp + 0x18]
0041b530  fld        dword ptr [edi + 8]
0041b533  fadd       dword ptr [esp + 0x1c]
0041b537  fstp       dword ptr [esp + 0x1c]
0041b53b  fld        dword ptr [0x4a4010]
0041b541  fstp       dword ptr [esp]
0041b544  call       0x437980

// block 0041b549
0041b549  test       dword ptr [esi + 0x16b8], 0x200
0041b553  je         0x41b57e

// block 0041b555
0041b555  cmp        eax, 2
0041b558  jne        0x41b57e

// block 0041b55a
0041b55a  mov        eax, dword ptr [esi + 0x270]
0041b560  cdq        
0041b561  mov        ecx, 3
0041b566  idiv       ecx
0041b568  test       edx, edx
0041b56a  jne        0x41b57e

// block 0041b56c
0041b56c  mov        edx, dword ptr [0x4b4514]
0041b572  add        edx, 0x97c
0041b578  push       edx
0041b579  call       0x4391c0

// block 0041b57e
0041b57e  fld        dword ptr [ebx + 0x2c]
0041b581  sub        esp, 0xc
0041b584  fstp       dword ptr [esp + 0x18]
0041b588  mov        eax, edi
0041b58a  fld        dword ptr [esi + 0x250]
0041b590  fstp       dword ptr [esp + 8]
0041b594  fld        dword ptr [esi + 0x24c]
0041b59a  fstp       dword ptr [esp + 4]
0041b59e  fld        dword ptr [esp + 0x18]
0041b5a2  fstp       dword ptr [esp]
0041b5a5  call       0x437a80

// block 0041b5aa
0041b5aa  test       dword ptr [esi + 0x16b8], 0x200
0041b5b4  je         0x41b5df

// block 0041b5b6
0041b5b6  cmp        eax, 2
0041b5b9  jne        0x41b5df

// block 0041b5bb
0041b5bb  mov        eax, dword ptr [esi + 0x270]
0041b5c1  cdq        
0041b5c2  mov        ecx, 3
0041b5c7  idiv       ecx
0041b5c9  test       edx, edx
0041b5cb  jne        0x41b5df

// block 0041b5cd
0041b5cd  mov        edx, dword ptr [0x4b4514]
0041b5d3  add        edx, 0x97c
0041b5d9  push       edx
0041b5da  call       0x4391c0

// block 0041b5df
0041b5df  pop        edi
0041b5e0  pop        esi
0041b5e1  xor        eax, eax
0041b5e3  pop        ebx
0041b5e4  add        esp, 0x10
0041b5e7  ret        
