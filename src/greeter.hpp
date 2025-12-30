#pragma once

#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "greetd_response.hpp"
#include <json.hpp>
#include <sys/types.h>

using json = nlohmann::json;

class GreetdGreeter : public godot::RefCounted {
	GDCLASS(GreetdGreeter, godot::RefCounted)

protected:
	static void _bind_methods();

public:
	GreetdGreeter() = default;
	~GreetdGreeter() override = default;

	godot::Ref<GreetdResponse> create_session(const godot::String& username);
	godot::Ref<GreetdResponse> answer_auth_message(const godot::String& answer);
	godot::Ref<GreetdResponse> start_session(const godot::String& cmd = "");
	godot::Ref<GreetdResponse> cancel_session();
	godot::TypedArray<godot::Dictionary> get_wayland_sessions();
	godot::TypedArray<godot::String> get_users();

private:
	godot::Ref<GreetdResponse> send_greetd_request(int fd, json request);
	godot::Error write_json(int fd, json request);
	godot::Error read_json(int fd, json& response);
	ssize_t write_all(int fd, const void* data, size_t len);
	ssize_t read_all(int fd, void* data, size_t len);
	godot::String get_cmd();
	int connect_to_socket();
};
