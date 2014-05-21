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

#include "Tile.h"

#include "../DebugOutput.h"
#include "../DataTypes.h"

Tile::Tile() {
	this->id = 0;
	this->name = "";
	this->sizeX = 0;
	this->sizeZ = 0;
	this->yRotation = 0;
	this->height = 0;
	this->model = NULL;
	//	this->modelTransform = identityTransformation();
	sceneGraphAttachment = NULL;
} // Tile

Tile::~Tile() {
	if (sceneGraphAttachment != NULL) {
		printd(ERROR, "Tile::~Tile(): detach me from scene graph interface first!\n");
	} // if
} // ~Tile

Tile* Tile::clone() {
	return new Tile(this);
} // clone

void Tile::setId(unsigned int id) {
	this->id = id;
} // setId

void Tile::setName(std::string name) {
	this->name = name;
} // setName

void Tile::setXSize(int sizeX) {
	this->sizeX = sizeX;
} // setXSize

void Tile::setZSize(int sizeZ) {
	this->sizeZ = sizeZ;
} // setZSize

void Tile::setYRotation(int yRotation) {
	this->yRotation = yRotation;
} // setYRotation

void Tile::setHeight(float height) {
	this->height = height;
} // setHeight

void Tile::setModel(ModelInterface* mdl) {
	model = mdl;
} // setModel

void Tile::setXmlFilename(std::string filename) {
	this->xmlFilename = filename;
} // setXmlFilename

unsigned int Tile::getId() {
	return id;
} // getId

std::string Tile::getName() {
	return name;
} // getName

int Tile::getXSize() {
	return sizeX;
} // getXSize

int Tile::getZSize() {
	return sizeZ;
} // getZSize

int Tile::getYRotation() {
	return yRotation;
} // getYRotation

float Tile::getHeight() {
	return height;
} // getHeight

ModelInterface* Tile::getModel() {
	return model;
} // getModel

std::string Tile::getXmlFilename() {
	return xmlFilename;
} // getXmlFilename

void Tile::dump() {
	printd(INFO, "Tile::dump(): Tile-ID: %u NAME: %s HEIGHT: %i SIZE: %ix%i zROTATION %i\n",
			this->id, this->name.c_str(), this->height, this->sizeX, this->sizeZ, this->yRotation);
}


Tile::Tile(Tile* src) :
	id(src->id), name(src->name), sizeX(src->sizeX), sizeZ(src->sizeZ), yRotation(src->yRotation),
			height(src->height) {

	if (src->model) {
		model = src->model->clone();
	} // if
	else {
		model = NULL;
	} // else
	//	modelTransform = src->modelTransform;
	sceneGraphAttachment = NULL;
} // Tile
