
// block 00472ca8
00472ca8  mov        edi, edi
00472caa  push       ebp
00472cab  mov        ebp, esp
00472cad  push       esi
00472cae  mov        esi, dword ptr [ebp + 8]
00472cb1  xor        eax, eax
00472cb3  jmp        0x472cc4

// block 00472cb5
00472cb5  test       eax, eax
00472cb7  jne        0x472cc9

// block 00472cb9
00472cb9  mov        ecx, dword ptr [esi]
00472cbb  test       ecx, ecx
00472cbd  je         0x472cc1

// block 00472cbf
00472cbf  call       ecx

// block 00472cc1
00472cc1  add        esi, 4

// block 00472cc4
00472cc4  cmp        esi, dword ptr [ebp + 0xc]
00472cc7  jb         0x472cb5

// block 00472cc9
00472cc9  pop        esi
00472cca  pop        ebp
00472ccb  ret        
