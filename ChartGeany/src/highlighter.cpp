/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "highlighter.h"

//! [0]
Highlighter::Highlighter(QTextDocument *parent)
  : QSyntaxHighlighter(parent)
{
  HighlightingRule rule;

  keywordFormat.setForeground(Qt::darkBlue);
  keywordFormat.setFontWeight(QFont::Bold);
  QStringList keywordPatterns;
  keywordPatterns << "\\bchar\\b" << "\\bconst\\b"
                  << "\\bfloat\\b" << "\\bdouble\\b"
                  << "\\benum\\b" << "\\bArray_t\\b"
                  << "\\binline\\b" << "\\bint\\b"
                  << "\\blong\\b" << "\\bString_t\\b"
                  << "\\bTimeFrame_t\\b"
                  << "\\bshort\\b" << "\\bsigned\\b"
                  << "\\bstatic\\b" << "\\bstruct\\b"
                  << "\\btypedef\\b" << "\\breturn\\b"
                  << "\\bunion\\b" << "\\bunsigned\\b"
                  << "\\bvoid\\b" << "\\bvolatile\\b"
                  << "\\bModuleLocal\\b" << "\\bbool\\b"
                  << "\\bint8_t\\b" << "\\buint8_t\\b"
                  << "\\bint16_t\\b" << "\\buint16_t\\b"
                  << "\\bint32_t\\b" << "\\buint32_t\\b"
                  << "\\bint64_t\\b" << "\\buint64_t\\b"
                  << "\\bObjectHandler_t\\b" << "\\bProperty\\b"
                  << "\\bColor_t\\b" << "\\bcase\\b"
                  << "\\bdefault:\\b" << "\\else\\b";
  foreach (const QString &pattern, keywordPatterns)
  {
    rule.pattern = QRegExp(pattern);
    rule.format = keywordFormat;
    highlightingRules.append(rule);
//! [0] //! [1]
  }
//! [1]

//! [2]
  QStringList functionPatterns;
  functionPatterns << "\\bInit\\b" << "\\bLoop\\b" << "\\bEvent\\b";

  classFormat.setFontWeight(QFont::Bold);
  classFormat.setForeground(Qt::darkMagenta);
  foreach (const QString &pattern, functionPatterns)
  {
    rule.pattern = QRegExp(pattern);
    rule.format = classFormat;
    highlightingRules.append(rule);
  }
//! [2]

//! [3]
  singleLineCommentFormat.setForeground(Qt::red);
  rule.pattern = QRegExp("//[^\n]*");
  rule.format = singleLineCommentFormat;
  highlightingRules.append(rule);

  multiLineCommentFormat.setForeground(Qt::red);
//! [3]

//! [4]
  quotationFormat.setForeground(Qt::darkGreen);
  rule.pattern = QRegExp("\".*\"");
  rule.format = quotationFormat;
  highlightingRules.append(rule);
//! [4]

//! [5]
  functionFormat.setFontItalic(true);
  functionFormat.setForeground(Qt::blue);
  rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
  rule.format = functionFormat;
  highlightingRules.append(rule);
//! [5]

//! [6]
  commentStartExpression = QRegExp("/\\*");
  commentEndExpression = QRegExp("\\*/");
}
//! [6]

//! [7]
void Highlighter::highlightBlock(const QString &text)
{
  foreach (const HighlightingRule &rule, highlightingRules)
  {
    QRegExp expression(rule.pattern);
    int index = expression.indexIn(text);
    while (index >= 0)
    {
      int length = expression.matchedLength();
      setFormat(index, length, rule.format);
      index = expression.indexIn(text, index + length);
    }
  }
//! [7] //! [8]
  setCurrentBlockState(0);
//! [8]

//! [9]
  int startIndex = 0;
  if (previousBlockState() != 1)
    startIndex = commentStartExpression.indexIn(text);

//! [9] //! [10]
  while (startIndex >= 0)
  {
//! [10] //! [11]
    int endIndex = commentEndExpression.indexIn(text, startIndex);
    int commentLength;
    if (endIndex == -1)
    {
      setCurrentBlockState(1);
      commentLength = text.length() - startIndex;
    }
    else
    {
      commentLength = endIndex - startIndex
                      + commentEndExpression.matchedLength();
    }
    setFormat(startIndex, commentLength, multiLineCommentFormat);
    startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
  }
}
//! [11]


//! [0]
CompileHighlighter::CompileHighlighter(QTextDocument *parent)
  : QSyntaxHighlighter(parent)
{
  HighlightingRule rule;

  fatalErrorFormat.setForeground(Qt::darkYellow);
  fatalErrorFormat.setFontWeight(QFont::Bold);
  QStringList fatalErrorPatterns;
  fatalErrorPatterns << "\\b: fatal error\\b";
  foreach (const QString &pattern, fatalErrorPatterns)
  {
    rule.pattern = QRegExp(pattern);
    rule.format = fatalErrorFormat;
    highlightingRules.append(rule);
  }

  errorFormat.setForeground(Qt::darkRed);
  errorFormat.setFontWeight(QFont::Bold);
  QStringList errorPatterns;
  errorPatterns << "\\b: error\\b" << "\\bcompilation failed on: \\b"
                << "\\bmodule loading failed\\b";
  foreach (const QString &pattern, errorPatterns)
  {
    rule.pattern = QRegExp(pattern);
    rule.format = errorFormat;
    highlightingRules.append(rule);
  }

  warningFormat.setForeground(Qt::darkMagenta);
  warningFormat.setFontWeight(QFont::Bold);
  QStringList warningPatterns;
  warningPatterns << "\\b: warning\\b";
  foreach (const QString &pattern, warningPatterns)
  {
    rule.pattern = QRegExp(pattern);
    rule.format = warningFormat;
    highlightingRules.append(rule);
  }

  noteFormat.setForeground(Qt::darkBlue);
  noteFormat.setFontWeight(QFont::Bold);
  QStringList notePatterns;
  notePatterns << "\\b: note\\b";
  foreach (const QString &pattern, notePatterns)
  {
    rule.pattern = QRegExp(pattern);
    rule.format = noteFormat;
    highlightingRules.append(rule);
  }

  messageFormat.setForeground(Qt::darkCyan);
  messageFormat.setFontWeight(QFont::Bold);
  QStringList messagePatterns;
  messagePatterns << "\\bcompilation succeeded on: \\b";
  foreach (const QString &pattern, messagePatterns)
  {
    rule.pattern = QRegExp(pattern);
    rule.format = messageFormat;
    highlightingRules.append(rule);
  }
}
//! [6]

//! [7]
void CompileHighlighter::highlightBlock(const QString &text)
{

  foreach (const HighlightingRule &rule, highlightingRules)
  {
    QRegExp expression(rule.pattern);
    int index = expression.indexIn(text);
    while (index >= 0)
    {
      int length = expression.matchedLength();
      setFormat(index, length, rule.format);
      index = expression.indexIn(text, index + length);
    }
  }
}
//! [11]
