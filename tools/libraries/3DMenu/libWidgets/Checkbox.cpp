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

#include "Checkbox.h"
#include "Config.h"

Checkbox::Checkbox(XMLNode *xml) : Widget(xml){
    const char *my_name;
    const char *label;
    char tmp[255];
    int i = 0;
    selected = false;
    SceneGraph *scene_chkbox = new SceneGraph();
    SceneGraph *temp_scene = new SceneGraph();
    checked = new SceneGraph();

    // First, prepare the radiobox and the checked marker
    temp_scene->loadModel("components/chkbox/base.wrl");
    scene_chkbox->mergeScene(temp_scene);
    checked->loadModel("components/chkbox/checked.wrl");
    checked->setVisibility(selected);
    scene_chkbox->mergeScene(checked);
    temp_scene->loadModel("components/button/side_left.wrl");
    scene_chkbox->mergeScene(temp_scene);
    label = xml->getAttributeValue("label");
    if(label != NULL){
        my_name = label;
    }else{
        my_name = getName();
    }
    // add the symbols / characters to the scene
    while(my_name[i] != '\0' && i < 19){
        // create new nodes
        temp_scene = new SceneGraph();
        // parse the current symbol / character
        if(my_name[i] != ' '){
            parse_symbol(tmp, my_name[i]);
            // read the component model, and move it to its destination coordinates
            temp_scene->loadModel(tmp);
            temp_scene->setTranslate(0.6*(i+1),0,0);
            scene_chkbox->mergeScene(temp_scene);
        }
        i++;
    }
    // add the base of the button
    temp_scene = new SceneGraph();
    temp_scene->loadModel("components/button/base.wrl");
    temp_scene->setScale(i+1,1,1);
    scene_chkbox->mergeScene(temp_scene);
    // add the right side of the button
    temp_scene = new SceneGraph();
    temp_scene->loadModel("components/button/side_right.wrl");
    temp_scene->setTranslate((0.6*(i)),0,0);
    scene_chkbox->mergeScene(temp_scene);
    getSceneGraph()->mergeScene(scene_chkbox);
    // Load the animation
    const char *ani = xml->getAttributeValue ( "clickanimation" );
    if ( ani != NULL )
        animationName = new std::string(ani);
    else
        animationName = NULL;
    // Load the callback functions
    ani = xml->getAttributeValue ( "callbackSelected" );
    if ( ani != NULL )
        callbackSelected = new std::string(ani);
    else
        callbackSelected = NULL;
    ani = xml->getAttributeValue ( "callbackDeselected" );
    if ( ani != NULL )
        callbackDeselected = new std::string(ani);
    else
        callbackDeselected = NULL;
}

void Checkbox::parse_symbol(char *path, char symbol){
    if(symbol >= 'a' && symbol <= 'z'){
        sprintf(path, "components/small/%c.wrl", symbol);
    }else if(symbol >= 'A' && symbol <= 'Z'){
        sprintf(path, "components/capital/%c.wrl", tolower(symbol));
    }else if(symbol >= '0' && symbol <= '9'){
        sprintf(path, "components/numbers/%c.wrl", symbol);
    }else{
        switch(symbol){
            case 35:{ sprintf(path, "components/signs/number_sign.wrl"); break; }   // '#'
            case 40:{ sprintf(path, "components/signs/brck_open.wrl"); break; }     // '('
            case 41:{ sprintf(path, "components/signs/brck_close.wrl"); break; }    // ')'
            case 42:{ sprintf(path, "components/signs/asterix.wrl"); break; }       // '*'
            case 43:{ sprintf(path, "components/signs/plus.wrl"); break; }          // '+'
            case 44:{ sprintf(path, "components/signs/comma.wrl"); break; }         // ','
            case 45:{ sprintf(path, "components/signs/minus.wrl"); break; }         // '-'
            case 46:{ sprintf(path, "components/signs/dot.wrl"); break; }           // '.'
            case 47:{ sprintf(path, "components/signs/slash.wrl"); break; }         // '/'
            case 59:{ sprintf(path, "components/signs/semicolon.wrl"); break; }     // ';'
            case 60:{ sprintf(path, "components/signs/less_then.wrl"); break; }     // '<'
            case 62:{ sprintf(path, "components/signs/greater_then.wrl"); break; }  // '>'
            case 92:{ sprintf(path, "components/signs/backslash.wrl"); break; }     // '\'
            case 95:{ sprintf(path, "components/signs/underscore.wrl"); break; }    // '_'
            default:{ sprintf(path, "components/signs/unknown.wrl"); break; }
        }
    }
}

void Checkbox::pressed()
{
    INF ( "Widget <" << getName() << "> has been pressed." );
    if(selected){
        selected = false;
        checked->setVisibility(selected);
        if(callbackDeselected)
            callCallBack ( getName(), callbackDeselected->c_str() );
    }else{
        selected = true;
        checked->setVisibility(selected);
        if(callbackSelected)
            callCallBack ( getName(), callbackSelected->c_str() );
    }
    if(animationName)
        callAnimate ( this, animationName->c_str() );
}

void Checkbox::released()
{
    INF ( "Widget <" << getName() << "> has been released." );
    if(animationName)
        callUnAnimate ( this, animationName->c_str() );
}

// The class factories

extern "C" Widget* _create_Checkbox ( XMLNode *xml )
{
    return new Checkbox ( xml );
}

extern "C" void _destroy_Checkbox ( Widget* w )
{
    delete w;
}
