# questforge

A C++20 command-line tool that assembles randomized exams from a YAML question catalog and renders them as PDFs via [Typst](https://typst.app).

## Table of Contents
- [What it does](#what-it-does)
- [Getting started](#getting-started)
- [Build](#build)
- [Usage](#usage)
- [Question catalog format](#question-catalog-format)
- [Project status](#project-status)
- [Contributing](#contributing)
- [License](#license)

## What it does

Given a catalog of questions (with metadata like topic, difficulty, and points), questforge selects a random subset matching your criteria and produces a print-ready PDF. Multiple test variants with different question selections can be generated from the same catalog using explicit seeds for reproducibility.

## Getting started

1. Clone the repository:
```bash
git clone https://codeberg.org/Gwynspring/questforge.git
```

2. Install dependencies:

**Prerequisites:** CMake ≥ 3.25, Ninja, a C++20 compiler, and the following libraries: yaml-cpp, inja, CLI11, spdlog, GoogleTest. Install them via your package manager of choice (apt, brew, vcpkg, Nix, ...).

### Fedora

All dependencies are available directly via `dnf`:

```bash
sudo dnf install -y cmake ninja-build gcc-c++ \
  yaml-cpp-devel inja-devel cli11-devel spdlog-devel \
  gtest-devel gmock-devel
```

### Debian / Ubuntu

```bash
sudo apt install -y cmake ninja-build g++ \
  libyaml-cpp-dev libcli11-dev libspdlog-dev \
  libgtest-dev libgmock-dev
```

`inja` is not packaged on current Debian stable or Ubuntu LTS releases (it
only landed in Debian trixie+1/sid and Ubuntu 26.04+ as `inja-dev`). On
older releases, install it manually (header-only, just drop
`include/inja` somewhere on your include path) or vendor it via CMake's
`FetchContent`.

### Arch Linux

All dependencies are in the official `extra` repo:

```bash
sudo pacman -S --needed cmake ninja gcc \
  yaml-cpp spdlog gtest cli11 inja
```

### NixOS

For an ad-hoc shell with all required dependencies:

```bash
nix-shell -p cmake ninja gcc yaml-cpp inja cli11 spdlog gtest
```

### All distros

To install `typst` follow the official documentation available [here](https://github.com/typst/typst?tab=readme-ov-file#installation)

## Build

To build the project run the following commands in the project root

```bash
cmake --preset default
cmake --build build

# Run tests
ctest --test-dir build --output-on-failure
```

## Usage

To generate a test, use the following command:

```bash
./build/questforge generate --catalog data/catalog/algebra.yaml --easy 1 --medium 1 --hard 1 --out /tmp/test.pdf
```

Available options for `generate`:

| Option | Required | Description |
|---|---|---|
| `-c, --catalog` | yes | Path to the YAML question catalog. |
| `-o, --out` | yes | Path of the generated PDF. |
| `--easy` | no | Number of easy questions to select (default: 0). |
| `--medium` | no | Number of medium questions to select (default: 0). |
| `--hard` | no | Number of hard questions to select (default: 0). |
| `--topics` | no | Comma-separated list of topics to filter by (e.g. `algebra,geometry`). |
| `-s, --seed` | no | Explicit random seed for reproducible selection. |
| `--template` | no | Path to the Typst template (default: `templates/test.typ.jinja`). |

## Question catalog format

```yaml
questions:
  - id: alg-001
    topic: algebra
    difficulty: easy      # easy | medium | hard
    points: 2
    text: "Solve: $2x + 3 = 7$"
    image: null           # optional path relative to catalog
    tags: [equations, linear]
```

Math is written in Typst syntax (not LaTeX).

If you are not familiar with the typst syntax, take a look at the official typst tutorial available [here](https://typst.app/docs/tutorial/)

## Project status

First prototype is finished. Right now I'm hardening the current code before adding new features.

## Contributing
This is a private learning project. It's not accepting contributions or pull requests at this time.

## License
MIT — see [LICENSE](LICENSE).

