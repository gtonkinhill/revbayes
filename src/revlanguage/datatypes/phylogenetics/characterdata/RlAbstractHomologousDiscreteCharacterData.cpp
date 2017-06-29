#include "RlAbstractHomologousDiscreteCharacterData.h"
#include "RlAbstractDiscreteTaxonData.h"
#include "ArgumentRule.h"
#include "RlMatrixReal.h"
#include "MemberProcedure.h"
#include "ModelVector.h"
#include "Natural.h"
#include "RlBoolean.h"
#include "Probability.h"
#include "RlString.h"
#include "RlDiscreteTaxonData.h"
#include "RlSimplex.h"
#include "RbBitSet.h"


using namespace RevLanguage;

AbstractHomologousDiscreteCharacterData::AbstractHomologousDiscreteCharacterData(void) :
    HomologousCharacterData( ),
    dag_node( NULL )
{
    
    initMethods();

}


AbstractHomologousDiscreteCharacterData::AbstractHomologousDiscreteCharacterData( const RevBayesCore::AbstractHomologousDiscreteCharacterData &d) :
    HomologousCharacterData( ),
    dag_node( new ConstantNode<valueType>("",d.clone()) )
{
    
    // increment the reference count to the value
    dag_node->incrementReferenceCount();
    
    initMethods();
    
}


AbstractHomologousDiscreteCharacterData::AbstractHomologousDiscreteCharacterData( RevBayesCore::AbstractHomologousDiscreteCharacterData *d) :
    HomologousCharacterData( ),
    dag_node( new ConstantNode<valueType>("",d) )
{
    
    // increment the reference count to the value
    dag_node->incrementReferenceCount();
    
    initMethods();

}


AbstractHomologousDiscreteCharacterData::AbstractHomologousDiscreteCharacterData( RevBayesCore::Typeddag_node<RevBayesCore::AbstractHomologousDiscreteCharacterData> *d) :
    HomologousCharacterData( ),
    dag_node( d )
{
    
    // increment the reference count to the value
    dag_node->incrementReferenceCount();
    
    initMethods();

}


AbstractHomologousDiscreteCharacterData::AbstractHomologousDiscreteCharacterData(const AbstractHomologousDiscreteCharacterData &d) :
    HomologousCharacterData( d ),
    dag_node( NULL )
{
    
    if ( d.dag_node != NULL )
    {
        
        dag_node = d.dag_node->clone();
        
        // increment the reference count to the value
        dag_node->incrementReferenceCount();
    }
    
}


AbstractHomologousDiscreteCharacterData::~AbstractHomologousDiscreteCharacterData()
{
    
    // free the old value
    if ( dag_node != NULL )
    {
        if ( dag_node->decrementReferenceCount() == 0 )
        {
            delete dag_node;
        }
        
    }
    
}


AbstractHomologousDiscreteCharacterData& AbstractHomologousDiscreteCharacterData::operator=(const AbstractHomologousDiscreteCharacterData &v) {
    
    if ( this != &v )
    {
        // delegate to base class
        HomologousCharacterData::operator=( v );
        
        // free the old value
        if ( dag_node != NULL )
        {
            if ( dag_node->decrementReferenceCount() == 0 )
            {
                delete dag_node;
            }
            
            dag_node = NULL;
        }
        
        // create own copy
        if ( v.dag_node != NULL )
        {
            dag_node = v.dag_node->clone();
            
            // increment the reference count to the value
            dag_node->incrementReferenceCount();
        }
        
    }
    
    return *this;
}



AbstractHomologousDiscreteCharacterData* AbstractHomologousDiscreteCharacterData::concatenate(const RevObject &d, std::string type) const
{
    const AbstractHomologousDiscreteCharacterData* tmp = dynamic_cast<const AbstractHomologousDiscreteCharacterData*>( &d );
    if ( tmp != NULL )
    {
        return concatenate( *tmp, type );
    }
    else
    {
        throw RbException("Cannot add an object of type '" + d.getType() + "' to a character data object.");
    }
    
}



AbstractHomologousDiscreteCharacterData* AbstractHomologousDiscreteCharacterData::concatenate(const AbstractHomologousDiscreteCharacterData &d, std::string type) const
{
    AbstractHomologousDiscreteCharacterData* cloneObj = clone();

    // we need to make this a constant DAG node so that we can actually modify the value
    // otherwise the value might be overwritten again, e.g., if this is a deterministic node.
    cloneObj->makeConstantValue();
    
    // now concatenate
    cloneObj->getdag_node()->getValue().concatenate( d.getValue(), type );
    
    // return the copy
    return cloneObj;
}



AbstractHomologousDiscreteCharacterData* AbstractHomologousDiscreteCharacterData::clone() const
{
    return new AbstractHomologousDiscreteCharacterData( *this );
}


/* Map calls to member methods */
RevPtr<RevVariable> AbstractHomologousDiscreteCharacterData::executeMethod(std::string const &name, const std::vector<Argument> &args, bool &found)
{
    
    RevPtr<RevVariable> retVal = dynamic_cast<RevMemberObject *>( dag_node )->executeMethod(name, args, found);
    
    if ( found == true )
    {
        return retVal;
    }
    
    if ( this->getdag_node() != NULL )
    {
        // set the internal value pointer
//        setCharacterDataObject( &this->getdag_node()->getValue() );
    }
    
    retVal = executeCharacterDataMethod(name, args, found);
    
    if ( found == true )
    {
        return retVal;
    }
    else if (name == "[]")
    {
        found = true;
    
        // get the member with give index
        const Natural& index = static_cast<const Natural&>( args[0].getVariable()->getRevObject() );
    
        if (this->dag_node->getValue().getNumberOfTaxa() < (size_t)(index.getValue()) )
        {
            throw RbException("Index out of bounds in []");
        }
    
        const RevBayesCore::AbstractDiscreteTaxonData& element = dag_node->getValue().getTaxonData(size_t(index.getValue()) - 1);
    
        return new RevVariable( new AbstractDiscreteTaxonData( element.clone() ) );
    }
    else if (name == "computeStateFrequencies")
    {
        found = true;
        
        RevBayesCore::MatrixReal sf = this->dag_node->getValue().computeStateFrequencies();
        
        return new RevVariable( new MatrixReal(sf) );
    }
    else if ( name == "expandCharacters" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        int n = static_cast<const Natural&>( argument ).getValue();
        
        RevBayesCore::AbstractHomologousDiscreteCharacterData *trans_data = this->dag_node->getValue().expandCharacters( n );
        
        return new RevVariable( new AbstractHomologousDiscreteCharacterData(trans_data) );
    }
    else if (name == "getEmpiricalBaseFrequencies")
    {
        found = true;
        
        std::vector<double> ebf = this->dag_node->getValue().getEmpiricalBaseFrequencies();
        
        return new RevVariable( new Simplex(ebf) );
    }
    else if (name == "getNumInvariantSites")
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        bool excl = static_cast<const RlBoolean&>( argument ).getValue();
        
        size_t n = this->dag_node->getValue().getNumberOfInvariantSites( excl );
        
        return new RevVariable( new Natural(n) );
    }
    else if (name == "isHomologous")
    {
        found = true;
        
        bool ih = this->dag_node->getValue().isHomologyEstablished();
        
        return new RevVariable( new RlBoolean(ih) );
    }
    else if ( name == "maxGcContent" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        bool excl = static_cast<const RlBoolean&>( argument ).getValue();
        
        double max_gc = this->dag_node->getValue().maxGcContent( excl );
        
        return new RevVariable( new Probability(max_gc) );
    }
    else if ( name == "maxInvariableBlockLength" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        bool excl = static_cast<const RlBoolean&>( argument ).getValue();
        
        size_t max_inv = this->dag_node->getValue().maxInvariableBlockLength( excl );
        
        return new RevVariable( new Natural(max_inv) );
    }
    else if ( name == "maxPairwiseDifference" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        bool excl = static_cast<const RlBoolean&>( argument ).getValue();
        
        size_t max_pd = this->dag_node->getValue().getMaxPaiwiseSequenceDifference( excl );
        
        return new RevVariable( new Natural(max_pd) );
    }
    else if ( name == "maxVariableBlockLength" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        bool excl = static_cast<const RlBoolean&>( argument ).getValue();
        
        size_t max_var = this->dag_node->getValue().maxVariableBlockLength( excl );
        
        return new RevVariable( new Natural(max_var) );
    }
    else if ( name == "meanGcContent" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        bool excl = static_cast<const RlBoolean&>( argument ).getValue();
        
        double mean_gc = this->dag_node->getValue().meanGcContent( excl );
        
        return new RevVariable( new Probability(mean_gc) );
    }
    else if ( name == "meanGcContentByCodonPosition" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        size_t n = size_t( static_cast<const Natural&>( argument ).getValue() );
        
        const RevObject& excl_argument = args[1].getVariable()->getRevObject();
        bool excl = static_cast<const RlBoolean&>( excl_argument ).getValue();
        
        double mean_gc = this->dag_node->getValue().meanGcContentByCodon( n, excl );
        
        return new RevVariable( new Probability(mean_gc) );
    }
    else if ( name == "minGcContent" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        bool excl = static_cast<const RlBoolean&>( argument ).getValue();
        
        double min_gc = this->dag_node->getValue().minGcContent( excl );
        
        return new RevVariable( new Probability(min_gc) );
    }
    else if ( name == "minPairwiseDifference" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        bool excl = static_cast<const RlBoolean&>( argument ).getValue();
        
        size_t min_pd = this->dag_node->getValue().getMinPaiwiseSequenceDifference( excl );
        
        return new RevVariable( new Natural(min_pd) );
    }
    else if ( name == "numInvariableBlocks" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        bool excl = static_cast<const RlBoolean&>( argument ).getValue();
        
        size_t num_blocks = this->dag_node->getValue().numInvariableSiteBlocks( excl );
        
        return new RevVariable( new Natural(num_blocks) );
    }
    else if ( name == "numTaxaMissingSequence" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        double percentage = static_cast<const Probability&>( argument ).getValue();

        size_t num_taxa = this->dag_node->getValue().numberTaxaMissingSequence( percentage );
        
        return new RevVariable( new Natural(num_taxa) );
    }
    else if (name == "setCodonPartition")
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        RevBayesCore::AbstractHomologousDiscreteCharacterData &v = dag_node->getValue();
        size_t nChars = v.getNumberOfCharacters();
        
        // e.g. data.setCodonPartition(sites=v(3))
        if ( argument.isType( Natural::getClassTypeSpec() ) )
        {
            size_t n = size_t( static_cast<const Natural&>( argument ).getValue() );
            size_t i = 0; // index of included characters
            for (size_t j = 0; j < nChars; j++)
            {
                // only set codon partition for previously included characters
                if ( !v.isCharacterExcluded(j) )
                {
                    if ( i % 3 == (n-1) )
                    {
                        v.includeCharacter(j);
                    }
                    else
                    {
                        v.excludeCharacter(j);
                    }
                    i++;
                }
            }
            
        }
        
        // e.g. data.setCodonPartition(sites=v(1,2))
        else if ( argument.isType( ModelVector<Natural>::getClassTypeSpec() ) )
        {
            const ModelVector<Natural>& x = static_cast<const ModelVector<Natural>&>( argument );
            if (x.size() == 0)
            {
                return NULL;
            }
            
            size_t i = 0; // index of included characters
            for (size_t j = 0; j < nChars; j++)
            {
                // only set codon partition for previously included characters
                if ( !v.isCharacterExcluded(j) )
                {
                    bool included_codon = false;
                    for (size_t k = 0; k < x.size(); k++)
                    {
                        size_t n = x[k];
                        if ( i % 3 == (n-1) )
                        {
                            v.includeCharacter(j);
                            included_codon = true;
                            break;
                        }
                    }
                    if ( !included_codon )
                    {
                        v.excludeCharacter(j);
                    }
                    i++;
                }
            }
        }
        return NULL;
    }
    else if (name == "setNumStatesPartition")
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        RevBayesCore::AbstractHomologousDiscreteCharacterData &v = dag_node->getValue();
        size_t nChars = v.getNumberOfCharacters();
        size_t nTaxa = v.getNumberOfTaxa();
        
        // e.g. data.setNumStatesPartition(2)
        size_t n = size_t( static_cast<const Natural&>( argument ).getValue() );
        for (size_t i = 0; i < nChars; i++)
        {
            int max = 0;
            for (size_t j = 0; j < nTaxa; j++)
            {
                const RevBayesCore::AbstractDiscreteTaxonData& td = v.getTaxonData(j);
                if ( td.getCharacter(i).isMissingState() == false && td.getCharacter(i).isGapState() == false)
                {
                    if(td.getCharacter(i).getNumberObservedStates() > 1)
                    {
                        const RevBayesCore::RbBitSet& state = td.getCharacter(i).getState();
                        for(size_t k = 0; k < state.size(); k++)
                        {
                            if(state.isSet(k) && k +1 > max)
                            {
                                max = k+1;
                            }
                        }
                    }
                    else
                    {
                        int k = int(td.getCharacter(i).getStateIndex()) + 1;
                        if (k > max)
                        {
                            max = k;
                        }
                    }
                }
            }
            
            if (max == n)
            {
                v.includeCharacter(i);
            }
            else
            {
                v.excludeCharacter(i);
            }
            
        }
        return NULL;
    }
    else if ( name == "translateCharacters" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        const std::string &type = static_cast<const RlString&>( argument ).getValue();
        
        RevBayesCore::AbstractHomologousDiscreteCharacterData *trans_data = this->dag_node->getValue().translateCharacters( type );
        
        return new RevVariable( new AbstractHomologousDiscreteCharacterData(trans_data) );
    }
    else if ( name == "varGcContent" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        bool excl = static_cast<const RlBoolean&>( argument ).getValue();
        
        double var_gc = this->dag_node->getValue().varGcContent( excl );
        
        return new RevVariable( new Probability(var_gc) );
    }
    else if ( name == "varGcContentByCodonPosition" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        size_t n = size_t( static_cast<const Natural&>( argument ).getValue() );
        
        const RevObject& excl_argument = args[1].getVariable()->getRevObject();
        bool excl = static_cast<const RlBoolean&>( excl_argument ).getValue();
        
        double var_gc = this->dag_node->getValue().varGcContentByCodon( n, excl );
        
        return new RevVariable( new Probability(var_gc) );
    }
    
    return HomologousCharacterData::executeMethod( name, args, found );
}


/* Get Rev type of object */
const std::string& AbstractHomologousDiscreteCharacterData::getClassType(void)
{
    
    static std::string rev_type = "AbstractHomologousDiscreteCharacterData";
    
    return rev_type;
}

/* Get class type spec describing type of object */
const TypeSpec& AbstractHomologousDiscreteCharacterData::getClassTypeSpec(void)
{
    
    static TypeSpec rev_type_spec = TypeSpec( getClassType(), new TypeSpec( HomologousCharacterData::getClassTypeSpec() ) );
    
    return rev_type_spec;
}


RevBayesCore::Typeddag_node<RevBayesCore::AbstractHomologousDiscreteCharacterData>* AbstractHomologousDiscreteCharacterData::getdag_node( void ) const
{
    
    return dag_node;
}


/** Get the type spec of this class. We return a member variable because instances might have different element types. */
const TypeSpec& AbstractHomologousDiscreteCharacterData::getTypeSpec(void) const
{
    
    static TypeSpec type_spec = getClassTypeSpec();
    return type_spec;
}


const RevBayesCore::AbstractHomologousDiscreteCharacterData& AbstractHomologousDiscreteCharacterData::getValue( void ) const
{
    
    if ( dag_node == NULL )
    {
        throw RbException( "Invalid attempt to get value from an object with NULL DAG node" );
    }
    
    return dag_node->getValue();
}


RevBayesCore::AbstractHomologousDiscreteCharacterData& AbstractHomologousDiscreteCharacterData::getValue( void )
{
    
    if ( dag_node == NULL )
    {
        throw RbException( "Invalid attempt to get value from an object with NULL DAG node" );
    }
    
    return dag_node->getValue();
}


/**
 * Is the object or any of its upstream members or elements
 * modifiable by the user through assignment? We simply ask
 * our DAG node.
 */
bool AbstractHomologousDiscreteCharacterData::isAssignable( void ) const
{
    if ( dag_node == NULL )
    {
        return false;
    }
    
    return dag_node->isAssignable();
}


bool AbstractHomologousDiscreteCharacterData::isConstant( void ) const
{
    
    return dag_node->isConstant();
}


bool AbstractHomologousDiscreteCharacterData::isModelObject( void ) const
{
    
    return true;
}


void AbstractHomologousDiscreteCharacterData::initMethods( void )
{

    // add the DAG node member methods
    // note that this is a sage case because all DAG nodes are member objects
    if ( dag_node != NULL )
    {
        const MethodTable &dagMethods = dynamic_cast<RevMemberObject*>( dag_node )->getMethods();
        methods.insertInheritedMethods( dagMethods );
    }
    
    // insert the character data specific methods
    MethodTable charDataMethods = getCharacterDataMethods();
    methods.insertInheritedMethods( charDataMethods );
    
    ArgumentRules* chartypeArgRules                 = new ArgumentRules();
    ArgumentRules* compStateFreqArgRules            = new ArgumentRules();
    ArgumentRules* empiricalBaseArgRules            = new ArgumentRules();
    ArgumentRules* ishomologousArgRules             = new ArgumentRules();
    ArgumentRules* invSitesArgRules                 = new ArgumentRules();
    ArgumentRules* setCodonPartitionArgRules        = new ArgumentRules();
    ArgumentRules* setCodonPartitionArgRules2       = new ArgumentRules();
    ArgumentRules* setNumStatesPartitionArgRules    = new ArgumentRules();
    ArgumentRules* squareBracketArgRules            = new ArgumentRules();
    
    ArgumentRules* maxGcContentArgRules                 = new ArgumentRules();
    ArgumentRules* maxInvariableBlockLengthArgRules     = new ArgumentRules();
    ArgumentRules* maxVariableBlockLengthArgRules       = new ArgumentRules();
    ArgumentRules* minGcContentArgRules                 = new ArgumentRules();
    ArgumentRules* maxPairwiseDifferenceArgRules        = new ArgumentRules();
    ArgumentRules* minPairwiseDifferenceArgRules        = new ArgumentRules();
    ArgumentRules* meanGcContentArgRules                = new ArgumentRules();
    ArgumentRules* meanGcContentByCodonPositionArgRules = new ArgumentRules();
    ArgumentRules* numInvariableBlocksArgRules          = new ArgumentRules();
    ArgumentRules* numTaxaMissingSequenceArgRules       = new ArgumentRules();
    ArgumentRules* varGcContentArgRules                 = new ArgumentRules();
    ArgumentRules* varGcContentByCodonPositionArgRules  = new ArgumentRules();
    
    ArgumentRules* translateCharactersArgRules          = new ArgumentRules();
    ArgumentRules* expandCharactersArgRules             = new ArgumentRules();
    
    
    setCodonPartitionArgRules->push_back(       new ArgumentRule("",        Natural::getClassTypeSpec()              , "The index of the codon position.", ArgumentRule::BY_VALUE, ArgumentRule::ANY ) );
    setCodonPartitionArgRules2->push_back(      new ArgumentRule("",        ModelVector<Natural>::getClassTypeSpec() , "The indicies of the codon positions.", ArgumentRule::BY_VALUE, ArgumentRule::ANY ) );
    setNumStatesPartitionArgRules->push_back(   new ArgumentRule("",        Natural::getClassTypeSpec()              , "The number of states in this partition.", ArgumentRule::BY_VALUE, ArgumentRule::ANY ) );
    squareBracketArgRules->push_back(           new ArgumentRule( "index" , Natural::getClassTypeSpec()              , "The index of the taxon.", ArgumentRule::BY_VALUE, ArgumentRule::ANY  ) );

    expandCharactersArgRules->push_back(                new ArgumentRule( "factor"           , Natural::getClassTypeSpec()            , "The factor by which the state space is expanded.", ArgumentRule::BY_VALUE, ArgumentRule::ANY  ) );
    invSitesArgRules->push_back(                        new ArgumentRule( "excludeAmbiguous" , RlBoolean::getClassTypeSpec()          , "Should we exclude ambiguous and missing characters?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean( false )  ) );
    maxGcContentArgRules->push_back(                    new ArgumentRule( "excludeAmbiguous" , RlBoolean::getClassTypeSpec()          , "Should we exclude ambiguous and missing characters?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean( false )  ) );
    maxInvariableBlockLengthArgRules->push_back(        new ArgumentRule( "excludeAmbiguous" , RlBoolean::getClassTypeSpec()          , "Should we exclude ambiguous and missing characters?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean( false )  ) );
    maxVariableBlockLengthArgRules->push_back(          new ArgumentRule( "excludeAmbiguous" , RlBoolean::getClassTypeSpec()          , "Should we exclude ambiguous and missing characters?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean( false )  ) );
    minGcContentArgRules->push_back(                    new ArgumentRule( "excludeAmbiguous" , RlBoolean::getClassTypeSpec()          , "Should we exclude ambiguous and missing characters?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean( false )  ) );
    maxPairwiseDifferenceArgRules->push_back(           new ArgumentRule( "excludeAmbiguous" , RlBoolean::getClassTypeSpec()          , "Should we exclude ambiguous and missing characters?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean( false )  ) );
    minPairwiseDifferenceArgRules->push_back(           new ArgumentRule( "excludeAmbiguous" , RlBoolean::getClassTypeSpec()          , "Should we exclude ambiguous and missing characters?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean( false )  ) );
    meanGcContentArgRules->push_back(                   new ArgumentRule( "excludeAmbiguous" , RlBoolean::getClassTypeSpec()          , "Should we exclude ambiguous and missing characters?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean( false )  ) );
    meanGcContentByCodonPositionArgRules->push_back(    new ArgumentRule( "index" , Natural::getClassTypeSpec()          , "The index of the codon position.", ArgumentRule::BY_VALUE, ArgumentRule::ANY  ) );
    meanGcContentByCodonPositionArgRules->push_back(    new ArgumentRule( "excludeAmbiguous" , RlBoolean::getClassTypeSpec()          , "Should we exclude ambiguous and missing characters?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean( false )  ) );
    numInvariableBlocksArgRules->push_back(             new ArgumentRule( "excludeAmbiguous" , RlBoolean::getClassTypeSpec()          , "Should we exclude ambiguous and missing characters?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean( false )  ) );
    numTaxaMissingSequenceArgRules->push_back(          new ArgumentRule( "x" ,     Probability::getClassTypeSpec()          , "The percentage/threshold for the missing sequence.", ArgumentRule::BY_VALUE, ArgumentRule::ANY  ) );
    translateCharactersArgRules->push_back(             new ArgumentRule( "type" ,     RlString::getClassTypeSpec()          , "The character type into which we want to translate.", ArgumentRule::BY_VALUE, ArgumentRule::ANY  ) );
    varGcContentArgRules->push_back(                    new ArgumentRule( "excludeAmbiguous" , RlBoolean::getClassTypeSpec()          , "Should we exclude ambiguous and missing characters?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean( false )  ) );
    varGcContentByCodonPositionArgRules->push_back(     new ArgumentRule( "index" , Natural::getClassTypeSpec()          , "The index of the codon position.", ArgumentRule::BY_VALUE, ArgumentRule::ANY  ) );
    varGcContentByCodonPositionArgRules->push_back(     new ArgumentRule( "excludeAmbiguous" , RlBoolean::getClassTypeSpec()          , "Should we exclude ambiguous and missing characters?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean( false )  ) );
    
    
    methods.addFunction( new MemberProcedure( "chartype",                       RlString::getClassTypeSpec(),       chartypeArgRules                ) );
    methods.addFunction( new MemberProcedure( "computeStateFrequencies",        RlString::getClassTypeSpec(),       compStateFreqArgRules           ) );
    methods.addFunction( new MemberProcedure( "setCodonPartition",              RlUtils::Void,                      setCodonPartitionArgRules       ) );
    methods.addFunction( new MemberProcedure( "setCodonPartition",              RlUtils::Void,                      setCodonPartitionArgRules2      ) );
    methods.addFunction( new MemberProcedure( "setNumStatesPartition",          RlUtils::Void,                      setNumStatesPartitionArgRules   ) );
    methods.addFunction( new MemberProcedure( "isHomologous",                   RlBoolean::getClassTypeSpec(),      ishomologousArgRules            ) );
    methods.addFunction( new MemberProcedure( "expandCharacters",               AbstractHomologousDiscreteCharacterData::getClassTypeSpec(),        expandCharactersArgRules         ) );
    methods.addFunction( new MemberProcedure( "getEmpiricalBaseFrequencies",    Simplex::getClassTypeSpec(),        empiricalBaseArgRules           ) );
    methods.addFunction( new MemberProcedure( "getNumInvariantSites",           Natural::getClassTypeSpec(),        invSitesArgRules                ) );
    methods.addFunction( new MemberProcedure( "maxGcContent",                   Probability::getClassTypeSpec(),    maxGcContentArgRules                ) );
    methods.addFunction( new MemberProcedure( "maxInvariableBlockLength",       Natural::getClassTypeSpec(),        maxInvariableBlockLengthArgRules    ) );
    methods.addFunction( new MemberProcedure( "maxVariableBlockLength",         Natural::getClassTypeSpec(),        maxVariableBlockLengthArgRules      ) );
    methods.addFunction( new MemberProcedure( "minGcContent",                   Probability::getClassTypeSpec(),    minGcContentArgRules                ) );
    methods.addFunction( new MemberProcedure( "maxPairwiseDifference",          Natural::getClassTypeSpec(),        maxPairwiseDifferenceArgRules       ) );
    methods.addFunction( new MemberProcedure( "minPairwiseDifference",          Natural::getClassTypeSpec(),        minPairwiseDifferenceArgRules       ) );
    methods.addFunction( new MemberProcedure( "meanGcContent",                  Probability::getClassTypeSpec(),    meanGcContentArgRules                ) );
    methods.addFunction( new MemberProcedure( "meanGcContentByCodonPosition",   Probability::getClassTypeSpec(),    meanGcContentByCodonPositionArgRules                ) );
    methods.addFunction( new MemberProcedure( "numInvariableBlocks",            Natural::getClassTypeSpec(),        numInvariableBlocksArgRules         ) );
    methods.addFunction( new MemberProcedure( "numTaxaMissingSequence",         Natural::getClassTypeSpec(),        numTaxaMissingSequenceArgRules         ) );
    methods.addFunction( new MemberProcedure( "translateCharacters",            AbstractHomologousDiscreteCharacterData::getClassTypeSpec(),        translateCharactersArgRules         ) );
    methods.addFunction( new MemberProcedure( "varGcContent",                   Probability::getClassTypeSpec(),    varGcContentArgRules                ) );
    methods.addFunction( new MemberProcedure( "varGcContentByCodonPosition",    Probability::getClassTypeSpec(),    varGcContentByCodonPositionArgRules                ) );
    methods.addFunction( new MemberProcedure( "[]",                             AbstractDiscreteTaxonData::getClassTypeSpec(), squareBracketArgRules) );
    
}


void AbstractHomologousDiscreteCharacterData::makeConstantValue( void )
{
    
    if ( dag_node == NULL )
    {
        throw RbException("Cannot convert a variable without value to a constant value.");
    }
    else if ( dag_node->getdag_nodeType() != RevBayesCore::dag_node::CONSTANT )
    {
        RevBayesCore::ConstantNode<valueType>* newNode = new ConstantNode<valueType>(dag_node->getName(), RevBayesCore::Cloner<valueType, IsDerivedFrom<valueType, RevBayesCore::Cloneable>::Is >::createClone( dag_node->getValue() ) );
        dag_node->replace(newNode);
        
        // delete the value if there are no other references to it.
        if ( dag_node->decrementReferenceCount() == 0 )
        {
            delete dag_node;
        }
        
        dag_node = newNode;
        
        // increment the reference counter
        dag_node->incrementReferenceCount();
    }
    
}


/**
 * Make an indirect reference to the variable. This is appropriate for the contexts
 * where the object occurs on the righ-hand side of expressions like a := b
 */
AbstractHomologousDiscreteCharacterData* AbstractHomologousDiscreteCharacterData::makeIndirectReference(void)
{
    
    RevBayesCore::IndirectReferenceFunction< valueType > *func = new RevBayesCore::IndirectReferenceFunction<valueType>( this->getdag_node() );
    RevBayesCore::DeterministicNode< valueType >* newNode = new RevBayesCore::DeterministicNode< valueType >( "", func );
    
    AbstractHomologousDiscreteCharacterData* newObj = this->clone();
    
    const std::vector<RevBayesCore::Move*>& mvs = newObj->getdag_node()->getMoves();
    while ( mvs.empty() == false )
    {
        newObj->getdag_node()->removeMove( *mvs.begin() );
    }
    
    newObj->setdag_node( newNode );
    
    return newObj;
}


/** Convert a model object to a deterministic object, the value of which is determined by a user-defined Rev function */
void AbstractHomologousDiscreteCharacterData::makeUserFunctionValue( UserFunction* fxn )
{
    UserFunctionNode< AbstractHomologousDiscreteCharacterData >*  detNode = new UserFunctionNode< AbstractHomologousDiscreteCharacterData >( "", fxn );
    
    // Signal replacement and delete the value if there are no other references to it.
    if ( dag_node != NULL )
    {
        dag_node->replace( detNode );
        if ( dag_node->decrementReferenceCount() == 0 )
            delete dag_node;
    }
    
    // Shift the actual node
    dag_node = detNode;
    
    // Increment the reference counter
    dag_node->incrementReferenceCount();
}


/**
 * Print value for user. The DAG node pointer may be NULL, in which
 * case we print "NA".
 */
void AbstractHomologousDiscreteCharacterData::printValue(std::ostream &o, bool user) const
{
    if ( dag_node == NULL )
    {
        o << "NA";
    }
    else
    {
        dag_node->printValue( o, "," );
    }
    
}


/** Copy name of variable onto DAG node, if it is not NULL */
void AbstractHomologousDiscreteCharacterData::setName(std::string const &n)
{
    if ( dag_node != NULL )
    {
        dag_node->setName( n );
    }
    
}


/**
 * Set dag node. We also accommodate the possibility of setting the DAG node to null.
 */
void AbstractHomologousDiscreteCharacterData::setdag_node(RevBayesCore::dag_node* newNode)
{
    
    // Take care of the old value node
    if ( dag_node != NULL )
    {
        if ( newNode != NULL )
        {
            newNode->setName( dag_node->getName() );
        }
        
        dag_node->replace(newNode);
        
        if ( dag_node->decrementReferenceCount() == 0 )
        {
            delete dag_node;
        }
        
    }
    
    // Set the new value node
    dag_node = static_cast< RevBayesCore::Typeddag_node<valueType>* >( newNode );
    
    // Increment the reference count to the new value node
    if ( dag_node != NULL )
    {
        dag_node->incrementReferenceCount();
    }
    
}


void AbstractHomologousDiscreteCharacterData::setValue(valueType *x)
{
    
    RevBayesCore::ConstantNode<valueType>* newNode;
    
    if ( dag_node == NULL )
    {
        newNode = new ConstantNode<valueType>("",x);
    }
    else
    {
        newNode = new ConstantNode<valueType>(dag_node->getName(),x);
        dag_node->replace(newNode);
        
        if ( dag_node->decrementReferenceCount() == 0 )
        {
            delete dag_node;
        }
        
    }
    
    dag_node = newNode;
    
    // increment the reference count to the value
    dag_node->incrementReferenceCount();
    
}
