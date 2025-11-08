#include "greetd_response.hpp"
#include "godot_cpp/core/class_db.hpp"

using namespace godot;

// Base class
void GreetdResponse::_bind_methods() {
}

// Success class
void GreetdSuccess::_bind_methods() {
}

GreetdError::GreetdError(const String& type, const String& description)
	: error_type(type), error_description(description) {
}

// Error class
void GreetdError::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_error_description"), &GreetdError::get_error_description);
	ClassDB::bind_method(D_METHOD("get_error_type"), &GreetdError::get_error_type);
	ClassDB::bind_method(D_METHOD("is_general_error"), &GreetdError::is_general_error);
	ClassDB::bind_method(D_METHOD("is_auth_error"), &GreetdError::is_auth_error);
	ClassDB::bind_method(D_METHOD("is_internal_error"), &GreetdError::is_internal_error);
}

String GreetdError::get_error_description() const {
	return error_description;
}

String GreetdError::get_error_type() const {
	return error_type;
}

bool GreetdError::is_general_error() const {
	return error_type == "error";
}

bool GreetdError::is_auth_error() const {
	return error_type == "auth_error";
}

bool GreetdError::is_internal_error() const {
	return error_type == "internal_error";
}

// Auth message class
void GreetdAuthMessage::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_auth_message_type"), &GreetdAuthMessage::get_auth_message_type);
	ClassDB::bind_method(D_METHOD("get_auth_message"), &GreetdAuthMessage::get_auth_message);
}

GreetdAuthMessage::GreetdAuthMessage(const String& type, const String& message)
	: auth_message_type(type), auth_message(message) {
}

String GreetdAuthMessage::get_auth_message_type() const {
	return auth_message_type;
}

String GreetdAuthMessage::get_auth_message() const {
	return auth_message;
}
