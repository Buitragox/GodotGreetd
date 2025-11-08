#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/variant/string.hpp"

// Base class for all Greetd responses
class GreetdResponse : public godot::RefCounted {
	GDCLASS(GreetdResponse, RefCounted)

protected:
	static void _bind_methods();

public:
	GreetdResponse() = default;
	~GreetdResponse() override = default;
};

// Simple success response
class GreetdSuccess : public GreetdResponse {
	GDCLASS(GreetdSuccess, GreetdResponse)

protected:
	static void _bind_methods();

public:
	GreetdSuccess() = default;
	~GreetdSuccess() override = default;
};

// Error
class GreetdError : public GreetdResponse {
	GDCLASS(GreetdError, GreetdResponse)

private:
	godot::String error_type; // Greetd errors: "error" or "auth_error".
	godot::String error_description;

protected:
	static void _bind_methods();

public:
	GreetdError() = default;
	GreetdError(const godot::String& type, const godot::String& description);
	~GreetdError() override = default;

	godot::String get_error_description() const;
	godot::String get_error_type() const;
	bool is_general_error() const;
	bool is_auth_error() const;
	bool is_internal_error() const;
};

class GreetdAuthMessage : public GreetdResponse {
	GDCLASS(GreetdAuthMessage, GreetdResponse)

private:
	godot::String auth_message_type; // Types can be "visible", "secret", "info" or "error"
	godot::String auth_message;

protected:
	static void _bind_methods();

public:
	GreetdAuthMessage() = default;
	GreetdAuthMessage(const godot::String& message_type, const godot::String& message);
	~GreetdAuthMessage() override = default;

	godot::String get_auth_message_type() const;
	godot::String get_auth_message() const;
};
