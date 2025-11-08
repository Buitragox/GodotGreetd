#pragma once

#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/string.hpp"
#include "greetd_response.hpp"
#include <json.hpp>
#include <sys/types.h>

using json = nlohmann::json;

class Greeter : public godot::RefCounted {
	GDCLASS(Greeter, godot::RefCounted)

protected:
	static void _bind_methods();

public:
	Greeter() = default;
	~Greeter() override = default;

	godot::Ref<GreetdResponse> create_session(const godot::String username);
	godot::Error start_session();

private:
	godot::Ref<GreetdResponse> send_greetd_request(int fd, json request);
	godot::Error write_json(int fd, json request);
	godot::Error read_json(int fd, json& response);
	ssize_t write_all(int fd, const void* data, size_t len);
	ssize_t read_all(int fd, void* data, size_t len);
	godot::String get_cmd();
	godot::String socket_path();
};
