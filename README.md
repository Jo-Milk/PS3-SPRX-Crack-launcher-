# PS3-SPRX-Crack-launcher-
PS3 .sprx how to bypass authentication using another .sprx

Why I put this on Github because this can be reproduced on other platforms and for other purposes. 
I already used this technic for dlls and executable.

Once you have full read write access to memory and a key to the program you want to bypass auth. 
All you need is to dump the executable from memory once running as desired and all the variables.
if the program hooks to another program you can use a executable to load the targeted executable and accomplish the hooks by only using your executable
Now on pc this is way more demanding but on ps3 most modders use same hooking technic and threading

So my executable loads the sprx in memory but doesn't execute it and then threads the threads it would and hooks where it would
I dumped the variables that the key downlaods from the server 
most devs do the following 

opd_s structure_name_t = { function_offset , TOC_of_the_game };

since the devs download the function_offset from their server it will be nulled with a reference to it and the TOC sitting next to it

ida
ref to [adresse contains] 0x00000000
       [adresse + 4]      TOC

Sneaky security might require more nops in the threads or hooks but it's easy to find sig for internet related function or imports to recv function and nop att recv


Hopefully this can be useful for more then just bypassing authentication of mod menus 

This method avoid using assembler to patch the program instead you use C++ to read write to the executable loaded in memory.

Although, using assembler patches (PPC in this case is way more stable) why? PS3 is very limited in memory usage compared to other platforms
You'd need to allocate more memory but the ps3 doesn't have much more. 
