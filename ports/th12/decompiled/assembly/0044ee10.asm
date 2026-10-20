
// block 0044ee10
0044ee10  push       ebx
0044ee11  mov        ebx, dword ptr [esp + 8]
0044ee15  push       esi
0044ee16  mov        esi, ecx
0044ee18  cmp        ebx, -2
0044ee1b  jbe        0x44ee22

// block 0044ee1d
0044ee1d  call       0x491687

// block 0044ee22
0044ee22  mov        eax, dword ptr [esi + 0x18]
0044ee25  cmp        eax, ebx
0044ee27  jae        0x44ee42

// block 0044ee29
0044ee29  mov        eax, dword ptr [esi + 0x14]
0044ee2c  push       eax
0044ee2d  push       ebx
0044ee2e  mov        ecx, esi
0044ee30  call       0x44ef10

// block 0044ee35
0044ee35  xor        ecx, ecx
0044ee37  cmp        ecx, ebx
0044ee39  sbb        eax, eax
0044ee3b  pop        esi
0044ee3c  neg        eax
0044ee3e  pop        ebx
0044ee3f  ret        8

// block 0044ee42
0044ee42  cmp        byte ptr [esp + 0x10], 0
0044ee47  je         0x44ee9b

// block 0044ee49
0044ee49  cmp        ebx, 0x10
0044ee4c  jae        0x44ee9b

// block 0044ee4e
0044ee4e  push       edi
0044ee4f  mov        edi, dword ptr [esi + 0x14]
0044ee52  cmp        ebx, edi
0044ee54  jae        0x44ee58

// block 0044ee56
0044ee56  mov        edi, ebx

// block 0044ee58
0044ee58  cmp        eax, 0x10
0044ee5b  jb         0x44ee7e

// block 0044ee5d
0044ee5d  lea        eax, [esi + 4]
0044ee60  push       ebp
0044ee61  mov        ebp, dword ptr [eax]
0044ee63  test       edi, edi
0044ee65  jbe        0x44ee74

// block 0044ee67
0044ee67  push       edi
0044ee68  push       ebp
0044ee69  push       0x10
0044ee6b  push       eax
0044ee6c  call       0x46e210

// block 0044ee71
0044ee71  add        esp, 0x10

// block 0044ee74
0044ee74  push       ebp
0044ee75  call       0x46ca4f

// block 0044ee7a
0044ee7a  add        esp, 4
0044ee7d  pop        ebp

// block 0044ee7e
0044ee7e  mov        dword ptr [esi + 0x14], edi
0044ee81  mov        dword ptr [esi + 0x18], 0xf
0044ee88  xor        ecx, ecx
0044ee8a  mov        byte ptr [esi + edi + 4], 0
0044ee8f  cmp        ecx, ebx
0044ee91  pop        edi
0044ee92  sbb        eax, eax
0044ee94  pop        esi
0044ee95  neg        eax
0044ee97  pop        ebx
0044ee98  ret        8

// block 0044ee9b
0044ee9b  test       ebx, ebx
0044ee9d  jne        0x44eebf

// block 0044ee9f
0044ee9f  mov        dword ptr [esi + 0x14], ebx
0044eea2  cmp        eax, 0x10
0044eea5  jb         0x44eeb9

// block 0044eea7
0044eea7  mov        esi, dword ptr [esi + 4]
0044eeaa  xor        ecx, ecx
0044eeac  cmp        ecx, ebx
0044eeae  mov        byte ptr [esi], bl
0044eeb0  sbb        eax, eax
0044eeb2  pop        esi
0044eeb3  neg        eax
0044eeb5  pop        ebx
0044eeb6  ret        8

// block 0044eeb9
0044eeb9  add        esi, 4
0044eebc  mov        byte ptr [esi], 0

// block 0044eebf
0044eebf  xor        ecx, ecx
0044eec1  cmp        ecx, ebx
0044eec3  sbb        eax, eax
0044eec5  pop        esi
0044eec6  neg        eax
0044eec8  pop        ebx
0044eec9  ret        8
