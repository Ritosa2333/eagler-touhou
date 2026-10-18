export const BUILD_AUTHORITY_PRIVATE = "private";
export const BUILD_AUTHORITY_PUBLICATION = "publication";

const PRIVATE_PROFILE = /^web-(?:development|validation(?:-[a-z0-9]+)*)$/;
const PUBLICATION_PROFILE = /^web-release(?:-[a-z0-9]+)*$/;

export function classifyBuildProfile(profile) {
  if (typeof profile !== "string") return null;
  if (PRIVATE_PROFILE.test(profile)) return BUILD_AUTHORITY_PRIVATE;
  if (PUBLICATION_PROFILE.test(profile)) return BUILD_AUTHORITY_PUBLICATION;
  return null;
}
