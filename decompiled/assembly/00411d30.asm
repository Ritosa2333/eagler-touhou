
// block 00411d30
00411d30  mov        edx, dword ptr [ecx + 4]
00411d33  mov        ecx, dword ptr [edx + 4]
00411d36  push       esi
00411d37  movzx      esi, word ptr [ecx + 8]
00411d3b  mov        ecx, dword ptr [esp + 8]
00411d3f  push       edi
00411d40  mov        edi, 1
00411d45  shl        edi, cl
00411d47  test       edi, esi
00411d49  pop        edi
00411d4a  pop        esi
00411d4b  je         0x411d58

// block 00411d4d
00411d4d  mov        dword ptr [esp + 4], eax
00411d51  mov        ecx, edx
00411d53  jmp        0x468f70

// block 00411d58
00411d58  ret        4
