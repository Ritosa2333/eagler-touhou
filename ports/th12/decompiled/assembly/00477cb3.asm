
// block 00477cb3
00477cb3  mov        edi, edi
00477cb5  push       ebp
00477cb6  mov        ebp, esp
00477cb8  mov        eax, dword ptr [esi]
00477cba  cmp        dword ptr [ebp + 8], eax
00477cbd  jne        0x477d05

// block 00477cbf
00477cbf  mov        ecx, dword ptr [edi]
00477cc1  push       2
00477cc3  push       eax
00477cc4  cmp        ecx, dword ptr [ebp + 0xc]
00477cc7  jne        0x477cf4

// block 00477cc9
00477cc9  call       0x477813

// block 00477cce
00477cce  pop        ecx
00477ccf  pop        ecx
00477cd0  mov        dword ptr [edi], eax
00477cd2  test       eax, eax
00477cd4  jne        0x477cda

// block 00477cd6
00477cd6  xor        eax, eax
00477cd8  pop        ebp
00477cd9  ret        

// block 00477cda
00477cda  mov        eax, dword ptr [ebp + 0x10]
00477cdd  mov        dword ptr [eax], 1
00477ce3  push       dword ptr [esi]
00477ce5  push       dword ptr [ebp + 0xc]
00477ce8  push       dword ptr [edi]
00477cea  call       0x47a330

// block 00477cef
00477cef  add        esp, 0xc
00477cf2  jmp        0x477d03

// block 00477cf4
00477cf4  push       ecx
00477cf5  call       0x4778ad

// block 00477cfa
00477cfa  add        esp, 0xc
00477cfd  test       eax, eax
00477cff  je         0x477cd6

// block 00477d01
00477d01  mov        dword ptr [edi], eax

// block 00477d03
00477d03  shl        dword ptr [esi], 1

// block 00477d05
00477d05  xor        eax, eax
00477d07  inc        eax
00477d08  pop        ebp
00477d09  ret        
