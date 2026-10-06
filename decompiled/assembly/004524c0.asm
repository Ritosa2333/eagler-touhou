
// block 004524c0
004524c0  push       ecx
004524c1  push       esi
004524c2  mov        esi, ecx
004524c4  cmp        dword ptr [esi + 0x2c], 0
004524c8  jne        0x4524ff

// block 004524ca
004524ca  mov        eax, dword ptr [esi + 0x1c]
004524cd  mov        dword ptr [esp + 4], eax
004524d1  test       eax, eax
004524d3  je         0x452523

// block 004524d5
004524d5  cmp        dword ptr [esi + 0x34], eax
004524d8  jg         0x452523

// block 004524da
004524da  fld        dword ptr [esi + 0x38]
004524dd  fmul       qword ptr [0x4a3d48]
004524e3  fidiv      dword ptr [esp + 4]
004524e7  call       0x4931e0

// block 004524ec
004524ec  mov        dword ptr [esi + 0x18], eax
004524ef  add        esi, 0x30
004524f2  call       0x464a80

// block 004524f7
004524f7  mov        eax, 1
004524fc  pop        esi
004524fd  pop        ecx
004524fe  ret        

// block 004524ff
004524ff  cmp        dword ptr [esi + 0x34], 8
00452503  jg         0x452533

// block 00452505
00452505  fld        dword ptr [esi + 0x38]
00452508  fmul       qword ptr [0x4a3d48]
0045250e  fmul       qword ptr [0x4a3ec8]
00452514  call       0x4931e0

// block 00452519
00452519  mov        ecx, 0x80
0045251e  sub        ecx, eax
00452520  mov        dword ptr [esi + 0x18], ecx

// block 00452523
00452523  add        esi, 0x30
00452526  call       0x464a80

// block 0045252b
0045252b  mov        eax, 1
00452530  pop        esi
00452531  pop        ecx
00452532  ret        

// block 00452533
00452533  mov        eax, 7
00452538  pop        esi
00452539  pop        ecx
0045253a  ret        
