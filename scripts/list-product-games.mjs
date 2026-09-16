import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";

process.stdout.write(`${JSON.stringify(Object.keys(PRODUCT_GAMES))}\n`);
