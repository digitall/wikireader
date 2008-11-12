/*
 * Wiki Handling Tool
 *
 * Copyright (C) 2008 Openmoko Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "CreateIndex.h"
#include <QTextStream>
#include <QRegExp>

CreateIndex::CreateIndex(const QString& fileName, const QString pattern)
    : FileOutputArticleHandler(fileName)
    , m_pattern(pattern)
{}

// TODO recognize redirections and resolve them
void CreateIndex::handleArticle(const Article& article)
{
    QTextStream stream(&m_file); 
    QString title = article.title().title();

    QRegExp filter(m_pattern);                                         
    bool match = filter.exactMatch(title);                                   
    int len = filter.matchedLength();                                       

    if(match){
        stream << title << " ";
        stream << (article.isRedirect() ? article.redirectsTo() : article.hash());
        stream << "\n"; 
    }
}
