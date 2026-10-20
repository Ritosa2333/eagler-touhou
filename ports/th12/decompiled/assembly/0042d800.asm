
// block 0042d800
0042d800  push       ecx
0042d801  push       esi
0042d802  mov        esi, ecx
0042d804  mov        eax, dword ptr [esi + 0xf0]
0042d80a  cmp        eax, dword ptr [esi + 0x114]
0042d810  jl         0x42d821

// block 0042d812
0042d812  and        dword ptr [esi + 0x430], 0xfffffff7
0042d819  mov        eax, 1
0042d81e  pop        esi
0042d81f  pop        ecx
0042d820  ret        

// block 0042d821
0042d821  fld        dword ptr [esi + 0x104]
0042d827  sub        esp, 8
0042d82a  fmul       dword ptr [0x4b2ed0]
0042d830  fstp       dword ptr [esp + 0xc]
0042d834  fld        dword ptr [esp + 0xc]
0042d838  fstp       dword ptr [esp + 4]
0042d83c  fld        dword ptr [esi + 0x68]
0042d83f  fstp       dword ptr [esp]
0042d842  call       0x464640

// block 0042d847
0042d847  fstp       dword ptr [esi + 0x68]
0042d84a  sub        esp, 8
0042d84d  fld        dword ptr [esi + 0x100]
0042d853  lea        ecx, [esi + 0x5c]
0042d856  fmul       dword ptr [0x4b2ed0]
0042d85c  fadd       dword ptr [esi + 0x74]
0042d85f  fstp       dword ptr [esp + 0xc]
0042d863  fld        dword ptr [esp + 0xc]
0042d867  fst        dword ptr [esi + 0x74]
0042d86a  fstp       dword ptr [esp + 4]
0042d86e  fld        dword ptr [esi + 0x68]
0042d871  fstp       dword ptr [esp]
0042d874  call       0x42e880

// block 0042d879
0042d879  add        esi, 0xec
0042d87f  call       0x464a80

// block 0042d884
0042d884  xor        eax, eax
0042d886  pop        esi
0042d887  pop        ecx
0042d888  ret        
