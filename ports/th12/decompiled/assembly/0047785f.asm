
// block 0047785f
0047785f  mov        edi, edi
00477861  push       ebp
00477862  mov        ebp, esp
00477864  push       esi
00477865  push       edi
00477866  xor        esi, esi

// block 00477868
00477868  push       dword ptr [ebp + 0xc]
0047786b  push       dword ptr [ebp + 8]
0047786e  call       0x48b606

// block 00477873
00477873  mov        edi, eax
00477875  pop        ecx
00477876  pop        ecx
00477877  test       edi, edi
00477879  jne        0x4778a7

// block 0047787b
0047787b  cmp        dword ptr [ebp + 0xc], eax
0047787e  je         0x4778a7

// block 00477880
00477880  cmp        dword ptr [0x4b41d0], eax
00477886  jbe        0x4778a7

// block 00477888
00477888  push       esi
00477889  call       dword ptr [0x498084]

// block 0047788f
0047788f  lea        eax, [esi + 0x3e8]
00477895  cmp        eax, dword ptr [0x4b41d0]
0047789b  jbe        0x4778a0

// block 0047789d
0047789d  or         eax, 0xffffffff

// block 004778a0
004778a0  mov        esi, eax
004778a2  cmp        eax, -1
004778a5  jne        0x477868

// block 004778a7
004778a7  mov        eax, edi
004778a9  pop        edi
004778aa  pop        esi
004778ab  pop        ebp
004778ac  ret        
