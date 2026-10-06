
// block 00423480
00423480  push       ebx
00423481  push       esi
00423482  push       0x24
00423484  call       0x46c9ea

// block 00423489
00423489  xor        ecx, ecx
0042348b  add        esp, 4
0042348e  cmp        eax, ecx
00423490  je         0x4234ae

// block 00423492
00423492  and        dword ptr [eax + 4], 0xfffffffe
00423496  mov        dword ptr [eax + 8], ecx
00423499  mov        dword ptr [eax + 0xc], ecx
0042349c  mov        dword ptr [eax + 0x10], ecx
0042349f  mov        dword ptr [eax], ecx
004234a1  mov        dword ptr [eax + 0x14], eax
004234a4  mov        dword ptr [eax + 0x18], ecx
004234a7  mov        dword ptr [eax + 0x1c], ecx
004234aa  mov        esi, eax
004234ac  jmp        0x4234b0

// block 004234ae
004234ae  xor        esi, esi

// block 004234b0
004234b0  mov        eax, dword ptr [esi + 4]
004234b3  and        eax, 0xfffffffd
004234b6  or         eax, 1
004234b9  mov        ebx, 0x1a
004234be  mov        dword ptr [esi + 8], 0x424190
004234c5  mov        dword ptr [esi + 0xc], ecx
004234c8  mov        dword ptr [esi + 0x10], ecx
004234cb  mov        dword ptr [esi + 0x20], edi
004234ce  mov        dword ptr [esi + 4], eax
004234d1  call       0x462380

// block 004234d6
004234d6  push       0x24
004234d8  mov        dword ptr [edi + 8], esi
004234db  call       0x46c9ea

// block 004234e0
004234e0  xor        ecx, ecx
004234e2  add        esp, 4
004234e5  cmp        eax, ecx
004234e7  je         0x423505

// block 004234e9
004234e9  and        dword ptr [eax + 4], 0xfffffffe
004234ed  mov        dword ptr [eax + 8], ecx
004234f0  mov        dword ptr [eax + 0xc], ecx
004234f3  mov        dword ptr [eax + 0x10], ecx
004234f6  mov        dword ptr [eax], ecx
004234f8  mov        dword ptr [eax + 0x14], eax
004234fb  mov        dword ptr [eax + 0x18], ecx
004234fe  mov        dword ptr [eax + 0x1c], ecx
00423501  mov        esi, eax
00423503  jmp        0x423507

// block 00423505
00423505  xor        esi, esi

// block 00423507
00423507  mov        dword ptr [esi + 0xc], ecx
0042350a  mov        dword ptr [esi + 0x10], ecx
0042350d  mov        ecx, dword ptr [esi + 4]
00423510  and        ecx, 0xfffffffd
00423513  or         ecx, 1
00423516  mov        ebx, 0x35
0042351b  mov        dword ptr [esi + 8], 0x4241a0
00423522  mov        dword ptr [esi + 0x20], edi
00423525  mov        dword ptr [esi + 4], ecx
00423528  call       0x462420

// block 0042352d
0042352d  mov        dword ptr [edi + 0xc], esi
00423530  mov        dword ptr [edi + 0x10], 0
00423537  cmp        byte ptr [0x4cead2], 0
0042353e  pop        esi
0042353f  pop        ebx
00423540  je         0x42355c

// block 00423542
00423542  push       0
00423544  push       0x4a00f8
00423549  push       edi
0042354a  call       0x424240

// block 0042354f
0042354f  push       1
00423551  push       0x4a010c
00423556  push       edi
00423557  call       0x424240

// block 0042355c
0042355c  xor        eax, eax
0042355e  ret        
