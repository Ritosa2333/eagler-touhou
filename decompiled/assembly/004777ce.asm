
// block 004777ce
004777ce  mov        edi, edi
004777d0  push       ebp
004777d1  mov        ebp, esp
004777d3  push       esi
004777d4  push       edi
004777d5  xor        esi, esi

// block 004777d7
004777d7  push       dword ptr [ebp + 8]
004777da  call       0x46d04a

// block 004777df
004777df  mov        edi, eax
004777e1  pop        ecx
004777e2  test       edi, edi
004777e4  jne        0x47780d

// block 004777e6
004777e6  cmp        dword ptr [0x4b41d0], eax
004777ec  jbe        0x47780d

// block 004777ee
004777ee  push       esi
004777ef  call       dword ptr [0x498084]

// block 004777f5
004777f5  lea        eax, [esi + 0x3e8]
004777fb  cmp        eax, dword ptr [0x4b41d0]
00477801  jbe        0x477806

// block 00477803
00477803  or         eax, 0xffffffff

// block 00477806
00477806  mov        esi, eax
00477808  cmp        eax, -1
0047780b  jne        0x4777d7

// block 0047780d
0047780d  mov        eax, edi
0047780f  pop        edi
00477810  pop        esi
00477811  pop        ebp
00477812  ret        
