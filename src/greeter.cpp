#include "greeter.hpp"
#include "godot_cpp/classes/dir_access.hpp"
#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/os.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "greetd_response.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdint>
#include <cstring>
#include <string>
#include <pwd.h>

using namespace godot;

void GreetdGreeter::_bind_methods() {
	ClassDB::bind_method(D_METHOD("create_session", "username"), &GreetdGreeter::create_session);
	ClassDB::bind_method(D_METHOD("answer_auth_message", "answer"), &GreetdGreeter::answer_auth_message);
	ClassDB::bind_method(D_METHOD("start_session"), &GreetdGreeter::start_session);
	ClassDB::bind_method(D_METHOD("cancel_session"), &GreetdGreeter::cancel_session);
	ClassDB::bind_method(D_METHOD("get_wayland_sessions"), &GreetdGreeter::get_wayland_sessions);
	ClassDB::bind_method(D_METHOD("get_users"), &GreetdGreeter::get_users);
}

Ref<GreetdResponse> GreetdGreeter::create_session(const String& username) {
	int fd = connect_to_socket();
	if (fd < 0) {
		return memnew(GreetdError("internal_error", "Failed to connect socket"));
	}

	json request = {{"type", "create_session"}, {"username", username.utf8().get_data()}};
	Ref<GreetdResponse> response = send_greetd_request(fd, request);
	close(fd);
	return response;
}

Ref<GreetdResponse> GreetdGreeter::answer_auth_message(const String& answer) {
	int fd = connect_to_socket();
	if (fd < 0) {
		return memnew(GreetdError("internal_error", "Failed to connect socket"));
	}

	json request = {{"type", "post_auth_message_response"}, {"response", answer.utf8().get_data()}};
	Ref<GreetdResponse> response = send_greetd_request(fd, request);
	close(fd);
	return response;
}

// TODO: Receive cmd as an argument. Maybe as an optional argument?
// I think I just need to create another function with a different signature?
// Need to think about how to approach this
// - We should support both cmd and getting the session from gdscript
// - Would it be alright to just make cmd a required argument and make `get_cmd?
Ref<GreetdResponse> GreetdGreeter::start_session() {
	int fd = connect_to_socket();
	if (fd < 0) {
		return memnew(GreetdError("internal_error", "Failed to connect socket"));
	}

	std::string cmd = get_cmd().utf8().get_data();
	UtilityFunctions::print("cmd: ", cmd.data());

	json request = {{"type", "start_session"}, {"cmd", {cmd}}};
	Ref<GreetdResponse> response = send_greetd_request(fd, request);
	close(fd);
	return response;
}

Ref<GreetdResponse> GreetdGreeter::cancel_session() {
	int fd = connect_to_socket();
	if (fd < 0) {
		return memnew(GreetdError("internal_error", "Failed to connect socket"));
	}

	json request = {{"type", "cancel_session"}};
	Ref<GreetdResponse> response = send_greetd_request(fd, request);
	close(fd);
	return response;
}

TypedArray<Dictionary> GreetdGreeter::get_wayland_sessions() {
	TypedArray<Dictionary> sessions;
	// TODO: I don't think this a standard on every distro. Probably need to check other places
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
		while (file->get_position() < file->get_length()) {
			String line = file->get_line();

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

TypedArray<String> GreetdGreeter::get_users() {
	TypedArray<String> users;
	struct passwd* pw;

	// NOTE: not sure if these values are always correct.
	const int MIN_USER_ID = 1000;
	const int MAX_USER_ID = 60000;

	setpwent();

	while ((pw = getpwent()) != nullptr) {
		if (pw->pw_uid < 1000 || pw->pw_uid >= 60000) {
			continue;
		}

		users.append(pw->pw_name);
	}

	endpwent();

	return users;
}

Ref<GreetdResponse> GreetdGreeter::send_greetd_request(int fd, json request) {
	Error err = write_json(fd, request);
	if (err != Error::OK) {
		return memnew(GreetdError("internal_error", "Failed to write JSON request"));
	}

	json response;
	err = read_json(fd, response);

	if (err != Error::OK) {
		return memnew(GreetdError("internal_error", "Failed to read JSON response"));
	}

	if (!response.contains("type") || !response["type"].is_string()) {
		String err_message = "Missing or invalid 'type' field in response";
		UtilityFunctions::printerr(err_message);
		return memnew(GreetdError("internal_error", err_message));
	}

	Ref<GreetdResponse> result;
	std::string type = response["type"];
	if (type == "success") {
		result = Ref<GreetdResponse>(memnew(GreetdSuccess()));
	} else if (type == "error") {
		String err_type = response["error_type"].get<std::string>().c_str();
		String err_message = response["description"].get<std::string>().c_str();
		UtilityFunctions::printerr(err_message);
		result = Ref<GreetdResponse>(memnew(GreetdError(err_type, err_message)));
	} else if (type == "auth_message") {
		String message_type = response["auth_message_type"].get<std::string>().c_str();
		String auth_message = response["auth_message"].get<std::string>().c_str();
		result = Ref<GreetdResponse>(memnew(GreetdAuthMessage(message_type, auth_message)));
	}

	return result;
}

Error GreetdGreeter::write_json(int fd, json request) {
	std::string json_str = request.dump();
	const char* c_str = json_str.c_str();
	uint32_t size = json_str.size();

	ssize_t n = write_all(fd, &size, 4);
	if (n < 0) {
		UtilityFunctions::printerr("Failed to write size to socket");
		return Error::ERR_FILE_CANT_WRITE;
	}

	n = write_all(fd, c_str, size);
	if (n < 0) {
		UtilityFunctions::printerr("Failed to write payload to socket");
		return Error::ERR_FILE_CANT_WRITE;
	}
	return Error::OK;
}

Error GreetdGreeter::read_json(int fd, json& response) {
	uint32_t response_size;
	ssize_t n = read_all(fd, &response_size, 4);
	if (n < 0) {
		UtilityFunctions::printerr("Failed to read response size");
		return godot::ERR_FILE_CANT_READ;
	}

	std::vector<char> buffer(response_size + 1);
	n = read_all(fd, buffer.data(), response_size);
	if (n < 0) {
		UtilityFunctions::printerr("Failed to read payload");
		return godot::ERR_FILE_CANT_READ;
	}

	buffer[response_size] = '\0';
	UtilityFunctions::print("Received: ", buffer.data());

	response = json::parse(buffer.data(), nullptr, false);
	if (response.is_discarded()) {
		UtilityFunctions::printerr("Failed to parse JSON response", buffer.data());
		return Error::ERR_PARSE_ERROR;
	}

	return Error::OK;
}


ssize_t GreetdGreeter::write_all(int fd, const void* data, size_t len) {
	const uint8_t* ptr = static_cast<const uint8_t*>(data);
	ssize_t total = 0;

	while (total < len) {
		ssize_t n = write(fd, ptr + total, len - total);
		if (n <= 0) {
			return -1;
		}
		total += n;
	}
	return total;
}

ssize_t GreetdGreeter::read_all(int fd, void* data, size_t len) {
	uint8_t* ptr = static_cast<uint8_t*>(data);
	ssize_t total = 0;

	while (total < len) {
		ssize_t n = read(fd, ptr + total, len - total);
		if (n <= 0) {
			return -1;
		}
		total += n;
	}
	return total;
}

// If more command line arguments are required, it would be better make a hash from the user args
String GreetdGreeter::get_cmd() {
	OS* os = OS::get_singleton();
	PackedStringArray args = os->get_cmdline_user_args();

	for (int i = 0; i < args.size(); i++) {
		String arg = args[i];

		if (arg.contains("=")) {
			PackedStringArray key_value = arg.split("=");
			String key = key_value[0].trim_prefix("--");

			if (key == "cmd") {
				return key_value[1];
			}
		}
	}
	// TODO: probably delete this? No need to asume defaults.
	// Run hyprland by default after login
	return String("hyprland");
}

int GreetdGreeter::connect_to_socket() {
	String env_sock = OS::get_singleton()->get_environment("GREETD_SOCK");
	// HACK for debugging
	if (env_sock.is_empty()) {
		env_sock = "/tmp/example_socket";
	}
	const char* path = env_sock.utf8().get_data();

	int fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd < 0) {
		UtilityFunctions::printerr("Failed to create socket");
		return -1;
	}

	sockaddr_un address{};
	address.sun_family = AF_UNIX;
	std::strncpy(address.sun_path, path, sizeof(address.sun_path) - 1);

	int err = ::connect(fd, (struct sockaddr*)&address, sizeof(address));
	if (err < 0) {
		close(fd);
		String err_message = "Failed to connect to socket (" + String::utf8(strerror(errno)) + ")";
		UtilityFunctions::printerr(err_message);
		return -1;
	}

	return fd;
}
