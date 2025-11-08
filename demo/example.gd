extends Node


func _ready() -> void:
	var greeter := Greeter.new()
	var response := greeter.create_session("cooluser")
	
	
	if response is GreetdSuccess:
		print("Success")
	elif response is GreetdError:
		print("Error")
		print(response.is_auth_error())
		print(response.is_general_error())
	
