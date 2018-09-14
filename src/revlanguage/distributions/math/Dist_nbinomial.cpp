#include "ArgumentRule.h"
#include "ArgumentRules.h"
#include "NegativeBinomialDistribution.h"
#include "ContinuousStochasticNode.h"
#include "Dist_nbinomial.h"
#include "Probability.h"
#include "RealPos.h"

using namespace RevLanguage;

Dist_nbinomial::Dist_nbinomial(void) : TypedDistribution<Natural>()
{
    
}


Dist_nbinomial::~Dist_nbinomial(void)
{
    
}



Dist_nbinomial* Dist_nbinomial::clone( void ) const
{
    
    return new Dist_nbinomial(*this);
}


RevBayesCore::NegativeBinomialDistribution* Dist_nbinomial::createDistribution( void ) const
{
    
    // get the parameters
    RevBayesCore::TypedDagNode<long>*   vr = static_cast<const Natural     &>( r->getRevObject() ).getDagNode();
    RevBayesCore::TypedDagNode<double>* vp = static_cast<const Probability &>( p->getRevObject() ).getDagNode();
    RevBayesCore::NegativeBinomialDistribution* d  = new RevBayesCore::NegativeBinomialDistribution( vr, vp );
    return d;
}



/* Get Rev type of object */
const std::string& Dist_nbinomial::getClassType(void)
{
    
    static std::string rev_type = "Dist_nbinomial";
	return rev_type;
}

/* Get class type spec describing type of object */
const TypeSpec& Dist_nbinomial::getClassTypeSpec(void)
{
    
    static TypeSpec rev_type_spec = TypeSpec( getClassType(), new TypeSpec( Distribution::getClassTypeSpec() ) );
	return rev_type_spec;
}


/**
 * Get the alternative Rev names (aliases) for the constructor function.
 *
 * \return Rev aliases of constructor function.
 */
std::vector<std::string> Dist_nbinomial::getDistributionFunctionAliases( void ) const
{
    // create alternative constructor function names variable that is the same for all instance of this class
    std::vector<std::string> a_names;
    a_names.push_back( "nbinom" );

    return a_names;
}


/**
 * Get the Rev name for the distribution.
 * This name is used for the constructor and the distribution functions,
 * such as the density and random value function
 *
 * \return Rev name of constructor function.
 */
std::string Dist_nbinomial::getDistributionFunctionName( void ) const
{
    // create a distribution name variable that is the same for all instance of this class
    std::string d_name = "nbinomial";
    
    return d_name;
}


/**
 * Get the author(s) of this function so they can receive credit (and blame) for it.
 */




/**
 * Get the (brief) description for this function
 */



/**
 * Get the more detailed description of the function
 */



/**
 * Get an executable and instructive example.
 * These example should help the users to show how this function works but
 * are also used to test if this function still works.
 */



/**
 * Get some references/citations for this function
 *
 */



/**
 * Get the names of similar and suggested other functions
 */



/**
 * Get the title of this help entry
 */



/** Return member rules (no members) */
const MemberRules& Dist_nbinomial::getParameterRules(void) const
{
    
    static MemberRules dist_member_rules;
    static bool rules_set = false;
    
    if ( !rules_set ) 
    {
        dist_member_rules.push_back( new ArgumentRule( "r", Natural::getClassTypeSpec(), "Number of failures.", ArgumentRule::BY_CONSTANT_REFERENCE, ArgumentRule::ANY ) );
        dist_member_rules.push_back( new ArgumentRule( "p", Probability::getClassTypeSpec()    , "Probability of success.", ArgumentRule::BY_CONSTANT_REFERENCE, ArgumentRule::ANY ) );
        rules_set = true;
    }
    
    return dist_member_rules;
}


const TypeSpec& Dist_nbinomial::getTypeSpec( void ) const
{
    
    static TypeSpec ts = getClassTypeSpec();
    return ts;
}


/** Print value for user */
void Dist_nbinomial::printValue(std::ostream& o) const
{
    
    o << "NegativeBinomial(r=";
    if ( r != NULL )
    {
        o << r->getName();
    }
    else
    {
        o << "?";
    }
    o << ", p=";
    if ( p != NULL )
        o << p->getName();
    else
        o << "?";
    o << ")";
}


/** Set a member variable */
void Dist_nbinomial::setConstParameter(const std::string& name, const RevPtr<const RevVariable> &var)
{
        
    if ( name == "p" ) 
    {
        p = var;
    }
    else if ( name == "r" )
    {
        r = var;
    }
    else
    {
        TypedDistribution<Natural>::setConstParameter(name, var);
    }
}
