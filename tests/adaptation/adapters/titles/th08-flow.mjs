import {TITLE_FLOW_STEPS_SCHEMA} from '../flow-steps.mjs';
const source='ports/th08/web/cpp/game/TitleMenus.cpp | OnUpdateStartMenu | OnUpdateDifficultySelect | OnUpdateCharacterSelect';
const at=(field,value)=>({path:['status',field],op:'eq',value});
const all=(...values)=>({all:values});
const menu=(screen)=>all(at('supervisorActive',1),at('active',1),at('loading',0),at('titleScreen',screen),at('titlePhase',1));
const predicate=condition=>({condition,source});
const wait=until=>({op:'wait',until,source});
const press=(input,when)=>({op:'press',input,when,source});
/** Source-owned native gates replace repeated confirmation through loading and gameplay. */
export const TH08_TITLE_FLOW={schema:TITLE_FLOW_STEPS_SCHEMA,needsControlWarmup:true,
  budget:{timeoutMs:30000,maxSteps:12,maxPresses:12},
  predicates:{
    startReady:predicate(all(menu(0),at('titleInputReady',true))),
    startSelected:predicate(all(menu(0),at('titleInputReady',true),at('cursor',0))),
    difficultyReady:predicate(all(menu(4),at('titleInputReady',true))),
    characterReady:predicate(all(menu(5),{path:['status','titleStateTimer2'],op:'gte',value:8})),
    paused:{condition:all(at('supervisorActive',2),{path:['pause','active'],op:'eq',value:true}),
      source:'ports/th08/web/cpp/game/UiMenus.cpp | paused native R restart'},
  },
  enter:[wait('startReady'),{op:'press-until',input:'ArrowUp',when:'startReady',until:'startSelected',maxPresses:8,source},
    press('KeyZ','startSelected'),wait('difficultyReady'),press('KeyZ','difficultyReady'),wait('characterReady'),
    press('KeyZ','characterReady'),{op:'wait',until:'gameplayReady',source:'ports/th08/web/cpp/game/GameplayStart.cpp | completed ordinary gameplay startup'}],
  restart:[{op:'wait',until:'paused',source:'ports/th08/web/cpp/game/UiMenus.cpp | pause state'},
    {op:'press',input:'KeyR',when:'paused',source:'ports/th08/web/cpp/game/UiMenus.cpp | R restart'},
    {op:'wait',until:'gameplayReady',source:'ports/th08/web/cpp/game/GameApplication.cpp | restart and GameplayScene load'}],
};
