extends Button

@onready var username_input := %Username
@onready var password_input := %Password
@onready var alert_label := %Alert
var greeter := Greeter.new()

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pressed.connect(_button_pressed)

func _button_pressed():
	var username: String = username_input.text
	var password: String = password_input.text
	var response := greeter.create_session(username)
	
	if response is GreetdError:
		alert_label.text = response.get_error_description()
		return
	
	# TODO: Need to handle success
	 
	response = greeter.answer_auth_message(password)
	if response is GreetdError:
		alert_label.text = response.get_error_description()
		return
		
	alert_label.text = "Success :D. Exiting in 3 seconds"
	await get_tree().create_timer(3.0).timeout
	get_tree().quit()
	
