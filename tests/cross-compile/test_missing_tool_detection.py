"""
Property-based test for missing tool detection in the build script.

Feature: windows-x86-64-cross-compile, Property 2: Missing tool detection

For each required tool in {zig, cmake, moc}, if that tool is not available,
the build script exits with non-zero status and prints an error message
containing the missing tool name.

This is a small finite set, so exhaustive example testing covers it.

Validates: Requirements 3.1, 3.2, 3.6, 3.7
"""

import os
import shutil
import stat
import subprocess
import tempfile

import pytest


BUILD_SCRIPT = os.path.join(
    os.path.dirname(__file__), "..", "..", "build-windows-x64.sh"
)


def _create_mock_tool(directory: str, name: str, script: str) -> str:
    """Create a mock executable script in the given directory."""
    path = os.path.join(directory, name)
    with open(path, "w", encoding="utf-8") as f:
        f.write(script)
    os.chmod(
        path,
        stat.S_IRWXU | stat.S_IRGRP | stat.S_IXGRP
        | stat.S_IROTH | stat.S_IXOTH,
    )
    return path


def _minimal_path_without(excluded: set[str]) -> str:
    """
    Build a PATH string that contains common system directories
    but excludes any directory whose binaries include the excluded tools.
    We do this by creating a temp bin dir with symlinks to only the
    tools we want to keep.
    """
    # We'll handle this differently per test — see individual tests.
    raise NotImplementedError


def _run_build_script(
    script_path: str,
    env: dict,
    timeout: int = 10,
) -> subprocess.CompletedProcess:
    """Run the build script and return the result."""
    return subprocess.run(
        ["bash", script_path],
        capture_output=True,
        text=True,
        env=env,
        timeout=timeout,
        check=False,
    )


class TestMissingToolDetection:
    """
    Feature: windows-x86-64-cross-compile, Property 2: Missing tool detection

    For each tool in {zig, cmake, moc}, running the build script in an
    environment where that tool is absent produces a non-zero exit and
    an error message containing the missing tool name.

    **Validates: Requirements 3.1, 3.2, 3.6, 3.7**
    """

    @pytest.fixture()
    def work_dir(self, tmp_path):
        """
        Copy the build script into a temp directory so SCRIPT_DIR
        resolves relative to it.
        """
        dest = tmp_path / "build-windows-x64.sh"
        shutil.copy2(BUILD_SCRIPT, dest)
        os.chmod(str(dest), stat.S_IRWXU)
        return tmp_path

    @pytest.fixture()
    def mock_bin(self, tmp_path):
        """Create a directory for mock tool scripts."""
        d = tmp_path / "mock_bin"
        d.mkdir()
        return d

    # ── zig missing ──────────────────────────────────────────────

    def test_missing_zig(self, work_dir, mock_bin):
        """
        When zig is absent from PATH, the build script exits non-zero
        and the output contains 'zig'.

        **Validates: Requirements 3.1, 3.2**
        """
        # Provide cmake but NOT zig
        _create_mock_tool(
            str(mock_bin), "cmake",
            "#!/bin/sh\necho 'cmake version 3.28.0'\n",
        )

        env = {
            "PATH": f"{mock_bin}:/usr/bin:/bin",
            "HOME": os.environ.get("HOME", "/tmp"),
        }
        # Remove zig from PATH by ensuring mock_bin comes first
        # and doesn't contain zig. /usr/bin may have cmake but
        # we need to make sure zig is truly absent.
        # Build a restricted PATH with only our mock_bin + coreutils.
        restricted = tempfile.mkdtemp(dir=str(work_dir))
        # Symlink essential coreutils
        for tool in ("bash", "sh", "env", "cat", "mkdir",
                     "echo", "dirname", "pwd", "nproc", "rm",
                     "rmdir", "mv", "ls", "sed", "grep"):
            src = shutil.which(tool)
            if src:
                os.symlink(src, os.path.join(restricted, tool))
        # Symlink cmake but NOT zig
        cmake_real = shutil.which("cmake")
        if cmake_real:
            os.symlink(cmake_real, os.path.join(restricted, "cmake"))

        env["PATH"] = f"{restricted}"

        script = str(work_dir / "build-windows-x64.sh")
        result = _run_build_script(script, env)

        assert result.returncode != 0, (
            f"Expected non-zero exit when zig is missing, "
            f"got {result.returncode}"
        )
        combined = result.stdout + result.stderr
        assert "zig" in combined.lower(), (
            f"Expected 'zig' in error output, got:\n{combined}"
        )

    # ── cmake missing ────────────────────────────────────────────

    def test_missing_cmake(self, work_dir, mock_bin):
        """
        When cmake is absent from PATH, the build script exits non-zero
        and the output contains 'cmake'.

        **Validates: Requirements 3.1, 3.2**
        """
        restricted = tempfile.mkdtemp(dir=str(work_dir))
        for tool in ("bash", "sh", "env", "cat", "mkdir",
                     "echo", "dirname", "pwd", "nproc", "rm",
                     "rmdir", "mv", "ls", "sed", "grep"):
            src = shutil.which(tool)
            if src:
                os.symlink(src, os.path.join(restricted, tool))
        # Symlink zig but NOT cmake
        zig_real = shutil.which("zig")
        if zig_real:
            os.symlink(zig_real, os.path.join(restricted, "zig"))

        env = {
            "PATH": restricted,
            "HOME": os.environ.get("HOME", "/tmp"),
        }

        script = str(work_dir / "build-windows-x64.sh")
        result = _run_build_script(script, env)

        assert result.returncode != 0, (
            f"Expected non-zero exit when cmake is missing, "
            f"got {result.returncode}"
        )
        combined = result.stdout + result.stderr
        assert "cmake" in combined.lower(), (
            f"Expected 'cmake' in error output, got:\n{combined}"
        )

    # ── moc missing ──────────────────────────────────────────────

    def test_missing_moc(self, work_dir, mock_bin):
        """
        When moc is not found in the Linux Qt installation, the build
        script exits non-zero and the output contains 'moc'.

        **Validates: Requirements 3.6, 3.7**
        """
        # Create fake Qt directory structures so the script skips
        # downloading Qt but still fails at the moc check.
        qt_base = work_dir / "6.10.2"

        # Windows Qt: needs lib/cmake/Qt6 to exist
        win_qt = qt_base / "llvm-mingw_64"
        (win_qt / "lib" / "cmake" / "Qt6").mkdir(parents=True)

        # Linux Qt: directory exists (so download is skipped)
        # but moc is NOT present at libexec/moc or bin/moc
        linux_qt = qt_base / "gcc_64"
        linux_qt.mkdir(parents=True)

        # Build a restricted PATH with zig and cmake available
        restricted = tempfile.mkdtemp(dir=str(work_dir))
        for tool in ("bash", "sh", "env", "cat", "mkdir",
                     "echo", "dirname", "pwd", "nproc", "rm",
                     "rmdir", "mv", "ls", "sed", "grep",
                     "command"):
            src = shutil.which(tool)
            if src:
                os.symlink(src, os.path.join(restricted, tool))

        # Mock zig so `zig version` works
        _create_mock_tool(
            restricted, "zig",
            "#!/bin/sh\necho '0.15.0'\n",
        )
        # Real cmake
        cmake_real = shutil.which("cmake")
        if cmake_real:
            os.symlink(
                cmake_real, os.path.join(restricted, "cmake")
            )

        env = {
            "PATH": restricted,
            "HOME": os.environ.get("HOME", "/tmp"),
        }

        script = str(work_dir / "build-windows-x64.sh")
        result = _run_build_script(script, env)

        assert result.returncode != 0, (
            f"Expected non-zero exit when moc is missing, "
            f"got {result.returncode}"
        )
        combined = result.stdout + result.stderr
        assert "moc" in combined.lower(), (
            f"Expected 'moc' in error output, got:\n{combined}"
        )
