#include "ArgumentRule.h"
#include "ArgumentRules.h"
#include "PoissonDistribution.h"
#include "RlPoissonDistribution.h"
#include "Natural.h"
#include "Probability.h"
#include "StochasticNode.h"

using namespace RevLanguage;


/**
 * Default constructor.
 * 
 * The default constructor does nothing except allocating the object.
 */
PoissonDistribution::PoissonDistribution() : TypedDistribution<Natural>() 
{
    
}


/**
 * The clone function is a convenience function to create proper copies of inherited objected.
 * E.g. a.clone() will create a clone of the correct type even if 'a' is of derived type 'B'.
 *
 * \return A new copy of the model. 
 */
PoissonDistribution* PoissonDistribution::clone( void ) const 
{
    
    return new PoissonDistribution(*this);
}


/**
 * Create a new internal distribution object.
 *
 * This function simply dynamically allocates a new internal distribution object that can be 
 * associated with the variable. The internal distribution object is created by calling its
 * constructor and passing the distribution-parameters (other DAG nodes) as arguments of the 
 * constructor. The distribution constructor takes care of the proper hook-ups.
 *
 * \return A new internal distribution object.
 */
RevBayesCore::PoissonDistribution* PoissonDistribution::createDistribution( void ) const 
{
    // get the parameters
    RevBayesCore::TypedDagNode<double>* rate    = static_cast<const RealPos &>( lambda->getValue() ).getValueNode();
    RevBayesCore::PoissonDistribution* d      = new RevBayesCore::PoissonDistribution( rate );
    
    return d;
}



/**
 * Get class name of object 
 *
 * \return The class' name.
 */
const std::string& PoissonDistribution::getClassName(void) 
{ 
    
    static std::string rbClassName = "Poisson distribution";
    
	return rbClassName; 
}


/**
 * Get class type spec describing type of an object from this class (static).
 *
 * \return TypeSpec of this class.
 */
const TypeSpec& PoissonDistribution::getClassTypeSpec(void) 
{ 
    
    static TypeSpec rbClass = TypeSpec( getClassName(), new TypeSpec( TypedDistribution<Natural>::getClassTypeSpec() ) );
    
	return rbClass; 
}



/** 
 * Get the member rules used to create the constructor of this object.
 *
 * The member rules of the Poisson distribution are:
 * (1) the rate lambda which must be a positive real between 0 and 1 (= a probability).
 *
 * \return The member rules.
 */
const MemberRules& PoissonDistribution::getMemberRules(void) const 
{
    
    static MemberRules distPoisMemberRules;
    static bool rulesSet = false;
    
    if ( !rulesSet ) 
    {
        distPoisMemberRules.push_back( new ArgumentRule( "lambda", true, RealPos::getClassTypeSpec() ) );
        
        rulesSet = true;
    }
    
    return distPoisMemberRules;
}


/**
 * Get type-specification on this object (non-static).
 *
 * \return The type spec of this object.
 */
const TypeSpec& PoissonDistribution::getTypeSpec( void ) const 
{
    
    static TypeSpec ts = getClassTypeSpec();
    
    return ts;
}



/** 
 * Set a member variable.
 * 
 * Sets a member variable with the given name and store the pointer to the variable.
 * The value of the variable might still change but this function needs to be called again if the pointer to
 * the variable changes. The current values will be used to create the distribution object.
 *
 * \param[in]    name     Name of the member variable.
 * \param[in]    var      Pointer to the variable.
 */
void PoissonDistribution::setConstMemberVariable(const std::string& name, const RevPtr<const Variable> &var) 
{
    
    if ( name == "lambda" )
    {
        lambda = var;
    }
    else 
    {
        TypedDistribution<Natural>::setConstMemberVariable(name, var);
    }
    
}
