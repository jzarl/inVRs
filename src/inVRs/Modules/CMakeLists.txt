# ADD SEPARATE LINE FOR EACH MODULE WHICH SHOULD BE BUILT

# Build Network module
add_subdirectory (Network)

# Build Navigation module
add_subdirectory (Navigation)

# Build Interaction module
add_subdirectory (Interaction)

# Build 2DPhysics module
autofeature(2DPhysics INVRS_ENABLE_MODULE_2DPHYSICS "Enable/disable compilation of the 2DPhysics module")
if ( INVRS_ENABLE_MODULE_2DPHYSICS )
	add_subdirectory (2DPhysics)
endif ( INVRS_ENABLE_MODULE_2DPHYSICS )

autofeature(3DPhysics INVRS_ENABLE_MODULE_3DPHYSICS "Enable/disable compilation of the 3DPhysics module"
	REQUIRED_PACKAGES ODE
	REQUIRED_CONDITIONS ODE_SOURCE_DIR)
if ( INVRS_ENABLE_MODULE_3DPHYSICS )
	# Build 3DPhysics module
	add_subdirectory (3DPhysics)
endif ( INVRS_ENABLE_MODULE_3DPHYSICS )

# Build JointInteraction module
autofeature(JointInteraction INVRS_ENABLE_MODULE_JOINTINTERACTION "Enable/disable compilation of the JointInteraction module"
	REQUIRED_PACKAGES ODE)
if ( INVRS_ENABLE_MODULE_JOINTINTERACTION )
	add_subdirectory (JointInteraction)
endif ( INVRS_ENABLE_MODULE_JOINTINTERACTION )

