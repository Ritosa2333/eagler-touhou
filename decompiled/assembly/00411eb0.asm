
// block 00411eb0
00411eb0  push       ebx
00411eb1  mov        ebx, ecx
00411eb3  push       esi
00411eb4  mov        esi, dword ptr [ebx + 0x1034]
00411eba  mov        dword ptr [ebx], 0x49fc34
00411ec0  test       esi, esi
00411ec2  je         0x411ee0

// block 00411ec4
00411ec4  push       edi

// block 00411ec5
00411ec5  mov        eax, dword ptr [esi]
00411ec7  mov        edi, dword ptr [esi + 4]
00411eca  push       eax
00411ecb  call       0x46ca4f

// block 00411ed0
00411ed0  push       esi
00411ed1  call       0x46ca4f

// block 00411ed6
00411ed6  add        esp, 8
00411ed9  mov        esi, edi
00411edb  test       edi, edi
00411edd  jne        0x411ec5

// block 00411edf
00411edf  pop        edi

// block 00411ee0
00411ee0  test       byte ptr [esp + 0xc], 1
00411ee5  je         0x411ef0

// block 00411ee7
00411ee7  push       ebx
00411ee8  call       0x46ca4f

// block 00411eed
00411eed  add        esp, 4

// block 00411ef0
00411ef0  pop        esi
00411ef1  mov        eax, ebx
00411ef3  pop        ebx
00411ef4  ret        4
