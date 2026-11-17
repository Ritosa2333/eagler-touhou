import unittest
from turn_selection import choose


def path(source, target, status='healthy', rounds=3):
    return dict(source=source, target=target, status=status, goodRounds=rounds if status == 'healthy' else 0,
                ageSeconds=30, complete=True, lossPercent=0 if status == 'healthy' else 10, sendDropped=0)


class SelectionTests(unittest.TestCase):
    def test_one_bad_source_does_not_remove_best(self):
        selected, _ = choose(['best', '1145', 'main'], 'best', [path('main', 'best', 'degraded'), path('1145', 'best')])
        self.assertEqual(selected, 'best')

    def test_confirmed_failure_selects_backup_and_recovers(self):
        paths = [path('main', 'best', 'degraded'), path('1145', 'best', 'degraded'), path('best', '1145')]
        self.assertEqual(choose(['best', '1145', 'main'], 'best', paths)[0], '1145')
        paths = [path('main', 'best'), path('1145', 'best'), path('best', '1145')]
        self.assertEqual(choose(['best', '1145', 'main'], '1145', paths)[0], 'best')

    def test_correlated_failures_switch_even_during_cooldown(self):
        paths = [path('main', 'best', 'observing'), path('1145', 'best', 'degraded'),
                 path('main', '1145')]
        selected, states = choose(['best', '1145', 'main'], 'best', paths, cooldown=True)
        self.assertEqual(selected, '1145')
        self.assertEqual(states['best'], 'degraded')

    def test_recovery_waits_for_contradictory_probe(self):
        paths = [path('main', 'best'), path('1145', 'best', 'degraded'), path('main', '1145')]
        selected, states = choose(['best', '1145', 'main'], '1145', paths)
        self.assertEqual(selected, '1145')
        self.assertEqual(states['best'], 'unknown')

    def test_stale_data_and_cooldown_hold_current(self):
        self.assertEqual(choose(['best', '1145', 'main'], '1145', [path('main', 'best')], cooldown=True)[0], '1145')
        value = path('main', 'best'); value['ageSeconds'] = 1000
        self.assertEqual(choose(['best', '1145', 'main'], '1145', [value])[0], '1145')

    def test_main_is_last_healthy_fallback_even_during_cooldown(self):
        paths = [path('main', 'best', 'degraded'), path('1145', 'best', 'degraded'),
                 path('main', '1145', 'degraded'), path('best', '1145', 'degraded'), path('best', 'main')]
        self.assertEqual(choose(['best', '1145', 'main'], '1145', paths, cooldown=True)[0], 'main')
