/** Same structured-error handling as original main app.mts385. */
import { localizeKnownError } from '../../src/launcher/localized-error-text.mts';
export function errorText(error: unknown): string {
  const message =
    error instanceof Error
      ? error.message
      : error &&
          typeof error === 'object' &&
          !Array.isArray(error) &&
          'message' in error &&
          typeof error.message === 'string'
        ? error.message
        : error == null
          ? ''
          : String(error);
  return localizeKnownError(message);
}
