
// block 0040b800
0040b800  push       ebp
0040b801  mov        ebp, esp
0040b803  and        esp, 0xfffffff8
0040b806  sub        esp, 0x14
0040b809  push       esi
0040b80a  mov        esi, eax
0040b80c  mov        eax, dword ptr [esi + 0x940]
0040b812  cmp        eax, dword ptr [esi + 0x964]
0040b818  jl         0x40b82e

// block 0040b81a
0040b81a  and        dword ptr [esi + 0x528], 0xdfffffff
0040b824  mov        eax, 1
0040b829  pop        esi
0040b82a  mov        esp, ebp
0040b82c  pop        ebp
0040b82d  ret        

// block 0040b82e
0040b82e  fld        dword ptr [esi + 0x950]
0040b834  fmul       dword ptr [0x4b2ed0]
0040b83a  fadd       dword ptr [esi + 0x4d4]
0040b840  fstp       dword ptr [esi + 0x4d4]
0040b846  fld        dword ptr [esi + 0x958]
0040b84c  fld        dword ptr [0x4b2ed0]
0040b852  fld        st(0)
0040b854  fmulp      st(2)
0040b856  fxch       st(1)
0040b858  fstp       dword ptr [esp + 0xc]
0040b85c  fld        dword ptr [esi + 0x95c]
0040b862  fmul       st(1)
0040b864  fstp       dword ptr [esp + 0x10]
0040b868  fmul       dword ptr [esi + 0x960]
0040b86e  fstp       dword ptr [esp + 0x14]
0040b872  fld        dword ptr [esi + 0x4c8]
0040b878  fadd       dword ptr [esp + 0xc]
0040b87c  fstp       dword ptr [esi + 0x4c8]
0040b882  fld        dword ptr [esi + 0x4cc]
0040b888  fadd       dword ptr [esp + 0x10]
0040b88c  fstp       dword ptr [esi + 0x4cc]
0040b892  fld        dword ptr [esp + 0x14]
0040b896  fadd       dword ptr [esi + 0x4d0]
0040b89c  fstp       dword ptr [esi + 0x4d0]
0040b8a2  fld        dword ptr [esi + 0x4c8]
0040b8a8  fabs       
0040b8aa  fstp       dword ptr [esp + 8]
0040b8ae  fld        dword ptr [esp + 8]
0040b8b2  fld        qword ptr [0x4a3ff0]
0040b8b8  fcom       st(1)
0040b8ba  fnstsw     ax
0040b8bc  fstp       st(1)
0040b8be  test       ah, 5
0040b8c1  jnp        0x40b8de

// block 0040b8c3
0040b8c3  fld        dword ptr [esi + 0x4cc]
0040b8c9  fabs       
0040b8cb  fstp       dword ptr [esp + 8]
0040b8cf  fld        dword ptr [esp + 8]
0040b8d3  fcompp     
0040b8d5  fnstsw     ax
0040b8d7  test       ah, 0x41
0040b8da  jne        0x40b8ff

// block 0040b8dc
0040b8dc  jmp        0x40b8e0

// block 0040b8de
0040b8de  fstp       st(0)

// block 0040b8e0
0040b8e0  fld        dword ptr [esi + 0x4cc]
0040b8e6  fld        dword ptr [esi + 0x4c8]
0040b8ec  call       0x4937aa

// block 0040b8f1
0040b8f1  fstp       dword ptr [esp + 8]
0040b8f5  fld        dword ptr [esp + 8]
0040b8f9  fstp       dword ptr [esi + 0x4d8]

// block 0040b8ff
0040b8ff  add        esi, 0x93c
0040b905  call       0x464a80

// block 0040b90a
0040b90a  xor        eax, eax
0040b90c  pop        esi
0040b90d  mov        esp, ebp
0040b90f  pop        ebp
0040b910  ret        
