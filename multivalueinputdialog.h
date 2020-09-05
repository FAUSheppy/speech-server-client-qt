#ifndef MULTIVALUEINPUTDIALOG_H
#define MULTIVALUEINPUTDIALOG_H

#include <QDialog>
#include <QMainWindow>

class MultiValueInputDialog : public QDialog
{
public:
    MultiValueInputDialog(QStringList *inputValueLabels, QWidgetList *inputWidgets);
};

#endif // MULTIVALUEINPUTDIALOG_H
