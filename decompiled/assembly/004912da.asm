
// block 004912da
004912da  mov        edi, edi
004912dc  push       ebp
004912dd  mov        ebp, esp
004912df  push       esi
004912e0  xor        esi, esi
004912e2  cmp        dword ptr [0x4d52dc], esi
004912e8  je         0x49130c

// block 004912ea
004912ea  call       0x491202

// block 004912ef
004912ef  mov        ecx, dword ptr [ebp + 8]
004912f2  and        ecx, dword ptr [ebp + 0xc]
004912f5  mov        esi, eax
004912f7  mov        eax, dword ptr [ebp + 0xc]
004912fa  not        eax
004912fc  or         eax, 0xffff807f
00491301  and        eax, esi
00491303  or         eax, ecx
00491305  push       eax
00491306  call       0x491220

// block 0049130b
0049130b  pop        ecx

// block 0049130c
0049130c  mov        eax, esi
0049130e  pop        esi
0049130f  pop        ebp
00491310  ret        
