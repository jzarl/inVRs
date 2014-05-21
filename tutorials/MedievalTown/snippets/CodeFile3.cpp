//----------------------------------------------------------------------------//
// Snippet-3-1 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	// generate or load and configure height maps of the used tiles
	HeightMapManager::generateTileHeightMaps();
//----------------------------------------------------------------------------//
// Snippet-3-1 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-3-2 - BEGIN                                                        //
//----------------------------------------------------------------------------//
void initCoreComponents(CoreComponents comp) {
	// register factory for HeightMapModifier as soon as the
	// TransformationManager is initialized
	if (comp == TRANSFORMATIONMANAGER) {
		TransformationManager::registerModifierFactory
			(new HeightMapModifierFactory());
	// register factory for CheckCollisionModifier
		TransformationManager::registerModifierFactory
			(new CheckCollisionModifierFactory);
	}
}
//----------------------------------------------------------------------------//
// Snippet-3-2 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-3-3 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	SystemCore::registerCoreComponentInitCallback(initCoreComponents);
//----------------------------------------------------------------------------//
// Snippet-3-3 - END                                                          //
//----------------------------------------------------------------------------//
