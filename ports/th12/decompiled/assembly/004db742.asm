
// block 004db742
004db742  cmpsb      byte ptr [esi], byte ptr es:[edi]
004db743  cmp        byte ptr [ebx], cl

// block 004db746

// block 004db74a
004db74a  lodsb      al, byte ptr [esi]
004db74b  mov        dh, 0x81
004db74d  cli        

// block 004db74e
004db74e  adc        eax, 0x8883e0e2
004db753  jnp        0x4db7c2

// block 004db7c2
004db7c2  jl         0x4db7db

// block 004db7c5
004db7c5  sub        ecx, dword ptr [edx + 0x1827109d]
004db7cb  xlatb      
004db7cc  xor        byte ptr [esi - 0x5c], ch
004db7cf  mov        al, byte ptr [0x20ed2472]
004db7d4  dec        ebp
004db7d5  call       0x5d4b34a7

// block 004db7da

// block 004db7db
004db7db  scasb      al, byte ptr es:[edi]
004db7dc  aas        
004db7dd  test       eax, 0x355f6722
