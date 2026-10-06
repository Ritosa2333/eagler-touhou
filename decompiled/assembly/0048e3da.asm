
// block 0048e3da
0048e3da  mov        edi, edi
0048e3dc  push       ebp
0048e3dd  mov        ebp, esp
0048e3df  push       ecx
0048e3e0  and        dword ptr [ebp - 4], 0
0048e3e4  push       esi
0048e3e5  lea        eax, [ebp - 4]
0048e3e8  push       eax
0048e3e9  push       dword ptr [ebp + 0xc]
0048e3ec  push       dword ptr [ebp + 8]
0048e3ef  call       0x48b4e8

// block 0048e3f4
0048e3f4  mov        esi, eax
0048e3f6  add        esp, 0xc
0048e3f9  test       esi, esi
0048e3fb  jne        0x48e415

// block 0048e3fd
0048e3fd  cmp        dword ptr [ebp - 4], eax
0048e400  je         0x48e415

// block 0048e402
0048e402  call       0x46e96f

// block 0048e407
0048e407  test       eax, eax
0048e409  je         0x48e415

// block 0048e40b
0048e40b  call       0x46e96f

// block 0048e410
0048e410  mov        ecx, dword ptr [ebp - 4]
0048e413  mov        dword ptr [eax], ecx

// block 0048e415
0048e415  mov        eax, esi
0048e417  pop        esi
0048e418  leave      
0048e419  ret        
