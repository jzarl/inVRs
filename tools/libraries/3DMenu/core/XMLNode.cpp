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

#include "XMLNode.h"
#include "Config.h"
#include <stdlib.h>

XMLNode::XMLNode ( irr::io::IrrXMLReader* xml )
{
    if ( xml == NULL )
    {
        // This class can only be used with valid IrrXMLReader-object!
        ERR ( "NULL passed to XMLNode-constructor!" );
        exit ( 1 );
    }
    this->xml = xml;
}

const char* XMLNode::getNodeName()
{
    return xml->getNodeName();
}

const char* XMLNode::getAttributeValue ( const char* name )
{
    const char* tmp = xml->getAttributeValue ( name );
    // Check return value
    if ( tmp == 0 )
    {
        return NULL;
    }
    else
    {
        return tmp;
    }
}

float XMLNode::getAttributeValueAsFloat ( const char* name, float fail )
{
    // Check if the attribute was found.
    if ( !xml->getAttributeValue ( name ) )
        return fail;
    else
        return xml->getAttributeValueAsFloat ( name );
}

bool XMLNode::readNextSubNode()
{
    // Depth of the sub-nodes
    static int depth;
    //
    // Test the root-node, if sub-nodes are available
    //
    if ( depth == 0 )
    { /* Depth is 0. So this is the root-node */
        // Test 1:
        if ( xml->isEmptyElement() )
        {
            // The root-node looks like < ... />.
            // There are no sub-nodes !
            return false;
        }
        // Test 2:
        if ( xml->getNodeType() == irr::io::EXN_ELEMENT_END )
        {
            // We found the end-tag of the root-node.
            // There are no sub-nodes !
            return false;
        }
    }
    //
    // We haven't seen an end-tag of the root-node yet and
    // we know, that there is an end-tag, because the root-node
    // is not empty. So it is save to advance in the XML-structure.
    //
    while ( xml->read() /* advance */ )
    {
        if ( xml->getNodeType() == irr::io::EXN_ELEMENT_END )
        {
            // We found an end-tag.
            if ( depth == 0 )
            {
                // This is the end-tag of the root-node
                INF ( "No more sub-nodes found." );
                return false;
            }
            else
            {
                // This is an end-tag of a sub-node.
                // So we have to reduce the depth.
                depth--;
            }
        }
        else if ( xml->getNodeType() == irr::io::EXN_ELEMENT )
        {
            // We found a start-tag
            INF ( "New sub-node found <" << xml->getNodeName() << ">" );
            if ( !xml->isEmptyElement() )
            {
                // We discovered sub-node which looks like <...> ... </...>.
                // So we have to expect an end-tag and to increase the depth.
                depth++;
            }
            return true;
        }
    }
    // This "return" is never used ...
    return false;
}
