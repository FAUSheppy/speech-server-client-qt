#include "multivalueinputdialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>

MultiValueInputDialog::MultiValueInputDialog(QStringList *inputValueLabels, QWidgetList *inputWidgets){

    QFormLayout *form = new QFormLayout();
    for(int i = 0; i<inputValueLabels->size(); i++){
        form->addRow(new QLabel(inputValueLabels->at(i)));
        form->addRow(inputWidgets->at(i));
    }
    // Add some text above the fields

    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    form->addRow(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    this->setLayout(form);
}
