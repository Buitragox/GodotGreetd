extends Button

@onready var username_input := %Username
@onready var password_input := %Password
@onready var alert_label := %Alert
@onready var log_label := %Log
var greeter := Greeter.new()

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pressed.connect(_button_pressed)

func _button_pressed() -> void:
	var username: String = username_input.text
	var password: String = password_input.text
	var response := greeter.create_session(username)
	
	if response is GreetdError:
		alert_label.text = response.get_error_description()
		log_info(response.get_error_description())
		return
	elif response is GreetdAuthMessage:
		log_info("%s - %s" % [response.get_auth_message_type(), response.get_auth_message()])
	else:
		log_info("Session created")
	 
	response = greeter.answer_auth_message(password)
	if response is GreetdError:
		alert_label.text = response.get_error_description()
		log_info(response.get_error_description())
		return
	elif response is GreetdAuthMessage:
		log_info("Unexpected: %s - %s" % [response.get_auth_message_type(), response.get_auth_message()])
		return
	else:
		log_info("Auth answered")
		
	response = greeter.start_session()
	if response is GreetdError:
		alert_label.text = response.get_error_description()
		log_info(response.get_error_description())
		return
	elif response is GreetdAuthMessage:
		log_info("Unexpected: %s - %s" % [response.get_auth_message_type(), response.get_auth_message()])
		return
	else:
		log_info("Session started")
	
		
	alert_label.text = "Success :D. Exiting in 3 seconds"
	await get_tree().create_timer(3.0).timeout
	get_tree().quit()
	

func log_info(text: String) -> void:
	log_label.add_text(text + "\n")
	
	
	
	
