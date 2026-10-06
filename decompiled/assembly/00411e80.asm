
// block 00411e80
00411e80  push       esi
00411e81  mov        esi, dword ptr [ecx + 0x1034]
00411e87  mov        dword ptr [ecx], 0x49fc34
00411e8d  test       esi, esi
00411e8f  je         0x411ead

// block 00411e91
00411e91  push       edi

// block 00411e92
00411e92  mov        eax, dword ptr [esi]
00411e94  mov        edi, dword ptr [esi + 4]
00411e97  push       eax
00411e98  call       0x46ca4f

// block 00411e9d
00411e9d  push       esi
00411e9e  call       0x46ca4f

// block 00411ea3
00411ea3  add        esp, 8
00411ea6  mov        esi, edi
00411ea8  test       edi, edi
00411eaa  jne        0x411e92

// block 00411eac
00411eac  pop        edi

// block 00411ead
00411ead  pop        esi
00411eae  ret        
