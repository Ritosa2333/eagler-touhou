import {TICK_MS} from '../frame-queue.js';

export const SYNC_PROTOCOL='th03-rollback/7';
export const MAX_INPUT_DELAY=6,RTT_SAMPLES=3,ZERO_DELAY_RTT_MS=20;
export const HYBRID_RESERVE=2,PREDICTION_JITTER_STEPS=2;
export function validInputReport(report){
  return report&&typeof report.rollbackEnabled==='boolean'
    &&Number.isSafeInteger(report.rttMs)&&report.rttMs>=0
    &&Number.isSafeInteger(report.maxRttMs)&&report.maxRttMs>=report.rttMs&&report.maxRttMs<=60000;
}
export function chooseInputPolicy(local,remote){
  if(!validInputReport(local)||!validInputReport(remote)||local.rollbackEnabled!==remote.rollbackEnabled)throw Error('双方输入缓冲策略不一致');
  const rttMs=Math.max(local.rttMs,remote.rttMs),rollbackEnabled=local.rollbackEnabled;
  const transitSteps=rollbackEnabled&&rttMs>ZERO_DELAY_RTT_MS?Math.ceil(rttMs/2/TICK_MS):0;
  const predictionSteps=Math.min(HYBRID_RESERVE,Math.max(0,transitSteps-1));
  const inputDelay=Math.min(MAX_INPUT_DELAY,transitSteps-predictionSteps);
  const predictionLead=rollbackEnabled?predictionSteps+PREDICTION_JITTER_STEPS:0;
  return {schema:2,rollbackEnabled,rttMs,transitSteps,inputDelay,predictionSteps,predictionLead};
}
export function validInputPolicy(policy){
  if(!policy)return false;
  const report={rollbackEnabled:policy.rollbackEnabled,rttMs:policy.rttMs,maxRttMs:policy.rttMs};
  if(!validInputReport(report))return false;
  const expected=chooseInputPolicy(report,report);
  return Object.keys(policy).length===Object.keys(expected).length&&Object.entries(expected).every(([key,value])=>policy[key]===value);
}
