import { PRODUCT_CONTENT } from "./content-definition.mjs";
import { HOST_PROTOCOL, PRODUCT_GAMES } from "./contracts/product-catalog.mjs";

// Build-time seed only. It intentionally lacks Runtime/content identities and
// therefore is not yet a Host Manifest. package-server.mjs materializes those
// identities before validating and publishing host-manifest.json.
export function createPublicationHostSeed(profile) {
  if (typeof profile !== "string" || !profile || profile === "web-development") {
    throw new Error("publication host seed requires a non-development profile");
  }
  return {
    protocol: HOST_PROTOCOL,
    profile,
    shared: {},
    games: Object.fromEntries(Object.entries(PRODUCT_GAMES).map(([game, product]) => {
      const content = PRODUCT_CONTENT[game];
      if (!content) throw new Error(`${game}: product content declaration is missing`);
      return [game, {
        number: product.number,
        title: product.title,
        subtitle: product.subtitle,
        gameData: {
          path: product.package.dataTarget.slice(1),
          ...(content.dataLayout ? { layout: content.dataLayout } : {}),
        },
        features: { thprac: product.features.thprac },
        music: {
          midi: { files: [] },
          ...Object.fromEntries(Object.entries(content.music || {}).map(([mode, declaration]) => [mode, {
            mount: declaration.mount,
            files: [...declaration.files],
          }])),
        },
      }];
    })),
  };
}
