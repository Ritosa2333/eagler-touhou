
// block 00464580
00464580  sub        esp, 8
00464583  push       esi
00464584  mov        esi, ecx
00464586  call       0x4643d0

// block 0046458b
0046458b  mov        word ptr [esp + 4], ax
00464590  call       0x4643d0

// block 00464595
00464595  mov        word ptr [esp + 6], ax
0046459a  call       0x4643d0

// block 0046459f
0046459f  call       0x4643d0

// block 004645a4
004645a4  mov        eax, dword ptr [esp + 4]
004645a8  pop        esi
004645a9  add        esp, 8
004645ac  ret        
