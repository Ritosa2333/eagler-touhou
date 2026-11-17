import { copyFile, mkdir } from "node:fs/promises";

await mkdir(new URL("../public/vendor/", import.meta.url), { recursive: true });
for (const name of ['react-easy-crop','normalize-wheel']) {
  await copyFile(new URL(`../node_modules/${name}/LICENSE`,import.meta.url),new URL(`../public/vendor/${name}.LICENSE`,import.meta.url));
}
await copyFile(
  new URL("../node_modules/webaudio-tinysynth/webaudio-tinysynth.min.js", import.meta.url),
  new URL("../public/vendor/webaudio-tinysynth.min.js", import.meta.url),
);
await copyFile(
  new URL("../node_modules/webaudio-tinysynth/LICENSE", import.meta.url),
  new URL("../public/vendor/webaudio-tinysynth.LICENSE", import.meta.url),
);
await copyFile(
  new URL("../node_modules/fflate/umd/index.js", import.meta.url),
  new URL("../public/vendor/fflate.min.js", import.meta.url),
);
await copyFile(
  new URL("../node_modules/fflate/LICENSE", import.meta.url),
  new URL("../public/vendor/fflate.LICENSE", import.meta.url),
);
await copyFile(
  new URL("../node_modules/country-flag-icons/LICENSE", import.meta.url),
  new URL("../public/vendor/country-flag-icons.LICENSE", import.meta.url),
);
await copyFile(
  new URL("../node_modules/browser-image-compression/dist/browser-image-compression.js", import.meta.url),
  new URL("../public/vendor/browser-image-compression.min.js", import.meta.url),
);
await copyFile(
  new URL("../node_modules/browser-image-compression/LICENSE", import.meta.url),
  new URL("../public/vendor/browser-image-compression.LICENSE", import.meta.url),
);
await copyFile(
  new URL("../node_modules/uzip/LICENSE", import.meta.url),
  new URL("../public/vendor/uzip.LICENSE", import.meta.url),
);
