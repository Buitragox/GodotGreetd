# Creating Your Greeter

> [!WARNING]
> This document is under development. Specially the "Deploying your greeter" section.

This guide is for building a greeter (login screen) using the GodotGreetd extension.

## Protocol Overview

This extension implements the greetd IPC protocol. Your greeter still needs to use the protocol appropriately.

A typical login flow:

1. Create a session for a user.
2. Answer an authentication message (usually the user's password).
3. Start the session with a command (usually a Wayland session).
4. Exit the greeter app.

After the greeter closes, greetd logs in the user and launches the command.

See [greetd-protocol.md](greetd-protocol.md) for the full protocol reference, state machine, and flow diagrams.

## Example

See [`demo/login_screen.gd`](../demo/login_screen.gd) for a working example.

## Utilities

The extension provides some useful utilities:

- **Get Wayland sessions** (e.g. Plasma, Hyprland) — parses files in `/usr/share/wayland-sessions/`.
  - TODO: X11 sessions.
- **Get users** — lists available system users.

## Testing with the mock server

[`mock_server.py`](../mock_server.py) implements a mock greetd server that connects to `/tmp/example_socket`.

```sh
python mock_server.py
```

Options:
- `--no-delay` — disable the simulated auth delay on wrong passwords
- `-v` — verbose output (debug logging)

Run your app from the Godot editor and test it while the mock server is running.

The extension looks for a socket in the `GREETD_SOCK` env variable. If it's empty, it connects to `/tmp/example_socket`. This is a hack for testing.

## Deploying your greeter

First, [install greetd](https://sr.ht/~kennylevinsen/greetd/#installation).

<!-- TODO: finish this part -->

When the computer starts, it uses the "greeter" user to launch the greeter app. Godot wants to create/save files in the user directory, but the greeter user has no home directory, so you need to create directories with proper permissions:

```sh
sudo mkdir -p /home/greeter/.cache
sudo mkdir -p /home/greeter/.config/dconf
sudo mkdir -p /home/greeter/.local

sudo chown -R greeter:greeter /home/greeter/

# You may need to give permissions:
sudo chmod -R 700 /home/greeter
```

# TODO

- Explain that you obviously need to install godot (min version?)
- Explain the greetd config
- Easily download and install the extension
	  - First release?
- Explain the full installation process
- Rename "Deploying your greeter", maybe "Install/Use your greeter"?
- Explain that the demo does not fully implement all expected functionality from greetd
		- Or just implement it? lol
- Add an easy install for the demo greeter
		- Rename the demo greeter to something better.
      While it is a demo, the idea is for it to be fully functional and you should be able to actually use it.
