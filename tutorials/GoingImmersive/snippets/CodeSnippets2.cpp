//----------------------------------------------------------------------------//
// Snippet-2-1 - BEGIN                                                        //
//----------------------------------------------------------------------------//
#ifdef WITH_VRPN_SUPPORT
#include <inVRs/tools/libraries/VrpnDevice/VrpnDevice.h>
#endif
#ifdef WITH_TRACKD_SUPPORT
#include <inVRs/tools/libraries/TrackdDevice/TrackdDevice.h>
#endif
//----------------------------------------------------------------------------//
// Snippet-2-1 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-2-2 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	void initInputInterfaceCallback(ModuleInterface* moduleInterface) {
#ifdef WITH_VRPN_SUPPORT
		if (moduleInterface->getName() == "ControllerManager") {
			ControllerManager* contInt = dynamic_cast<ControllerManager*>(moduleInterface);
			assert(contInt);
			contInt->registerInputDeviceFactory(new VrpnDeviceFactory);
		} // if
#endif
#ifdef WITH_TRACKD_SUPPORT
		if (moduleInterface->getName() == "ControllerManager") {
			ControllerManager* contInt = dynamic_cast<ControllerManager*>(moduleInterface);
			assert(contInt);
			contInt->registerInputDeviceFactory(new TrackdDeviceFactory);
		} // if
#endif
	} // initInterfaceCallback
//----------------------------------------------------------------------------//
// Snippet-2-2 - END                                                          //
//----------------------------------------------------------------------------//


