
#include "ArgumentRule.h"
#include "ArgumentRules.h"
#include "MinEssStoppingRule.h"
#include "OptionRule.h"
#include "RlMinEssStoppingRule.h"
#include "RealPos.h"
#include "RbException.h"
#include "RlString.h"
#include "TypeSpec.h"


using namespace RevLanguage;

/**
 * Default constructor.
 * Create the default instance.
 */
MinEssStoppingRule::MinEssStoppingRule(void) : AbstractConvergenceStoppingRule()
{
    
}


/**
 * Clone object
 */
MinEssStoppingRule* MinEssStoppingRule::clone(void) const
{
    
    return new MinEssStoppingRule(*this);
}


void MinEssStoppingRule::constructInternalObject( void )
{
    // we free the memory first
    delete value;
    
    // now allocate a new stopping rule
    double min = static_cast<const RealPos &>( minEss->getRevObject() ).getValue();
    int fq = static_cast<const Natural &>( frequency->getRevObject() ).getValue();
    const std::string &fn = static_cast<const RlString &>( filename->getRevObject() ).getValue();
    
    RevBayesCore::BurninEstimatorContinuous *burninEst = constructBurninEstimator();
    
    value = new RevBayesCore::MinEssStoppingRule(min, fn, size_t(fq), burninEst);

}


/** Get Rev type of object */
const std::string& MinEssStoppingRule::getClassType(void)
{
    
    static std::string revType = "MinEssStoppingRule";
    
    return revType;
}

/** Get class type spec describing type of object */
const TypeSpec& MinEssStoppingRule::getClassTypeSpec(void)
{
    
    static TypeSpec revTypeSpec = TypeSpec( getClassType(), new TypeSpec( AbstractConvergenceStoppingRule::getClassTypeSpec() ) );
    
    return revTypeSpec;
}



/** Return member rules */
const MemberRules& MinEssStoppingRule::getParameterRules(void) const
{
    
    static MemberRules memberRules;
    static bool rulesSet = false;
    
    if ( !rulesSet )
    {
        
        memberRules.push_back( new ArgumentRule( "minEss"   , RealPos::getClassTypeSpec() , ArgumentRule::BY_VALUE ) );
        
        /* Inherit weight from Move, put it after variable */
        const MemberRules& inheritedRules = AbstractConvergenceStoppingRule::getParameterRules();
        memberRules.insert( memberRules.end(), inheritedRules.begin(), inheritedRules.end() );


        rulesSet = true;
    }
    
    return memberRules;
}

/** Get type spec */
const TypeSpec& MinEssStoppingRule::getTypeSpec( void ) const
{
    
    static TypeSpec typeSpec = getClassTypeSpec();
    
    return typeSpec;
}


/** Get type spec */
void MinEssStoppingRule::printValue(std::ostream &o) const
{
    
    o << "MinEssStoppingRule";
}


/** Set a member variable */
void MinEssStoppingRule::setConstParameter(const std::string& name, const RevPtr<const RevVariable> &var)
{
    
    if ( name == "minEss" )
    {
        minEss = var;
    }
    else
    {
        AbstractConvergenceStoppingRule::setConstParameter(name, var);
    }
    
}