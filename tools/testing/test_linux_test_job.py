"""Exercise Linux test-job resource boundaries against real systemd services."""
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import time
import unittest

RUNNER = Path(__file__).with_name("linux-test-job.py")

@unittest.skipUnless(os.environ.get("TOUHOU_TEST_JOB_INTEGRATION") == "1", "explicit Linux integration entry")
class IsolatedJobs(unittest.TestCase):
    def setUp(self):
        self.root = Path(tempfile.mkdtemp(prefix="touhou-job-integration-"))
        self.root.chmod(0o755)
        self.owner = os.environ.get("TOUHOU_TEST_OWNER", "i")

    def launch(self, code, timeout=10, memory=128):
        self.job = self.root / "job"
        cmd = [sys.executable, str(RUNNER), "--owner", self.owner, "--job-dir", str(self.job), "--cwd", str(self.root), "--timeout", str(timeout), "--stop-timeout", "3", "--memory-mib", str(memory), "--", sys.executable, "-c", code]
        result = subprocess.run(cmd, capture_output=True, text=True)
        self.assertEqual(result.returncode, 0, result.stderr)
        deadline = time.monotonic() + timeout + 12
        while not (self.job / "result.json").exists() and time.monotonic() < deadline:
            time.sleep(.1)
        self.assertTrue((self.job / "result.json").exists(), result.stdout + result.stderr)
        return json.loads((self.job / "result.json").read_text())

    def assert_no_live_children(self):
        p = self.job / "children.json"
        if not p.exists():
            self.fail("payload did not publish its child identities")
        pids = json.loads(p.read_text())
        for _ in range(30):
            live = [pid for pid in pids if Path(f"/proc/{pid}/stat").exists() and Path(f"/proc/{pid}/stat").read_text().split(") ", 1)[1].split()[0] != "Z"]
            if not live:
                return
            time.sleep(.1)
        self.fail(f"Live children after service completion: {live}")

    def test_success_observes_real_cgroup_limits_and_nonroot_owner(self):
        d = self.launch("import os,pathlib,json; p=pathlib.Path('/sys/fs/cgroup'+pathlib.Path('/proc/self/cgroup').read_text().strip().split(':',2)[2]); print(json.dumps({'uid':os.getuid(),'cgroup':str(p),'max':(p/'memory.max').read_text().strip(),'swap':(p/'memory.swap.max').read_text().strip()}))")
        self.assertEqual(d["exitStatus"], 0)
        self.assertEqual(d["serviceResult"], "success")
        actual = json.loads((self.job / "output.log").read_text().strip())
        self.assertNotEqual(actual["uid"], 0)
        self.assertNotIn("user.slice", actual["cgroup"])
        self.assertEqual(actual["max"], str(128 * 1024 * 1024))
        self.assertEqual(actual["swap"], "0")

    def test_preserves_nonzero_exit(self):
        d = self.launch("raise SystemExit(23)")
        self.assertEqual(d["exitStatus"], 23)
        self.assertEqual(d["serviceResult"], "exit-code")

    def test_timeout_kills_setsid_child_even_if_term_is_ignored(self):
        code = "import os,signal,time,json; signal.signal(signal.SIGTERM,signal.SIG_IGN); pid=os.fork(); os.setsid() if pid==0 else None; open('JOB/children.json','w').write(json.dumps([os.getpid(),pid])) if pid else None; time.sleep(100)".replace("JOB", str(self.root / "job"))
        d = self.launch(code, timeout=2)
        self.assertEqual(d["exitStatus"], 124)
        self.assertEqual(d["serviceResult"], "timeout")
        self.assert_no_live_children()

    def test_timeout_is_not_success_when_term_handler_exits_zero(self):
        d = self.launch("import signal,sys,time; signal.signal(signal.SIGTERM,lambda *_:sys.exit(0)); time.sleep(100)", timeout=2)
        self.assertEqual(d["serviceResult"], "timeout")
        self.assertEqual(d["exitStatus"], 124)

    def test_oom_kills_whole_job_without_using_swap(self):
        code = "import os,time,json; pid=os.fork(); open('JOB/children.json','w').write(json.dumps([os.getpid(),pid])) if pid else None; chunks=[]; exec('while True: chunks.append(bytearray([1])*(8*1024*1024))') if pid==0 else time.sleep(100)".replace("JOB", str(self.root / "job"))
        d = self.launch(code, timeout=15)
        self.assertEqual(d["exitStatus"], 137)
        self.assertEqual(d["serviceResult"], "oom-kill")
        self.assertGreater(d["memoryEvents"].get("oom_kill", 0), 0)
        self.assert_no_live_children()

    def test_rejects_invalid_budget_without_launching(self):
        r = subprocess.run([sys.executable, str(RUNNER), "--owner", self.owner, "--job-dir", str(self.root / "invalid"), "--cwd", str(self.root), "--timeout", "0", "--", "/bin/true"], capture_output=True, text=True)
        self.assertEqual(r.returncode, 2)
        self.assertIn("timeout must be", r.stderr)
        self.assertFalse((self.root / "invalid").exists())

    def test_existing_job_is_not_replayed(self):
        self.launch("print('executed once')")
        original = (self.job / "result.json").read_bytes()
        r = subprocess.run([sys.executable, str(RUNNER), "--owner", self.owner, "--job-dir", str(self.job), "--cwd", str(self.root), "--", "/bin/false"], capture_output=True, text=True)
        self.assertNotEqual(r.returncode, 0)
        self.assertEqual((self.job / "result.json").read_bytes(), original)

if __name__ == "__main__":
    unittest.main(verbosity=2)
