
// block 0047e2f7
0047e2f7  mov        edi, edi
0047e2f9  push       ebp
0047e2fa  mov        ebp, esp
0047e2fc  mov        eax, dword ptr [ebp + 8]
0047e2ff  push       esi
0047e300  mov        esi, ecx
0047e302  and        dword ptr [esi + 4], 0xffff00ff
0047e309  mov        byte ptr [esi + 4], al
0047e30c  cmp        eax, 1
0047e30f  jne        0x47e324

// block 0047e311
0047e311  push       eax
0047e312  call       0x47deeb

// block 0047e317
0047e317  pop        ecx
0047e318  mov        dword ptr [esi], eax
0047e31a  test       eax, eax
0047e31c  jne        0x47e327

// block 0047e31e
0047e31e  mov        byte ptr [esi + 4], 3
0047e322  jmp        0x47e327

// block 0047e324
0047e324  and        dword ptr [esi], 0

// block 0047e327
0047e327  mov        eax, esi
0047e329  pop        esi
0047e32a  pop        ebp
0047e32b  ret        4
