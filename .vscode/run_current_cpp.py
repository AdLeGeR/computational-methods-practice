import os
import platform
import subprocess
import sys
from pathlib import Path


def run(command):
    print("$", " ".join(map(str, command)))
    result = subprocess.run(command)

    if result.returncode != 0:
        sys.exit(result.returncode)


def main():
    if len(sys.argv) != 2:
        print("Usage: run_current_cpp.py <cpp-file>")
        sys.exit(1)

    cpp_file = Path(sys.argv[1]).resolve()
    project_dir = Path.cwd().resolve()
    build_dir = project_dir / "build"

    # Проверяем, что файл находится внутри проекта.
    try:
        relative_path = cpp_file.relative_to(project_dir)
    except ValueError:
        print(f"Error: {cpp_file} is outside the project.")
        sys.exit(1)

    if cpp_file.suffix.lower() != ".cpp":
        print(f"Error: {cpp_file} is not a .cpp file.")
        sys.exit(1)

    # Делаем имя target так же, как в CMakeLists.txt:
    #
    # algorithms/sort.cpp
    # ->
    # algorithms_sort
    #
    target_name = relative_path.with_suffix("").as_posix()
    target_name = target_name.replace("/", "_")
    target_name = target_name.replace(" ", "_")
    target_name = target_name.replace("\\", "_")

    # Собираем только текущий target.
    run([
        "cmake",
        "--build",
        str(build_dir),
        "--target",
        target_name
    ])

    # Определяем, где CMake положил executable.
    #
    # CMake по умолчанию:
    #   Windows -> build/<config>/<target>.exe
    #   macOS/Linux -> build/<target>
    #
    candidates = []

    if platform.system() == "Windows":
        candidates.extend([
            build_dir / "Debug" / f"{target_name}.exe",
            build_dir / "Release" / f"{target_name}.exe",
            build_dir / f"{target_name}.exe",
        ])
    else:
        candidates.extend([
            build_dir / target_name,
            build_dir / "Debug" / target_name,
            build_dir / "Release" / target_name,
        ])

    executable = next(
        (path for path in candidates if path.is_file()),
        None
    )

    if executable is None:
        print(f"Error: executable for target '{target_name}' not found.")
        print("Searched:")
        for path in candidates:
            print(f"  {path}")
        sys.exit(1)

    print()
    print(f"Running: {executable}")
    print()

    run([str(executable)])


if __name__ == "__main__":
    main()