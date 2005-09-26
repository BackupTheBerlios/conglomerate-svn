Ogre 1.0.5 Application Wizard v.1.0
------------------------------------------------------------------------------


Quick Install -----------------------------------------------------------------

1) Place the OgreWizard71 folder somewhere sensible.

2) Run Setup.js to install this Ogre Application Wizard!
(Double-click on it)

3) Remember: Do Not delete this folder - your Ogre Wizard needs the files.
The install script changes the ABSOLUTE_PATH in OgreWizard71.vsz to point to whereever you put the wizard files.


NOTE: This wizard depends upon you having setup Internet Explorer to accept cookies.



If you have problems with another program having registered with .js files, like Dreamweaver, try this:

[quote="jenocin"][quote="Kageetai"]I have a little problem with the installation of the wizard!

When I click on the setup.js-file, Macromedia Dreamweaver shows up. What program or interpreter do I need to run the script?[/quote]

try adding this to the js extension under run
[code]C:\WINDOWS\System32\WScript.exe "%1" %*[/code]

or from the run box

[code]C:\WINDOWS\System32\WScript.exe locationofsetupjs[/code]

I personally used the first adding a new command "run" as the default thus still able to right click and edit with dreamweaver.[/quote]


Costumizing  -------------------------------------------------------------------

It is easy to costumize the Application Wizard. It is completely script driven. And you don't need to re-install anything after changing it.
Look at script.js in ogrewizard71\Files\Scripts\1033 and the template files in ogrewizard71\Files\Templates.
Notice the macros -> [!output PROJECT_NAME], [!if CEGUI_YES], [!if FRAMEWORK_OWN], [!else], [!endif]. You get the idea. ;-))



enjoy!



jacmoe

jacmoe@users.berlios.de




Manual Install ------------------------------------------------------------------

1) Edit the OgreWizard71.vsz -> change "ABSOLUTE_PATH" to where you have unzipped this Ogre-wizard.
2) Copy the three "OgreWizard71.*" files over to your [Visual Studio]/vc7/vcprojects folder.
