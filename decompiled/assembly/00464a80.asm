
// block 00464a80
00464a80  push       ecx
00464a81  mov        edx, dword ptr [esi + 4]
00464a84  mov        ecx, dword ptr [esi + 0xc]
00464a87  mov        dword ptr [esi], edx
00464a89  fld        dword ptr [ecx]
00464a8b  fcomp      qword ptr [0x4a3db0]
00464a91  fnstsw     ax
00464a93  test       ah, 0x41
00464a96  jne        0x464abb

// block 00464a98
00464a98  fld        dword ptr [0x4a3dac]
00464a9e  fcomp      dword ptr [ecx]
00464aa0  fnstsw     ax
00464aa2  test       ah, 0x41
00464aa5  jne        0x464abb

// block 00464aa7
00464aa7  fld        dword ptr [esi + 8]
00464aaa  inc        edx
00464aab  fadd       qword ptr [0x4a3ce0]
00464ab1  mov        dword ptr [esi + 4], edx
00464ab4  mov        eax, edx
00464ab6  fstp       dword ptr [esi + 8]
00464ab9  pop        ecx
00464aba  ret        

// block 00464abb
00464abb  fld        dword ptr [ecx]
00464abd  fadd       dword ptr [esi + 8]
00464ac0  fstp       dword ptr [esp]
00464ac3  fld        dword ptr [esp]
00464ac6  fst        dword ptr [esi + 8]
00464ac9  call       0x4931e0

// block 00464ace
00464ace  mov        dword ptr [esi + 4], eax
00464ad1  pop        ecx
00464ad2  ret        
