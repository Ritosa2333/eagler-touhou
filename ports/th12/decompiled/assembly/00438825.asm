
// block 00438825
00438825  mov        edx, dword ptr [esp + 0x4c]
00438829  mov        eax, dword ptr [edx + ebx*4 - 4]
0043882d  mov        ecx, dword ptr [ebp + 0xa2c]
00438833  add        eax, edi
00438835  fld        dword ptr [ecx + eax*8 + 0x28]
00438839  lea        eax, [ecx + eax*8 + 0x28]
0043883d  fmul       st(1)
0043883f  mov        dword ptr [esp + 0x14], eax
00438843  call       0x4931e0

// block 00438848
00438848  mov        edx, dword ptr [esp + 0x14]
0043884c  mov        dword ptr [esi - 0xc], eax
0043884f  fld        dword ptr [edx + 4]
00438852  fmul       st(1)
00438854  call       0x4931e0

// block 00438859
00438859  mov        dword ptr [esi - 8], eax
0043885c  mov        eax, dword ptr [esp + 0x4c]
00438860  mov        ecx, dword ptr [eax + ebx*4 - 4]
00438864  mov        edx, dword ptr [ebp + 0xa2c]
0043886a  add        ecx, edi
0043886c  fld        dword ptr [edx + ecx*8 + 0x78]
00438870  lea        eax, [edx + ecx*8 + 0x78]
00438874  fmul       st(1)
00438876  mov        dword ptr [esp + 0x14], eax
0043887a  call       0x4931e0

// block 0043887f
0043887f  mov        dword ptr [esi - 4], eax
00438882  mov        eax, dword ptr [esp + 0x14]
00438886  fmul       dword ptr [eax + 4]
00438889  call       0x4931e0

// block 0043888e
0043888e  mov        dword ptr [esi], eax
00438890  cmp        dword ptr [ebp + 0xc598], 0
00438897  lea        edx, [esi - 4]
0043889a  jne        0x43889f

// block 0043889c
0043889c  lea        edx, [esi - 0xc]

// block 0043889f
0043889f  mov        eax, dword ptr [ebp + 0x988]
004388a5  mov        ecx, dword ptr [ebp + 0x98c]
004388ab  add        ecx, dword ptr [edx + 4]
004388ae  add        eax, dword ptr [edx]
004388b0  mov        dword ptr [esi - 0x18], ecx
004388b3  mov        dword ptr [esi - 0x1c], eax
004388b6  push       0
004388b8  mov        dword ptr [esi - 0x10], ecx
004388bb  push       0x12
004388bd  mov        dword ptr [esi - 0x14], eax
004388c0  mov        edx, dword ptr [ebp + 0x10]
004388c3  lea        ecx, [esp + 0x20]
004388c7  push       ecx
004388c8  push       edx
004388c9  mov        eax, 0xc
004388ce  xor        ecx, ecx
004388d0  call       0x4615a0

// block 004388d5
004388d5  mov        eax, dword ptr [esp + 0x18]
004388d9  mov        dword ptr [esi + 0x40], eax
004388dc  jmp        0x438c85

// block 00438c85
00438c85  fld        qword ptr [0x4a3d48]
