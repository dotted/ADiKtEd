#!/usr/bin/env python3
from __future__ import annotations

import argparse
import shutil
import tarfile
import zipfile
from pathlib import Path


def copy_file(src: Path, dest: Path) -> None:
    dest.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(src, dest)


def copy_tree(src: Path, dest: Path) -> None:
    if src.exists():
        shutil.copytree(src, dest, dirs_exist_ok=True)


def copy_matching_files(src_dir: Path, dest_dir: Path, patterns: tuple[str, ...]) -> None:
    if not src_dir.exists():
        return

    for path in src_dir.iterdir():
        if not path.is_file():
            continue

        if any(path.match(pattern) for pattern in patterns):
            copy_file(path, dest_dir / path.name)


def assert_exists(path: Path, description: str) -> None:
    if not path.exists():
        raise FileNotFoundError(f"Expected {description} at {path}")


def verify_install_tree(install_dir: Path) -> None:
    bin_dir = install_dir / "bin"
    share_dir = install_dir / "share" / "map"

    map_candidates = (bin_dir / "map", bin_dir / "map.exe")
    if not any(candidate.exists() for candidate in map_candidates):
        raise FileNotFoundError("Expected map executable in install/bin")

    assert_exists(share_dir / "map.hlp", "mapslang help file")
    assert_exists(share_dir / "map.ini", "mapslang config file")
    assert_exists(share_dir / "map.ico", "mapslang icon file")
    assert_exists(install_dir / "include" / "libadikted" / "adikted.h", "public libadikted header")
    assert_exists(install_dir / "lib" / "pkgconfig" / "libadikted.pc", "pkg-config metadata")
    assert_exists(
        install_dir / "lib" / "cmake" / "libadikted" / "libadiktedConfig.cmake",
        "CMake package config",
    )


def build_user_stage(install_dir: Path, stage_dir: Path) -> None:
    copy_tree(install_dir / "bin", stage_dir / "bin")
    copy_tree(install_dir / "share" / "map", stage_dir / "share" / "map")
    copy_matching_files(
        install_dir / "lib",
        stage_dir / "lib",
        (
            "libadikted*.so*",
            "libadikted*.dylib*",
            "libslang*.so*",
            "libslang*.dylib*",
            "adikted*.so*",
            "adikted*.dylib*",
        ),
    )


def build_sdk_stage(install_dir: Path, stage_dir: Path) -> None:
    copy_tree(install_dir / "include", stage_dir / "include")
    copy_tree(install_dir / "lib", stage_dir / "lib")
    copy_matching_files(
        install_dir / "bin",
        stage_dir / "bin",
        (
            "adikted*.dll",
            "libadikted*.dll",
        ),
    )


def add_common_files(repo_root: Path, stage_dir: Path) -> None:
    for name in ("LICENSE", "README.md"):
        source = repo_root / name
        if source.exists():
            copy_file(source, stage_dir / name)


def verify_user_stage(stage_dir: Path) -> None:
    bin_dir = stage_dir / "bin"
    share_dir = stage_dir / "share" / "map"

    map_candidates = (bin_dir / "map", bin_dir / "map.exe")
    if not any(candidate.exists() for candidate in map_candidates):
        raise FileNotFoundError("End-user bundle is missing the map executable")

    assert_exists(share_dir / "map.hlp", "end-user help file")
    assert_exists(share_dir / "map.ini", "end-user config file")
    assert_exists(share_dir / "map.ico", "end-user icon file")


def verify_sdk_stage(stage_dir: Path) -> None:
    assert_exists(stage_dir / "include" / "libadikted" / "adikted.h", "SDK public header")
    assert_exists(stage_dir / "lib" / "pkgconfig" / "libadikted.pc", "SDK pkg-config metadata")
    assert_exists(stage_dir / "lib" / "cmake" / "libadikted" / "libadiktedConfig.cmake", "SDK CMake metadata")


def create_zip(source_dir: Path, archive_path: Path, root_name: str) -> None:
    with zipfile.ZipFile(archive_path, "w", compression=zipfile.ZIP_DEFLATED) as archive:
        for path in sorted(source_dir.rglob("*")):
            archive.write(path, Path(root_name) / path.relative_to(source_dir))


def create_tar_gz(source_dir: Path, archive_path: Path, root_name: str) -> None:
    with tarfile.open(archive_path, "w:gz") as archive:
        archive.add(source_dir, arcname=root_name)


def create_archive(source_dir: Path, output_dir: Path, archive_name: str, archive_format: str) -> Path:
    suffix = ".zip" if archive_format == "zip" else ".tar.gz"
    archive_path = output_dir / f"{archive_name}{suffix}"
    if archive_path.exists():
        archive_path.unlink()

    if archive_format == "zip":
        create_zip(source_dir, archive_path, archive_name)
        return archive_path

    if archive_format == "tar.gz":
        create_tar_gz(source_dir, archive_path, archive_name)
        return archive_path

    raise ValueError(f"Unsupported archive format: {archive_format}")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Prepare ADiKtEd release artifacts from a staged install tree.")
    parser.add_argument("--install-dir", required=True, type=Path)
    parser.add_argument("--output-dir", required=True, type=Path)
    parser.add_argument("--user-archive-name", required=True)
    parser.add_argument("--sdk-archive-name", required=True)
    parser.add_argument("--archive-format", choices=("zip", "tar.gz"), required=True)
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    repo_root = Path(__file__).resolve().parents[2]
    install_dir = args.install_dir.resolve()
    output_dir = args.output_dir.resolve()
    work_dir = output_dir / "_staging"

    verify_install_tree(install_dir)

    if work_dir.exists():
        shutil.rmtree(work_dir)
    work_dir.mkdir(parents=True, exist_ok=True)
    output_dir.mkdir(parents=True, exist_ok=True)

    user_stage = work_dir / args.user_archive_name
    sdk_stage = work_dir / args.sdk_archive_name

    build_user_stage(install_dir, user_stage)
    build_sdk_stage(install_dir, sdk_stage)
    add_common_files(repo_root, user_stage)
    add_common_files(repo_root, sdk_stage)

    verify_user_stage(user_stage)
    verify_sdk_stage(sdk_stage)

    user_archive = create_archive(user_stage, output_dir, args.user_archive_name, args.archive_format)
    sdk_archive = create_archive(sdk_stage, output_dir, args.sdk_archive_name, args.archive_format)
    shutil.rmtree(work_dir)

    print(f"Created {user_archive}")
    print(f"Created {sdk_archive}")


if __name__ == "__main__":
    main()
