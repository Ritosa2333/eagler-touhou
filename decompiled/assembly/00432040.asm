
// block 00432040
00432040  mov        eax, dword ptr [edi + 4]
00432043  push       esi
00432044  cmp        eax, 0x1a
00432047  ja         0x4320a9

// block 00432049
00432049  movzx      eax, byte ptr [eax + 0x4320d0]
00432050  jmp        dword ptr [eax*4 + 0x4320c0]

// block 00432057
00432057  test       byte ptr [0x4b0ce0], 0x20
0043205e  jne        0x4320a9

// block 00432060
00432060  test       dword ptr [0x4d48c4], 0x100
0043206a  jne        0x432075

// block 0043206c
0043206c  test       byte ptr [0x4cee78], 0x10
00432073  je         0x4320a9

// block 00432075
00432075  call       0x4358c0

// block 0043207a
0043207a  test       eax, eax
0043207c  je         0x4320a9

// block 0043207e
0043207e  mov        ecx, dword ptr [0x4b44e8]
00432084  cmp        dword ptr [ecx + 0x14], 0x1e
00432088  jl         0x4320a9

// block 0043208a
0043208a  mov        esi, edi
0043208c  call       0x432720

// block 00432091
00432091  jmp        0x4320a9

// block 00432093
00432093  push       edi
00432094  call       0x4329a0

// block 00432099
00432099  jmp        0x4320a9

// block 0043209b
0043209b  push       edi
0043209c  call       0x433bb0

// block 004320a1
004320a1  jmp        0x4320a9

// block 004320a3
004320a3  push       edi
004320a4  call       0x434900

// block 004320a9
004320a9  lea        esi, [edi + 0x10]
004320ac  call       0x464a80

// block 004320b1
004320b1  lea        esi, [edi + 0x24]
004320b4  call       0x464a80

// block 004320b9
004320b9  mov        eax, 1
004320be  pop        esi
004320bf  ret        
