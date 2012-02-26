/**
 * @file
 * This file contains the implementation of DAGNode, which is the base
 * class for nodes in the model DAG.
 *
 * @brief Implementation of DAGNode
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The REvBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-08-16, version 1.0
 * @interface Distribution
 * @extends RbObject
 *
 * $Id$
 */

#include "DAGNode.h"
#include "RbException.h"
#include "RbUtil.h"
#include "RbObject.h"
#include "RbOptions.h"
#include "RbString.h"
#include "Variable.h"
#include "VectorNatural.h"
#include "VectorString.h"
#include "VariableNode.h"
#include "Workspace.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>



/** Constructor of filled node */
DAGNode::DAGNode(void) : children(), parents() {
    
    refCount = 0;
}


/**
 * Copy constructor should not copy children because it creates an
 * independent node. The new node is simply not used by any other objects
 * at this point. The parent nodes are left empty here because they are better
 * dealt with by the derived VariableNode classes, which have to maintain
 * dual copies of them (function arguments, distribution parameters,
 * or container elements).
 */
DAGNode::DAGNode( const DAGNode& x ) : children(), parents() {
    
    // copy the name so that we still be able to identify the variable in a cloned DAG
    name = x.name;
    
    refCount = 0;
}


/** Destructor deletes value if not NULL */
DAGNode::~DAGNode( void ) {
    
    if (refCount != 0) {
        std::cerr << "Uh oh, deleting DAG node which still is referenced to!!!";
    }

    for (std::set<VariableNode*>::iterator i = children.begin(); i != children.end(); i++) {
        VariableNode* theNode = *i;
        theNode->removeParentNode( this );
    }
}


/** add a child node */
void DAGNode::addChildNode(VariableNode *c) {
    PRINTF("Adding child with name \"%s\" to parent with name \"%s\".\n",c->getName().c_str(),getName().c_str());
        
    children.insert(c);
    
    // if this node is eliminated, than we pass on the factor root
    // the factor root is the node which start the likelihood calculation for the part of the graph which is eliminated
    if ( isEliminated() ) {
        c->setFactorRoot( static_cast<const VariableNode*>( this )->getFactorRoot() );
        
        // we know that this must be a variable node
        static_cast<VariableNode*>( this )->likelihoodsNeedUpdates();
    }
}


/** Decrement the reference count. */
size_t DAGNode::decrementReferenceCount( void ) {
    refCount--;
    
    return refCount;
}


/** Get class name of object */
const std::string& DAGNode::getClassName(void) { 
    
    static std::string rbClassName = "DAG node";
    
	return rbClassName; 
}

/** Get class type spec describing type of object */
const TypeSpec& DAGNode::getClassTypeSpec(void) { 
    
    static TypeSpec rbClass = TypeSpec( getClassName(), new TypeSpec( RbLanguageObject::getClassTypeSpec() ) );
    
	return rbClass; 
}


/**
 * Restore all affected nodes this DAGNode.
 * This means we call restoreMe() of all children. restoreMe() is pure virtual.
 */
void DAGNode::getAffectedNodes(std::set<StochasticNode* > &affected) {
    
    // get all my affected children
    for ( std::set<VariableNode*>::iterator i = children.begin(); i != children.end(); i++ )
        (*i)->getAffected(affected);
}

const RbObject& DAGNode::getElement(size_t index) const {
    
    // test whether the value supports indexing, i.e. is a container
    if (getValue().supportsIndex()) {
        
        return getValue().getElement(index);
    
    } else {
        
        std::ostringstream  msg;
        msg << "Illegal access of element at index [" << index << "] in object with tpye \"" << getValue().getType() << "\"";
        throw RbException( msg );
    }
    
}


RbObject& DAGNode::getElement(size_t index) {
    
    // test whether the value supports indexing, i.e. is a container
    if (getValue().supportsIndex()) {
        
        return getValue().getElement(index);
        
    } else {
        
        std::ostringstream  msg;
        msg << "Illegal access of element at index [" << index << "] in object with tpye \"" << getValue().getType() << "\"";
        throw RbException( msg );
    }
    
}

/** Get name of DAG node from its surrounding objects */
const std::string& DAGNode::getName( void ) const {

    return name;
}


const std::set<DAGNode*>& DAGNode::getParents( void ) const {
    return parents;
}


/** Get the reference count for this instance. */
size_t DAGNode::getReferenceCount(void) const {
    return refCount;
}

//const Variable& DAGNode::getVariable(void) const {
//    return *variable;
//}


/** Increment the reference count for this instance. */
void DAGNode::incrementReferenceCount( void ) {
    refCount++;
}

/** Is this a constant node */
bool DAGNode::isConst( void ) const {

    return false;
}


/** Check if node is a parent of node x in the DAG: needed to check for cycles in the DAG */
bool DAGNode::isParentInDAG( const DAGNode* x, std::list<DAGNode*>& done ) const {

    for( std::set<DAGNode*>::const_iterator i = parents.begin(); i != parents.end(); i++ ) {

        if ( std::find( done.begin(), done.end(), (*i) ) == done.end() ) {
            if ( (*i)->isParentInDAG( x, done ) )
                return true;
        }
    }

    return false;
}


/**
 * Keep the value of the node.
 * This function delegates the call to keepMe() and calls keepAffected() too.
 */
void DAGNode::keep(void) {
    // keep myself first
    keepMe();
    
    // next, keep all my children
    keepAffected();
}

/**
 * Tell affected variable nodes to keep the current value.
 */
void DAGNode::keepAffected() {
    
    // keep all my children
    for ( std::set<VariableNode*>::iterator i = children.begin(); i != children.end(); i++ )
        (*i)->keepMe();
}

/** Print children */
void DAGNode::printChildren( std::ostream& o ) const {

    o << "[ ";

    for ( std::set<VariableNode*>::const_iterator i = children.begin(); i != children.end(); i++) {
        if ( i != children.begin() )
            o << ", ";
        if ( getName() == "" ) {
            o << "<";
            (*i)->printValue(o);
            o << ">";
        }
        else
            o << (*i)->getName();
    }
    o << " ]";
}


/** Print parents */
void DAGNode::printParents( std::ostream& o ) const {

    o << "[ ";

    for ( std::set<DAGNode*>::const_iterator i = parents.begin(); i != parents.end(); i++) {
        if ( i != parents.begin() )
            o << ", ";
        if ( getName() == "" ) {
            o << "<";
            (*i)->printValue(o);
            o << ">";
        }
        else
            o << (*i)->getName();
    }
    o << " ]";
}

/** Remove a child from this DAG node. Free the pointer to the child. */
void DAGNode::removeChildNode(VariableNode *c) {
    
    // test if we even have this node as a child
    if (children.find(c) != children.end()) {
        // we do not own our children! See addChildNode for explanation
       
        // remove the child from our list
        children.erase(c);
    }
    
}


/**
 * Restore this DAGNode.
 * This means we call restoreMe() and restoreAffected(). There is a default implementation of restoreAffected()
 * which call restoreMe() of all children of this node. restoreMe() is pure virtual.
 */
void DAGNode::restore(void) {
    // first restore myself
    restoreMe();
    
    // next, restore all my children
    restoreAffected();
}


/**
 * Restore all affected nodes this DAGNode.
 * This means we call restoreMe() of all children. restoreMe() is pure virtual.
 */
void DAGNode::restoreAffected(void) {
    
    // next, restore all my children
    for ( std::set<VariableNode*>::iterator i = children.begin(); i != children.end(); i++ )
        (*i)->restoreMe();
}


/**
 * Touch the DAG node.
 *
 * This function should be called if the value of the variable has changed or if you want this node to be reevaluated. 
 * The function will automatically call the touchMe() which is implemented differently in the different DAG node types.
 *
 * Since the DAG node was touch and possibly changed, we tell affected variable nodes to keep the current value.
 */
void DAGNode::touch() {
    // first touch myself
    touchMe();
    
    // next, touch all my children
    touchAffected();
}


/**
 * Tell affected variable nodes to touch themselves (i.e. that they've been touched).
 */
void DAGNode::touchAffected() {
    
    // touch all my children
    for ( std::set<VariableNode*>::iterator i = children.begin(); i != children.end(); i++ )
        (*i)->touchMe();
}

