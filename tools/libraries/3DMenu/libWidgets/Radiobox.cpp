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

#include "Radiobox.h"
#include "Config.h"

std::list<RadioboxItem*> RadioboxMaster::radiobox_list;

void RadioboxMaster::registerRadiobox(Radiobox *box, std::string name, std::string group){
    radiobox_list.push_back(new RadioboxItem(name, group, box));
}

void RadioboxMaster::selectedRadiobox(std::string name, std::string group){
    std::list<RadioboxItem*>::iterator iter = RadioboxMaster::radiobox_list.begin();
    while(iter != RadioboxMaster::radiobox_list.end()){
        if((*iter)->group == group && (*iter)->name != name){
            (*iter)->box->setSelection(false);
        }
        iter++;
    }
}

Radiobox::Radiobox(XMLNode *xml) : Widget(xml){
    const char *my_name;
    const char *label;
    char tmp[255];
    int i = 0;
    selected = false;
    SceneGraph *scene_radio = new SceneGraph();
    SceneGraph *temp_scene = new SceneGraph();
    checked = new SceneGraph();

    // First, prepare the radiobox and the checked marker
    temp_scene->loadModel("components/radiobox/base.wrl");
    scene_radio->mergeScene(temp_scene);
    checked->loadModel("components/radiobox/checked.wrl");
    checked->setVisibility(selected);
    scene_radio->mergeScene(checked);
    temp_scene->loadModel("components/button/side_left.wrl");
    scene_radio->mergeScene(temp_scene);
    label = xml->getAttributeValue("label");
    if(label != NULL){
        my_name = label;
    }else{
        my_name = getName();
    }
    radiobox_name = my_name;
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
            scene_radio->mergeScene(temp_scene);
        }
        i++;
    }
    // add the base of the button
    temp_scene = new SceneGraph();
    temp_scene->loadModel("components/button/base.wrl");
    temp_scene->setScale(i+1,1,1);
    scene_radio->mergeScene(temp_scene);
    // add the right side of the button
    temp_scene = new SceneGraph();
    temp_scene->loadModel("components/button/side_right.wrl");
    temp_scene->setTranslate((0.6*(i)),0,0);
    scene_radio->mergeScene(temp_scene);
    getSceneGraph()->mergeScene(scene_radio);
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
    ani = xml->getAttributeValue ( "groupName" );
    if ( ani != NULL ){
        groupName = new std::string(ani);
        RadioboxMaster::registerRadiobox(this, radiobox_name, *groupName);
    }else
        groupName = NULL;
    
}

void Radiobox::parse_symbol(char *path, char symbol){
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

void Radiobox::setSelection(bool selection){
    if(selected != selection){
        selected = selection;
        checked->setVisibility(selected);
        if(selected){
            if(callbackSelected)
                callCallBack(getName(), callbackSelected->c_str());
        }else{
            if(callbackDeselected)
                callCallBack(getName(), callbackDeselected->c_str());
        }
    }
}

void Radiobox::pressed()
{
    INF ( "Widget <" << getName() << "> has been pressed." );
    if(groupName){
        // there is a group of radio boxes, so deselect all other boxes in the group
        if(!selected){
            // go through the list of radioboxes, and mark all others as deselected
            RadioboxMaster::selectedRadiobox(radiobox_name, *groupName);
            // select this Radiobox, and call the select callback function
            selected = true;
            checked->setVisibility(selected);
            if(callbackSelected)
                callCallBack ( getName(), callbackSelected->c_str() );
        }else{
            // allready selected, nothing to do
        }
    }else{
        // only one radiobox
        if(!selected){
            selected = true;
            checked->setVisibility(selected);
            if(callbackSelected)
                callCallBack ( getName(), callbackSelected->c_str() );
        }
    }
    if(animationName)
        callAnimate ( this, animationName->c_str() );
}

void Radiobox::released()
{
    INF ( "Widget <" << getName() << "> has been released." );
    if(animationName)
        callUnAnimate ( this, animationName->c_str() );
}

// The class factories

extern "C" Widget* _create_Radiobox ( XMLNode *xml )
{
    return new Radiobox ( xml );
}

extern "C" void _destroy_Radiobox ( Widget* w )
{
    delete w;
}
