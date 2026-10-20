
// block 004388e1
004388e1  mov        ecx, dword ptr [esp + 0x4c]
004388e5  mov        edx, dword ptr [ecx + ebx*4 - 4]
004388e9  mov        eax, dword ptr [ebp + 0xa2c]
004388ef  add        edx, edi
004388f1  fld        dword ptr [eax + edx*8 + 0x28]
004388f5  lea        eax, [eax + edx*8 + 0x28]
004388f9  fmul       st(1)
004388fb  mov        dword ptr [esp + 0x14], eax
004388ff  call       0x4931e0

// block 00438904
00438904  mov        ecx, dword ptr [esp + 0x14]
00438908  mov        dword ptr [esi - 0xc], eax
0043890b  fld        dword ptr [ecx + 4]
0043890e  fmul       st(1)
00438910  call       0x4931e0

// block 00438915
00438915  mov        edx, dword ptr [esp + 0x4c]
00438919  mov        dword ptr [esi - 8], eax
0043891c  mov        eax, dword ptr [edx + ebx*4 - 4]
00438920  mov        ecx, dword ptr [ebp + 0xa2c]
00438926  add        eax, edi
00438928  fld        dword ptr [ecx + eax*8 + 0x78]
0043892c  lea        eax, [ecx + eax*8 + 0x78]
00438930  fmul       st(1)
00438932  mov        dword ptr [esp + 0x14], eax
00438936  call       0x4931e0

// block 0043893b
0043893b  mov        edx, dword ptr [esp + 0x14]
0043893f  mov        dword ptr [esi - 4], eax
00438942  fmul       dword ptr [edx + 4]
00438945  call       0x4931e0

// block 0043894a
0043894a  mov        dword ptr [esi], eax
0043894c  cmp        dword ptr [ebp + 0xc598], 0
00438953  lea        edx, [esi - 4]
00438956  jne        0x43895b

// block 00438958
00438958  lea        edx, [esi - 0xc]

// block 0043895b
0043895b  mov        eax, dword ptr [ebp + 0x988]
00438961  mov        ecx, dword ptr [ebp + 0x98c]
00438967  add        eax, dword ptr [edx]
00438969  add        ecx, dword ptr [edx + 4]
0043896c  mov        dword ptr [esi - 0x1c], eax
0043896f  mov        dword ptr [esi - 0x18], ecx
00438972  push       0
00438974  mov        dword ptr [esi - 0x14], eax
00438977  push       0x13
00438979  mov        dword ptr [esi - 0x10], ecx
0043897c  mov        ecx, dword ptr [ebp + 0x10]
0043897f  lea        eax, [esp + 0x24]
00438983  push       eax
00438984  push       ecx
00438985  mov        eax, 0xc
0043898a  xor        ecx, ecx
0043898c  call       0x4615a0

// block 00438991
00438991  mov        edx, dword ptr [esp + 0x1c]
00438995  mov        dword ptr [esi + 0x40], edx
00438998  jmp        0x438c85
