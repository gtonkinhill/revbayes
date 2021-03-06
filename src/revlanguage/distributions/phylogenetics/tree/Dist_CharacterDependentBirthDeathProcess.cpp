#include "ArgumentRule.h"
#include "ArgumentRules.h"
#include "Clade.h"
#include "Dist_CharacterDependentBirthDeathProcess.h"
#include "HomologousDiscreteCharacterData.h"
#include "ModelVector.h"
#include "Natural.h"
#include "OptionRule.h"
#include "Probability.h"
#include "RateGenerator.h"
#include "Real.h"
#include "RealPos.h"
#include "RlAbstractHomologousDiscreteCharacterData.h"
#include "RlBoolean.h"
#include "RlCladogeneticSpeciationRateMatrix.h"
#include "RlDistributionMemberFunction.h"
#include "RlRateGenerator.h"
#include "RlSimplex.h"
#include "RlString.h"
#include "RlTaxon.h"
#include "RlTimeTree.h"
#include "StateDependentSpeciationExtinctionProcess.h"
#include "StochasticNode.h"

using namespace RevLanguage;


Dist_CharacterDependentBirthDeathProcess::Dist_CharacterDependentBirthDeathProcess() : TypedDistribution<TimeTree>()
{
    
}


Dist_CharacterDependentBirthDeathProcess::~Dist_CharacterDependentBirthDeathProcess()
{
    
}



Dist_CharacterDependentBirthDeathProcess* Dist_CharacterDependentBirthDeathProcess::clone( void ) const
{
    
    return new Dist_CharacterDependentBirthDeathProcess( *this );
}


RevBayesCore::TypedDistribution<RevBayesCore::Tree>* Dist_CharacterDependentBirthDeathProcess::createDistribution( void ) const
{
    
    // get process start age
    RevBayesCore::TypedDagNode<double>* ra   = static_cast<const RealPos &>( start_age->getRevObject() ).getDagNode();
    
    // condition off origin age or root age?
    bool uo = ( start_condition == "originAge" ? true : false );
   
    // extinction rates
    RevBayesCore::TypedDagNode<RevBayesCore::RbVector<double> >* ex  = static_cast<const ModelVector<RealPos> &>( extinction_rates->getRevObject() ).getDagNode();
    
    // get speciation rates or cladogenetic speciation rate event map
    RevBayesCore::TypedDagNode<RevBayesCore::RbVector<double> >* sp;
    RevBayesCore::TypedDagNode<RevBayesCore::CladogeneticSpeciationRateMatrix>* cp;
    if ( speciation_rates->getRevObject().isType( ModelVector<RealPos>::getClassTypeSpec() ) )
    {
        sp  = static_cast<const ModelVector<RealPos> &>( speciation_rates->getRevObject() ).getDagNode();
    }
    else if ( speciation_rates->getRevObject().isType( CladogeneticSpeciationRateMatrix::getClassTypeSpec() ) )
    {
        cp = static_cast<const CladogeneticSpeciationRateMatrix &>( speciation_rates->getRevObject() ).getDagNode();
    } 
        
    // rate matrix
    RevBayesCore::TypedDagNode<RevBayesCore::RateGenerator>* q      = NULL;
    if ( event_rate_matrix->getRevObject() != RevNullObject::getInstance() )
    {
        q = static_cast<const RateGenerator &>( event_rate_matrix->getRevObject() ).getDagNode();
    }

    RevBayesCore::TypedDagNode<double>* r = NULL;
    if ( event_rate->getRevObject() != RevNullObject::getInstance() )
    {
        r = static_cast<const RealPos &>( event_rate->getRevObject() ).getDagNode();
    }

    RevBayesCore::TypedDagNode<RevBayesCore::Simplex >* bf = NULL;
    if ( root_frequencies->getRevObject() != RevNullObject::getInstance() )
    {
        bf = static_cast<const Simplex &>( root_frequencies->getRevObject() ).getDagNode();
    }

    RevBayesCore::TypedDagNode<double>* rh   = static_cast<const Probability &>( rho->getRevObject() ).getDagNode();
    
    // condition
    const std::string& cond                  = static_cast<const RlString &>( condition->getRevObject() ).getValue();
   
    // condition for simulating under
    const std::string& simulate_cond                  = static_cast<const RlString &>( simulation_condition->getRevObject() ).getValue();
    
    bool cond_tip_states = false;
    bool cond_num_tips = false;
    if (simulate_cond == "tipStates")
    {
        cond_tip_states = true;
        cond_num_tips = false;
    }
    if (simulate_cond == "numTips")
    {
        cond_tip_states = false;
        cond_num_tips = true;
    }
    
    size_t max_l = static_cast<const Integer &>( max_lineages->getRevObject() ).getValue();
    size_t min_l = static_cast<const Integer &>( min_lineages->getRevObject() ).getValue();
    size_t exact_l = static_cast<const Integer &>( exact_lineages->getRevObject() ).getValue();
    double max_t = static_cast<const RealPos &>( max_time->getRevObject() ).getValue();
    
    size_t prune = static_cast<const RlBoolean &>( prune_extinct_lineages->getRevObject() ).getValue();
    
    // finally make the distribution 
    RevBayesCore::StateDependentSpeciationExtinctionProcess*   d = new RevBayesCore::StateDependentSpeciationExtinctionProcess( ra, ex, q, r, bf, rh, cond, uo, min_l, max_l, exact_l, max_t, prune, cond_tip_states, cond_num_tips );
   
    // set speciation/cladogenetic event rates
    if (speciation_rates->getRevObject().isType( ModelVector<RealPos>::getClassTypeSpec() ))
    {
        d->setSpeciationRates( sp );
    }
    else if (speciation_rates->getRevObject().isType( CladogeneticSpeciationRateMatrix::getClassTypeSpec() ))
    {
        d->setCladogenesisMatrix( cp );
    } 
    
    // set the number of time slices for the numeric ODE
    double n = static_cast<const RealPos &>( num_time_slices->getRevObject() ).getValue();
    d->setNumberOfTimeSlices( n );
    
    RevBayesCore::TypedDagNode<RevBayesCore::RbVector<double> >* ps = NULL;
    if ( psi->getRevObject() != RevNullObject::getInstance() )
    {
        ps  = static_cast<const ModelVector<RealPos> &>( psi->getRevObject() ).getDagNode();

        d->setSerialSamplingRates( ps );
    }

    return d;
}



/* Get Rev type of object */
const std::string& Dist_CharacterDependentBirthDeathProcess::getClassType(void)
{
    
    static std::string rev_type = "Dist_CharacterDependentBirthDeathProcess";
    
    return rev_type;
}


/* Get class type spec describing type of object. TODO: Check if the correct parent is TypedDistribution or Distribution */
const TypeSpec& Dist_CharacterDependentBirthDeathProcess::getClassTypeSpec(void)
{
    
    static TypeSpec rev_type_spec = TypeSpec( getClassType(), new TypeSpec( TypedDistribution<TimeTree>::getClassTypeSpec() ) );
    
    return rev_type_spec;
}


/**
 * Get the Rev name for the distribution.
 * This name is used for the constructor and the distribution functions,
 * such as the density and random value function
 *
 * \return Rev name of constructor function.
 */
std::string Dist_CharacterDependentBirthDeathProcess::getDistributionFunctionName( void ) const
{
    // create a distribution name variable that is the same for all instance of this class
    std::string d_name = "CDBDP";
    
    return d_name;
}


/**
 * Get the alternative Rev names (aliases) for the constructor function.
 *
 * \return Rev aliases of constructor function.
 */
std::vector<std::string> Dist_CharacterDependentBirthDeathProcess::getDistributionFunctionAliases( void ) const
{
    // create alternative constructor function names variable that is the same for all instance of this class
    std::vector<std::string> a_names;
    a_names.push_back( "CDSSBDP" );
    a_names.push_back( "CDFBDP" );
    a_names.push_back( "BirthDeathMultiRate" );
    a_names.push_back( "CDCladoBDP" );

    return a_names;
}


MethodTable Dist_CharacterDependentBirthDeathProcess::getDistributionMethods( void ) const
{
    MethodTable methods = TypedDistribution<TimeTree>::getDistributionMethods();
    
    ArgumentRules* clampCharDataArgRules = new ArgumentRules();
    clampCharDataArgRules->push_back( new ArgumentRule( "value", AbstractHomologousDiscreteCharacterData::getClassTypeSpec(), "The observed value.", ArgumentRule::BY_VALUE, ArgumentRule::ANY ) );
    methods.addFunction( new MemberProcedure( "clampCharData", RlUtils::Void, clampCharDataArgRules ) );
   
    ArgumentRules* getCharDataArgRules = new ArgumentRules();
    methods.addFunction( new MemberProcedure( "getCharData", AbstractHomologousDiscreteCharacterData::getClassTypeSpec(), getCharDataArgRules ) ); 
    
    ArgumentRules* getCharHistoryArgRules = new ArgumentRules();
    methods.addFunction( new MemberProcedure( "getCharHistory", RlString::getClassTypeSpec(), getCharHistoryArgRules   ) );
    
    ArgumentRules* avgSpeciationArgRules = new ArgumentRules();
    methods.addFunction( new DistributionMemberFunction<Dist_CharacterDependentBirthDeathProcess, ModelVector<RealPos> >( "averageSpeciationRate", variable, avgSpeciationArgRules   ) );

    ArgumentRules* avgExtinctionArgRules = new ArgumentRules();
    methods.addFunction( new DistributionMemberFunction<Dist_CharacterDependentBirthDeathProcess, ModelVector<RealPos> >( "averageExtinctionRate", variable, avgExtinctionArgRules   ) );
    
    ArgumentRules* timeInStateArgRules = new ArgumentRules();
    methods.addFunction( new DistributionMemberFunction<Dist_CharacterDependentBirthDeathProcess, ModelVector<RealPos> >( "getTimeInState", variable, timeInStateArgRules   ) );
    
    return methods;
}


/* Return member rules */
const MemberRules& Dist_CharacterDependentBirthDeathProcess::getParameterRules(void) const
{
    
    static MemberRules memberRules;
    static bool rules_set = false;
    
    if ( !rules_set )
    {
        
        std::vector<std::string> aliases;
        aliases.push_back("rootAge");
        aliases.push_back("originAge");
        memberRules.push_back( new ArgumentRule( aliases, RealPos::getClassTypeSpec()    , "The start time of the process.", ArgumentRule::BY_CONSTANT_REFERENCE, ArgumentRule::ANY ) );
        std::vector<std::string> slabels;
        slabels.push_back("speciationRates");
        slabels.push_back("lambda");
        slabels.push_back("cladoEventMap");
        std::vector<TypeSpec> speciationTypes;
        speciationTypes.push_back( CladogeneticSpeciationRateMatrix::getClassTypeSpec() );
        speciationTypes.push_back( ModelVector<RealPos>::getClassTypeSpec() );
        memberRules.push_back( new ArgumentRule( slabels     , speciationTypes ,                          "The vector of speciation rates (for anagenetic-only models), or the map of speciation rates to cladogenetic event types."             , ArgumentRule::BY_CONSTANT_REFERENCE   , ArgumentRule::ANY ) );
        std::vector<std::string> elabels;
        elabels.push_back("extinctionRates");
        elabels.push_back("mu");
        memberRules.push_back( new ArgumentRule( elabels     , ModelVector<RealPos>::getClassTypeSpec() , "The vector of extinction rates."             , ArgumentRule::BY_CONSTANT_REFERENCE   , ArgumentRule::ANY ) );
        std::vector<std::string> flabels;
        flabels.push_back("psi");
        flabels.push_back("phi");
        memberRules.push_back( new ArgumentRule( flabels     , ModelVector<RealPos>::getClassTypeSpec() , "The vector of serial sampling rates."             , ArgumentRule::BY_CONSTANT_REFERENCE   , ArgumentRule::ANY, NULL ) );
        memberRules.push_back( new ArgumentRule( "Q"         , RateGenerator::getClassTypeSpec()        , "The rate matrix of jumping between rate categories.", ArgumentRule::BY_CONSTANT_REFERENCE   , ArgumentRule::ANY, NULL ) );
        memberRules.push_back( new ArgumentRule( "delta"     , RealPos::getClassTypeSpec()              , "The rate-factor of jumping between rate categories.", ArgumentRule::BY_CONSTANT_REFERENCE   , ArgumentRule::ANY, new RealPos(1.0) ) );
        memberRules.push_back( new ArgumentRule( "pi"        , Simplex::getClassTypeSpec()              , "State frequencies at the root."              , ArgumentRule::BY_CONSTANT_REFERENCE, ArgumentRule::ANY, NULL ) );
        memberRules.push_back( new ArgumentRule( "rho"       , Probability::getClassTypeSpec()          , "The taxon sampling probability."             , ArgumentRule::BY_CONSTANT_REFERENCE   , ArgumentRule::ANY, new RealPos(1.0) ) );
        
        std::vector<std::string> optionsCondition;
        optionsCondition.push_back( "time" );
        optionsCondition.push_back( "survival" );
        memberRules.push_back( new OptionRule( "condition"    , new RlString("time"), optionsCondition, "The condition of the birth-death process." ) );
        memberRules.push_back( new ArgumentRule("nTimeSlices", RealPos::getClassTypeSpec(), "The number of time slices for the numeric ODE.", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RealPos(500.0) ) );
        std::vector<std::string> optionsSimulateCondition;
        optionsSimulateCondition.push_back("startTime");
        optionsSimulateCondition.push_back("numTips");
        optionsSimulateCondition.push_back("tipStates");
        memberRules.push_back( new OptionRule("simulateCondition", new RlString("startTime"), optionsSimulateCondition, "The conditions under which to simulate." ) );
        memberRules.push_back( new ArgumentRule("minNumLineages", Natural::getClassTypeSpec(), "The minimum number of lineages to simulate; applied under the startTime condition.", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new Natural() ) );
        memberRules.push_back( new ArgumentRule("maxNumLineages", Natural::getClassTypeSpec(), "The maximum number of lineages to simulate; applied under the startTime condition.", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new Natural(500) ) );
        memberRules.push_back( new ArgumentRule("exactNumLineages", Natural::getClassTypeSpec(), "The exact number of lineages to simulate; applied under the numTips condition.", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new Natural(100) ) );
        memberRules.push_back( new ArgumentRule("maxTime", RealPos::getClassTypeSpec(), "Maximum time for lineages to coalesce when simulating; applied under the numTips and tipStates condition.", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RealPos(1000.0) ) );
        memberRules.push_back( new ArgumentRule("pruneExtinctLineages", RlBoolean::getClassTypeSpec(), "When simulating should extinct lineages be pruned off?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean(true) ) );

        rules_set = true;
    }
    
    return memberRules;
}


const TypeSpec& Dist_CharacterDependentBirthDeathProcess::getTypeSpec( void ) const
{
    
    static TypeSpec ts = getClassTypeSpec();
    
    return ts;
}


/** Set a member variable */
void Dist_CharacterDependentBirthDeathProcess::setConstParameter(const std::string& name, const RevPtr<const RevVariable> &var)
{
    
    if ( name == "rootAge" || name == "originAge" )
    {
        start_age = var;
        start_condition = name;
    }
    else if ( name == "pi" )
    {
        root_frequencies = var;
    }
    else if ( name == "speciationRates" || name == "lambda" || name == "cladoEventMap" )
    {
        speciation_rates = var;
    }
    else if ( name == "extinctionRates" || name == "mu" )
    {
        extinction_rates = var;
    }
    else if ( name == "psi" || name == "phi" || name == "psi/phi" )
    {
        psi = var;
    }
    else if ( name == "Q" )
    {
        event_rate_matrix = var;
    }
    else if ( name == "delta" )
    {
        event_rate = var;
    }
    else if ( name == "rho" )
    {
        rho = var;
    }
    else if ( name == "condition" )
    {
        condition = var;
    }
    else if ( name == "nTimeSlices" )
    {
        num_time_slices = var;
    }
    else if ( name == "minNumLineages" )
    {
        min_lineages = var;
    }
    else if ( name == "maxNumLineages" )
    {
        max_lineages = var;
    }
    else if ( name == "exactNumLineages" )
    {
        exact_lineages = var;
    }
    else if ( name == "maxTime" )
    {
        max_time = var;
    }
    else if ( name == "pruneExtinctLineages" )
    {
        prune_extinct_lineages = var;
    }
    else if ( name == "simulateCondition" )
    {
        simulation_condition = var;
    }
    else
    {
        Distribution::setConstParameter(name, var);
    }
}

