extends Node

var greeter := GreetdGreeter.new()

func _ready() -> void:
	%Password.text_submitted.connect(_on_password_submitted)
	%LogIn.pressed.connect(login)
	%SessionSelect.add_item("hey")
	%SessionSelect.add_item("no")

func hey(text: String) -> void:
	print("doing something", text)
	
func _on_password_submitted(_password):
	login()

func login() -> void:
	var username: String = %Username.text
	var password: String = %Password.text
	var response := greeter.create_session(username)
	
	if response is GreetdError:
		%Alert.text = response.get_error_description()
		log_info(response.get_error_description())
		return
	elif response is GreetdAuthMessage:
		log_info("%s - %s" % [response.get_auth_message_type(), response.get_auth_message()])
	else:
		# TODO if the response is a success then we can start the session
		# How does that happen? Users without passwords?
		pass
	
	log_info("Session created") 
	response = greeter.answer_auth_message(password)
	if response is GreetdError:
		%Alert.text = response.get_error_description()
		log_info(response.get_error_description())
		cancel_session()
		return
	elif response is GreetdAuthMessage:
		log_info("Unexpected: %s - %s" % [response.get_auth_message_type(), response.get_auth_message()])
		cancel_session()
		return
	else:
		log_info("Auth answered")
		
	response = greeter.start_session()
	if response is GreetdError:
		%Alert.text = response.get_error_description()
		log_info(response.get_error_description())
		cancel_session()
		return
	elif response is GreetdAuthMessage:
		log_info("Unexpected: %s - %s" % [response.get_auth_message_type(), response.get_auth_message()])
		cancel_session()
		return
	else:
		log_info("Session started")
	
	%Alert.text = "Success :D Exiting in 3 seconds"
	await get_tree().create_timer(3.0).timeout
	get_tree().quit()


func log_info(text: String) -> void:
	%Log.add_text(text + "\n")


func cancel_session() -> void:
	var response := greeter.cancel_session()
	if response is GreetdError:
		log_info(response.get_error_description())
	elif response is GreetdAuthMessage:
		log_info("Unexpected: %s - %s" % [response.get_auth_message_type(), response.get_auth_message()])
	else:
		log_info("Cancelled successfully")
