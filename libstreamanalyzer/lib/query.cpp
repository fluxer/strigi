/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include <strigi/query.h>

using namespace Strigi;

class Strigi::TermPrivate {
public:
    int64_t i_value;     // value for Integer, Date and Boolean
    double d_value;      // value for Float
    std::string s_value; // value for String

    // flags for String terms (ordered by size)
    std::string language;
    float fuzzy;
    int slack;
    int proximityDistance;
    Term::Type type;
    bool phrase;
    bool caseSensitive;
    bool diacriticSensitive;
    bool ordered;
    bool enableStemming;
    bool wordbased;

    TermPrivate()
        : i_value(0), d_value(0.0), fuzzy(0.0), slack(0), proximityDistance(10)
        , type(Term::String), phrase(true), caseSensitive(true)
        , diacriticSensitive(true), ordered(true), enableStemming(false)
        , wordbased(true)
    {}
};
class Strigi::QueryPrivate {
public:
    // Selection fields
    Term term;
    Term termTwo; // needed for Proximity
    std::vector<std::string> fields;
    // Collectible fields
    std::vector<Query> subs;
    // Shared fields
    float boost;
    bool negate;
    // Type
    Query::Type type;

    bool valid;

    QueryPrivate() :boost(1), negate(false), type(Query::Contains), valid(true)
    {}
};

Term::Term() :p(new TermPrivate()) {
}
Term::Term(const Term& t) :p(new TermPrivate(*t.p)) {
}
Term::~Term() {
    delete p;
}
void
Term::operator=(const Term& t) {
    *p = *t.p;
}
int
Term::proximityDistance() const {
    return p->proximityDistance;
}
void
Term::setProximityDistance(int d) {
    p->proximityDistance = d;
}
bool
Term::caseSensitive() const {
    return p->caseSensitive;
}
void
Term::setCaseSensitive(bool b) {
    p->caseSensitive = b;
}
bool
Term::diacriticSensitive() const {
    return p->diacriticSensitive;
}
void
Term::setDiacriticSensitive(bool b) {
    p->diacriticSensitive = b;
}
bool
Term::stemming() const {
    return p->enableStemming;
}
void
Term::setStemming(bool b) {
    p->enableStemming = b;
}
float
Term::fuzzy() const {
    return p->fuzzy;
}
void
Term::setFuzzy(float f) {
    p->fuzzy = f;
}
int
Term::slack() const {
    return p->slack;
}
void
Term::setSlack(int s) {
    p->slack = s;
}
bool
Term::ordered() const {
    return p->ordered;
}
void
Term::setOrdered(bool b) {
    p->ordered = b;
}
bool
Term::wordBased() const {
    return p->wordbased;
}
void
Term::setWordBased(bool b) {
    p->wordbased = b;
}
void
Term::setValue(const std::string& s) {
    p->s_value = s;
    p->type = String;
}
void
Term::setValue(const char* s) {
    p->s_value.assign(s);
    p->type = String;
}
const std::string&
Term::string() const {
    return p->s_value;
}
std::ostream&
operator<< ( std::ostream &stream, Strigi::Term term ) {
    stream << "<term>" << std::endl;
    stream << "<slack>" << term.slack() << "</slack>" << std::endl;
    stream << "<fuzzy>" << term.fuzzy() << "</fuzzy>" << std::endl;
    stream << "<string>" << term.string() << "</string>" << std::endl;
    stream << "<proximityDistance>" << term.proximityDistance()
            << "</proximityDistance>" << std::endl;
    stream << "<stemming>" << ( term.stemming() ? "yes" : "no" )
            << "</stemming>" << std::endl;
    stream << "<ordered>" << ( term.ordered() ? "yes" : "no" )
            << "</ordered>" << std::endl;
    stream << "<diacriticSensitive>"
            << ( term.diacriticSensitive() ? "yes" : "no" )
            << "</diacriticSensitive>" << std::endl;
    stream << "<wordbased>" << ( term.wordBased() ? "yes" : "no" )
            << "</wordbased>" << std::endl;
    stream << "<caseSensitive>" << ( term.caseSensitive() ? "yes" : "no" )
            << "</caseSensitive>" << std::endl;
    stream << "</term>" << std::endl;
    return stream;
}
Query::Query() :p(new QueryPrivate()) {
}
Query::Query(const Query& t) :p(new QueryPrivate(*t.p)) {
}
Query::~Query() {
    delete p;
}
void
Query::operator=(const Query& q) {
    *p = *q.p;
}
Term&
Query::term() {
    return p->term;
}
const Term&
Query::term() const {
    return p->term;
}
void
Query::setTerm(const Term& t) {
    p->term = t;
}
float
Query::boost() const {
    return p->boost;
}
void
Query::setBoost(float b) {
    p->boost = b;
}
Query::Type
Query::type() const {
    return p->type;
}
void
Query::setType(Type t) {
    p->type = t;
}
bool
Query::negate() const {
    return p->negate;
}
void
Query::setNegate(bool n) {
    p->negate = n;
}
const std::vector<std::string>&
Query::fields() const {
    return p->fields;
}
std::vector<std::string>&
Query::fields() {
    return p->fields;
}
const std::vector<Query>&
Query::subQueries() const {
    return p->subs;
}
std::vector<Query>&
Query::subQueries() {
    return p->subs;
}
bool
Query::valid() const {
    return p->valid;
}
std::ostream &
operator<< ( std::ostream &stream, Strigi::Query query )
{
    stream << "<query>" << std::endl;
    stream << query.term();
    stream << "<Boost>" << query.boost() << "</Boost>" << std::endl;

    stream << "<negate>" << ( query.negate() ? "yes" : "no" )
            << "</negate>" << std::endl;

    for ( std::vector<std::string>::iterator it = query.fields().begin();
          it != query.fields().end(); ++it )
    {
        stream << "<field>"<< *it << "</field>" << std::endl;
    }

    if ( query.fields().size() == 0 )
        stream << "<field/>" << std::endl;

    std::string typeTag;

    switch ( query.type() )
    {
        case Strigi::Query::And:
            stream << "<And>" << std::endl;
            typeTag = "And";
            break;
        case Strigi::Query::Or:
            stream << "<Or>" << std::endl;
            typeTag = "Or";
            break;
        case Strigi::Query::Proximity:
            stream << "<Proximity>" << std::endl;
            typeTag = "Proximity";
            break;
        case Strigi::Query::RegExp:
            stream << "<RegExp>" << std::endl;
            typeTag = "RegExp";
            break;
        case Strigi::Query::Equals:
            stream << "<Equals>" << std::endl;
            typeTag = "Equals";
            break;
        case Strigi::Query::Contains:
            stream << "<Contains>" << std::endl;
            typeTag = "Contains";
            break;
        case Strigi::Query::LessThan:
            stream << "<LessThan>" << std::endl;
            typeTag = "LessThan";
            break;
        case Strigi::Query::LessThanEquals:
            stream << "<LessThanEquals>" << std::endl;
            typeTag = "LessThanEquals";
            break;
        case Strigi::Query::GreaterThan:
            stream << "<GreaterThan>" << std::endl;
            typeTag = "GreaterThan";
            break;
        case Strigi::Query::GreaterThanEquals:
            stream << "<GreaterThanEquals>" << std::endl;
            typeTag = "GreaterThanEquals";
            break;
        case Strigi::Query::StartsWith:
            stream << "<StartsWith>" << std::endl;
            typeTag = "StartsWith";
            break;
        case Strigi::Query::FullText:
            stream << "<FullText>" << std::endl;
            typeTag = "FullText";
            break;
        case Strigi::Query::Keyword:
            stream << "<Keyword>" << std::endl;
            typeTag = "Keyword";
            break;
    }

    if (!query.subQueries().empty())
    {
        stream << "<subQueries>" << std::endl;
        unsigned int counter = 0;
        for ( std::vector<Strigi::Query>::iterator it = query.subQueries().begin();
              it != query.subQueries().end(); it++, counter++ )
        {
            Strigi::Query subQuery = *it;
            stream << subQuery;
        }
        stream << "</subQueries>" << std::endl;
    }

    stream << "</" << typeTag << ">" << std::endl;

    stream << "</query>" << std::endl;
    return stream;
}
