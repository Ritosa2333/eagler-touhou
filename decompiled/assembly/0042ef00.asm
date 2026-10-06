
// block 0042ef00
0042ef00  push       ecx
0042ef01  push       edi
0042ef02  mov        edi, 1
0042ef07  cmp        dword ptr [esi + 0x4ec], edi
0042ef0d  jne        0x42ef39

// block 0042ef0f
0042ef0f  mov        ecx, dword ptr [esi + 0x4e8]
0042ef15  push       0
0042ef17  push       0
0042ef19  lea        eax, [esp + 0xc]
0042ef1d  push       eax
0042ef1e  push       ecx
0042ef1f  lea        eax, [edi + 0x16]
0042ef22  xor        ecx, ecx
0042ef24  call       0x4615a0

// block 0042ef29
0042ef29  mov        edx, dword ptr [esp + 4]
0042ef2d  add        dword ptr [esi + 0x4ec], edi
0042ef33  mov        dword ptr [esi + 0x4e4], edx

// block 0042ef39
0042ef39  cmp        dword ptr [esi + 0x4f0], edi
0042ef3f  jne        0x42ef62

// block 0042ef41
0042ef41  fld        dword ptr [0x4a4260]

// block 0042ef47
0042ef47  sub        esp, 8
0042ef4a  fstp       dword ptr [esp + 4]
0042ef4e  fld        dword ptr [0x4a3ec0]
0042ef54  fstp       dword ptr [esp]
0042ef57  call       0x411b80

// block 0042ef5c
0042ef5c  add        dword ptr [esi + 0x4f0], edi

// block 0042ef62
0042ef62  add        dword ptr [esi + 0x4f4], edi
0042ef68  mov        eax, edi
0042ef6a  pop        edi
0042ef6b  pop        ecx
0042ef6c  ret        
