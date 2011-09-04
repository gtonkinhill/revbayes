/**
 * @file
 * This file contains the declaration of DistributionFunction, which
 * is used for functions related to a statistical distribution.
 *
 * @brief Declaration of DistributionFunction
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 *
 * $Id$
 */

#ifndef DistributionFunction_H
#define DistributionFunction_H

#include "Real.h"
#include "RbFunction.h"

#include <map>
#include <set>
#include <string>
#include <vector>

class ArgumentRule;
class DAGNode;
class Distribution;
class VectorInteger;
class VectorString;

const std::string DistributionFunction_name = "Distribution Function";

class DistributionFunction :  public RbFunction {

    public:
        enum FuncType { DENSITY, RVALUE, PROB, QUANTILE };                                                  //!< Enum specifying function type

                                    DistributionFunction(Distribution* dist, FuncType funcType);            //!< Constructor
                                    DistributionFunction(const DistributionFunction& x);                    //!< Copy constructor
                                   ~DistributionFunction(void);                                             //!< Destructor

        // Assignment operator
        DistributionFunction&       operator=(const DistributionFunction& x);                               //!< Assignment operator

        // Basic utility functions
        DistributionFunction*       clone(void) const;                                                      //!< Clone object
        const VectorString&         getClass(void) const;                                                   //!< Get class vector
        const TypeSpec&             getTypeSpec(void) const;                                                //!< Get language type of the object

        // DistributionFunction functions
        RbLanguageObject*           execute(void);                                                          //!< Execute function
        const ArgumentRules&        getArgumentRules(void) const;                                           //!< Get argument rules
        const TypeSpec&             getReturnType(void) const;                                              //!< Get type spec of return value
        bool                        processArguments(   const std::vector<Argument*>&   args,
                                                        VectorInteger*                  matchScore=NULL);   //!< Process args, set member variables of distribution

	protected:
        ArgumentRules               argumentRules;                                                          //!< Argument rules
        TypeSpec                    returnType;                                                             //!< Return type
        Distribution*               distribution;                                                           //!< The distribution
        FuncType                    functionType;                                                           //!< Function type
    
    private:
        static const TypeSpec       typeSpec;
};

#endif

