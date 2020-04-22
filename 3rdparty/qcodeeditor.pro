TEMPLATE = lib

CONFIG += staticlib release
QT += widgets

OBJECTS_DIR = obj

INCLUDEPATH += qcodeeditor/include

RESOURCES += qcodeeditor/resources/qcodeeditor_resources.qrc

HEADERS += qcodeeditor/include/internal/QHighlightRule.hpp \
    qcodeeditor/include/internal/QHighlightBlockRule.hpp \
    qcodeeditor/include/internal/QCodeEditor.hpp \
    qcodeeditor/include/internal/QCXXHighlighter.hpp \
    qcodeeditor/include/internal/QLineNumberArea.hpp \
    qcodeeditor/include/internal/QStyleSyntaxHighlighter.hpp \
    qcodeeditor/include/internal/QSyntaxStyle.hpp \
    qcodeeditor/include/internal/QGLSLCompleter.hpp \
    qcodeeditor/include/internal/QGLSLHighlighter.hpp \
    qcodeeditor/include/internal/QLanguage.hpp \
    qcodeeditor/include/internal/QXMLHighlighter.hpp \
    qcodeeditor/include/internal/QJSONHighlighter.hpp \
    qcodeeditor/include/internal/QLuaCompleter.hpp \
    qcodeeditor/include/internal/QLuaHighlighter.hpp \
    qcodeeditor/include/internal/QPythonCompleter.hpp \
    qcodeeditor/include/internal/QPythonHighlighter.hpp \
    qcodeeditor/include/internal/QFramedTextAttribute.hpp

SOURCES += qcodeeditor/src/internal/QCodeEditor.cpp \
    qcodeeditor/src/internal/QLineNumberArea.cpp \
    qcodeeditor/src/internal/QCXXHighlighter.cpp \
    qcodeeditor/src/internal/QSyntaxStyle.cpp \
    qcodeeditor/src/internal/QStyleSyntaxHighlighter.cpp \
    qcodeeditor/src/internal/QGLSLCompleter.cpp \
    qcodeeditor/src/internal/QGLSLHighlighter.cpp \
    qcodeeditor/src/internal/QLanguage.cpp \
    qcodeeditor/src/internal/QXMLHighlighter.cpp \
    qcodeeditor/src/internal/QJSONHighlighter.cpp \
    qcodeeditor/src/internal/QLuaCompleter.cpp \
    qcodeeditor/src/internal/QLuaHighlighter.cpp \
    qcodeeditor/src/internal/QPythonCompleter.cpp \
    qcodeeditor/src/internal/QPythonHighlighter.cpp \
    qcodeeditor/src/internal/QFramedTextAttribute.cpp
