import argparse
import hashlib
import json
from pathlib import Path
from uuid import UUID


def message_record(record):
    payload = record.get('payload', {})
    if record.get('type') != 'response_item':
        return False
    if payload.get('type') != 'message':
        return False
    if payload.get('role') not in {'user', 'assistant'}:
        return False
    content = '\n'.join(
        item.get('text', '') for item in payload.get('content', [])
        if isinstance(item, dict)
    )
    return bool(content.strip()) and not any(
        marker in content for marker in (
            '<environment_context>', '<external_codex_apps_open_page>'
        )
    )


def main():
    parser = argparse.ArgumentParser(
        description='Export original coursework conversation messages.'
    )
    parser.add_argument(
        '--source', type=Path, default=Path.home() / '.codex' / 'sessions'
    )
    parser.add_argument('--related-workdir', type=Path)
    args = parser.parse_args()
    repository = Path(__file__).resolve().parents[1]
    workdirs = {repository}
    if args.related_workdir is not None:
        workdirs.add(args.related_workdir.expanduser().resolve())
    source = args.source.expanduser().resolve()
    if not source.is_dir():
        parser.error('The Codex session directory does not exist.')
    destination = repository / '.agent-sessions' / 'source' / 'exports'
    copied = 0
    for path in sorted(source.glob('[0-9][0-9][0-9][0-9]/*/*/rollout-*.jsonl')):
        if path.is_symlink() or not path.is_file():
            continue
        if any(parent.is_symlink() for parent in path.parents
               if parent != source and source in parent.parents):
            continue
        with path.open('rb') as stream:
            try:
                header = json.loads(stream.readline())
            except (ValueError, UnicodeDecodeError):
                continue
        metadata = header.get('payload', {})
        if header.get('type') != 'session_meta':
            continue
        kind = metadata.get('source')
        if not isinstance(kind, str) or kind not in {'cli', 'vscode'}:
            continue
        cwd = metadata.get('cwd')
        if not isinstance(cwd, str):
            continue
        session_workdir = Path(cwd).resolve()
        if not any(session_workdir == root or root in session_workdir.parents
                   for root in workdirs):
            continue
        try:
            filename = str(UUID(metadata['id'])) + '.jsonl'
        except (KeyError, ValueError, TypeError, AttributeError):
            continue
        raw = path.read_bytes()
        if not raw.endswith(b'\n'):
            raise RuntimeError('Session is being written; run export again.')
        messages = [line for line in raw.splitlines(keepends=True)
                    if message_record(json.loads(line))]
        if not messages:
            continue
        export_header = {
            'cwd': str(repository),
            'type': 'coursework_conversation_export',
            'source_agent': 'codex',
            'source_session_id': metadata['id'],
            'source_cwd': cwd,
            'source_size': len(raw),
            'source_sha256': hashlib.sha256(raw).hexdigest(),
            'selection': 'original user and assistant message records',
            'omitted': 'tools, system context, environment, internal reasoning',
        }
        destination.mkdir(parents=True, exist_ok=True)
        target = destination / filename
        prefix = json.dumps(export_header, ensure_ascii=False).encode() + b'\n'
        target.write_bytes(prefix + b''.join(messages))
        copied += 1
    print(f'Exported {copied} conversation files to {destination}')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
