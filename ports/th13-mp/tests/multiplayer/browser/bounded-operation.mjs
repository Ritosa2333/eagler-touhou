// Fail closed: timing out does not imply cancellation of an underlying CDP or
// file operation. The caller must treat its data as incomplete and not retry a
// timed-out stateful operation concurrently.
export async function boundedOperation(label, operation, milliseconds = 15000) {
  if (!Number.isFinite(milliseconds) || milliseconds <= 0) throw Error('Invalid deadline');
  let expired = false, timer;
  try {
    return await Promise.race([
      Promise.resolve().then(() => operation(() => expired)),
      new Promise((_, reject) => {timer = setTimeout(() => {expired = true; reject(Error(`${label} timed out after ${milliseconds}ms`));}, milliseconds);}),
    ]);
  } finally { clearTimeout(timer); }
}
