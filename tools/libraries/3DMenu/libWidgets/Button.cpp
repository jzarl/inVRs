/***************************************************************************
 *   Copyright (C) 2007 by Thomas Webernorfer                              *
 *   thomas_weberndorfer@hotmail.com                                       *
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

#include "Button.h"
#include "Config.h"
#include "SceneGraph.h"

Button::Button(XMLNode *xml) : Widget(xml){
    // Get the attribute "model" from the XML-file
    const char *model = xml->getAttributeValue ("model");
    // Load the model
    if(model != NULL && strcmp(model, "render") !=  0){
        // we have to load a model
        getSceneGraph()->loadModel(model);
    }else{
        // we have to render the name of the button as a new button widget
        const char *my_name;
        const char *label;
        char tmp[255];
        int i = 0;
        // create the button object
        SceneGraph *scene_button = new SceneGraph();
        SceneGraph *temp_scene = new SceneGraph();
        label = xml->getAttributeValue("label");
        if(label != NULL){
            my_name = label;
        }else{
            my_name = getName();
        }
        // add the left side of the button to the group
        temp_scene->loadModel("components/button/side_left.wrl");
        scene_button->mergeScene(temp_scene);
        // add the symbols / characters to the group
        while(my_name[i] != '\0' && i < 20){
            // create new nodes
            temp_scene = new SceneGraph();
            // parse the current symbol / character
            if(my_name[i] != ' '){
                parse_symbol(tmp, my_name[i]);
                // read the component model, and move it to its destination coordinates
                temp_scene->loadModel(tmp);
                temp_scene->setTranslate(0.6*i,0,0);
                scene_button->mergeScene(temp_scene);
            }
            i++;
        }
        // add the base of the button
        temp_scene = new SceneGraph();
        temp_scene->loadModel("components/button/base.wrl");
        temp_scene->setScale(i,1,1);
        scene_button->mergeScene(temp_scene);
        // add the right side of the button
        temp_scene = new SceneGraph();
        temp_scene->loadModel("components/button/side_right.wrl");
        temp_scene->setTranslate((0.6*(i-1)),0,0);
        scene_button->mergeScene(temp_scene);
        // finally, add the group node to the SceneGraph
        getSceneGraph()->mergeScene(scene_button);
    }
    // Load the animation
    const char *ani = xml->getAttributeValue ( "clickanimation" );
    if ( ani != NULL )
        animationName = new std::string(ani);
    else
        animationName = NULL;
    // Load the callback functions
    ani = xml->getAttributeValue ( "callbackPressed" );
    if ( ani != NULL )
        callbackPressed = new std::string(ani);
    else
        callbackPressed = NULL;
    ani = xml->getAttributeValue ( "callbackReleased" );
    if ( ani != NULL )
        callbackReleased = new std::string(ani);
    else
        callbackReleased = NULL;
}

void Button::parse_symbol(char *path, char symbol){
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

void Button::pressed()
{
    INF ( "Widget <" << getName() << "> has been pressed." );
    if(callbackPressed)
        callCallBack ( getName(), callbackPressed->c_str() );
    if(animationName)
        callAnimate ( this, animationName->c_str() );
}

void Button::released()
{
    INF ( "Widget <" << getName() << "> has been released." );
    if(callbackReleased)
        callCallBack ( getName(), callbackReleased->c_str() );
    if(animationName)
        callUnAnimate ( this, animationName->c_str() );
}

// The class factories

extern "C" Widget* _create_Button ( XMLNode *xml )
{
    return new Button ( xml );
}

extern "C" void _destroy_Button ( Widget* w )
{
    delete w;
}
