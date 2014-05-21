//----------------------------------------------------------------------------//
// Snippet-5-1 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	// store the NetworkInterface as soon as it is initialized
	else if (module->getName() == "Network") {
		network = (NetworkInterface*)module;
	}
//----------------------------------------------------------------------------//
// Snippet-5-1 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-5-2 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	// try to connect to network first command line argument is {hostname|IP}:port
	if (argc > 1) {
		printf("Trying to connect to %s\n", argv[1]);
		network->connect(argv[1]);
	}
	SystemCore::synchronize();   // synchronize both VEs
//----------------------------------------------------------------------------//
// Snippet-5-2 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-5-3 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	SystemCore::step();  //update the system core, needed for event handling
//----------------------------------------------------------------------------//
// Snippet-5-3 - END                                                          //
//----------------------------------------------------------------------------//

