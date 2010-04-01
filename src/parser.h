#ifndef LSST_QSERV_MASTER_PARSER_H
#define LSST_QSERV_MASTER_PARSER_H

// C++ 
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <vector>
// Boost
#include "boost/shared_ptr.hpp"

// Forward
class Substitution;

// Typedefs:
typedef std::map<std::string,std::string> StringMapping;

class Substitution {
public:
    typedef std::pair<std::string, std::string> StringPair;
    typedef std::map<std::string, std::string> Mapping;
    typedef std::vector<StringPair> MapVector;
    typedef std::vector<std::string> StringVector;

    Substitution(std::string template_, std::string const& delim);
    
    std::string transform(Mapping const& m);

private:
    struct Item { //where subs are needed and how big the placeholders are.
	std::string name;
	int position;
	int length;
    };

    template<typename Iter> 
    inline unsigned _max(Iter begin, Iter end) {
	unsigned m=0;
	for(Iter i = begin; i != end; ++i) {
	    if(m < i->size()) m = i->size();
	}
	return m;
    }

    void _build(std::string const& delim);

    std::vector<Item> _index;
    std::string _template;
	
};


class SqlSubstitution {
public:
    typedef StringMapping Mapping;

    SqlSubstitution(std::string const& sqlStatement, Mapping const& mapping);
    
    std::string transform(Mapping const& m);

private:
    typedef boost::shared_ptr<Substitution> SubstPtr;

    void _build(std::string const& sqlStatement, Mapping const& mapping);

    std::string _delimiter;
    std::string _errorMsg;
    SubstPtr _substitution;
};


class ChunkMapping {
public:
    typedef std::map<std::string,std::string> Map;
    typedef Map::value_type MapValue;

    ChunkMapping() {}

    Map getMapping(int chunk, int subChunk);
    void addChunkKey(std::string const& key) { _map[key] = CHUNK; }
    void addSubChunkKey(std::string const& key) { _map[key] = CHUNK_WITH_SUB; }
        
private:
    enum Mode {UNKNOWN, CHUNK, CHUNK_WITH_SUB};
    typedef std::map<std::string, Mode> ModeMap;
    typedef ModeMap::value_type ModeMapValue;

    template <typename T>
    std::string _toString(T const& t) {
	std::ostringstream oss;
	oss << t;
	return oss.str();
    }

    ModeMap _map;
};


#endif // LSST_QSERV_MASTER_PARSER_H
