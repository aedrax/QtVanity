"""
Property-based test for Linux ARM64 Zig wrapper argument filtering.

Feature: linux-arm64-cross-compile, Property 1: Argument filtering
preserves non-target, non-host-include arguments

Validates: Requirements 1.3, 1.4
"""

import os
import stat
import subprocess
import tempfile

import pytest
from hypothesis import given, settings
from hypothesis import strategies as st


# --- Strategies ---

# Normal compiler flags that should pass through unchanged.
# Excludes bare -isystem/-I (which trigger the wrapper's state
# machine) and any -I/-isystem with host system paths.
NORMAL_FLAGS = st.sampled_from([
    "-O2", "-O3", "-Os", "-Wall", "-Wextra", "-Werror",
    "-std=c++17", "-std=c11", "-g", "-DNDEBUG", "-DFOO=bar",
    "-L/usr/lib", "-lm", "-lpthread",
    "-fPIC", "-shared", "-c", "-o", "output.o", "input.c",
    "-Wno-unused-parameter", "-march=armv8-a", "-pipe",
    "-DSOME_DEFINE=1",
])

# Safe (non-host) include paths for space-separated form
SAFE_INCLUDE_PATHS = st.sampled_from([
    "/opt/custom/include",
    "/home/user/project/include",
    "/some/other/path",
    "/opt/sysroot/aarch64/include",
])

# Safe include pair: [-isystem, /safe/path] or [-I, /safe/path]
SAFE_INCLUDE_FLAG = st.sampled_from(["-isystem", "-I"])
SAFE_INCLUDE_PAIR = st.tuples(
    SAFE_INCLUDE_FLAG, SAFE_INCLUDE_PATHS
).map(lambda t: [t[0], t[1]])

# Safe concatenated includes: -I/opt/custom/include
SAFE_INCLUDE_CONCAT = st.tuples(
    SAFE_INCLUDE_FLAG, SAFE_INCLUDE_PATHS
).map(lambda t: [t[0] + t[1]])

# Values for --target= and -target flags
TARGET_VALUES = st.text(
    alphabet=st.sampled_from(
        "abcdefghijklmnopqrstuvwxyz0123456789-_"
    ),
    min_size=1,
    max_size=30,
)

# A --target=<value> flag
TARGET_EQUALS_FLAG = TARGET_VALUES.map(lambda v: f"--target={v}")

# A -target <value> pair (represented as a 2-element list)
TARGET_SPACE_FLAG = TARGET_VALUES.map(lambda v: ["-target", v])

# Host system include paths
HOST_INCLUDE_PATHS = st.sampled_from([
    "/usr/include",
    "/usr/include/x86_64-linux-gnu",
    "/usr/include/x86_64-linux-gnu/bits",
    "/usr/local/include",
    "/usr/local/include/foo",
    "/usr/local/include/bar/baz",
    "/usr/lib/gcc/x86_64-linux-gnu/12",
    "/usr/lib/gcc/x86_64-linux-gnu/12/include",
])

HOST_INCLUDE_FLAG = st.sampled_from(["-isystem", "-I"])

# Space-separated host include: ["-isystem", "/usr/include/..."]
HOST_INCLUDE_SPACE = st.tuples(
    HOST_INCLUDE_FLAG, HOST_INCLUDE_PATHS
).map(lambda t: [t[0], t[1]])

# Concatenated host include: ["-isystem/usr/include/..."]
HOST_INCLUDE_CONCAT = st.tuples(
    HOST_INCLUDE_FLAG, HOST_INCLUDE_PATHS
).map(lambda t: [t[0] + t[1]])

# A single normal flag (as a 1-element list)
NORMAL_FLAG_ITEM = NORMAL_FLAGS.map(lambda f: [f])

# An argument "chunk": normal flag, safe include, target flag,
# or host include
ARG_CHUNK = st.one_of(
    NORMAL_FLAG_ITEM,
    SAFE_INCLUDE_PAIR,
    SAFE_INCLUDE_CONCAT,
    TARGET_EQUALS_FLAG.map(lambda f: [f]),
    TARGET_SPACE_FLAG,
    HOST_INCLUDE_SPACE,
    HOST_INCLUDE_CONCAT,
)

# A full argument list: a sequence of chunks flattened
ARG_LIST = st.lists(ARG_CHUNK, min_size=0, max_size=20).map(
    lambda chunks: [arg for chunk in chunks for arg in chunk]
)


HOST_PREFIXES = (
    "/usr/include",
    "/usr/local/include",
    "/usr/lib/gcc/",
)


def _create_mock_zig(tmpdir: str) -> str:
    """Create a mock 'zig' that prints all arguments, one per line."""
    mock_path = os.path.join(tmpdir, "zig")
    with open(mock_path, "w", encoding="utf-8") as f:
        f.write('#!/bin/sh\n')
        f.write('for arg in "$@"; do\n')
        f.write('  echo "$arg"\n')
        f.write('done\n')
    os.chmod(
        mock_path,
        stat.S_IRWXU | stat.S_IRGRP | stat.S_IXGRP
        | stat.S_IROTH | stat.S_IXOTH,
    )
    return tmpdir


def _run_wrapper(
    wrapper_path: str, args: list[str], mock_zig_dir: str
) -> list[str]:
    """Run a wrapper script with given args and return output lines."""
    env = os.environ.copy()
    env["PATH"] = mock_zig_dir + ":" + env.get("PATH", "")

    result = subprocess.run(
        [wrapper_path] + args,
        capture_output=True,
        text=True,
        env=env,
        timeout=10,
        check=False,
    )
    return [
        line for line in result.stdout.strip().split("\n") if line
    ]


def _is_host_include_path(path: str) -> bool:
    """Check if a path is a host system include path."""
    return any(path.startswith(p) for p in HOST_PREFIXES)


def _expected_filtered_args(args: list[str]) -> list[str]:
    """
    Compute expected arguments after filtering targets and host
    includes. Mirrors the wrapper's state machine exactly.
    """
    result = []
    skip_next = False
    skip_include = False
    saved_result = None

    for arg in args:
        if skip_next:
            skip_next = False
            continue

        if skip_include:
            skip_include = False
            if _is_host_include_path(arg):
                # Drop the flag we tentatively added + this path
                result = saved_result
                continue
            else:
                # Keep both the flag and the path
                result.append(arg)
                continue

        if arg.startswith("--target="):
            continue
        if arg == "-target":
            skip_next = True
            continue

        # Space-separated -isystem or -I
        if arg in ("-isystem", "-I"):
            skip_include = True
            saved_result = list(result)
            result.append(arg)
            continue

        # Concatenated forms: -isystem/usr/... or -I/usr/...
        if arg.startswith("-isystem"):
            path_part = arg[len("-isystem"):]
            if _is_host_include_path(path_part):
                continue
        elif arg.startswith("-I"):
            path_part = arg[len("-I"):]
            if _is_host_include_path(path_part):
                continue

        result.append(arg)

    return result


WRAPPER_CC = os.path.join(
    os.path.dirname(__file__), "..", "..",
    "cmake", "zig-aarch64-linux-cc",
)
WRAPPER_CXX = os.path.join(
    os.path.dirname(__file__), "..", "..",
    "cmake", "zig-aarch64-linux-cxx",
)


@pytest.fixture(scope="module")
def mock_zig_dir():
    """Create a temporary directory with a mock zig binary."""
    with tempfile.TemporaryDirectory() as tmpdir:
        _create_mock_zig(tmpdir)
        yield tmpdir


class TestArgFilteringCC:
    """
    Feature: linux-arm64-cross-compile, Property 1: Argument
    filtering preserves non-target, non-host-include arguments

    Validates: Requirements 1.3, 1.4
    """

    @given(args=ARG_LIST)
    @settings(max_examples=100)
    def test_filtering_preserves_non_target_non_host_args(
        self, args, mock_zig_dir
    ):
        """
        Property: For any argument list, after passing through the
        CC wrapper, all --target= and -target <value> flags and all
        host system include paths are removed, and all other
        arguments appear in their original order.

        **Validates: Requirements 1.3, 1.4**
        """
        output_args = _run_wrapper(WRAPPER_CC, args, mock_zig_dir)

        # Wrapper prepends: cc -target aarch64-linux-gnu
        assert len(output_args) >= 3, (
            f"Expected at least 3 args "
            f"(cc -target aarch64-linux-gnu), "
            f"got: {output_args}"
        )
        assert output_args[0] == "cc"
        assert output_args[1] == "-target"
        assert output_args[2] == "aarch64-linux-gnu"

        forwarded = output_args[3:]
        expected = _expected_filtered_args(args)

        assert forwarded == expected, (
            f"Mismatch!\n"
            f"  Input:    {args}\n"
            f"  Expected: {expected}\n"
            f"  Got:      {forwarded}"
        )

        # Verify no target flags in output
        for arg in forwarded:
            assert not arg.startswith("--target="), (
                f"Found --target= in output: {arg}"
            )

        # Verify no host include paths in output
        for i, arg in enumerate(forwarded):
            if arg in ("-isystem", "-I"):
                if i + 1 < len(forwarded):
                    assert not _is_host_include_path(
                        forwarded[i + 1]
                    ), (
                        f"Found host include: "
                        f"{arg} {forwarded[i + 1]}"
                    )
            elif arg.startswith("-isystem"):
                path = arg[len("-isystem"):]
                assert not _is_host_include_path(path), (
                    f"Found host include: {arg}"
                )
            elif arg.startswith("-I") and arg != "-I":
                path = arg[len("-I"):]
                assert not _is_host_include_path(path), (
                    f"Found host include: {arg}"
                )


class TestArgFilteringCXX:
    """
    Feature: linux-arm64-cross-compile, Property 1: Argument
    filtering preserves non-target, non-host-include arguments

    Validates: Requirements 1.3, 1.4
    """

    @given(args=ARG_LIST)
    @settings(max_examples=100)
    def test_filtering_preserves_non_target_non_host_args(
        self, args, mock_zig_dir
    ):
        """
        Property: For any argument list, after passing through the
        CXX wrapper, all --target= and -target <value> flags and
        all host system include paths are removed, and all other
        arguments appear in their original order.

        **Validates: Requirements 1.3, 1.4**
        """
        output_args = _run_wrapper(WRAPPER_CXX, args, mock_zig_dir)

        # Wrapper prepends: c++ -target aarch64-linux-gnu
        # and optionally: -nostdinc++ -isystem <path> ... (libstdc++)
        assert len(output_args) >= 3, (
            f"Expected at least 3 args "
            f"(c++ -target aarch64-linux-gnu), "
            f"got: {output_args}"
        )
        assert output_args[0] == "c++"
        assert output_args[1] == "-target"
        assert output_args[2] == "aarch64-linux-gnu"

        # Skip past the libstdc++ prefix args injected by the wrapper
        # These are: -nostdinc++ -cxx-isystem <path> ...
        idx = 3
        if idx < len(output_args) and output_args[idx] == "-nostdinc++":
            idx += 1
        # Skip -cxx-isystem <path> pairs for aarch64 libstdc++ includes
        while (idx + 1 < len(output_args)
               and output_args[idx] == "-cxx-isystem"
               and "/usr/aarch64-linux-gnu/" in output_args[idx + 1]):
            idx += 2

        forwarded = output_args[idx:]

        # Strip trailing -nostdlib++ -lstdc++ link flags added by wrapper
        while (forwarded
               and forwarded[-1] in ("-nostdlib++", "-lstdc++")):
            forwarded = forwarded[:-1]

        expected = _expected_filtered_args(args)

        assert forwarded == expected, (
            f"Mismatch!\n"
            f"  Input:    {args}\n"
            f"  Expected: {expected}\n"
            f"  Got:      {forwarded}"
        )

        for arg in forwarded:
            assert not arg.startswith("--target="), (
                f"Found --target= in output: {arg}"
            )

        for i, arg in enumerate(forwarded):
            if arg in ("-isystem", "-I"):
                if i + 1 < len(forwarded):
                    assert not _is_host_include_path(
                        forwarded[i + 1]
                    ), (
                        f"Found host include: "
                        f"{arg} {forwarded[i + 1]}"
                    )
            elif arg.startswith("-isystem"):
                path = arg[len("-isystem"):]
                assert not _is_host_include_path(path), (
                    f"Found host include: {arg}"
                )
            elif arg.startswith("-I") and arg != "-I":
                path = arg[len("-I"):]
                assert not _is_host_include_path(path), (
                    f"Found host include: {arg}"
                )
