
// block 004757b7
004757b7  mov        edi, edi
004757b9  push       ebp
004757ba  mov        ebp, esp
004757bc  sub        esp, 0x24
004757bf  mov        eax, dword ptr [0x4ad138]
004757c4  xor        eax, ebp
004757c6  mov        dword ptr [ebp - 4], eax
004757c9  mov        eax, dword ptr [ebp + 0x1c]
004757cc  mov        ecx, dword ptr [ebp + 0xc]
004757cf  push       ebx
004757d0  push       esi
004757d1  mov        esi, dword ptr [ebp + 8]
004757d4  push       edi
004757d5  push       eax
004757d6  xor        edi, edi
004757d8  push       edi
004757d9  push       edi
004757da  push       edi
004757db  push       edi
004757dc  push       ecx
004757dd  lea        eax, [ebp - 0x24]
004757e0  push       eax
004757e1  lea        eax, [ebp - 0x10]
004757e4  push       eax
004757e5  mov        dword ptr [ebp - 0x20], ecx
004757e8  xor        ebx, ebx
004757ea  call       0x476077

// block 004757ef
004757ef  add        esp, 0x20
004757f2  mov        dword ptr [ebp - 0x14], eax
004757f5  test       al, 4
004757f7  je         0x475806

// block 004757f9
004757f9  mov        ebx, 0x200
004757fe  mov        dword ptr [ebp - 0x1c], edi
00475801  mov        dword ptr [ebp - 0x18], edi
00475804  jmp        0x475836

// block 00475806
00475806  lea        eax, [ebp - 0x1c]
00475809  push       eax
0047580a  lea        eax, [ebp - 0x10]
0047580d  push       eax
0047580e  call       0x4895ea

// block 00475813
00475813  test       byte ptr [ebp - 0x14], 2
00475817  pop        ecx
00475818  pop        ecx
00475819  jne        0x475820

// block 0047581b
0047581b  cmp        eax, 1
0047581e  jne        0x475825

// block 00475820
00475820  mov        ebx, 0x80

// block 00475825
00475825  test       byte ptr [ebp - 0x14], 1
00475829  jne        0x475830

// block 0047582b
0047582b  cmp        eax, 2
0047582e  jne        0x475836

// block 00475830
00475830  or         ebx, 0x100

// block 00475836
00475836  mov        eax, dword ptr [ebp - 0x24]
00475839  sub        eax, dword ptr [ebp - 0x20]
0047583c  mov        ecx, dword ptr [ebp - 4]
0047583f  mov        dword ptr [esi + 4], eax
00475842  mov        eax, dword ptr [ebp - 0x1c]
00475845  mov        dword ptr [esi + 0x10], eax
00475848  mov        eax, dword ptr [ebp - 0x18]
0047584b  mov        dword ptr [esi + 0x14], eax
0047584e  pop        edi
0047584f  mov        dword ptr [esi], ebx
00475851  mov        eax, esi
00475853  pop        esi
00475854  xor        ecx, ebp
00475856  pop        ebx
00475857  call       0x46c932

// block 0047585c
0047585c  leave      
0047585d  ret        
