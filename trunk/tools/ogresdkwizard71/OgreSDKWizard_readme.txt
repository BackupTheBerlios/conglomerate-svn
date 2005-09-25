Ogre 1.0.1 Application Wizard v.0.13
------------------------------------------------------------------------------


Quick Install -----------------------------------------------------------------

1) Place the OgreSDKWizard71 folder somewhere sensible.

2) Run Setup.js to install this Ogre SDK Application Wizard!
(Double-click on it)

3) Remember: Do Not delete this folder - your Ogre Wizard needs the files.
The install script changes the ABSOLUTE_PATH in OgreSDKWizard71.vsz to point to whereever you put the wizard files.



Costumizing  -------------------------------------------------------------------

It is easy to costumize the Application Wizard. It is completely script driven. And you don't need to re-install anything after changing it.
Look at script.js in ogresdkwizard71\Files\Scripts\1033 and the template files in ogresdkwizard71\Files\Templates.
Notice the macros -> [!output PROJECT_NAME], [!if CEGUI_YES], [!if FRAMEWORK_OWN], [!else], [!endif]. You get the idea. ;-))



enjoy!



jacmoe

jacmoe@users.berlios.de




Manual Install ------------------------------------------------------------------

1) Edit the OgreSDKWizard71.vsz -> change "ABSOLUTE_PATH" to where you have unzipped this Ogre-wizard.
2) Copy the three "OgreSDKWizard71.*" files over to your [Visual Studio]/vc7/vcprojects folder.
