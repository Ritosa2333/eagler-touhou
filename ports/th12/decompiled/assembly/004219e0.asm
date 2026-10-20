
// block 004219e0
004219e0  mov        eax, dword ptr [esi + 0x494]
004219e6  test       eax, eax
004219e8  je         0x421a00

// block 004219ea
004219ea  mov        edx, 3
004219ef  mov        ecx, esi
004219f1  call       eax

// block 004219f3
004219f3  mov        eax, 3
004219f8  mov        word ptr [esi + 0x3c4], ax
004219ff  ret        

// block 00421a00
00421a00  mov        ecx, 3
00421a05  mov        word ptr [esi + 0x3c4], cx
00421a0c  ret        
