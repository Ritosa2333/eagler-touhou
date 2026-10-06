
// block 004737ae
004737ae  mov        edi, edi
004737b0  push       esi
004737b1  push       edi
004737b2  mov        esi, eax
004737b4  push       0x101
004737b9  xor        edi, edi
004737bb  lea        eax, [esi + 0x1c]
004737be  push       edi
004737bf  push       eax
004737c0  call       0x477420

// block 004737c5
004737c5  xor        eax, eax
004737c7  movzx      ecx, ax
004737ca  mov        eax, ecx
004737cc  mov        dword ptr [esi + 4], edi
004737cf  mov        dword ptr [esi + 8], edi
004737d2  mov        dword ptr [esi + 0xc], edi
004737d5  shl        ecx, 0x10
004737d8  or         eax, ecx
004737da  lea        edi, [esi + 0x10]
004737dd  stosd      dword ptr es:[edi], eax
004737de  stosd      dword ptr es:[edi], eax
004737df  stosd      dword ptr es:[edi], eax
004737e0  mov        ecx, 0x4ad4b0
004737e5  add        esp, 0xc
004737e8  lea        eax, [esi + 0x1c]
004737eb  sub        ecx, esi
004737ed  mov        edi, 0x101

// block 004737f2
004737f2  mov        dl, byte ptr [ecx + eax]
004737f5  mov        byte ptr [eax], dl
004737f7  inc        eax
004737f8  dec        edi
004737f9  jne        0x4737f2

// block 004737fb
004737fb  lea        eax, [esi + 0x11d]
00473801  mov        esi, 0x100

// block 00473806
00473806  mov        dl, byte ptr [eax + ecx]
00473809  mov        byte ptr [eax], dl
0047380b  inc        eax
0047380c  dec        esi
0047380d  jne        0x473806

// block 0047380f
0047380f  pop        edi
00473810  pop        esi
00473811  ret        
