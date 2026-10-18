export function createPackageMutationQueue() {
  const tails = new Map();

  return Object.freeze({
    run(key, operation) {
      if (typeof key !== "string" || !key || typeof operation !== "function") {
        return Promise.reject(new Error("invalid Package mutation"));
      }
      const previous = tails.get(key) || Promise.resolve();
      const result = previous.then(operation);
      const tail = result.then(() => undefined, () => undefined);
      tails.set(key, tail);
      void tail.then(() => {
        if (tails.get(key) === tail) tails.delete(key);
      });
      return result;
    },
  });
}
