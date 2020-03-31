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

#ifndef DYNPARAMSDIALOG_H
#define DYNPARAMSDIALOG_H

#include <QDialog>
#include <QPixmap>
#include <QPushButton>
#include <QIcon>
#include "appColorDialog.h"
#include "DynParam.h"


class DPColorButton : public QPushButton
{
  Q_OBJECT

public:
  explicit DPColorButton (QWidget *parent, int paramidx, int buttonidx);

  int paramidx;
  int buttonidx;
};

typedef QVector <QPixmap *> PixmapVector;
typedef QVector <QIcon *> IconVector;
typedef QVector <DPColorButton *> ButtonVector;


class QLabel;
class QCheckBox;
class QDialogButtonBox;
class QFormLayout;

class DynParamsDialog : public QDialog
{
    Q_OBJECT

public:
    // new indicator constructor
    explicit DynParamsDialog (QString title, QWidget *parent = 0);

    // modify constructor
    DynParamsDialog (const ParamVector& PVector, QString title);

    ~DynParamsDialog();

    // add a parameter
    void addParam(QString paramName, QString title, qint32 type, qreal defvalue);

    // add a parameter with callback variable
    void addParam(QString paramName, QString label, qint32 type, qreal defvalue,
                  void *cbvar, bool show);

    // get the title label
    QString getTitle() const;

    // get parameter data
    qreal getParam (const QString& name) const;

    // get parameter vector
    const ParamVector& parameters() const { return Param; };
    ParamVector& parameters() { return Param; };

    bool removeSelected() const { return _removed; }

protected:
    virtual void showEvent(QShowEvent * event);

private slots:
    void color_clicked();
    void colorAccepted();
    void colorRejected();
    void intChanged(int);
    void text_changed(QString);
    void function_accepted();
    void removeClicked();

private:
    void constructorSetup();
    void makeColorDialog();

    ParamVector Param;    // parameters' vector
    PixmapVector Pixmap;  // pixmaps' vector
    IconVector Icon;      // icons' vector
    ButtonVector Button;  // buttons' vector
    int param_height;     // height of each parameter
    int Height;           // dialog's height
    int ncolorbuttons;    // number of color buttons
    int paramidx;         // parameter index
    int cbidx;            // color button index
    bool modify;          // create = false, modify = true
    bool _removed;

    QDialogButtonBox* buttonBox;
    appColorDialog* colorDialog;
    QFormLayout* _form;
};


#endif // DYNPARAMSDIALOG_H
