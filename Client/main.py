import pygame
import time

class bcolors:
    HEADER = '\033[95m' # magenta
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m' + '[WARNING] '
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def init_controller():
    print(bcolors.BOLD + "Initialize Joystick" + bcolors.ENDC)

    pygame.joystick.quit()
    pygame.joystick.init()
    num_of_devides = pygame.joystick.get_count()

    joystick = pygame.joystick

    if num_of_devides == 1:
        joystick = pygame.joystick.Joystick(0)

    elif num_of_devides > 1:
        joysticks = [pygame.joystick.Joystick(x) for x in range(num_of_devides)]
        print(bcolors.OKCYAN + "[INFO]" + bcolors.ENDC + f" {len(joysticks)} joysticks detected, select one:")
        joystick_num = 1

        for joystick in joysticks:
            print("  ●  " + bcolors.HEADER + f"{joystick_num}" + bcolors.ENDC + " to select " + bcolors.BOLD +  f"{joystick.get_name()}" + bcolors.ENDC + ".")
            joystick_num = joystick_num + 1
        
        try:
            selected_joystick = int(input())-1
            while(selected_joystick > len(joysticks) - 1 or selected_joystick <= 0):
                if selected_joystick > len(joysticks)-1 or selected_joystick <= 0:
                    print(bcolors.FAIL + "[ERROR]" + bcolors.ENDC + f" Joystick number \"{selected_joystick + 1}\" out of range.")
                selected_joystick = int(input())-1
            
            joystick = pygame.joystick.Joystick(selected_joystick)

        except ValueError:
            print(bcolors.FAIL + "[ERROR]" + bcolors.ENDC + " This is not a joystick number.")
            init_controller()
        
    elif num_of_devides == 0: 
        print(bcolors.WARNING + "No joysticks available!" + bcolors.ENDC + "\nWaiting 5 seconds for joystick.")
        time.sleep(5)
        init_controller()

    print(bcolors.OKCYAN + "[INFO] " + bcolors.ENDC + f"Joystick " + bcolors.BOLD + f"{joystick.get_name()}" + bcolors.ENDC + " is selected." )

    return joystick

def main():
    print(bcolors.ENDC)
    joystick = init_controller()


if __name__ == "__main__":
    main()