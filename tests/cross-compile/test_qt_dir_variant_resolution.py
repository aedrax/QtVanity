"""
Property-based test for Qt directory name variant resolution.

Feature: windows-x86-64-cross-compile, Property 3: Qt directory name variant resolution

For each known aqtinstall directory name variant for the win64_llvm_mingw archive
(e.g., ``llvm-mingw_64``, ``llvm_mingw``), the build script resolves to a valid
Windows Qt path that contains ``lib/cmake/Qt6``.

This is a small finite set, so exhaustive example testing covers it.

Validates: Requirements 4.1, 4.2, 4.3
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

# Known directory name variants produced by aqtinstall for win64_llvm_mingw
KNOWN_VARIANTS = ["llvm-mingw_64", "llvm_mingw"]


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


class TestQtDirVariantResolution:
    """
    Feature: windows-x86-64-cross-compile, Property 3: Qt directory name variant resolution

    For each known aqtinstall directory name variant for the win64_llvm_mingw
    archive, the build script resolves to a valid Windows Qt path containing
    ``lib/cmake/Qt6``.

    **Validates: Requirements 4.1, 4.2, 4.3**
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
    def restricted_env(self, work_dir):
        """
        Build a restricted PATH with mock zig, cmake, and essential
        coreutils so the build script can run past preflight checks.
        """
        restricted = str(work_dir / "mock_bin")
        os.makedirs(restricted, exist_ok=True)

        # Symlink essential coreutils
        for tool in (
            "bash", "sh", "env", "cat", "mkdir", "echo",
            "dirname", "pwd", "nproc", "rm", "rmdir", "mv",
            "ls", "sed", "grep", "command",
        ):
            src = shutil.which(tool)
            if src:
                os.symlink(src, os.path.join(restricted, tool))

        # Mock zig — just prints a version string
        _create_mock_tool(
            restricted, "zig",
            "#!/bin/sh\necho '0.15.0'\n",
        )

        # Mock cmake — exits successfully for configure and build
        _create_mock_tool(
            restricted, "cmake",
            "#!/bin/sh\nexit 0\n",
        )

        # Mock aqt — no-op so the download section passes without error
        _create_mock_tool(
            restricted, "aqt",
            "#!/bin/sh\nexit 0\n",
        )

        env = {
            "PATH": restricted,
            "HOME": os.environ.get("HOME", "/tmp"),
        }
        return env

    def _setup_qt_dirs(self, work_dir, win_variant: str):
        """
        Create the minimal Qt directory structure needed for the build
        script to pass variant resolution and moc checks.
        """
        qt_base = work_dir / "6.10.2"

        # Windows Qt: create lib/cmake/Qt6 under the given variant
        win_qt = qt_base / win_variant
        (win_qt / "lib" / "cmake" / "Qt6").mkdir(parents=True)

        # Linux Qt: create gcc_64 with a mock moc at libexec/moc
        linux_qt = qt_base / "gcc_64"
        (linux_qt / "libexec").mkdir(parents=True)
        _create_mock_tool(
            str(linux_qt / "libexec"), "moc",
            "#!/bin/sh\necho 'moc 6.10.2'\n",
        )

    # ── Variant: llvm-mingw_64 (actual aqtinstall output) ───────────

    def test_resolves_llvm_mingw_64_variant(self, work_dir, restricted_env):
        """
        When the Windows Qt directory is named ``llvm-mingw_64`` (the actual
        aqtinstall output for win64_llvm_mingw), the build script resolves
        to that path and prints it.

        **Validates: Requirements 4.1, 4.2, 4.3**
        """
        self._setup_qt_dirs(work_dir, "llvm-mingw_64")

        script = str(work_dir / "build-windows-x64.sh")
        result = _run_build_script(script, restricted_env)

        combined = result.stdout + result.stderr
        expected_suffix = os.path.join("6.10.2", "llvm-mingw_64")
        assert "Windows Qt (resolved):" in combined, (
            f"Expected 'Windows Qt (resolved):' in output, got:\n{combined}"
        )
        # The resolved path should end with the variant directory
        for line in combined.splitlines():
            if "Windows Qt (resolved):" in line:
                assert expected_suffix in line, (
                    f"Expected resolved path to contain '{expected_suffix}', "
                    f"got line: {line}"
                )
                break

    # ── Variant: llvm_mingw (legacy name) ────────────────────────────

    def test_resolves_llvm_mingw_variant(self, work_dir, restricted_env):
        """
        When the Windows Qt directory is named ``llvm_mingw`` (legacy name
        with underscore instead of hyphen), the build script resolves to
        that path and prints it.

        **Validates: Requirements 4.1, 4.2, 4.3**
        """
        self._setup_qt_dirs(work_dir, "llvm_mingw")

        script = str(work_dir / "build-windows-x64.sh")
        result = _run_build_script(script, restricted_env)

        combined = result.stdout + result.stderr
        expected_suffix = os.path.join("6.10.2", "llvm_mingw")
        assert "Windows Qt (resolved):" in combined, (
            f"Expected 'Windows Qt (resolved):' in output, got:\n{combined}"
        )
        for line in combined.splitlines():
            if "Windows Qt (resolved):" in line:
                assert expected_suffix in line, (
                    f"Expected resolved path to contain '{expected_suffix}', "
                    f"got line: {line}"
                )
                break

    # ── Both variants present: first match wins ──────────────────────

    def test_prefers_llvm_mingw_64_when_both_exist(self, work_dir, restricted_env):
        """
        When both ``llvm-mingw_64`` and ``llvm_mingw`` directories exist,
        the build script resolves to ``llvm-mingw_64`` (the default and
        first variant checked).

        **Validates: Requirements 4.1, 4.2**
        """
        self._setup_qt_dirs(work_dir, "llvm-mingw_64")
        # Also create the legacy variant
        qt_base = work_dir / "6.10.2"
        legacy = qt_base / "llvm_mingw"
        (legacy / "lib" / "cmake" / "Qt6").mkdir(parents=True)

        script = str(work_dir / "build-windows-x64.sh")
        result = _run_build_script(script, restricted_env)

        combined = result.stdout + result.stderr
        expected_suffix = os.path.join("6.10.2", "llvm-mingw_64")
        assert "Windows Qt (resolved):" in combined, (
            f"Expected 'Windows Qt (resolved):' in output, got:\n{combined}"
        )
        for line in combined.splitlines():
            if "Windows Qt (resolved):" in line:
                assert expected_suffix in line, (
                    f"Expected resolved path to prefer 'llvm-mingw_64', "
                    f"got line: {line}"
                )
                break

    # ── No variant present: error exit ───────────────────────────────

    def test_error_when_no_variant_exists(self, work_dir, restricted_env):
        """
        When no known Windows Qt directory variant exists, the build script
        exits non-zero and prints an error about missing Qt libraries.

        **Validates: Requirements 4.2, 4.3**
        """
        # Create gcc_64 with moc but NO Windows Qt variant
        qt_base = work_dir / "6.10.2"
        linux_qt = qt_base / "gcc_64"
        (linux_qt / "libexec").mkdir(parents=True)
        _create_mock_tool(
            str(linux_qt / "libexec"), "moc",
            "#!/bin/sh\necho 'moc 6.10.2'\n",
        )

        script = str(work_dir / "build-windows-x64.sh")
        result = _run_build_script(script, restricted_env)

        assert result.returncode != 0, (
            f"Expected non-zero exit when no Qt variant exists, "
            f"got {result.returncode}"
        )
        combined = result.stdout + result.stderr
        assert "cannot find windows qt" in combined.lower(), (
            f"Expected error about missing Windows Qt, got:\n{combined}"
        )
