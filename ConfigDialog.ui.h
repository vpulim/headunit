/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

#include "HeadUnit.h"
#include "DBHandler.h"

void ConfigDialog::init()
{
    //    textLabelPath->setText(QString("Bob"));
    //    textLabelPath.setText(getMediaPath());
    
    lineEditPath->setText(getMediaPath());
    textLabelInfo->setText(QString(""));
    listBoxExtensions->clear();
    populateExtensions();
    connect(tabWidget,SIGNAL(currentChanged(QWidget *)),this,SLOT(tabChanged(QWidget *)));
}

void ConfigDialog::populateExtensions()
{
    QStringList list = settings.entryList( "/headunit/extensions" );
    QStringList::Iterator it = list.begin();
    settings.beginGroup("/headunit/extensions");
    while( it != list.end() ) {
	QString ext = settings.readEntry((*it).latin1());	
	listBoxExtensions->insertItem(ext.latin1());
	it++;
    }
    settings.endGroup();
}

void ConfigDialog::saveExtensions()
{
    QStringList list = settings.entryList( "/headunit/extensions" );
    QStringList::Iterator it = list.begin();
    settings.beginGroup("/headunit/extensions");
    while( it != list.end() ) {
	settings.removeEntry((*it).latin1());
	it++;
    }
    settings.endGroup();
    
    for (unsigned int i=0; i<(listBoxExtensions->count());++i) {
	settings.writeEntry(QString("/headunit/extensions/")+QString::number(i), listBoxExtensions->item(i)->text());
    }
    //   settings.endGroup();
}

void ConfigDialog::pushButtonBrowse_clicked()
{
    setMediaPath(askMediaPath());
    lineEditPath->setText(getMediaPath());
    dbHandler->resetDB();
}

void ConfigDialog::pushButtonRebuild_clicked()
{
    dbHandler->resetDB();
}

void ConfigDialog::currentStatus(int numFiles, QString currentDir)
{
    textLabelInfo->setText(QString("Num Files: ")+QString::number(numFiles));
    textLabelInfo->repaint(true);
}

void ConfigDialog::pushButtonAdd_clicked()
{
    listBoxExtensions->insertItem(lineEditExtension->text(),-1);
    lineEditExtension->clear();
    fileTypesDirty=true;
}

void ConfigDialog::pushButtonRemove_clicked()
{
    listBoxExtensions->removeItem(listBoxExtensions->currentItem());
    fileTypesDirty=true;
}

void ConfigDialog::tabChanged( QWidget *tabWidget )
{
    if (fileTypesDirty && (tabWidget->name()!="fileTypes")) {
	this->saveExtensions();
    }
}
