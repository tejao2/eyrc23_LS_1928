#python

def sysCall_init():
    # do some initialization here
    global bike, reference_frame, motor, roll_ini, x5, x6, prev_x2_err, prev_x1_err,x1,x2,x3,x4,drive_wheel, drive_speed
    
    bike = sim.getObjectHandle("bike_respondable")
    drive_wheel = sim.getObjectHandle("drive_motor")
    reference_frame = sim.getObjectHandle("reference_frame")
    motor = sim.getObjectHandle("front_motor")
    
    bike_orientation = sim.getObjectOrientation(bike, reference_frame)
    angle_xb, angle_yb, angle_zb = bike_orientation
    pitch_b, roll_b, yaw_b = sim.alphaBetaGammaToYawPitchRoll(angle_xb, angle_yb, angle_zb) 
    
    roll_ini = roll_b
    drive_speed = 0
    x5 = 0
    x6 = 0
    x1 = 0
    x2 = 0
    x3 = 0
    x4 = 0
    pass

def sysCall_actuation():
    # put your actuation code here
    #sim.setJointTargetVelocity(motor, -5)
    global x1, x2, x3, x4, x5, x6, drive_speed
    U = 103 * x4 - 0.035 * x2 + 1.82 * x6 - 0.000099 * x5 - 0.56 * x3 + 0.35 * x1 
    sim.setJointTargetVelocity(motor, -U)
    sim.setJointTargetVelocity(drive_wheel, drive_speed)
    
    pass

def sysCall_sensing():
    # put your sensing code here
    global yaw_setpoint, bike, reference_frame, motor, roll_ini, x5, x6, prev_x2_err, prev_x1_err, x1,x2,x3,x4,drive_speed

    
    
    bike_orientation = sim.getObjectOrientation(bike, reference_frame)
    angle_xb, angle_yb, angle_zb = bike_orientation
    pitch_b, roll_b, yaw_b = sim.alphaBetaGammaToYawPitchRoll(angle_xb, angle_yb, angle_zb) 
    
    
    ############### Keyboard Input ##############
    message,data,data2 = sim.getSimulatorMessage()
    
    if (message == sim.message_keypress):    
        if (data[0]==2007): # forward up arrow
            drive_speed = -5#add drive wheel speed here
        
        if (data[0]==2008): # backward down arrow
            drive_speed = 5#add drive wheel speed here
        
        if (data[0]==2009): # left arrow key
            yaw_setpoint = 0.436#change yaw_setpoint for required turning over here
                
        if (data[0]==2010): # right arrow key
            yaw_setpoint = -0.436#change yaw_setpoint for required turning over here
            
    else:
        drive_speed = 0# This is an example, decide what's best
        yaw_setpoint = yaw_b# # This is an example, decide what's best
    #########################################
    
    x2 = yaw_setpoint - yaw_b #(yaw_error)
    x4 = roll_ini - roll_b #(roll_error)
    x1 = 0-(x2/0.010) #(yaw_velocity_error)
    x3 = 0-(x4/0.010)#(roll_velocity_error)
    x5 = x5 + x2
    x6 = x6 + x4
     
    
    pass

def sysCall_cleanup():
    # do some clean-up here
    pass

# See the user manual or the available code snippets for additional callback functions and details
