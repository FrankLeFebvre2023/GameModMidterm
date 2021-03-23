# GameModMidterm
Quake 4

In this project, I created a mod for Quake 4 that attempted to replicate the key features of several Resident Evil games.

Feature 1: Alter the enemy AI in order to make them behave more "zombie-like"
-Strogg Marine enemy:
    -Removed ranged attack tactical states
    -Removed evasion tactical states (average zombies don't evade attacks)
    -Removed "hide" tactical state
    -Now behaves more like a zombie:
        -Aggressive, rushes at the player always
        -Only uses melee attacks
-Grunt Enemy:
    -Removed chaingun attack
    -Enrages more frequently
-Berserker Enemy:
    -Removed beam attack
    -Removed Ranged Tactical state
    -Kept electric ground attack (if I got rid of this it would just be the same as the grunt, needed variation for enemies)
    -Kept evade actions and tactical states (berserkers are smarter, like tyrants from Resident Evil)
-Gunner Enemy:
    -Removed Ranged tactical states
    -Removed nailgun attack
    -Kept Grenade launcher attack (reminiscient of Nemesis and his rocket launcher)
    
Feature 2: Make weapons similar to Resident Evil weapons
Blaster (work in progress):
    -Remove infinite ammo feature (work in progress, surprisingly troublesome)
    -Remove charged alternate fire attack
    -Change bullet effects to better fit a "modern" pistol
Grenade Launcher:
    -Grenades now explode on impact
    -Magazine size changed to 1 (turned into breech-loaded launcher)
Rocket Launcher:
    -Starting ammo set to 1
    -Damage increased by a factor of 10 (basically the one-shot killing machine from Resident Evil 4)
Hyperblaster:
    -Effects changed to be like the machine gun
    -Ammo Regeneration disabled
    -Bullet Ricochet disabled
    -Ammo capacity increased
    -Damage decreased
    -Projectiles changed to hitscan
Nailgun:
    -Guided projectiles disabled
Railgun:
    -Repurposed as a sniper rifle
    -Effects changed so that the bullet looks and souns like a sniper shot

Feature 3: Resident Evil third person camera
-Camera is in third person by default
-HUD is drawn in third person
-Targeting HUD is drawn in third person


General Features Added:
-Main Menu changed to reflect the theme of the mod
-Shortcut to open mod by default when launching game
-No additional errors created
-All gui and definition files packaged in a .pk4 file



Failures:

Unable to make significant progress on the npc vendor and the gridded inventory system.
    
