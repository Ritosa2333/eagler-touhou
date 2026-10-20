
// block 00490fc6
00490fc6  mov        edi, edi
00490fc8  push       ebp
00490fc9  mov        ebp, esp
00490fcb  push       esi
00490fcc  mov        esi, dword ptr [ebp + 8]
00490fcf  push       edi
00490fd0  push       esi
00490fd1  call       0x48cac6

// block 00490fd6
00490fd6  pop        ecx
00490fd7  cmp        eax, -1
00490fda  je         0x49102c

// block 00490fdc
00490fdc  mov        eax, dword ptr [0x4d6320]
00490fe1  cmp        esi, 1
00490fe4  jne        0x490fef

// block 00490fe6
00490fe6  test       byte ptr [eax + 0x84], 1
00490fed  jne        0x490ffa

// block 00490fef
00490fef  cmp        esi, 2
00490ff2  jne        0x491010

// block 00490ff4
00490ff4  test       byte ptr [eax + 0x44], 1
00490ff8  je         0x491010

// block 00490ffa
00490ffa  push       2
00490ffc  call       0x48cac6

// block 00491001
00491001  push       1
00491003  mov        edi, eax
00491005  call       0x48cac6

// block 0049100a
0049100a  pop        ecx
0049100b  pop        ecx
0049100c  cmp        eax, edi
0049100e  je         0x49102c

// block 00491010
00491010  push       esi
00491011  call       0x48cac6

// block 00491016
00491016  pop        ecx
00491017  push       eax
00491018  call       dword ptr [0x4980a4]

// block 0049101e
0049101e  test       eax, eax
00491020  jne        0x49102c

// block 00491022
00491022  call       dword ptr [0x4980e4]

// block 00491028
00491028  mov        edi, eax
0049102a  jmp        0x49102e

// block 0049102c
0049102c  xor        edi, edi

// block 0049102e
0049102e  push       esi
0049102f  call       0x48ca40

// block 00491034
00491034  mov        eax, esi
00491036  sar        eax, 5
00491039  mov        eax, dword ptr [eax*4 + 0x4d6320]
00491040  and        esi, 0x1f
00491043  shl        esi, 6
00491046  pop        ecx
00491047  mov        byte ptr [eax + esi + 4], 0
0049104c  test       edi, edi
0049104e  je         0x49105c

// block 00491050
00491050  push       edi
00491051  call       0x46e995

// block 00491056
00491056  pop        ecx
00491057  or         eax, 0xffffffff
0049105a  jmp        0x49105e

// block 0049105c
0049105c  xor        eax, eax

// block 0049105e
0049105e  pop        edi
0049105f  pop        esi
00491060  pop        ebp
00491061  ret        
