free_will_cam = false
change_cam_mode_key = "TAB"
old_mouse_position = Vector2:new()
position = Vector3:new()
old_displacement = Vector3:new()
field_of_view = 45.0
orthographic = false
perspective = true
trans_pos_active = true
target_rotation = false
target_name = "Player"
old_y_movement = 0.0
camera_speed = 0.5
camera_rotation = 1.0
mouse_scroll = 0
original_camera_speed = 0.0
vp_info = Vector4:new()
cp_info = Vector2:new()
hdrExposure = 1.0
gamma = 2.2
bloom = false
rotate_speed = 0.02
maxLength = 10
cameraPivotSpeed = 1
minY = 32
maxY = 169

function Awake()
	controller = this:GetComponent("Controller")
	camera = this:GetComponent("Camera")
	transform = this:GetComponent("Transform")
end

function Initialize()
    if(trans_pos_active) then
		curr_pos = transform.Position
		SetPosition(curr_pos)
		camera:SetCameraPosition(curr_pos)
	end
	target = this:FindEntity(target_name)
	target_pos = target:GetComponent("Transform").Position
	camera:SetCameraTarget(target_pos)
	old_displacement.x = target_pos.x - position.x
	old_displacement.y = target_pos.y - position.y
	old_displacement.z = target_pos.z - position.z
	
	camera:SetFarDist(cp_info.y)
	camera:SetNearDist(cp_info.x)
	if (perspective) then
		camera:SetCameraIsPerspective()
	elseif (orthographic) then
		camera:SetCameraIsOrthographics()
	end
	original_camera_speed = camera_speed
end

function Update(dt)
	--[[if(controller:GetKeyDown(change_cam_mode_key)) then
		free_will_cam = not free_will_cam
		print("Camera Movement changed to ")
		if (free_will_cam) then
			print("Free-Will Camera")
		else
			print("Target-Based Camera")
		end
	end]]
	if(free_will_cam) then
		FreeWillCameraMovement()
	else
		TargetBasedCameraMovement()
	end
end

function FreeWillCameraMovement()
	scroll_amt = controller:GetScrollAmount()
	diff_scroll = scroll_amt - mouse_scroll
	
	mouse_y_movement = false
	if(controller:GetKeyDown("MMOUSE")) then
		mouse_y_movement = true
		old_y_movement = controller:GetMousePosition().y
	end
	if(controller:GetKeyHold("MMOUSE")) then
		mouse_y_movement = true
	end
	new_y_movement = controller:GetMousePosition().y
	if(controller:GetKeyDown("RMOUSE")) then
		old_mouse_position = controller:GetMousePosition()
	end
	if(controller:GetKeyDown("LCTRL")) then
		target_rotation = true
		print("Camera Rotation changed to Target-Based")
	end
	if(controller:GetKeyHold("LCTRL")) then
		target_rotation = true
	end
	if(controller:GetKeyUp("LCTRL")) then
		target_rotation = false
		print("Camera Rotation changed to Camera-Based")
	end
	
	pos = camera:GetCameraPosition()
	t_pos = camera:GetTargetPosition()
	up = camera:GetUp()
	front = Vector3:new()
	front.x = t_pos.x - pos.x
	front.y = t_pos.y - pos.y
	front.z = t_pos.z - pos.z
	left = Vector3:new()
	left.x = up.y * front.z - up.z * front.y
	left.y = up.z * front.x - up.x * front.z
	left.z = up.x * front.y - up.y * front.x
	
	if(controller:GetKeyHold("RMOUSE")) then
		mouse_move = Vector2:new()
		mouse_move.x = controller:GetMousePosition().x - old_mouse_position.x
		mouse_move.y = controller:GetMousePosition().y - old_mouse_position.y
		old_mouse_position = controller:GetMousePosition()
		if(mouse_move.x ~= 0) then
			front4 = Vector4:new()
			front4.x = front.x
			front4.y = front.y
			front4.z = front.z
			front4.w = 0
			x_rotated_front = Vector4:new()
			tmp = -1 * mouse_move.x * camera_rotation
			x_rotated_front = camera:M4Rotate(front4, tmp, up)
			if(target_rotation) then
				pos.x = t_pos.x - x_rotated_front.x
				pos.y = t_pos.y - x_rotated_front.y
				pos.z = t_pos.z - x_rotated_front.z
			else
				t_pos.x = pos.x + x_rotated_front.x
				t_pos.y = pos.y + x_rotated_front.y
				t_pos.z = pos.z + x_rotated_front.z
			end
			front.x = t_pos.x - pos.x
			front.y = t_pos.y - pos.y
			front.z = t_pos.z - pos.z
		end
		if(mouse_move.y ~= 0) then
			front4 = Vector4:new()
			front4.x = front.x
			front4.y = front.y
			front4.z = front.z
			front4.w = 0
			y_rotated_front = Vector4:new()
			tmp1 = mouse_move.y * camera_rotation
			tmp2 = Vector3:new()
			tmp2.x = left.x * -1
			tmp2.y = left.y * -1
			tmp2.z = left.z * -1
			y_rotated_front = camera:M4Rotate(front4, tmp1, tmp2)
			if(target_rotation) then
				pos.x = t_pos.x - y_rotated_front.x
				pos.y = t_pos.y - y_rotated_front.y
				pos.z = t_pos.z - y_rotated_front.z
			else
				t_pos.x = pos.x + y_rotated_front.x
				t_pos.y = pos.y + y_rotated_front.y
				t_pos.z = pos.z + y_rotated_front.z
			end
		end
	end
	front.x = t_pos.x - pos.x
	front.y = t_pos.y - pos.y
	front.z = t_pos.z - pos.z
	front = front:Unit()
	front.x = front.x * camera_speed
	front.y = front.y * camera_speed
	front.z = front.z * camera_speed
	left.x = up.y * front.z - up.z * front.y
	left.y = up.z * front.x - up.x * front.z
	left.z = up.x * front.y - up.y * front.x
	left.x = left.x * camera_speed
	left.y = left.y * camera_speed
	left.z = left.z * camera_speed
	upsized = Vector3:new()
	upsized.x = up.x * camera_speed
	upsized.y = up.y * camera_speed
	upsized.z = up.z * camera_speed
	
	--[[if(controller:GetKeyHold("A")) then
		pos.x = pos.x + left.x
		pos.y = pos.y + left.y
		pos.z = pos.z + left.z
		t_pos.x = t_pos.x + left.x
		t_pos.y = t_pos.y + left.y
		t_pos.z = t_pos.z + left.z
	end
	if(controller:GetKeyHold("D")) then
		pos.x = pos.x - left.x
		pos.y = pos.y - left.y
		pos.z = pos.z - left.z
		t_pos.x = t_pos.x - left.x
		t_pos.y = t_pos.y - left.y
		t_pos.z = t_pos.z - left.z
	end
	if(controller:GetKeyHold("W")) then
		pos.x = pos.x + front.x
		pos.y = pos.y + front.y
		pos.z = pos.z + front.z
		t_pos.x = t_pos.x + front.x
		t_pos.y = t_pos.y + front.y
		t_pos.z = t_pos.z + front.z
	elseif(diff_scroll > 0) then
		pos.x = pos.x + front.x * 3
		pos.y = pos.y + front.y * 3
		pos.z = pos.z + front.z * 3
		t_pos.x = t_pos.x + front.x * 3
		t_pos.y = t_pos.y + front.y * 3
		t_pos.z = t_pos.z + front.z * 3
	end
	if(controller:GetKeyHold("S")) then
		pos.x = pos.x - front.x
		pos.y = pos.y - front.y
		pos.z = pos.z - front.z
		t_pos.x = t_pos.x - front.x
		t_pos.y = t_pos.y - front.y
		t_pos.z = t_pos.z - front.z
	elseif(diff_scroll < 0) then
		pos.x = pos.x - front.x * 3
		pos.y = pos.y - front.y * 3
		pos.z = pos.z - front.z * 3
		t_pos.x = t_pos.x - front.x * 3
		t_pos.y = t_pos.y - front.y * 3
		t_pos.z = t_pos.z - front.z * 3
	end]]--
	if(controller:GetKeyHold("Q") or (mouse_y_movement and new_y_movement - old_y_movement > 0)) then
		pos.x = pos.x + upsized.x
		pos.y = pos.y + upsized.y
		pos.z = pos.z + upsized.z
		t_pos.x = t_pos.x + upsized.x
		t_pos.y = t_pos.y + upsized.y
		t_pos.z = t_pos.z + upsized.z
	end
	if(controller:GetKeyHold("E") or (mouse_y_movement and new_y_movement - old_y_movement < 0)) then
		pos.x = pos.x - upsized.x
		pos.y = pos.y - upsized.y
		pos.z = pos.z - upsized.z
		t_pos.x = t_pos.x - upsized.x
		t_pos.y = t_pos.y - upsized.y
		t_pos.z = t_pos.z - upsized.z
	end
	if(controller:GetKeyHold("LSHIFT")) then
		camera_speed = 5
	end
	if(controller:GetKeyUp("LSHIFT")) then
		camera_speed = original_camera_speed
	end
	
	camera:SetCameraPosition(pos)
	SetPosition(pos)
	camera:SetCameraTarget(t_pos)
	mouse_scroll = scroll_amt
end

function TargetBasedCameraMovement()
	rotated = false
	
	if(controller:GetKeyDown("RMOUSE")) then
		old_mouse_position = controller:GetMousePosition()
	end
	
	pos = camera:GetCameraPosition()
	t_pos = target:GetComponent("Transform").Position
	up = camera:GetUp()
	front = Vector3:new()
	front.x = t_pos.x - pos.x
	front.y = t_pos.y - pos.y
	front.z = t_pos.z - pos.z
	front:Normalize()
	left = Vector3:new()
	left.x = up.y * front.z - up.z * front.y
	left.y = up.z * front.x - up.x * front.z
	left.z = up.x * front.y - up.y * front.x
	
	mouse_rot = Vector3:new()
	front_rot = front
	
	rotate_speed = 0.02
	mouse_move = Vector2:new()
	mouse_move.x = (controller:GetMousePosition().x - old_mouse_position.x) * cameraPivotSpeed
	mouse_move.y = (controller:GetMousePosition().y - old_mouse_position.y) * cameraPivotSpeed
	old_mouse_position = controller:GetMousePosition()
	if(mouse_move.x ~= 0) then
		front4 = Vector4:new()
		front4.x = front_rot.x
		front4.y = front_rot.y
		front4.z = front_rot.z
		front4.w = 0
		front:Normalize()
		x_rotated_front = Vector4:new()
		tmp = mouse_move.x * -3 * camera_rotation
		x_rotated_front = camera:M4Rotate(front4, tmp, up)
		mouse_rot.x = x_rotated_front.x
		mouse_rot.y = x_rotated_front.y
		mouse_rot.z = x_rotated_front.z
		front_rot = mouse_rot
		rotated = true
	end
	if(mouse_move.y ~= 0) then
		front4 = Vector4:new()
		front4.x = front_rot.x
		front4.y = front_rot.y
		front4.z = front_rot.z
		front4.w = 0
		y_rotated_front = Vector4:new()
		tmp1 = -mouse_move.y * camera_rotation
		tmp2 = Vector3:new()
		tmp2.x = left.x
		tmp2.y = left.y
		tmp2.z = left.z
		y_rotated_front = camera:M4Rotate(front4, tmp1, tmp2)
		mouse_rot.x = y_rotated_front.x
		mouse_rot.y = y_rotated_front.y
		mouse_rot.z = y_rotated_front.z
		rotated = true
	end
	
	displacement = Vector3:new()
	displacement.x = t_pos.x - pos.x
	displacement.y = t_pos.y - pos.y
	displacement.z = t_pos.z - pos.z
	length = displacement:Length()
	diff_disp = Vector3:new()
	diff_disp.x = displacement.x - old_displacement.x
	diff_disp.y = displacement.y - old_displacement.y
	diff_disp.z = displacement.z - old_displacement.z
	pos.x = pos.x + diff_disp.x
	pos.y = pos.y + diff_disp.y
	pos.z = pos.z + diff_disp.z
	
	if(rotated) then
		vec = mouse_rot:Unit()
		if(length ~= maxLength) then
			length = maxLength
		end
		vec = vec * length
		vec.x = t_pos.x - vec.x
		vec.y = t_pos.y - vec.y
		vec.z = t_pos.z - vec.z
		vec.x = vec.x - pos.x
		vec.y = vec.y - pos.y
		vec.z = vec.z - pos.z
		pos.x = pos.x + vec.x
		newY = pos.y + vec.y
		if(newY > minY and newY < maxY) then
			pos.y = pos.y + vec.y
		end
		pos.z = pos.z + vec.z
	end
	
	--camera:SetCameraPosition(pos)
	SetPosition(pos)
	camera:SetCameraTarget(t_pos)
	old_displacement.x = t_pos.x - pos.x
	old_displacement.y = t_pos.y - pos.y
	old_displacement.z = t_pos.z - pos.z
end

function SetPosition(pos)
	if(trans_pos_active) then
		transform:Set_Position(pos)
	end
	position = pos
end

function SetTarget(name)
	target = this:FindEntity(name)
end

function SetViewportInfo(xy, width, height)
	vp_info.x = xy.x
	vp_info.y = xy.y
	vp_info.z = width
	vp_info.w = height
end

function SetClippingPlaneInfo(near, far)
	cp_info.x = near
	cp_info.y = far
end

function SetFieldOfView(fov)
	field_of_view = fov
end


