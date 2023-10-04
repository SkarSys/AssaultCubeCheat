# AssaultCubeCheat
## How it works:
- Gets game base address using game module base and PID
- (GetWindowThreadProcessId **)
- Gets handle to window using FindWindowA
- Gets static pointer address using the memory address and the offset(s)
- Gets entity list using commen entity address and loops through the array (4 bytes apart)
- Reads player health and x, y, z coordinate using the entity list pointer and the offset and displays it live
- Hack loop with hot-key type menu (f1,f2,etc..)

  ## Fetures:
  - Get players in entity list (including self) health, and 3d coords
  - World2Screen (needs fix cus i fucked up and deleted half of it but yh)
  - Teleport to nearest entity (in list) --> Key: F1
  - Teleport to second nearest entity (in list) --> Key: F2
  - Teleport to third nearest entity (in list) --> Key: F3
  - Nearst entity in list teleports to you --> F4
  - Second Nearst entity in list teleports to you --> F5
  - Third Nearst entity in list teleports to you --> F6
  - Magic walk (threw walls) using my own walk logic by writing your players coords
  - numpad 8 magic forward, numpad 8 backwords, numpad 4 side
  - SuperJump: write players jump hight to 8 meteres: Space bar
 
## Read This:
- I made this code some months ago, it is not very organized and stuff cus i did it fast and yeah i could have done better so excuse it please, hope this is usfull to somone as a POC. 
