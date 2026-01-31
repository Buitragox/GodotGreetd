# GodotGreetd
---
A GDExtension for the [Greetd](https://man.sr.ht/~kennylevinsen/greetd/) IPC Protocol.

Create **greeters** (login applications) using the [Godot game engine](https://godotengine.org/).

**Is this a good idea?** I don't know, but it's fun :)

# Development

## Setting up the environment
1. Clone the repo and cd into it.
2. Initialize the godot-cpp git submodule via `git submodule update --init`.
	- Currently using Godot 4.5 branch but you can probably use an older 4.x version if needed.
	- You can configure this in the `.gitmodules` file.
3. Install scons: `sudo pacman -S scons`
	- You should be able to use CMake but I'm not familiar with it.
4. Install a c++ compiler like clang (or use g++): `sudo pacman -S clang`
5. Compile the gdextension: `scons`

## Configuring an IDE or language server
To work with an IDE or language server, you can use a compilation database file called `compile_commands.json`. Most IDEs (and clangd language server) should automatically identify this file, and self-configure appropriately.
```sh
# Generate compile_commands.json while compiling
scons compiledb=yes

# Generate compile_commands.json without compiling
scons compiledb=yes compile_commands.json

# For clangd language server, add the `use_llvm=true` flag
scons compiledb=yes use_llvm=true
```

## Creating your greeter
This extension simply implements the Greetd IPC Protocol. Your greeter still needs to use the protocol appropiately.

A typical login flow looks like this:
1. Create a session for a user.
2. Answer an authenthication message (usually the user's password)
3. Start the session with a command (usually running a wayland session)
4. Exit the greeter app.

After the greeter closes, greetd will log in the user and launch the command.

You can see an example in `demo/login_screen.gd`

The extension also provide some useful utilities for building your greeter:
- Get wayland sessions (e.g. Plasma, Hyprland, etc)
	- This uses a simple parsing of files in "/usr/share/wayland-sessions/". Good enough for now.
	- TO DO: X11 sessions.
- Get users.

TODO: pending an in-depth explanation of the protocol and some diagrams.

## Testing your greeter
The `server.py` file implements a simple Greetd server with mocked data that connects to the `/tmp/example_socket` UNIX socket.

Run it with `python server.py`.

The extension looks for a socket in the `GREETD_SOCK` env variable. If it's empty it connects to `/tmp/example_socket`. This is a hack for testing. I'll probably change it in the future.

## Using your greeter
First, [install greetd](https://sr.ht/~kennylevinsen/greetd/#installation).

TODO: finish this part

When your computer starts it will use the "greeter" user to launch the greeter app.
Godot wants to create and save some files in the user directory. The problem is that the greeter user does not have a home directory, so we will need to create some directories with the proper permissions.
```sh
sudo mkdir -p /home/greeter/.cache
sudo mkdir -p /home/greeter/.config/dconf
sudo mkdir -p /home/greeter/.local

sudo chown -R greeter:greeter /home/greeter/

# You may need to give permissions:
sudo chmod -R 700 /home/greeter
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
- [ ] Should I implement an easier way to approach the protocol?
	- For example, handle the state of the login process or provide a simple `login` function that receives username, password, and does everything?
	- maybe a `next_request` function?
