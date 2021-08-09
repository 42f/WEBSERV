#include "TypesMime.hpp"

namespace files {

/* ............................... CONSTRUCTOR ...............................*/

bool	TypesMime::init( File & conf_mine )	{

	if conf_mine.isGood() {
		while (conf_mine.isGood()) {
			// READ CONF FILE, POPULATE MAP
		}
		return true;
	}
	else
		return false;
}

/* ..............................COPY CONSTRUCTOR.............................*/

/*
TypesMime::TypesMime( const TypesMime & src ) {
}
*/

/* ................................ DESTRUCTOR ...............................*/

TypesMime::~TypesMime( void )	{
}

/* ................................. METHODS .................................*/



/* ................................. ACCESSOR ................................*/

std::string & const	TypesMine::getMime(std::string & const extension) const {

	typesMap::iterator	it = types.find(extension);
	if (it == types.end())
		return "";
	else
		return it->second;
}

/* ................................. OVERLOAD ................................*/

/*
TypesMime &				TypesMime::operator=( TypesMime const & rhs )	{
	if ( this != &rhs )	{
		this->_value = rhs.getValue();
	}
	return *this;
}
*/

/*
std::ostream &			operator<<( std::ostream & o, TypesMime const & i )	{
	o << "Value = " << i.getValue();
	return o;
}
*/


/* ................................... DEBUG .................................*/

/*
void TypesMime::debugPrint( void ) const {

}
*/

/* ................................. END CLASS................................*/
} // end namespace files
