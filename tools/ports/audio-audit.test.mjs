import test from 'node:test';
import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {runNativeCpp} from '../../launcher/tests/support/native-cpp.mjs';

const includes=[resolve(import.meta.dirname,'../../ports/th08/web/cpp/sdl')];
test('native audio audit distinguishes operations and snapshot reads have no side effects',async()=>{
  const out=await runNativeCpp(`#define TH_PRESENTATION_AUDIT 1
#include "AudioAudit.hpp"
#include <cassert>
#include <array>
#include <iostream>
int main(){
 using namespace th08::audit;
 constexpr unsigned count=audio_operation_count;
 static_assert(count==23);
 const auto* snapshot=audio_snapshot();
 assert(snapshot[0]==1&&snapshot[1]==0);
 for(unsigned i=0;i<count;++i)assert(snapshot[i+2]==0);
 audio_operation(AudioOperation::Play);
 audio_operation(AudioOperation::Play);
 audio_operation(AudioOperation::ManagerOperation);
 assert(snapshot[1]==3&&snapshot[2+unsigned(AudioOperation::Play)]==2);
 assert(snapshot[2+unsigned(AudioOperation::ManagerOperation)]==1);
 std::array<unsigned,count+2> before;
 for(unsigned i=0;i<count+2;++i)before[i]=snapshot[i];
 for(int read=0;read<1000;++read){assert(audio_snapshot()==snapshot);for(unsigned i=0;i<count+2;++i)assert(snapshot[i]==before[i]);}
 std::cout<<"synchronous audio counter receipt is stable";
}`,{includes});assert.match(out,/receipt is stable/);
});
test('ordinary native build compiles the same audio call sites without diagnostic state',async()=>{
  const out=await runNativeCpp(`#include "AudioAudit.hpp"
#include <iostream>
int main(){for(unsigned i=0;i<th08::audit::audio_operation_count;++i)th08::audit::audio_operation(static_cast<th08::audit::AudioOperation>(i));std::cout<<"ordinary build";}
`,{includes});assert.equal(out,'ordinary build');
});
