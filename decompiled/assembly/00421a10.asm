
// block 00421a10
00421a10  add        eax, 7
00421a13  push       edi
00421a14  movzx      edi, ax
00421a17  mov        eax, dword ptr [esi + 0x494]
00421a1d  test       eax, eax
00421a1f  je         0x421a28

// block 00421a21
00421a21  movsx      edx, di
00421a24  mov        ecx, esi
00421a26  call       eax

// block 00421a28
00421a28  mov        word ptr [esi + 0x3c4], di
00421a2f  pop        edi
00421a30  ret        
