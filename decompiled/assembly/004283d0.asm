
// block 004283d0
004283d0  push       ecx
004283d1  mov        edx, dword ptr [0x4b44e8]
004283d7  mov        eax, dword ptr [edx + 0x60]
004283da  push       edi
004283db  mov        edi, ecx
004283dd  mov        ecx, eax
004283df  shr        ecx, 2
004283e2  or         ecx, eax
004283e4  test       cl, 1
004283e7  je         0x4283f1

// block 004283e9
004283e9  mov        eax, 1
004283ee  pop        edi
004283ef  pop        ecx
004283f0  ret        

// block 004283f1
004283f1  mov        edx, eax
004283f3  test       edx, 0x400
004283f9  jne        0x4283e9

// block 004283fb
004283fb  test       dl, 2
004283fe  je         0x428424

// block 00428400
00428400  fld        dword ptr [0x4b2ed0]
00428406  fstp       dword ptr [esp + 4]
0042840a  fldz       
0042840c  fstp       dword ptr [0x4b2ed0]
00428412  call       0x428270

// block 00428417
00428417  fld        dword ptr [esp + 4]
0042841b  fstp       dword ptr [0x4b2ed0]
00428421  pop        edi
00428422  pop        ecx
00428423  ret        

// block 00428424
00428424  call       0x428270

// block 00428429
00428429  pop        edi
0042842a  pop        ecx
0042842b  ret        
