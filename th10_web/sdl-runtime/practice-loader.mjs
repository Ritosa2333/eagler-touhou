// A non-THPrac binary has no practice exports or packaged practice modules.
// Decide from the compiled capability, never from a user's UI preference.
export async function createOptionalPractice(services, load = () => import('./practice.mjs')) {
  if (typeof services.core?.practice_enable !== 'function') return null;
  const {createPractice} = await load();
  return createPractice(services);
}
