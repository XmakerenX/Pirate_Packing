#include "CreatureNew.h"

template <class Encoding>
//constructor that creates a random creature based on the configuration;
CreatureNew<Encoding>::CreatureNew(Configuration* config)
    :encoding(config)
{
    
}

template <class Encoding>
//constructor that creates a creature based of a premade encoding
CreatureNew<Encoding>::CreatureNew(Encoding& encoding, Configuration* config)
    :encoding(config)
{
    
}
template <class Encoding>
//constructor that creates a creature based of a premade encoding
CreatureNew<Encoding>::CreatureNew(Encoding&& encoding, Configuration* config)
    :encoding(config)
{
    
}

template <class Encoding>
CreatureNew<Encoding>::CreatureNew(const CreatureNew<Encoding>& copy)
    :encoding(copy.encoding.getConfiguration())
{
    
}

template <class Encoding>
CreatureNew<Encoding>::CreatureNew(CreatureNew&& move)
    :encoding(move.encoding.getConfiguration())
{
    
}

template <class Encoding>
CreatureNew<Encoding>::~CreatureNew()
{
    
}
        
template <class Encoding>    
CreatureNew<Encoding>& CreatureNew<Encoding>::operator=(const CreatureNew<Encoding>& c)
{
    return *this;
}

template <class Encoding>
CreatureNew<Encoding>& CreatureNew<Encoding>::operator=(CreatureNew<Encoding>&& c)
{
    return *this;
}

template <class Encoding>
bool CreatureNew<Encoding>::operator==(const CreatureNew<Encoding>& b)
{
    return false;
}

template <class Encoding>
int CreatureNew<Encoding>::getFittness()
{
    return fitness;
}

template <class Encoding>
std::vector<BoxInfo> CreatureNew<Encoding>::getboxesPositions()
{
    return std::vector<BoxInfo>();
}

// Force instantiation of BinaryEncoding and PermutationEncoding
#include "Encoding/BinaryEncoding.h"
#include "Encoding/PermutationEncoding.h"
#include "Configuration.h"
template class CreatureNew<BinaryEncoding>;
template class CreatureNew<PermutationEncoding>;


