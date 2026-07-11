# Building the Extension

This guide is for developing the GDExtension C++ code itself.

## Setting up the environment

1. Clone the repo and cd into it.

2. Initialize the godot-cpp git submodule:
   ```sh
   git submodule update --init
   ```
   Currently using the Godot 4.5 branch, but older 4.x likely works too. You can configure this in [`.gitmodules`](../.gitmodules).

3. Install scons:
   ```sh
   sudo pacman -S scons
   ```
   CMake may work but isn't tested.

4. (Optional) Install a C++ the clang compiler (or use g++):
   ```sh
   sudo pacman -S clang
   ```

5. Compile the gdextension:
   ```sh
   scons
   ```

   The library is copied to `demo/bin/<platform>/`, ready for the demo project to load.

## Developing with mock system data

The system backend provides the values returned by `get_users()` and `get_wayland_sessions()`:

- Linux uses local system users and `/usr/share/wayland-sessions/` by default.
- macOS uses preset mock users and sessions automatically, so the demo can run without Linux system data.
- Linux can use the same mock backend with:

  ```sh
  scons mock_backend=yes
  ```

The backend does not mock greetd IPC. Run [`mock_server.py`](../mock_server.py) separately when testing the login flow locally.

## Configuring an IDE or language server

You can generate a `compile_commands.json` for your IDE/language server:

```sh
# Generate compile_commands.json while compiling
scons compiledb=yes

# Generate compile_commands.json without compiling
scons compiledb=yes compile_commands.json

# For clangd language server, add the use_llvm=true flag
scons compiledb=yes use_llvm=true
```

## GDExtension classes

Register classes in [`register_types.cpp`](../src/register_types.cpp) in the initialization method using `GDREGISTER_CLASS(ClassName);`.

## GitHub Actions

The repo has a GitHub action that builds the GDExtension on each push. Edit it in [`.github/workflows/builds.yml`](../.github/workflows/builds.yml). After a workflow run, find the artifact on the Actions tab.

## TODO

- [ ] Check if it compiles with CMake (how to generate compile_commands.json with CMake?)
- [ ] Should I implement an easier way to approach the protocol? (e.g. handle state, provide a simple `login` function, or a `next_request` function?)
