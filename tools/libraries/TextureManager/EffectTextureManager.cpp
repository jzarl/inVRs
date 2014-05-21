/*---------------------------------------------------------------------------*\
 *           interactive networked Virtual Reality system (inVRs)            *
 *                                                                           *
 *    Copyright (C) 2005-2009 by the Johannes Kepler University, Linz        *
 *                                                                           *
 *                            www.inVRs.org                                  *
 *                                                                           *
 *              contact: canthes@inVRs.org, rlander@inVRs.org                *
 \*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *                                License                                    *
 *                                                                           *
 * This library is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU Library General Public License as published    *
 * by the Free Software Foundation, version 2.                               *
 *                                                                           *
 * This library is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public         *
 * License along with this library; if not, write to the Free Software       *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
 \*---------------------------------------------------------------------------*/

#include "EffectTextureManager.h"

#include <OpenSG/OSGImage.h>

#include <inVRs/SystemCore/DebugOutput.h>

#if OSG_MAJOR_VERSION < 2
namespace OSG {
       typedef TextureChunk TextureObjChunk;
}
#endif

OSG_USING_NAMESPACE

EffectTextureManagerListEntry::EffectTextureManagerListEntry(std::string path, TextureObjChunkRefPtr tex) {
	this->name = path;
	this->tex = tex;
}


EffectTextureManager::EffectTextureManager() {

}

EffectTextureManager::~EffectTextureManager() {
	EffectTextureManagerListEntry* listEntry;
	while (!texturesLoaded.empty()) {
		listEntry = texturesLoaded[texturesLoaded.size() - 1];
		delete listEntry;
		texturesLoaded.pop_back();
	}
}

TextureObjChunkRefPtr EffectTextureManager::loadTexture(std::string path) {
	int i;
	EffectTextureManagerListEntry* listEntry;
#if OSG_MAJOR_VERSION >= 2
	TextureObjChunkRefPtr texture = NULL;
#else
	TextureObjChunkRefPtr texture = OSG::NullFC;
#endif
	ImageRefPtr img;
	img = Image::create();
	bool success = false;

	for (i = 0; i < (int)texturesLoaded.size(); i++) {
		listEntry = texturesLoaded[i];
		if (listEntry->name == path)
			return listEntry->tex;
	}

#if OSG_MAJOR_VERSION < 2
	beginEditCP(img);
#endif
	success = img->read((Char8*)path.c_str());
#if OSG_MAJOR_VERSION < 2
	endEditCP(img);
#endif

	if (!success) {
		printd(WARNING, "EffectTextureManager::loadTexture(): could not load texture %s\n", path.c_str());
	} else {
		texture = TextureObjChunk::create();
#if OSG_MAJOR_VERSION < 2
		beginEditCP(texture);
#endif
		texture->setImage(img);
		initializeTexture(path, texture);
#if OSG_MAJOR_VERSION < 2
		endEditCP(texture);
#endif

		if (success)
			printd(INFO, "EffectTextureManager::loadTexture(): loaded texture %s\n", path.c_str());
		else
		texturesLoaded.push_back(new EffectTextureManagerListEntry(path, texture));
	} // else

	return texture;
}

void EffectTextureManager::initializeTexture(std::string path, TextureObjChunkRefPtr tex) {
#if OSG_MAJOR_VERSION < 2
	// 2013-03-09 ZaJ: setEnvMode is available in TextureEnvChunk, but not TextureObjChunk
	// I think setting this is not neccessary anyways in TextureObjChunk, but I might be horribly wrong...
	tex->setEnvMode(GL_MODULATE);
#endif
}

