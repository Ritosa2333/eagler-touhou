import { Link, useHref, useLocation, useNavigate, type LinkProps } from 'react-router';
import { nextPageHistory, pageOf, readPageHistory, PAGE_HISTORY_KEY } from '../../navigation/page-history';
import { routerDestination } from '../../navigation/addresses';
import { isStandalonePage } from '../documents/catalog';

/** Known launcher document links share the Router's fixed page parent records. */
export function AppLink({ href, state, replace, rememberEntry, returnToEntry, onClick, ...props }: Omit<LinkProps, 'to'> & {
  href: string;
  rememberEntry?: boolean;
  returnToEntry?: boolean;
}) {
  const navigate = useNavigate();
  const location = useLocation(),
    routerBase = useHref('/'),
    currentAddress = useHref(location.pathname + location.search + location.hash);
  const currentUrl = new URL(currentAddress, 'https://launcher.invalid'),
    url = new URL(href, currentUrl);
  const destination = routerDestination(url.href, new URL(routerBase, url.origin).href);
  const destinationPage = pageOf({ pathname: url.pathname, search: url.search, hash: url.hash, state });
  const record = nextPageHistory(
    readPageHistory(location.state),
    destinationPage,
    replace === true,
  );
  const reloadDocument = props.reloadDocument || isStandalonePage(location.pathname) !== isStandalonePage(url.pathname);
  const entryHref = returnToEntry && !reloadDocument ? location.state?.launcherEntryHref : undefined;
  return (
    <Link
      {...props}
      reloadDocument={reloadDocument}
      to={typeof entryHref === 'string' ? entryHref : destination}
      replace={replace}
      onClick={event => {
        onClick?.(event);
        if (typeof entryHref !== 'string' || event.defaultPrevented || event.button !== 0 ||
            event.metaKey || event.ctrlKey || event.shiftKey || event.altKey ||
            (props.target && props.target !== '_self')) return;
        event.preventDefault();
        void navigate(-1);
      }}
      state={{ ...state, ...(rememberEntry && {launcherEntryHref: location.pathname + location.search + location.hash}), [PAGE_HISTORY_KEY]: record }}
    />
  );
}
