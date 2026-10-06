
// block 004d9cd4
004d9cd4  cmp        eax, 0x7d21de3c
004d9cd9  cmp        ebp, esp
004d9cdb  adc        dword ptr [ecx], eax
004d9cdd  mov        bh, 0xbd
004d9cdf  and        edi, dword ptr [ebx]
004d9ce1  dec        edx

// block 004d9d31
004d9d31  sub        esi, ebx
004d9d33  inc        ecx
004d9d34  sbb        al, 0xe
004d9d36  sub        dword ptr [eax + 4], esi
004d9d39  sub        eax, 0x6446896e
004d9d3e  test       bh, dh
004d9d40  mov        dword ptr [0xc30b0dc8], eax
004d9d45  jg         0x4d9cd4

// block 004d9d47
004d9d47  jns        0x4d9d9f

// block 004d9d49
004d9d49  popfd      
004d9d4a  sahf       

// block 004d9d9f
004d9d9f  push       ebp
004d9da0  clc        

// block 004d9da1
004d9da1  jno        0x4d9db2

// block 004d9da3
004d9da3  mov        byte ptr [edx], al
004d9da5  xlatb      

// block 004d9db2
004d9db2  push       ecx
004d9db3  std        
004d9db4  nop        
004d9db5  sbb        edx, edx
004d9db7  aas        
004d9db8  out        dx, al
004d9db9  sbb        byte ptr [edi - 0x2c], al
004d9dbc  cmp        eax, eax
004d9dbe  scasd      eax, dword ptr es:[edi]
004d9dbf  add        ebx, ebp
004d9dc1  mov        esp, 0xe95eb27c
004d9dc6  push       edi
004d9dc7  in         eax, 3
004d9dc9  xchg       ebx, eax
004d9dcb  jne        0x4d9da1

// block 004d9dcd
004d9dcd  inc        esp
004d9dce  xchg       byte ptr [esi - 0x4525ab9e], al
004d9dd4  sub        eax, 0xf3e177e1
004d9dd9  dec        edx
004d9dda  int        0xf7

// block 004d9ddc
004d9ddc  lea        ebp, [ecx - 0x527c5bcd]
004d9de2  push       ecx
004d9de3  and        eax, 0x5c2dafa9
004d9de8  sbb        al, 0x46
004d9dea  xor        ebp, ebx
004d9dec  xor        esp, ebp
004d9dee  test       al, 0x52
004d9df0  mov        ah, 0x68
004d9df2  test       byte ptr [edx], bl
004d9df4  jo         0x4d9e35

// block 004d9df6
004d9df6  aad        0xbe
004d9df8  jg         0x4d9e36

// block 004d9e35
004d9e35  add        eax, dword ptr [esi + edi*2 - 0x3f]
004d9e39  xor        al, 0xd9
004d9e3b  pop        ebx
