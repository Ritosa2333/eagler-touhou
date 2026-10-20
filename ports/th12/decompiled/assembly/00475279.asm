
// block 00475279
00475279  mov        edi, edi
0047527b  push       esi
0047527c  push       dword ptr [0x4adacc]
00475282  call       dword ptr [0x49814c]

// block 00475288
00475288  mov        esi, eax
0047528a  test       esi, esi
0047528c  jne        0x4752a9

// block 0047528e
0047528e  push       dword ptr [0x4b4104]
00475294  call       0x4751de

// block 00475299
00475299  pop        ecx
0047529a  mov        esi, eax
0047529c  push       esi
0047529d  push       dword ptr [0x4adacc]
004752a3  call       dword ptr [0x498144]

// block 004752a9
004752a9  mov        eax, esi
004752ab  pop        esi
004752ac  ret        
