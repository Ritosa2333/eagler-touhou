#include "../../../source_reconstruction/help_system/help.hpp"
extern "C" __attribute__((export_name("manual_probe"))) const int* manual_probe(){using namespace th20::source;static int s[5];auto* h=help::controller();s[0]=h?h->state:-1;s[1]=h?h->substate:-1;s[2]=h?h->age.current:-1;s[3]=h?h->cursor.current:-1;s[4]=h?h->finished:-1;return s;}

