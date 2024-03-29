/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
 * Copyright (C) 2007 Alexandr Goncearenco <neksa@neksa.net>
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
#include <strigi/fieldproperties.h>
#include <strigi/fieldpropertiesdb.h>
#include "fieldproperties_private.h"
#include <map>

using namespace Strigi;

FieldProperties::FieldProperties() :p(new Private()) {
}
FieldProperties::FieldProperties(const Private& pr) :p(new Private(pr)) {}
FieldProperties::FieldProperties(const std::string& k)
        :p(new Private(k)) {
    const FieldProperties& fp = FieldPropertiesDb::db().properties(k);
    if (fp.valid()) {
        *this = fp;
    }
}
FieldProperties::FieldProperties(const FieldProperties& p)
    :p(new Private(*p.p)) {
}
FieldProperties::~FieldProperties() {
    delete p;
}
const FieldProperties&
FieldProperties::operator=(const FieldProperties& f) {
    *p = *f.p;
    return f;
}
bool
FieldProperties::valid() const {
    return p->uri.size() != 0;
}
const std::string&
FieldProperties::uri() const {
    return p->uri;
}
const std::string&
FieldProperties::name() const {
    return p->name;
}
const std::string&
FieldProperties::typeUri() const {
    return p->typeuri;
}
const std::string&
FieldProperties::description() const {
    return p->description;
}
bool
FieldProperties::binary() const {
    return p->binary;
}
bool
FieldProperties::compressed() const {
    return p->compressed;
}
bool
FieldProperties::indexed() const {
    return p->indexed;
}
bool
FieldProperties::stored() const {
    return p->stored;
}
bool
FieldProperties::tokenized() const {
    return p->tokenized;
}
int
FieldProperties::minCardinality() const {
    return p->min_cardinality;
}
int
FieldProperties::maxCardinality() const {
    return p->max_cardinality;
}
const std::vector<std::string>&
FieldProperties::locales() const {
    return p->locales;
}
const std::string&
FieldProperties::localizedName(const std::string& locale) const {
    std::map<std::string,Localized>::iterator i = p->localized.find(locale);
    return (i == p->localized.end()) ?empty() :i->second.name;
}
const std::string&
FieldProperties::localizedDescription(const std::string& locale) const {
    std::map<std::string,Localized>::iterator i = p->localized.find(locale);
    return (i == p->localized.end()) ?empty() :i->second.description;
}
const std::vector<std::string>&
FieldProperties::parentUris() const {
    return p->parentUris;
}
const std::vector<std::string>&
FieldProperties::childUris() const {
    return p->childUris;
}
const std::vector<std::string>&
FieldProperties::applicableClasses() const {
    return p->applicableClasses;
}
