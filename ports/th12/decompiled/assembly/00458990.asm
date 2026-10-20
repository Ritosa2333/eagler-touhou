
// block 00458990
00458990  push       ebp
00458991  mov        ebp, dword ptr [esp + 0xc]
00458995  push       esi
00458996  push       edi
00458997  mov        edi, eax
00458999  mov        esi, ecx
0045899b  test       ebp, ebp
0045899d  je         0x4589d9

// block 0045899f
0045899f  push       ebx
004589a0  mov        bl, 0x10

// block 004589a2
004589a2  call       0x402520

// block 004589a7
004589a7  mov        ecx, dword ptr [esp + 0x14]
004589ab  push       edi
004589ac  push       esi
004589ad  mov        byte ptr [esi + 0x49d], bl
004589b3  mov        byte ptr [esi + 0x49c], bl
004589b9  call       0x454d10

// block 004589be
004589be  mov        ax, word ptr [esi + 0x3e4]
004589c5  mov        word ptr [esi + 0x3e8], ax
004589cc  inc        edi
004589cd  add        esi, 0x4b4
004589d3  sub        ebp, 1
004589d6  jne        0x4589a2

// block 004589d8
004589d8  pop        ebx

// block 004589d9
004589d9  pop        edi
004589da  pop        esi
004589db  pop        ebp
004589dc  ret        8
