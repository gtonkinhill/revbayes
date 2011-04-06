/**
 * @file
 * This file contains the declaration of ConstructorFunction, which is used
 * for functions that construct member objects.
 *
 * @brief Declaration of ConstructorFunction
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 *
 * $Id$
 */

#ifndef ConstructorFunction_H
#define ConstructorFunction_H

#include "RbFunction.h"

#include <map>
#include <set>
#include <string>
#include <vector>

class DAGNode;
class MemberObject;
class VectorString;

class ConstructorFunction :  public RbFunction {

    public:
                                    ConstructorFunction(MemberObject* obj);     //!< Object constructor

        // Basic utility functions
        ConstructorFunction*        clone(void) const;                          //!< Clone the object
    	const VectorString&         getClass(void) const;                       //!< Get class vector

        // Regular functions
        DAGNode*                    executeFunction(void);                      //!< Execute function
        const ArgumentRules&        getArgumentRules(void) const;               //!< Get argument rules
        const TypeSpec              getReturnType(void) const;                  //!< Get type of return value

	protected:
        MemberObject*               templateObject;                             //!< The template object
};

#endif

