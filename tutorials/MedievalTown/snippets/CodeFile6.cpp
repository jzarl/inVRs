//----------------------------------------------------------------------------//
// Snippet-6-1 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	if (controller->getButtonValue(2)) { // the right mouse button is pressed
		windMillSpeed += dt*0.5;	         // increase speed of the windmill
		if (windMillSpeed > 2*M_PI) {
			windMillSpeed = 2*M_PI;
		}
	} else if (windMillSpeed > 0) {	     // pressing mouse button stopped
		windMillSpeed -= dt*0.5;	         // decrease speed of windmill
	} else if (windMillSpeed < 0) {
		windMillSpeed = 0;
	} 

	if (windMillSpeed > 0) {	          // rotate sails
		// retrieve the windmill entity
		Entity* windMill = WorldDatabase::getEntityWithEnvironmentId(1, 27);
		ModelInterface* windMillModel = windMill->getVisualRepresentation();

		// retrieve the windmill's sails
		SceneGraphNodeInterface* sceneGraphNode =
			windMillModel->getSubNodeByName("Sails");

		// make sure this node is a transformation node
		assert(sceneGraphNode->getNodeType() ==
			SceneGraphNodeInterface::TRANSFORMATION_NODE);
		TransformationSceneGraphNodeInterface* transNode =
			dynamic_cast<TransformationSceneGraphNodeInterface*>(sceneGraphNode);
		assert(transNode);

		// rotate the sails
		TransformationData trans = transNode->getTransformation();
		gmtl::AxisAnglef axisAngle(windMillSpeed*dt, 0, 0, 1);
		gmtl::Quatf rotation;
		gmtl::set(rotation, axisAngle);
		trans.orientation *= rotation;
		transNode->setTransformation(trans);
	}
//----------------------------------------------------------------------------//
// Snippet-6-1 - END                                                          //
//----------------------------------------------------------------------------//
