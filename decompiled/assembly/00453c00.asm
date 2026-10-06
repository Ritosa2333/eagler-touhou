
// block 00453c00
00453c00  mov        eax, dword ptr [esi + 0x526c]
00453c06  test       eax, eax
00453c08  jne        0x453c10

// block 00453c0a
00453c0a  or         eax, 0xffffffff
00453c0d  ret        4

// block 00453c10
00453c10  call       0x466350

// block 00453c15
00453c15  test       eax, eax
00453c17  jl         0x453c0a

// block 00453c19
00453c19  mov        eax, dword ptr [esp + 4]
00453c1d  mov        dword ptr [esi + 0x5274], eax
00453c23  xor        eax, eax
00453c25  ret        4
