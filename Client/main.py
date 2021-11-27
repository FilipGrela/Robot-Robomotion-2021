import pygame
import time
import json
import socket

UDP_IP = "192.168.4.22"
UDP_PORT = 8001

ID_SPEED_AXIS =  1 # left axis (dualshock 4) -1 = full speed
ID_TURN_AXIS = 2 # right axis (dualshock 4) -1 = full left
ID_ATTACK_AXIS = 5 # right trigger (dualshock 4)

class bcolors:
    HEADER = '\033[95m' # magenta
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def init_controller():
    print(bcolors.BOLD + "Initialize Joystick..." + bcolors.ENDC)

    pygame.joystick.init()
    num_of_devides = pygame.joystick.get_count()

    joystick = pygame.joystick

    if num_of_devides == 1:
        joystick = pygame.joystick.Joystick(0)
        joystick.init()

    elif num_of_devides > 1:
        joysticks = [pygame.joystick.Joystick(x) for x in range(num_of_devides)]
        print(bcolors.OKCYAN + "[INFO] " + bcolors.ENDC + f" {len(joysticks)}joysticks detected, select one:")
        joystick_num = 1

        for joystick in joysticks:
            print("  ●  " + bcolors.HEADER + f"{joystick_num}" + bcolors.ENDC + " to select " + bcolors.BOLD +  f"{joystick.get_name()}" + bcolors.ENDC + ".")
            joystick_num = joystick_num + 1
        
        try:
            selected_joystick = int(input())-1
            while(selected_joystick > len(joysticks) - 1 or selected_joystick < 0):
                if selected_joystick > len(joysticks)-1 or selected_joystick < 0:
                    print(bcolors.FAIL + "[ERROR] " + bcolors.ENDC + f"Joystick number \"{selected_joystick + 1}\" is out of range.")
                selected_joystick = int(input())-1
            
            joystick = pygame.joystick.Joystick(selected_joystick)
            joystick.init()

        except ValueError:
            print(bcolors.FAIL + "[ERROR] " + bcolors.ENDC + "This is not a joystick number.")
            init_controller()
        
    elif num_of_devides == 0: 
        print(bcolors.WARNING + "[WARNING] " + "No joysticks available!" + bcolors.ENDC + "\nWaiting 5 seconds for joystick.")
        time.sleep(5)
        init_controller()

    print(bcolors.OKCYAN + "[INFO] " + bcolors.ENDC + f"Joystick " + bcolors.BOLD + f"{joystick.get_name()}" + bcolors.ENDC + " is selected." )

    
    return joystick

def main():
    print(bcolors.ENDC)

    pygame.init()

    clock = pygame.time.Clock()
    joystick = init_controller()

    joystick_data = update_joystick_data(joystick)
    
    done = False
    while not done:
        for event in pygame.event.get():
            if event.type == pygame.JOYAXISMOTION:
                joystick_data = update_joystick_data(joystick)
                calculate_moror_speed(joystick_data)


def calculate_moror_speed(joystick_data):
    send_data(joystick_data)

def update_joystick_data(joystick):
    if abs(joystick.get_axis(ID_SPEED_AXIS)) > 0.15:
        speed_axis = joystick.get_axis(ID_SPEED_AXIS)
    else:
        speed_axis = 0

    if abs(joystick.get_axis(ID_TURN_AXIS)) > 0.15:
        turn_axis = joystick.get_axis(ID_TURN_AXIS)
    else:
        turn_axis = 0
    
    if abs(joystick.get_axis(ID_ATTACK_AXIS)) > -0.90:
        attack_axis = joystick.get_axis(ID_ATTACK_AXIS)
    else:
        attack_axis = 0

    joystick_data = {"speed_axis": speed_axis,
                     "turn_axis": turn_axis,
                     "attack_axis": attack_axis
                     }
    return joystick_data

def send_json_data(msg):
    send_data(str(json.dumps(msg, sort_keys=True, separators=(',', ':'))))

def send_data(msg):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.sendto(str(msg).encode("utf-8"), (UDP_IP, UDP_PORT))
    print("Msg " + bcolors.UNDERLINE + f"{msg}" + bcolors.ENDC + " send")

if __name__ == "__main__":
    main()