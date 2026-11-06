import hashlib
import json
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

EXPORTER = Path(__file__).with_name('export_sessions.py')


class ConversationExportTests(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory(prefix='aim-export-test-')
        self.addCleanup(self.directory.cleanup)
        self.root = Path(self.directory.name).resolve()
        self.repository = self.root / 'submission'
        self.script = self.repository / 'local_tests' / 'export_sessions.py'
        self.script.parent.mkdir(parents=True)
        shutil.copyfile(EXPORTER, self.script)
        self.source = self.root / 'sessions'
        self.original = self.root / 'original-coursework'

    def session(self, number, cwd, kind='vscode'):
        session_id = f'00000000-0000-0000-0000-{number:012d}'
        records = [
            {'type': 'session_meta', 'payload': {
                'id': session_id, 'cwd': str(cwd), 'source': kind}},
            {'type': 'response_item', 'payload': {
                'type': 'message', 'role': 'user',
                'content': [{'text': 'Check this coursework boundary.'}]}},
            {'type': 'response_item', 'payload': {
                'type': 'message', 'role': 'assistant',
                'content': [{'text': 'The expected result is zero.'}]}},
            {'type': 'response_item', 'payload': {
                'type': 'function_call_output',
                'output': 'UNRELATED_BROWSER_OUTPUT'}},
        ]
        lines = [(json.dumps(row) + '\n').encode() for row in records]
        path = self.source / '2026/11/06' / f'rollout-{session_id}.jsonl'
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(b''.join(lines))
        return path, lines

    def run_export(self, include_original=False):
        command = [sys.executable, str(self.script), '--source',
                   str(self.source)]
        if include_original:
            command.extend(['--related-workdir', str(self.original)])
        subprocess.run(command, check=True, capture_output=True)
        return sorted(
            (self.repository / '.agent-sessions/source/exports').glob('*.jsonl')
        )

    def test_related_export_keeps_original_messages_and_provenance(self):
        path, lines = self.session(1, self.original)
        files = self.run_export(include_original=True)
        self.assertEqual(len(files), 1)
        exported = files[0].read_bytes().splitlines(keepends=True)
        header = json.loads(exported[0])
        self.assertEqual(header['cwd'], str(self.repository))
        self.assertEqual(header['source_cwd'], str(self.original))
        self.assertEqual(header['source_size'], len(path.read_bytes()))
        self.assertEqual(header['source_sha256'],
                         hashlib.sha256(path.read_bytes()).hexdigest())
        self.assertEqual(exported[1:], lines[1:3])
        self.assertNotIn(b'UNRELATED_BROWSER_OUTPUT', files[0].read_bytes())
        self.assertEqual(path.read_bytes(), b''.join(lines))

    def test_default_export_only_includes_current_repository(self):
        self.session(1, self.repository)
        self.session(2, self.original)
        files = self.run_export()
        self.assertEqual(len(files), 1)
        self.assertEqual(files[0].stem,
                         '00000000-0000-0000-0000-000000000001')

    def test_background_and_other_project_sessions_are_excluded(self):
        self.session(1, self.repository)
        self.session(2, self.root / 'unrelated-project')
        self.session(3, self.repository, {'subagent': {'other': 'test'}})
        self.assertEqual(len(self.run_export()), 1)


if __name__ == '__main__':
    unittest.main()
