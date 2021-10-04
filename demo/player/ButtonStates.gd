extends CanvasLayer

var controller : XRController3D

func set_controller(new_controller: XRController3D):
	controller = new_controller

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if controller:
		var primary = controller.get_axis("primary")
		$VBoxContainer/JoyX/Value.value = 100.0 * primary.x
		$VBoxContainer/JoyY/Value.value = 100.0 * primary.y
		
		var trigger = controller.get_value("trigger_value")
		$VBoxContainer/Trigger/Value.value = 100.0 * trigger
		$VBoxContainer/Trigger/Pressed.pressed = controller.is_button_pressed("trigger_click")

		var grip = controller.get_value("grip_value")
		$VBoxContainer/Grip/Value.value = 100.0 * grip
		$VBoxContainer/Grip/Pressed.pressed = controller.is_button_pressed("grip_click")
		$VBoxContainer/AX/Pressed.pressed = controller.is_button_pressed("ax")
		$VBoxContainer/BY/Pressed.pressed = controller.is_button_pressed("by")
		
