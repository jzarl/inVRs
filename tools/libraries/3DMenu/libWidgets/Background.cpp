/***************************************************************************
 *   Copyright (C) 2007 by Thomas Webernorfer   *
 *   thomas_weberndorfer@hotmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "Background.h"
#include "Config.h"
#include "SceneGraph.h"

Background::Background(XMLNode *xml) : Widget(xml){
    float scale_x, scale_y, scale_z;
    scale_x = xml->getAttributeValueAsFloat("scale_x", 1);
    scale_y = xml->getAttributeValueAsFloat("scale_y", 1);
    scale_z = xml->getAttributeValueAsFloat("scale_z", 1);
    SceneGraph *scene_background = new SceneGraph();
    scene_background->loadModel("components/bar/background.wrl");
    scene_background->setScale(scale_x, scale_y, scale_z);
    const char *ani = xml->getAttributeValue ( "clickanimation" );
    if ( ani != NULL )
        animationName = new std::string(ani);
    else
        animationName = NULL;
    getSceneGraph()->mergeScene(scene_background);
}

void Background::pressed()
{
    INF ( "Widget <" << getName() << "> has been pressed." );
    // Registry test
    if(animationName)
        callAnimate ( this, animationName->c_str() );
}

void Background::released()
{
    INF ( "Widget <" << getName() << "> has been released." );
    // Registry test
    if(animationName)
        callUnAnimate ( this, animationName->c_str() );
}

// The class factories

extern "C" Widget* _create_Background ( XMLNode *xml )
{
    return new Background ( xml );
}

extern "C" void _destroy_Background ( Widget* w )
{
    delete w;
}
