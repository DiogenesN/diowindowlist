# DioWindowList
A simple application which shows you the currently opened windows.
Currently it only works on Xorg.
It was tested on Debian 12.

# What you can do with DioWindowList
   1. Shows you a list of opened windows.
   2. Raise on click.

# Installation/Usage
  1. Open a terminal and run:

		 ./configure

  2. if all went well then run:

		 make
		 sudo make install
		 
		 (if you just want to test it then run: make run)
		
  4. Run the application:
  
		 diowindowlist

# Screenshots
 
![Alt text](https://raw.githubusercontent.com/DiogenesN/diowindowlist/main/diowindowlist.png)

That's it!

 Make sure you have the following packages installed:

		make
		gawk
		wmctrl
		pkgconf
		coreutils
		libgtk-4-dev

# Support

   My Libera IRC support channel: #linuxfriends
   
   Email: nicolas.dio@protonmail.com
