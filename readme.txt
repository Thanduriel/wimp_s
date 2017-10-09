#####################################################################
    _    _  _                                
   | |  | |(_)                               
   | |  | | _  _ __ ___   _ __           ___ 
   | |/\| || || '_ ` _ \ | '_ \         / __|
   \  /\  /| || | | | | || |_) |        \__ \
    \/  \/ |_||_| |_| |_|| .__/         |___/
                         | |     ______      
                         |_|    |______|     
#####################################################################

This readme aims to fill gaps that are not sufficiently explained ingame.

SYSTEM REQUIREMENTS #####################
opengl 4.0 compatible graphics card
56mb free space
windows 10

SETUP ###################################
In config.json you find a options regarding key bindings and Graphics.
To bind an action to a specific key lookup the corresponding number here: 
keyboard - http://www.glfw.org/docs/latest/group__keys.html
mouse - http://www.glfw.org/docs/latest/group__buttons.html
Using array syntax multiple keys can be bound to the same action.
example: "Fire":[0,1]
  This sets both mouse buttons to fire.

The render-windows resolution can be freely choosen,
but should not be larger then your screen resolution.
Recommended are 1920x1080 or 2560x2440 and fullscreen.

CONTROLS ################################
AccForward [W]          - temporarly accelerate
AccBackward [S]
AdjustTargetAcc[Shift]  - hold in combination with AccForward/AccBackward
                          to change the target speed that will be held
Brake[Tab]              - Set the target speed to zero
Fire[LeftMouse]         - fires all weapons targeting the ray of the cursor
RollCW[Q]               - rolls the ship around the forward axis
RollCCW[E]
Inventory [I]           - opens/closes the inventory
SwitchTactical[Space]   - once unlocked switches to the tactical targeting
                          mode that allows usage of special weapons
Pause[P]                - opens the pause menu

MECHANICS ##############################
Hud elements
The small bars on the right from top to bottom:
energy - required to fire weapons
shield - protects the health pool and regenerates after not beeing hit for some time
health - structural integrity of the ship, you loose when this falls to zero

blackhole
In tactical mode the camera can be moved using WASD and the mouse wheel.
A blackhole can then be placed with the fire action.
The mode can only be entered while the red indicator in the lower center 
of the screen is visible.

rocket launchers
One type of weapon fires homing and accelerating missiles.
The target is always the last ship that was focused in the center of the screen.
When something is in the focus the two arrows go outwards.



If you are having trouble or just want to mess around press F5 to generate weapons
out of thin air!