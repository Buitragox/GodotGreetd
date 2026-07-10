#include "greeter_backend.hpp"

#include "godot_cpp/classes/dir_access.hpp"
#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

#if defined(__linux__) && !defined(GREETD_MOCK_BACKEND)
#include <pwd.h>
#endif

using namespace godot;

class MockGreeterBackend : public GreeterBackend {
public:
	TypedArray<Dictionary> get_wayland_sessions() override {
		TypedArray<Dictionary> sessions;

		Dictionary godot_session;
		// TODO: verify these values are correct
		godot_session["Name"] = "Mock Godot";
		godot_session["Comment"] = "Mock Wayland session for local development";
		godot_session["Exec"] = "godot";
		godot_session["Type"] = "Application";
		sessions.append(godot_session);

		Dictionary desktop_session;
		desktop_session["Name"] = "Mock Desktop";
		desktop_session["Comment"] = "Preset desktop session";
		desktop_session["Exec"] = "mock-desktop-session";
		desktop_session["Type"] = "Application";
		sessions.append(desktop_session);

		return sessions;
	}

	TypedArray<String> get_users() override {
		TypedArray<String> users;
		users.append("demo");
		users.append("developer");
		users.append("guest");
		return users;
	}
};

#if defined(__linux__) && !defined(GREETD_MOCK_BACKEND)
class LinuxGreeterBackend : public GreeterBackend {
public:
	TypedArray<Dictionary> get_wayland_sessions() override {
		TypedArray<Dictionary> sessions;
		// NOTE: I don't think this a standard on every distro. Probably need to check other places
		// or add the ability for people to set their own paths?
		const String path = "/usr/share/wayland-sessions/";

		Ref<DirAccess> dir = DirAccess::open(path);
		if (dir.is_null()) {
			Error err = DirAccess::get_open_error();
			UtilityFunctions::printerr("Failed to access sessions directory (Error ", err, ")");
			return sessions;
		}

		// NOTE: This is a very simple parsing, maybe it will be necessary to refine this later
		for (String file_name : dir->get_files()) {
			Dictionary session;
			Ref<FileAccess> file = FileAccess::open(path + file_name, FileAccess::READ);
			if (file.is_null()) {
				Error err = FileAccess::get_open_error();
				UtilityFunctions::printerr("Failed to open session file '", file_name, "' (Error ", err, ")");
				continue;
			}

			while (file->get_position() < file->get_length()) {
				String line = file->get_line();

				String trimmed_line = line.strip_edges(true, false);
				if (trimmed_line.begins_with("#")) {
					continue;
				}

				if (line.find("=") == -1) {
					continue;
				}

				PackedStringArray key_value = line.split("=", true, 1);
				session[key_value[0]] = key_value[1];
			}
			sessions.append(session);
		}

		return sessions;
	}

	TypedArray<String> get_users() override {
		TypedArray<String> users;
		struct passwd *pw;

		// NOTE: not sure if these values are always correct.
		const int MIN_USER_ID = 1000;
		const int MAX_USER_ID = 60000;

		setpwent();

		while ((pw = getpwent()) != nullptr) {
			if (pw->pw_uid < MIN_USER_ID || pw->pw_uid >= MAX_USER_ID) {
				continue;
			}

			users.append(pw->pw_name);
		}

		endpwent();

		return users;
	}
};
#endif

std::unique_ptr<GreeterBackend> create_greeter_backend() {
#if defined(__linux__) && !defined(GREETD_MOCK_BACKEND)
	return std::make_unique<LinuxGreeterBackend>();
#else
	return std::make_unique<MockGreeterBackend>();
#endif
}
