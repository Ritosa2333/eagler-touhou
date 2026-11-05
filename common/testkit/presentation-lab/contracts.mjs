export const DRIVER_API_VERSION = 1;
export const OBSERVATION_API_VERSION = 1;

export const TICK_STATUS = Object.freeze({
  ADVANCED: 'advanced',
  BLOCKED_LOADING: 'blocked-loading',
  FINISHED: 'finished',
  ERROR: 'error',
});

const requiredDriverMethods = [
  'describe', 'freeze', 'resume', 'advanceOneTick', 'drawOnly',
  'setInput', 'clearInput', 'close',
];
const requiredObserverMethods = [
  'enable', 'readReferences', 'readObservation', 'readStateEvidence',
  'readTiming', 'readTrace', 'readScene', 'worldFrozen', 'readGate',
  'setNegativeControl', 'captureImage', 'analyze', 'compact', 'mergeIssues',
];

function requireObject(value, label) {
  if (!value || typeof value !== 'object') throw Error(`${label} must be an object`);
}
function requireMethods(value, names, label) {
  requireObject(value, label);
  for (const name of names)
    if (typeof value[name] !== 'function') throw Error(`${label} is missing ${name}()`);
}

export function validateCapabilities(value) {
  requireObject(value, 'Presentation Lab capabilities');
  if (value.driverApiVersion !== DRIVER_API_VERSION)
    throw Error(`Unsupported Presentation Lab driver API ${value.driverApiVersion}`);
  if (typeof value.game !== 'string' || !value.game)
    throw Error('Presentation Lab capabilities require a game identifier');
  if (typeof value.adapterVersion !== 'string' || !value.adapterVersion)
    throw Error('Presentation Lab capabilities require an adapter version');
  if (typeof value.nativeAbi !== 'string' || !value.nativeAbi)
    throw Error('Presentation Lab capabilities require a native ABI identifier');
  requireObject(value.features, 'Presentation Lab feature declaration');
  for (const feature of ['freeze', 'resume', 'step', 'drawOnly', 'references', 'stateEvidence', 'timing', 'capture'])
    if (value.features[feature] !== true)
      throw Error(`Presentation Lab adapter does not provide required feature ${feature}`);
  return value;
}

export function validateRuntimeDriver(driver) {
  requireMethods(driver, requiredDriverMethods, 'Presentation Lab runtime driver');
  return validateCapabilities(driver.describe());
}
export function validateObservationAdapter(observer) {
  requireMethods(observer, requiredObserverMethods, 'Presentation Lab observation adapter');
  if (observer.observationApiVersion !== OBSERVATION_API_VERSION)
    throw Error(`Unsupported Presentation Lab observation API ${observer.observationApiVersion}`);
  if (typeof observer.scanSchema !== 'string' || typeof observer.sessionSchema !== 'string')
    throw Error('Presentation Lab observation adapter requires report schemas');
  return observer;
}
export function validateTickReceipt(receipt) {
  requireObject(receipt, 'Presentation Lab tick receipt');
  if (!Object.values(TICK_STATUS).includes(receipt.status))
    throw Error(`Invalid Presentation Lab tick status ${receipt.status}`);
  if (!Number.isInteger(receipt.advancedTicks) || receipt.advancedTicks < 0 || receipt.advancedTicks > 1)
    throw Error('Presentation Lab tick receipt must report zero or one advanced tick');
  if (receipt.status === TICK_STATUS.ADVANCED && receipt.advancedTicks !== 1)
    throw Error('An advanced Presentation Lab tick must report one advanced tick');
  if (receipt.status !== TICK_STATUS.ADVANCED && receipt.advancedTicks !== 0)
    throw Error('A non-advanced Presentation Lab tick must report zero advanced ticks');
  return receipt;
}
export function validateReferencePair(pair) {
  requireObject(pair, 'Presentation Lab reference pair');
  requireObject(pair.previous, 'Presentation Lab previous reference');
  requireObject(pair.current, 'Presentation Lab current reference');
  return pair;
}
