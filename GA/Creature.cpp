#include "Creature.h"

template <class Encoding>
//constructor that creates a random creature based on the configuration;
Creature<Encoding>::Creature(Configuration* config)
	:encoding(config), configuration(config)
{
    
}
//------------------------------------------------------------------------------------
template <class Encoding>
//constructor that creates a creature based of a premade encoding
Creature<Encoding>::Creature(Encoding& encoding, Configuration* config)
    :encoding(config), configuration(config)
{
   
}
//------------------------------------------------------------------------------------
template <class Encoding>
//constructor that creates a creature based of a premade encoding
Creature<Encoding>::Creature(Encoding&& encoding, Configuration* config)
    :encoding(config), configuration(config)
{
    
}
//------------------------------------------------------------------------------------
template <class Encoding>
Creature<Encoding>::Creature(const Creature<Encoding>& copy)
    :encoding(copy.encoding.getConfiguration()), configuration(copy.configuration)
{
    
}
//------------------------------------------------------------------------------------
template <class Encoding>
Creature<Encoding>::Creature(Creature&& move)
    :encoding(move.encoding.getConfiguration()), configuration(move.configuration)
{
    
}
//------------------------------------------------------------------------------------
template <class Encoding>
Creature<Encoding>::~Creature()
{
    
}
//------------------------------------------------------------------------------------
template <class Encoding>    
Creature<Encoding>& Creature<Encoding>::operator=(const Creature<Encoding>& c)
{
    return *this;
}
//------------------------------------------------------------------------------------
template <class Encoding>
Creature<Encoding>& Creature<Encoding>::operator=(Creature<Encoding>&& c)
{
    return *this;
}
//------------------------------------------------------------------------------------
template <class Encoding>
bool Creature<Encoding>::operator==(const Creature<Encoding>& b)
{
    return false;
}
//------------------------------------------------------------------------------------
template <class Encoding>
int Creature<Encoding>::getFittness()
{
    return fitness;
}
//------------------------------------------------------------------------------------
template <class Encoding>
Configuration* Creature<Encoding>:: getConfiguration()
{
	return this->configuration;
}
//------------------------------------------------------------------------------------
template <class Encoding>
std::vector<BoxInfo> Creature<Encoding>::getboxesPositions()
{
    return std::vector<BoxInfo>();
}
//------------------------------------------------------------------------------------
// Force instantiation of BinaryEncoding and PermutationEncoding
#include "Encoding/BinaryEncoding.h"
#include "Encoding/PermutationEncoding.h"
#include "Configuration.h"
template class Creature<BinaryEncoding>;
template class Creature<PermutationEncoding>;




