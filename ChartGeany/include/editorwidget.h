/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */

#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H

#include <QProcess>
#include <QWidget>
#include <QFileDialog>
#if QT_VERSION < QT_VERSION_CHECK(5, 7, 0)
#include "codeeditor.h"
#else
#include "QCodeEditor"
#include "QCXXHighlighter"
#endif

#include "highlighter.h"

namespace Ui
{
  class EditorWidget;
}

class EditorWidget : public QWidget
{
  Q_OBJECT

public:
  explicit EditorWidget (QWidget *parent = 0);
  ~EditorWidget (void);

signals:
  void expandChartToggle ();

private:
  Ui::EditorWidget *ui;
#if QT_VERSION < QT_VERSION_CHECK(5, 7, 0)
  CodeEditor *ceditor;            // the code editor
#else
  QCodeEditor *ceditor;         // the code editor
  QCXXHighlighter *highlighter; // the syntax highlighter
#endif
  QFileDialog *openFileDialog;  // open file dialog
  QFileDialog *saveFileDialog;  // save file dialog
  QString fileName;             // the file name
  QString output;               // compiler messages
  int hpad;                     // height pad of ceditor
  bool debug;                   // true when debuging
  bool firstshow;               // true if this is the first show
private slots:
  void openButton_clicked (void);
  void saveButton_clicked (void);
  void saveAsButton_clicked (void);
  void compileButton_clicked (void);
  void messagesButton_clicked (void);
  void debugButton_clicked (void);
  void fontsize_changed (int);
  void fontfamily_changed(int);
protected:
  virtual void resizeEvent (QResizeEvent * event);
  virtual void showEvent (QShowEvent * event);
};

#endif // EDITORWIDGET_H
