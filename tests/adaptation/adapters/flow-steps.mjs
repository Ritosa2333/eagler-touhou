export const TITLE_FLOW_STEPS_SCHEMA='eagler-touhou/title-flow-steps/1';

const LEGACY_STRATEGIES=new Set(['confirm','th20-menu']);
const INPUT_CODES=/^(?:Key[A-Z]|Digit[0-9]|Arrow(?:Up|Down|Left|Right)|Enter|Escape|Space|ShiftLeft|ControlLeft)$/;
const IDENTIFIER=/^[A-Za-z_$][\w$]*$/;
const FORBIDDEN_PATH_SEGMENTS=new Set(['__proto__','prototype','constructor']);
const PREDICATE_OPERATORS=new Set(['eq','ne','in','exists','truthy','falsy','gt','gte','lt','lte']);
const FLOW_KEYS=new Set(['schema','needsControlWarmup','budget','predicates','enter','restart']);
const BUDGET_KEYS=new Set(['timeoutMs','maxSteps','maxPresses']);
const CONDITION_KEYS=new Set(['all','any','not','path','op','value']);

/** Map a validated Runtime flow key to the physical keyboard protocol fields. */
export function titleFlowKeyFields(code){
  if(typeof code!=='string'||!INPUT_CODES.test(code))
    throw new RangeError(`Unsupported title flow input code ${String(code)}`);
  if(/^Key[A-Z]$/.test(code)){
    const letter=code.slice(3);
    return{code,key:letter.toLowerCase(),keyCode:letter.charCodeAt(0),location:0};
  }
  if(/^Digit[0-9]$/.test(code)){
    const digit=code.slice(5);
    return{code,key:digit,keyCode:digit.charCodeAt(0),location:0};
  }
  const fields={
    ArrowUp:{key:'ArrowUp',keyCode:38},ArrowDown:{key:'ArrowDown',keyCode:40},
    ArrowLeft:{key:'ArrowLeft',keyCode:37},ArrowRight:{key:'ArrowRight',keyCode:39},
    Enter:{key:'Enter',keyCode:13},Escape:{key:'Escape',keyCode:27},Space:{key:' ',keyCode:32},
    ShiftLeft:{key:'Shift',keyCode:16,location:1},ControlLeft:{key:'Control',keyCode:17,location:1},
  }[code];
  return{code,...fields,location:fields.location??0};
}

function sourceAnchor(value){return typeof value==='string'&&value.trim().length>=3;}
function plainRecord(value){
  if(!value||typeof value!=='object'||Array.isArray(value))return false;
  const prototype=Object.getPrototypeOf(value);
  return prototype===Object.prototype||prototype===null;
}
function jsonData(value,path,issues,seen=new Set(),counter={nodes:0},depth=0){
  counter.nodes++;
  if(counter.nodes>2048||depth>24){
    issues.push({path,reason:'flow declaration exceeds its data size or depth limit'});
    return;
  }
  if(value===null||typeof value==='string'||typeof value==='boolean')return;
  if(typeof value==='number'){
    if(!Number.isFinite(value))issues.push({path,reason:'numbers must be finite'});
    return;
  }
  if(typeof value!=='object'){
    issues.push({path,reason:'flow declarations must contain only JSON data'});
    return;
  }
  if(seen.has(value)){
    issues.push({path,reason:'cyclic declarations are not supported'});
    return;
  }
  if(!Array.isArray(value)&&!plainRecord(value)){
    issues.push({path,reason:'flow declarations must use plain objects and arrays'});
    return;
  }
  seen.add(value);
  for(const key of Reflect.ownKeys(value)){
    if(typeof key!=='string'){
      issues.push({path,reason:'symbol properties are not serializable'});
      continue;
    }
    const descriptor=Object.getOwnPropertyDescriptor(value,key);
    if(!descriptor||descriptor.get||descriptor.set){
      issues.push({path:`${path}.${key}`,reason:'accessors are not permitted in flow declarations'});
      continue;
    }
    jsonData(descriptor.value,Array.isArray(value)?`${path}[${key}]`:`${path}.${key}`,issues,seen,counter,depth+1);
  }
  seen.delete(value);
}
function exactKeys(value,allowed,path,issues){
  for(const key of Object.keys(value))if(!allowed.has(key))issues.push({path:`${path}.${key}`,reason:'unknown declaration field'});
}
function reference(value,predicates,path,issues){
  const names=plainRecord(predicates)?predicates:{};
  if(typeof value!=='string'||(!Object.hasOwn(names,value)&&value!=='gameplayReady'))
    issues.push({path,reason:'must name a declared predicate or gameplayReady'});
}
function validateCondition(node,path,issues,{depth=0,nodes={value:0}}={}){
  nodes.value++;
  if(depth>8||nodes.value>64){issues.push({path,reason:'predicate tree exceeds its depth or node limit'});return;}
  if(!plainRecord(node)){issues.push({path,reason:'predicate must be a plain object'});return;}
  const keys=Object.keys(node);
  if(keys.length===1&&(keys[0]==='all'||keys[0]==='any')){
    const children=node[keys[0]];
    if(!Array.isArray(children)||children.length<1||children.length>16){
      issues.push({path,reason:'all/any predicates need 1 through 16 children'});return;
    }
    for(let index=0;index<children.length;index++)validateCondition(children[index],`${path}.${keys[0]}[${index}]`,issues,{depth:depth+1,nodes});
    return;
  }
  if(keys.length===1&&keys[0]==='not'){
    validateCondition(node.not,`${path}.not`,issues,{depth:depth+1,nodes});return;
  }
  exactKeys(node,CONDITION_KEYS,path,issues);
  if(!Array.isArray(node.path)||node.path.length<1||node.path.length>8||
    node.path.some(segment=>typeof segment!=='string'||!IDENTIFIER.test(segment)||FORBIDDEN_PATH_SEGMENTS.has(segment)))
    issues.push({path:`${path}.path`,reason:'path must be 1 through 8 safe own-property names'});
  if(!PREDICATE_OPERATORS.has(node.op)){
    issues.push({path:`${path}.op`,reason:'predicate operator is not in the supported closed set'});return;
  }
  if(node.op==='exists'||node.op==='truthy'||node.op==='falsy'){
    if(Object.hasOwn(node,'value'))issues.push({path:`${path}.value`,reason:`${node.op} does not take a value`});
    return;
  }
  if(!Object.hasOwn(node,'value')){issues.push({path:`${path}.value`,reason:`${node.op} requires a value`});return;}
  if(node.op==='in'){
    if(!Array.isArray(node.value)||node.value.length<1||node.value.length>32||
      node.value.some(value=>value!==null&&!['string','number','boolean'].includes(typeof value)))
      issues.push({path:`${path}.value`,reason:'in requires 1 through 32 primitive values'});
  }else if(node.value!==null&&!['string','number','boolean'].includes(typeof node.value))
    issues.push({path:`${path}.value`,reason:'predicate values must be primitive'});
  if(['gt','gte','lt','lte'].includes(node.op)&&typeof node.value!=='number')
    issues.push({path:`${path}.value`,reason:`${node.op} requires a finite number`});
}

function validateSteps(steps,path,flow,issues,{entry=false}={}){
  if(!Array.isArray(steps)||steps.length<1||steps.length>flow.budget.maxSteps){
    issues.push({path,reason:`must contain 1 through ${flow.budget.maxSteps} bounded steps`});return;
  }
  if(steps[0]?.op!=='wait')issues.push({path:`${path}[0]`,reason:'a flow must wait for an observed starting state before input'});
  if(steps.at(-1)?.op!=='wait')issues.push({path:`${path}[${steps.length-1}]`,reason:'flow must end on an observed state'});
  if(entry&&steps.at(-1)?.until!=='gameplayReady')
    issues.push({path:`${path}[${steps.length-1}].until`,reason:'entry flow must end on native gameplayReady'});
  let maxPresses=0;
  for(let index=0;index<steps.length;index++){
    const step=steps[index],stepPath=`${path}[${index}]`;
    if(!plainRecord(step)){issues.push({path:stepPath,reason:'step must be a plain object'});continue;}
    if(!sourceAnchor(step.source))issues.push({path:`${stepPath}.source`,reason:'step needs a production source anchor'});
    if(step.label!==undefined&&!(typeof step.label==='string'&&step.label.trim()&&step.label.length<=120))
      issues.push({path:`${stepPath}.label`,reason:'label must be a nonempty string of at most 120 characters'});
    if(step.op==='wait'){
      exactKeys(step,new Set(['op','until','label','timeoutMs','source']),stepPath,issues);
      reference(step.until,flow.predicates,`${stepPath}.until`,issues);
      if(step.timeoutMs!==undefined&&(!Number.isSafeInteger(step.timeoutMs)||step.timeoutMs<1||step.timeoutMs>flow.budget.timeoutMs))
        issues.push({path:`${stepPath}.timeoutMs`,reason:'wait timeout must fit the flow time budget'});
      continue;
    }
    if(step.op==='press'){
      exactKeys(step,new Set(['op','input','when','label','source']),stepPath,issues);
      if(typeof step.input!=='string'||!INPUT_CODES.test(step.input))issues.push({path:`${stepPath}.input`,reason:'input code is not supported'});
      reference(step.when,flow.predicates,`${stepPath}.when`,issues);
      maxPresses++;
      continue;
    }
    if(step.op==='press-until'){
      exactKeys(step,new Set(['op','input','when','until','maxPresses','label','source']),stepPath,issues);
      if(typeof step.input!=='string'||!INPUT_CODES.test(step.input))issues.push({path:`${stepPath}.input`,reason:'input code is not supported'});
      reference(step.when,flow.predicates,`${stepPath}.when`,issues);
      reference(step.until,flow.predicates,`${stepPath}.until`,issues);
      if(!Number.isSafeInteger(step.maxPresses)||step.maxPresses<1||step.maxPresses>flow.budget.maxPresses)
        issues.push({path:`${stepPath}.maxPresses`,reason:'press-until requires an explicit bounded press count'});
      else maxPresses+=step.maxPresses;
      continue;
    }
    issues.push({path:`${stepPath}.op`,reason:'step operation is not in the supported closed set'});
  }
  if(maxPresses>flow.budget.maxPresses)
    issues.push({path:`${path}`,reason:`maximum input count ${maxPresses} exceeds the declared flow budget`});
}

/** Validate legacy strategies and finite title-owned flow data without executing it. */
function validateFlow(flow,{gameplayEntry=true}={}){
  const issues=[];
  try{jsonData(flow,'flow',issues);}catch(error){
    issues.push({path:'flow',reason:`declaration could not be inspected: ${error?.message??String(error)}`});
  }
  if(issues.length)return{status:'failed',issues};
  if(!plainRecord(flow))return{status:'failed',issues:[{path:'flow',reason:'flow must be a plain object'}]};
  if(flow.schema===undefined){
    exactKeys(flow,new Set(['strategy','needsControlWarmup']),'flow',issues);
    if(!LEGACY_STRATEGIES.has(flow.strategy))issues.push({path:'flow.strategy',reason:'legacy strategy is unsupported'});
    if(typeof flow.needsControlWarmup!=='boolean')issues.push({path:'flow.needsControlWarmup',reason:'must be boolean'});
    return{status:issues.length?'failed':'passed',issues};
  }
  exactKeys(flow,FLOW_KEYS,'flow',issues);
  if(flow.schema!==TITLE_FLOW_STEPS_SCHEMA)issues.push({path:'flow.schema',reason:'unsupported title flow schema'});
  if(typeof flow.needsControlWarmup!=='boolean')issues.push({path:'flow.needsControlWarmup',reason:'must be boolean'});
  const budget=flow.budget;
  if(!plainRecord(budget)){
    issues.push({path:'flow.budget',reason:'bounded flow budget is required'});
    return{status:'failed',issues};
  }
  exactKeys(budget,BUDGET_KEYS,'flow.budget',issues);
  if(!Number.isSafeInteger(budget.timeoutMs)||budget.timeoutMs<1||budget.timeoutMs>120000)
    issues.push({path:'flow.budget.timeoutMs',reason:'total flow timeout must be between 1 and 120000 ms'});
  if(!Number.isSafeInteger(budget.maxSteps)||budget.maxSteps<1||budget.maxSteps>32)
    issues.push({path:'flow.budget.maxSteps',reason:'maxSteps must be between 1 and 32'});
  if(!Number.isSafeInteger(budget.maxPresses)||budget.maxPresses<0||budget.maxPresses>64)
    issues.push({path:'flow.budget.maxPresses',reason:'maxPresses must be between 0 and 64'});
  if(issues.length)return{status:'failed',issues};
  const predicates=plainRecord(flow.predicates)?flow.predicates:{};
  if(!plainRecord(flow.predicates))issues.push({path:'flow.predicates',reason:'named predicates are required'});
  else{
    const predicateNames=Object.keys(flow.predicates);
    if(predicateNames.length<1||predicateNames.length>32)issues.push({path:'flow.predicates',reason:'declare 1 through 32 named predicates'});
    for(const key of predicateNames){
      const declaration=flow.predicates[key],path=`flow.predicates.${key}`;
      if(!IDENTIFIER.test(key)||key==='gameplayReady')issues.push({path,reason:'predicate name is unsafe or reserved'});
      if(!plainRecord(declaration)){issues.push({path,reason:'predicate declaration must be a plain object'});continue;}
      exactKeys(declaration,new Set(['condition','source']),path,issues);
      if(!sourceAnchor(declaration.source))issues.push({path:`${path}.source`,reason:'predicate needs a production source anchor'});
      validateCondition(declaration.condition,`${path}.condition`,issues);
    }
  }
  const safeFlow={...flow,predicates};
  validateSteps(flow.enter,'flow.enter',safeFlow,issues,{entry:gameplayEntry});
  if(flow.restart!==undefined)validateSteps(flow.restart,'flow.restart',safeFlow,issues);
  const status=issues.length?'failed':'passed';
  return{status,issues};
}

export function validateTitleFlow(flow){return validateFlow(flow);}
/** The same finite input grammar may end at a declared non-gameplay scene. */
export function validateObservationFlow(flow){
  if(flow?.schema!==TITLE_FLOW_STEPS_SCHEMA)return{status:'failed',issues:[{path:'flow.schema',reason:'observation flow requires bounded steps'}]};
  return validateFlow(flow,{gameplayEntry:false});
}

function readPath(state,path){
  let value=state;
  for(const key of path){
    if(value===null||typeof value!=='object'||!Object.hasOwn(value,key))return{exists:false,value:undefined};
    value=value[key];
  }
  return{exists:true,value};
}
function evaluateCondition(state,node){
  if(Object.hasOwn(node,'all')){
    const values=node.all.map(child=>evaluateCondition(state,child));
    return values.includes(false)?false:values.includes(null)?null:true;
  }
  if(Object.hasOwn(node,'any')){
    const values=node.any.map(child=>evaluateCondition(state,child));
    return values.includes(true)?true:values.includes(null)?null:false;
  }
  if(Object.hasOwn(node,'not')){
    const value=evaluateCondition(state,node.not);return value===null?null:!value;
  }
  const actual=readPath(state,node.path);
  if(node.op==='exists')return actual.exists;
  if(!actual.exists)return null;
  const value=actual.value,expected=node.value;
  switch(node.op){
    case'eq':return value===expected;
    case'ne':return value!==expected;
    case'in':return node.value.includes(value);
    case'truthy':return Boolean(value);
    case'falsy':return !value;
    case'gt':case'gte':case'lt':case'lte':
      if(typeof value!=='number'||typeof expected!=='number'||!Number.isFinite(value))return null;
      if(node.op==='gt')return value>expected;
      if(node.op==='gte')return value>=expected;
      if(node.op==='lt')return value<expected;
      return value<=expected;
    default:return null;
  }
}
function testNamedPredicate(flow,name,state,isGameplayReady){
  if(name==='gameplayReady')return Boolean(isGameplayReady?.(state));
  const declaration=flow.predicates[name];
  return declaration?evaluateCondition(state,declaration.condition):null;
}

function incomplete(message,details={}){
  const error=new Error(message);
  error.name='TitleFlowIncomplete';error.status='incomplete';error.evidence=details;
  return error;
}
function requireCallback(value,name){
  if(typeof value!=='function')throw new TypeError(`title flow steps require ${name}()`);
  return value;
}

async function withinBudget(operation,deadline,details){
  const timeoutMs=Math.max(0,deadline-Date.now());
  if(timeoutMs===0)throw incomplete('Title flow exhausted its time budget before an IO operation.',details);
  let timer;
  const pending=Promise.resolve().then(operation);
  pending.catch(()=>{});
  try{
    return await Promise.race([pending,new Promise((_,reject)=>{
      timer=setTimeout(()=>reject(incomplete('Title flow IO did not finish within its time budget.',
        {...details,timeoutMs,pendingOperationMaySettle:true})),timeoutMs);
    })]);
  }finally{clearTimeout(timer);}
}

/** Execute a validated finite flow through production observation, input and wait callbacks. */
async function executeFlowSteps({flow,phase='enter',observe,press,waitUntil,
  isGameplayReady,onTitleReady,labelSuffix='',timeoutMs}={},validate){
  const validation=validate(flow);
  if(validation.status!=='passed'||flow.schema!==TITLE_FLOW_STEPS_SCHEMA)
    throw new TypeError(`Invalid declarative title flow: ${validation.issues.map(item=>`${item.path}: ${item.reason}`).join('; ')}`);
  if(phase!=='enter'&&phase!=='restart')throw new RangeError(`Unknown title-flow phase ${String(phase)}`);
  const steps=phase==='enter'?flow.enter:flow.restart;
  if(!steps){
    if(phase==='restart')throw incomplete('This title flow does not declare a restart operation.',
      {phase,capability:'restart',gameplayInputSent:false});
    return null;
  }
  requireCallback(waitUntil,'waitUntil');
  const started=Date.now(),totalBudget=Math.min(flow.budget.timeoutMs,
    Number.isSafeInteger(timeoutMs)&&timeoutMs>0?timeoutMs:flow.budget.timeoutMs);
  const deadline=started+totalBudget;
  let actionCount=0,lastState=null,firstWait=true;
  const remaining=()=>Math.max(0,deadline-Date.now());
  const suffix=labelSuffix?` ${labelSuffix}`:'';
  const stepLabel=(step,index)=>`${step.label??`${phase} step ${index+1}`}${suffix}`;
  const currentState=async(step,index)=>{
    if(typeof observe==='function')return withinBudget(()=>observe(),deadline,{phase,step:index,io:'observe'});
    const available=remaining();
    if(available<=0)throw incomplete(`Title flow exhausted its ${totalBudget} ms budget before observing state.`,{phase,step:index});
    return withinBudget(()=>waitUntil(`${stepLabel(step,index)} current state`,()=>true,available),deadline,
      {phase,step:index,io:'waitUntil-current-state'});
  };
  for(let index=0;index<steps.length;index++){
    const step=steps[index],name=stepLabel(step,index),left=remaining();
    if(left<=0)throw incomplete(`Title flow exhausted its ${totalBudget} ms budget.`,{phase,step:index});
    if(step.op==='wait'){
      const predicate=state=>testNamedPredicate(flow,step.until,state,isGameplayReady)===true;
      let state;
      try {
        state=await withinBudget(()=>waitUntil(name,predicate,Math.min(left,step.timeoutMs??left)),
          Math.min(deadline,Date.now()+Math.min(left,step.timeoutMs??left)),{phase,step:index,io:'waitUntil'});
      } catch(error) {
        if(error?.status==='incomplete')throw error;
        throw incomplete(`${name}: required native state was not observed within budget.`,
          {phase,step:index,predicate:step.until,cause:error?.message??String(error)});
      }
      if(testNamedPredicate(flow,step.until,state,isGameplayReady)!==true)
        throw incomplete(`${name}: waitUntil returned without the declared predicate being true.`,
          {phase,step:index,predicate:step.until,state:state??null});
      lastState=state;
      if(phase==='enter'&&firstWait&&typeof onTitleReady==='function')
        await withinBudget(()=>onTitleReady(state),deadline,{phase,step:index,io:'onTitleReady'});
      firstWait=false;
      continue;
    }
    requireCallback(press,'press');
    let state=await currentState(step,index);
    const precondition=testNamedPredicate(flow,step.when,state,isGameplayReady);
    if(precondition!==true)
      throw incomplete(`${name}: input precondition ${step.when} was not observed.`,
        {phase,step:index,predicate:step.when,state:state??null});
    if(step.op==='press'){
      if(++actionCount>flow.budget.maxPresses)throw incomplete(`${name}: flow exceeded its input action budget.`,{phase,step:index});
      await withinBudget(()=>press(step.input),deadline,{phase,step:index,io:'press',input:step.input});lastState=null;
      continue;
    }
    let reached=testNamedPredicate(flow,step.until,state,isGameplayReady);
    if(reached===null)throw incomplete(`${name}: target predicate ${step.until} is missing from the observed state.`,{phase,step:index,state:state??null});
    for(let count=0;reached!==true&&count<step.maxPresses;count++){
      if(actionCount>=flow.budget.maxPresses)throw incomplete(`${name}: flow exceeded its input action budget.`,{phase,step:index});
      await withinBudget(()=>press(step.input),deadline,{phase,step:index,io:'press',input:step.input});actionCount++;
      const available=remaining();
      if(available<=0)throw incomplete(`${name}: flow exhausted its time budget while navigating.`,{phase,step:index,presses:count+1});
      state=await currentState(step,index);
      reached=testNamedPredicate(flow,step.until,state,isGameplayReady);
      if(reached===null)throw incomplete(`${name}: target predicate ${step.until} is missing from the observed state.`,
        {phase,step:index,state:state??null});
      if(reached===true)break;
      const guard=testNamedPredicate(flow,step.when,state,isGameplayReady);
      if(guard!==true)throw incomplete(`${name}: navigation left precondition ${step.when} before reaching ${step.until}.`,
        {phase,step:index,predicate:step.when,state:state??null});
    }
    if(reached!==true)throw incomplete(`${name}: target predicate ${step.until} was not reached within ${step.maxPresses} presses.`,
      {phase,step:index,maxPresses:step.maxPresses,state:state??null});
    lastState=state;
  }
  return lastState;
}

export function executeTitleFlowSteps(options){return executeFlowSteps(options,validateTitleFlow);}
export function executeObservationFlowSteps(options){return executeFlowSteps(options,validateObservationFlow);}
export function matchesObservationFlowEnd(flow,state){
  const validation=validateObservationFlow(flow);if(validation.status!=='passed')throw new TypeError('Invalid observed scene flow');
  return testNamedPredicate(flow,flow.enter.at(-1).until,state,()=>false)===true;
}
