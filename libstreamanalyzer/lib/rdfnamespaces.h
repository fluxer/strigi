/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2009 Evgeny Egorochkin <phreedom.stdin@gmail.com>
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

#ifndef STRIGI_RDFNAMESPACES_H
#define STRIGI_RDFNAMESPACES_H

// This is a list of the most often encountered rdf namespaces used by analyzers
// For internal use ONLY and should not be installed
// Typical use: fieldName = NIE "title"

// General
#define STRIGI_RDF       "http://www.w3.org/1999/02/22-rdf-syntax-ns#"

// Nepomuk
#define STRIGI_NAO       "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#"
#define STRIGI_NIE       "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#"
#define STRIGI_NFO       "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#"
#define STRIGI_NCO       "http://www.semanticdesktop.org/ontologies/2007/03/22/nco#"
#define STRIGI_NMO       "http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#"
#define STRIGI_NCAL      "http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#"
#define STRIGI_NEXIF     "http://www.semanticdesktop.org/ontologies/2007/05/10/nexif#"// obsolete
#define STRIGI_NID3      "http://www.semanticdesktop.org/ontologies/2007/05/10/nid3#"// obsolete

// Draft
#define STRIGI_NMM_DRAFT "http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#"
#define STRIGI_NMO_DRAFT "http://www.semanticdesktop.org/ontologies/nmo#"

#endif
