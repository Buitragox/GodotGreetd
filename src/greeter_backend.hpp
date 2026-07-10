#pragma once

#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include <memory>

class GreeterBackend {
public:
	virtual ~GreeterBackend() = default;

	virtual godot::TypedArray<godot::Dictionary> get_wayland_sessions() = 0;
	virtual godot::TypedArray<godot::String> get_users() = 0;
};

std::unique_ptr<GreeterBackend> create_greeter_backend();
