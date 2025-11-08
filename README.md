# Greetd Godot Extension
TODO

## Development

### Setup
1. Initialize the godot-cpp git submodule via `git submodule update --init`. (Currently using Godot 4.5 branch)
2. Install scons: `sudo pacman -S scons`
3. Install a c++ compiler like clang (or g++): `sudo pacman -S clang`
4. Compile the gdextension: `scons`

### Wayland
To make your Godot project use Wayland:
1. Open "Project" -> "Project Settings..."
2. Enable "Advanced Settings" on the top right.
3. Go to "Display" -> "Display Server" -> "Driver.linuxbsd" -> "wayland".

### Configuring an IDE or language server
To work with an IDE or language server, you can use a compilation database file called `compile_commands.json`. Most IDEs (and clangd language server) should automatically identify this file, and self-configure appropriately.
```sh
# Generate compile_commands.json while compiling
scons compiledb=yes

# Generate compile_commands.json without compiling
scons compiledb=yes compile_commands.json

# For clangd language server, add the `use_llvm=true` flag
scons compiledb=yes use_llvm=true
```

# Notes to self

## Usage - Template

* register the classes you want Godot to interact with inside the `register_types.cpp` file in the initialization method (here `initialize_gdextension_types`) in the syntax `GDREGISTER_CLASS(CLASS-NAME);`.

## Usage - Actions

This repository comes with a GitHub action that builds the GDExtension. It triggers automatically for each pushed change. You can find and edit it in [builds.yml](.github/workflows/builds.yml).
After a workflow run is complete, you can find the file `godot-cpp-template.zip` on the `Actions` tab on GitHub.

## TODO
- [ ] Check if it compiles with CMake
	- How can you generate a compilation database file with CMake?
