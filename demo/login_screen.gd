extends Node

const CONFIG_PATH = "user://config.cfg"
var greeter := GreetdGreeter.new()
var config := ConfigFile.new()

func _ready() -> void:
	var err := config.load(CONFIG_PATH)
	if err != Error.OK:
		printerr("Failed to open config.cfg")
		
	print(config.encode_to_text())
		
	_init_sessions()
	_init_users()
	
	%Password.text_submitted.connect(_on_password_submitted)
	%LogIn.pressed.connect(login)
	%SessionSelect.item_selected.connect(_on_session_selected)
	%Password.grab_focus()
	get_tree().set_auto_accept_quit(false) # Handle quit manually


func _init_users():
	var last_user = config.get_value("General", "last_user", "")
	
	var users := greeter.get_users()
	for user in users:
		%Username.add_item(user)
		if user == last_user:
			%Username.select(-1)


func _init_sessions():
	var last_session = config.get_value("General", "last_session", "")
	
	var sessions := greeter.get_wayland_sessions()
	var index = 0
	for session in sessions:
		%SessionSelect.add_item(session["Name"])
		%SessionSelect.set_item_metadata(index, session["Exec"])
		index += 1
		if session["Name"] == last_session:
			%SessionSelect.select(%SessionSelect.item_count - 1)


func _on_session_selected(_index: int):
	var session = %SessionSelect.text
	config.set_value("General", "last_session", session)


func _on_user_selected(_index: int):
	var user = %Username.text
	config.set_value("General", "last_user", user)


func _on_password_submitted(_password):
	login()
	

func _notification(what: int) -> void:
	if what == NOTIFICATION_WM_CLOSE_REQUEST:
		_quit()


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
	
	var cmd = %SessionSelect.get_selected_metadata()
	response = greeter.start_session(cmd)
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
	
	if OS.has_feature("editor"):
		%Alert.text = "Success :D Exiting in 3 seconds"
		await get_tree().create_timer(3.0).timeout
	
	_quit()


func _quit() -> void:
	config.save(CONFIG_PATH)
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
