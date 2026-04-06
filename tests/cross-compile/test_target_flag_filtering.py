"""
Property-based test for Zig wrapper target flag filtering.

Feature: windows-x86-64-cross-compile, Property 1: Target flag filtering preserves non-target arguments

Validates: Requirements 1.3
"""

import os
import stat
import subprocess
import tempfile
import shlex

import pytest
from hypothesis import given, settings, assume
from hypothesis import strategies as st


# --- Strategies ---

# Normal compiler flags that should pass through unchanged
NORMAL_FLAGS = st.sampled_from([
    "-O2", "-O3", "-Os", "-Wall", "-Wextra", "-Werror",
    "-std=c++17", "-std=c11", "-g", "-DNDEBUG", "-DFOO=bar",
    "-I/usr/include", "-L/usr/lib", "-lm", "-lpthread",
    "-fPIC", "-shared", "-c", "-o", "output.o", "input.c",
    "-Wno-unused-parameter", "-march=x86-64", "-pipe",
    "-DSOME_DEFINE=1", "-isystem", "/some/path",
])

# Values for --target= and -target flags
TARGET_VALUES = st.text(
    alphabet=st.sampled_from("abcdefghijklmnopqrstuvwxyz0123456789-_"),
    min_size=1,
    max_size=30,
)

# A --target=<value> flag
TARGET_EQUALS_FLAG = TARGET_VALUES.map(lambda v: f"--target={v}")

# A -target <value> pair (represented as a 2-element list)
TARGET_SPACE_FLAG = TARGET_VALUES.map(lambda v: ["-target", v])

# A single normal flag (as a 1-element list)
NORMAL_FLAG_ITEM = NORMAL_FLAGS.map(lambda f: [f])

# An argument "chunk": either a normal flag, --target=val, or -target val
ARG_CHUNK = st.one_of(
    NORMAL_FLAG_ITEM,
    TARGET_EQUALS_FLAG.map(lambda f: [f]),
    TARGET_SPACE_FLAG,
)

# A full argument list: a sequence of chunks flattened
ARG_LIST = st.lists(ARG_CHUNK, min_size=0, max_size=20).map(
    lambda chunks: [arg for chunk in chunks for arg in chunk]
)


def _create_mock_zig(tmpdir: str) -> str:
    """Create a mock 'zig' script that prints all arguments, one per line."""
    mock_path = os.path.join(tmpdir, "zig")
    with open(mock_path, "w") as f:
        f.write('#!/bin/sh\n')
        f.write('for arg in "$@"; do\n')
        f.write('  echo "$arg"\n')
        f.write('done\n')
    os.chmod(mock_path, stat.S_IRWXU | stat.S_IRGRP | stat.S_IXGRP | stat.S_IROTH | stat.S_IXOTH)
    return tmpdir


def _run_wrapper(wrapper_path: str, args: list[str], mock_zig_dir: str) -> list[str]:
    """Run a wrapper script with given args and return the output lines."""
    env = os.environ.copy()
    # Put mock zig first in PATH
    env["PATH"] = mock_zig_dir + ":" + env.get("PATH", "")

    result = subprocess.run(
        [wrapper_path] + args,
        capture_output=True,
        text=True,
        env=env,
        timeout=10,
    )
    # The mock zig prints each arg on its own line
    return [line for line in result.stdout.strip().split("\n") if line]


def _expected_non_target_args(args: list[str]) -> list[str]:
    """Compute the expected arguments after filtering target flags."""
    result = []
    skip_next = False
    for arg in args:
        if skip_next:
            skip_next = False
            continue
        if arg.startswith("--target="):
            continue
        if arg == "-target":
            skip_next = True
            continue
        result.append(arg)
    return result



WRAPPER_CC = os.path.join(os.path.dirname(__file__), "..", "..", "cmake", "zig-x86_64-windows-cc")
WRAPPER_CXX = os.path.join(os.path.dirname(__file__), "..", "..", "cmake", "zig-x86_64-windows-cxx")


@pytest.fixture(scope="module")
def mock_zig_dir():
    """Create a temporary directory with a mock zig binary."""
    with tempfile.TemporaryDirectory() as tmpdir:
        _create_mock_zig(tmpdir)
        yield tmpdir


class TestTargetFlagFilteringCC:
    """
    Feature: windows-x86-64-cross-compile, Property 1: Target flag filtering preserves non-target arguments

    Validates: Requirements 1.3
    """

    @given(args=ARG_LIST)
    @settings(max_examples=100)
    def test_target_flags_removed_and_non_target_preserved(self, args, mock_zig_dir):
        """
        Property: For any argument list, after passing through the CC wrapper,
        all --target= and -target <value> flags are removed, and all other
        arguments appear in their original order.

        **Validates: Requirements 1.3**
        """
        output_args = _run_wrapper(WRAPPER_CC, args, mock_zig_dir)

        # The wrapper prepends: cc -target x86_64-windows-gnu
        # So output should be: ["cc", "-target", "x86_64-windows-gnu", ...remaining...]
        assert len(output_args) >= 3, f"Expected at least 3 args (cc -target x86_64-windows-gnu), got: {output_args}"
        assert output_args[0] == "cc"
        assert output_args[1] == "-target"
        assert output_args[2] == "x86_64-windows-gnu"

        forwarded_args = output_args[3:]
        expected = _expected_non_target_args(args)

        # All non-target args must be present in original order
        assert forwarded_args == expected, (
            f"Mismatch!\n"
            f"  Input args:    {args}\n"
            f"  Expected:      {expected}\n"
            f"  Got forwarded: {forwarded_args}"
        )

        # No target flags should appear in forwarded args
        for arg in forwarded_args:
            assert not arg.startswith("--target="), f"Found --target= flag in output: {arg}"
            # Note: bare "-target" could be a legitimate flag value in theory,
            # but the wrapper filters it, so it should not appear as a standalone flag
            # followed by a value that was meant as a target value


class TestTargetFlagFilteringCXX:
    """
    Feature: windows-x86-64-cross-compile, Property 1: Target flag filtering preserves non-target arguments

    Validates: Requirements 1.3
    """

    @given(args=ARG_LIST)
    @settings(max_examples=100)
    def test_target_flags_removed_and_non_target_preserved(self, args, mock_zig_dir):
        """
        Property: For any argument list, after passing through the CXX wrapper,
        all --target= and -target <value> flags are removed, and all other
        arguments appear in their original order.

        **Validates: Requirements 1.3**
        """
        output_args = _run_wrapper(WRAPPER_CXX, args, mock_zig_dir)

        # The CXX wrapper prepends: c++ -target x86_64-windows-gnu
        assert len(output_args) >= 3, f"Expected at least 3 args (c++ -target x86_64-windows-gnu), got: {output_args}"
        assert output_args[0] == "c++"
        assert output_args[1] == "-target"
        assert output_args[2] == "x86_64-windows-gnu"

        forwarded_args = output_args[3:]
        expected = _expected_non_target_args(args)

        assert forwarded_args == expected, (
            f"Mismatch!\n"
            f"  Input args:    {args}\n"
            f"  Expected:      {expected}\n"
            f"  Got forwarded: {forwarded_args}"
        )

        for arg in forwarded_args:
            assert not arg.startswith("--target="), f"Found --target= flag in output: {arg}"
