import { Link, useHref, useLocation, type LinkProps } from 'react-router';
import { nextPageHistory, pageOf, readPageHistory, PAGE_HISTORY_KEY } from '../../navigation/page-history';
import { routerDestination } from '../../navigation/addresses';
import { isStandalonePage } from '../documents/catalog';
import { useCompactNavigation } from '../../navigation/navigation-motion';

/** Known launcher document links share the Router's fixed page parent records. */
export function AppLink({ href, state, replace, ...props }: Omit<LinkProps, 'to'> & { href: string }) {
  const compact = useCompactNavigation();
  const location = useLocation(),
    routerBase = useHref('/'),
    currentAddress = useHref(location.pathname + location.search + location.hash);
  const currentUrl = new URL(currentAddress, 'https://launcher.invalid'),
    url = new URL(href, currentUrl);
  const destination = routerDestination(url.href, new URL(routerBase, url.origin).href);
  const record = nextPageHistory(
    readPageHistory(location.state),
    pageOf({ pathname: url.pathname, search: url.search, hash: url.hash, state }),
    replace === true,
  );
  const reloadDocument = props.reloadDocument || isStandalonePage(location.pathname) !== isStandalonePage(url.pathname);
  return (
    <Link
      {...props}
      reloadDocument={reloadDocument}
      viewTransition={!compact && !reloadDocument && url.pathname !== currentUrl.pathname && props.viewTransition !== false}
      to={destination}
      replace={replace}
      state={{ ...state, [PAGE_HISTORY_KEY]: record }}
    />
  );
}
