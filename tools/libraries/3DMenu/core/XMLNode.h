#ifndef _XMLNODE_H_
#define _XMLNODE_H_

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

#include "irrXML.h"

/**
 * \brief This is a wrapper-class for IrrXMLReader-objects.
 *
 * The class XMLNode is a wrapper for an IrrXMLReader -object. This wrapper allows only
 * retrieving the attributes of the actual node and its sub-nodes, which are in focus
 * of the wrapped IrrXMLReader -object.
 *
 * This is used for passing xml-nodes to widgets, which are so unable to harm the
 * 3dMenu by selecting other nodes in the xml-file.
 *
 * @date 23rd of March 2007
 * @author Thomas Weberndorfer
 *
 * <b>Updates:</b>
 *
 * - Widgets are allowed to access also sub-nodes. (3rd of April 2007)
 * - NULL will be returned instead of XMLNode::Error. (25th of April 2007)
 * - getAttributeValueAsFloat returns a preselectable return-value on failure.
 *   (25th of April 2007)
 * - Retrieving the node-type is possible. (6th May 2007)
 *
 **/
class XMLNode
{

    private:

        // This is the wrapped object.
        irr::io::IrrXMLReader* xml;

    public:

        /**
         * The constructor defines, which IrrXMLReader-object should be wrapped.
         * @param xml The IrrXMLReader -object, which will be wrapped.
         **/
        XMLNode ( irr::io::IrrXMLReader* xml );

        /**
         * This method returns the node-name.
         **/
        const char* getNodeName();

        /**
         * This method returns the attribute-value of an attribute from the
         * current xml-node.
         * @param name This is the name of the attribute from which the value is needed.
         * @return This method returns the value of an attribute, or NULL
         *         in case that no attribute with the given name exists.
         **/
        const char* getAttributeValue ( const char* name );

        /**
         * This method returns the attribute-value, converted into a float, of an
         * attribute from the current xml-node.
         * @param name This is the name of the attribute from which the value is needed.
         * @param fail This value will be returned in case of an error.
         * @return  This method returns the value of an attribute, converted into a float,
         *          or the value of the parameter <b>fail</b> in case of an error.
         **/
        float getAttributeValueAsFloat ( const char* name, float fail );

        /**
         * This method changes the focus of the wrapped IrrXMLReader -object to the next
         * sub-node and returns true or, if no sub-node could be found, false.
         *
         * If this method has been used and sub-nodes have been found, the other methods
         * of this class can be used to access the attributes of the sub-nodes. It is not
         * possible to return to the root-node. So read everything you need from the
         * root-node first!
         *
         * This method is state-full, because it has to count the start- and end-tags in
         * the XML-file. Once, this method has been called, you have the call it
         * repetitively until it returns false. Then, the internal state has been reseted.
         *
         * @return true if a sub-node has been found; false in all other cases.
         **/
        bool readNextSubNode();

};

#endif
