#python

def sysCall_init():
    # do some initialization here
    global bike, reference_frame, motor, roll_ini, x5, x6, prev_x2_err, prev_x1_err,x1,x2,x3,x4
    bike = sim.getObjectHandle("bike_respondable")
    reference_frame = sim.getObjectHandle("reference_frame")
    motor = sim.getObjectHandle("front_motor")
    prev_x2_err = 0
    prev_x1_err = 0
    bike_orientation = sim.getObjectOrientation(bike, reference_frame)
    angle_xb, angle_yb, angle_zb = bike_orientation
    pitch_b, roll_b, yaw_b = sim.alphaBetaGammaToYawPitchRoll(angle_xb, angle_yb, angle_zb) 
    roll_ini = roll_b
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
    global x1, x2, x3, x4, x5, x6
    U = 103 * x4 - 0.035 * x2 + 1.82 * x6 - 0.000099 * x5 - 0.59 * x3 + 0.36 * x1 
    sim.setJointTargetVelocity(motor, -U)
    pass

def sysCall_sensing():
    # put your sensing code here
    global yaw_setpoint, bike, reference_frame, motor, roll_ini, x5, x6, prev_x2_err, prev_x1_err, x1,x2,x3,x4
    yaw_setpoint = sim.getFloatSignal("yaw_setpoint")
    #count = count+1
    #print(str(yaw_setpoint) + " " + str(count))
    bike_orientation = sim.getObjectOrientation(bike, reference_frame)
    angle_xb, angle_yb, angle_zb = bike_orientation
    pitch_b, roll_b, yaw_b = sim.alphaBetaGammaToYawPitchRoll(angle_xb, angle_yb, angle_zb) 
    
    #print(str(roll_b) + " " + str(yaw_b))
    ##our_point = yaw_ setpoint/15
    #if ref_point != yaw_setpoint:
        
   
    x2 = yaw_setpoint - angle_zb #(yaw_error)
    #x2 = -0.523599 - angle_zb 
    x4 = roll_ini - roll_b #(roll_error)
    #x1 = 0 - ((x2-prev_x2_err)/0.010) #(yaw_velocity_error)
    #x3 = 0 - ((x1-prev_x1_err)/0.010) #(roll_velocity_error)
    x1 = 0-(x2/0.010)
    x3 = 0-(x4/0.010)
    x5 = x5 + x2
    x6 = x6 + x4
    
    prev_x1_err = x1
    prev_x2_err = x2
    #print(str(roll_ini) + " " + str(roll_b))
    #U = 25 * x4 #- 0.0008 * x5 - 0.008 * x2 - 0.0002 * x3
    #U = 25 * x4 + 0.00000005 * x3 + 0.0008 * x2 + 0.00008 * x1 + 0.00000005 * x5
    #U = 25 * x4 + 0.0000005 * x2 - 0.00000005 * x3
    #U = 38 * x4 - 0.05 * x5 - 0.5 * x2
    #--U = 200 * x4 - 0.5 * x2  - 0.0003 * x1 - 0.005 * x3 + 0.00005 * x5 
    #U = 60 * x4 - 0.0005 *x2
    #U = 255 * x4 - 10 * x2 - 0.5 * x1  - 0.004 * x3 - 0.0005 * x5
    #U = 103 * x4 - 0.0508 * x2 + 1.8 * x6 - 0.000099 * x5 - 0.56 * x3 + 0.32 * x1 
    #oooooU = 103 * x4 - 0.035 * x2 + 1.82 * x6 - 0.000099 * x5 - 0.59 * x3 + 0.36 * x1 
    #U =  20* x4 - 0.5* x3
    #sim.setJointTargetVelocity(motor, -U)
    #if x2<0.00:
        
     #sim.setJointTargetVelocity(motor, -U)
    #if x2 ==0:
        #c = 10 * x4
        #sim.setJointTargetVelocity(motor, c)
        #set_time = 0.90
        #c = 22 * x4
        #while set_time>0:
            #sim.setJointTargetVelocity(motor, c)
            #set_time = set_time-0.010
            #print(1) 
            
            
               
        
    
        
      
    print(str(yaw_setpoint) + " " + str(x2))
    
    
    pass

def sysCall_cleanup():
    # do some clean-up here
    pass

# See the user manual or the available code snippets for additional callback functions and details
