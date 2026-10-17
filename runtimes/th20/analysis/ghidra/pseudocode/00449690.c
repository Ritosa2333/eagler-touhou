/* Entry: 0x00449690; symbol: IntersectWith; body bytes: 26 */

/* Library Function - Single Match
    public: void __thiscall Concurrency::details::HardwareAffinity::IntersectWith(unsigned long)
   
   Libraries: Visual Studio 2012, Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

void __thiscall
Concurrency::details::HardwareAffinity::IntersectWith(HardwareAffinity *this,ulong param_1)

{
  *(uint *)this = *(uint *)this & param_1;
  return;
}

