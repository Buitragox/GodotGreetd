#include "greetd_response.hpp"
#include "godot_cpp/core/class_db.hpp"

using namespace godot;

void GreetdResponse::_bind_methods() {
}

void GreetdSuccess::_bind_methods() {
}

GreetdError::GreetdError(const String &type, const String &description) : error_type(type), error_description(description) {
}

void GreetdError::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_error_type"), &GreetdError::get_error_type);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "error_type"), "", "get_error_type");
	ClassDB::bind_method(D_METHOD("get_error_description"), &GreetdError::get_error_description);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "error_description"), "", "get_error_description");

	ClassDB::bind_method(D_METHOD("is_general_error"), &GreetdError::is_general_error);
	ClassDB::bind_method(D_METHOD("is_auth_error"), &GreetdError::is_auth_error);
	ClassDB::bind_method(D_METHOD("is_internal_error"), &GreetdError::is_internal_error);
}

// TODO: add enum for these values

bool GreetdError::is_general_error() const {
	return error_type == "error";
}

bool GreetdError::is_auth_error() const {
	return error_type == "auth_error";
}

bool GreetdError::is_internal_error() const {
	return error_type == "internal_error";
}

void GreetdAuthMessage::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_auth_message_type"), &GreetdAuthMessage::get_auth_message_type);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "auth_message_type"), "", "get_auth_message_type");
	ClassDB::bind_method(D_METHOD("get_auth_message"), &GreetdAuthMessage::get_auth_message);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "auth_message"), "", "get_auth_message");

	ClassDB::bind_method(D_METHOD("is_secret"), &GreetdAuthMessage::is_secret);
	ClassDB::bind_method(D_METHOD("is_visible"), &GreetdAuthMessage::is_visible);
	ClassDB::bind_method(D_METHOD("is_error"), &GreetdAuthMessage::is_error);
	ClassDB::bind_method(D_METHOD("is_info"), &GreetdAuthMessage::is_info);
}

GreetdAuthMessage::GreetdAuthMessage(const String &type, const String &message) : auth_message_type(type), auth_message(message) {}

String GreetdAuthMessage::get_auth_message_type() const { return auth_message_type; }

String GreetdAuthMessage::get_auth_message() const { return auth_message; }

// TODO: add enum for these values.

bool GreetdAuthMessage::is_secret() const {
	return auth_message_type == "secret";
}

bool GreetdAuthMessage::is_visible() const {
	return auth_message_type == "visible";
}

bool GreetdAuthMessage::is_error() const {
	return auth_message_type == "error";
}

bool GreetdAuthMessage::is_info() const {
	return auth_message_type == "info";
}
