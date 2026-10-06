
// block 0047c7ee
0047c7ee  mov        edi, edi
0047c7f0  push       ebp
0047c7f1  mov        ebp, esp
0047c7f3  push       dword ptr [ebp + 0xc]
0047c7f6  push       0x107
0047c7fb  push       dword ptr [ebp + 8]
0047c7fe  call       0x48d524

// block 0047c803
0047c803  add        esp, 0xc
0047c806  test       eax, eax
0047c808  jne        0x47c813

// block 0047c80a
0047c80a  cmp        word ptr [ebp + 8], 0x5f
0047c80f  je         0x47c813

// block 0047c811
0047c811  pop        ebp
0047c812  ret        

// block 0047c813
0047c813  xor        eax, eax
0047c815  inc        eax
0047c816  pop        ebp
0047c817  ret        
