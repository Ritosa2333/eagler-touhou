
// block 004619e0
004619e0  push       ebp
004619e1  mov        ebp, esp
004619e3  and        esp, 0xfffffff8
004619e6  mov        eax, dword ptr [ebp + 8]
004619e9  mov        edx, dword ptr [0x4ce8cc]
004619ef  push       esi
004619f0  push       edi
004619f1  push       eax
004619f2  call       0x461920

// block 004619f7
004619f7  mov        esi, eax
004619f9  test       esi, esi
004619fb  je         0x461a59

// block 004619fd
004619fd  mov        eax, dword ptr [esi + 0x494]
00461a03  test       eax, eax
00461a05  je         0x461a0e

// block 00461a07
00461a07  movsx      edx, bx
00461a0a  mov        ecx, esi
00461a0c  call       eax

// block 00461a0e
00461a0e  push       esi
00461a0f  mov        word ptr [esi + 0x3c4], bx
00461a16  call       0x455630

// block 00461a1b
00461a1b  cmp        dword ptr [esi + 0x18], 0
00461a1f  jne        0x461a59

// block 00461a21
00461a21  mov        esi, dword ptr [esi + 0x14]
00461a24  test       esi, esi
00461a26  je         0x461a59

// block 00461a28
00461a28  jmp        0x461a30

// block 00461a30
00461a30  mov        edi, dword ptr [esi]
00461a32  mov        eax, dword ptr [edi + 0x494]
00461a38  test       eax, eax
00461a3a  je         0x461a43

// block 00461a3c
00461a3c  movsx      edx, bx
00461a3f  mov        ecx, edi
00461a41  call       eax

// block 00461a43
00461a43  mov        word ptr [edi + 0x3c4], bx
00461a4a  mov        ecx, dword ptr [esi]
00461a4c  push       ecx
00461a4d  call       0x455630

// block 00461a52
00461a52  mov        esi, dword ptr [esi + 4]
00461a55  test       esi, esi
00461a57  jne        0x461a30

// block 00461a59
00461a59  pop        edi
00461a5a  pop        esi
00461a5b  mov        esp, ebp
00461a5d  pop        ebp
00461a5e  ret        4
