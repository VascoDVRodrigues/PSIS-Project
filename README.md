# Systems Programming Course - Final Project

Group 40

André Silva, 93014

Vasco Rodrigues, 93198

##Final Grade 18.5/20

Building
-----------

To compile all executables use the command

	make all

or if you prefer to compile a single executable:

	make KVS-localServer 	//compiles only the local server
	make KVS-authServer 	//compiles only the auth server
	make apps 		//compiles all apps
	make name_of_the_app 	//compiles only the app with the given name
	
Usage
-----

To run the servers just run the compiled executables. To create some groups automatically you can use the 
provided python script (you must have python3 installed) with

	python3 create_groups.py number_of_groups | ./KVS-localServer
	
where in number_of_groups you must put an integer representing how many groups you want to create

You can run your own client that must uses the KVS-lib, or you can run one of the provided apps

To run an interactive app where you can choose what to do from all the available options run

	./app_interactive

If you run this app you must respect all the scanfs or the app will crash and/or enter in a infinite loop.

When asked for a GroupID or a Secret you must type a string. When asked for an option you must write 2 string separated by a space. 
The first one represents the operation that will be made and the second the key associated to that operation.

If the option is put you will also be asked to input a value to store, here you can write whatever you want.

You can run an app that establishes a connection, makes a put of a big value (contact us if you recognize the value :)) and then gets that value with

	./app_big_put

You can run an app that establishes a connection, creates 2 threads and then each thread does some puts and gets with

	./app_many_put
	
You can run an app that creates 2 threads and then each thread establishes a connection, does some puts and gets and then closes the connection with

	./app_2_connects










