/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

void ConfigDialog::currentStatus( int numFiles, QString & path )
{
    labelNumSynched->setText(QString::number(numFiles));
}


void ConfigDialog::menuSelect( int item )
{
    switch (item) {
    case 0: 
	tabDatabase->raise();
	break;
    case 1:
	tabSkin->raise();
	break;
    default:
	break;
    }
}


void ConfigDialog::musicBrowseClicked()
{
    QString dir = QFileDialog::getExistingDirectory(
                    lineMusicPath->text(),
                    this,
                    0,
                    "Choose a directory containing music",
                    TRUE );
    if (!dir.isNull())
	lineMusicPath->setText(QDir::convertSeparators(dir));
}


void ConfigDialog::videoBrowseClicked()
{
    QString dir = QFileDialog::getExistingDirectory(
                    lineVideoPath->text(),
                    this,
                    0,
                    "Choose a directory containing videos",
                    TRUE );
    if (!dir.isNull())
	lineVideoPath->setText(QDir::convertSeparators(dir));
}


void ConfigDialog::extAddClicked()
{
    QString ext = lineAdd->text();
    lineAdd->clear();
    if (!ext.isNull() && !ext.isEmpty()) {
	ext.remove('.');
	if (listExtensions->findItem(ext, Qt::ExactMatch))
	    return;
	listExtensions->insertItem(ext);
    }
}


void ConfigDialog::extRemoveClicked()
{
    listExtensions->removeItem(listExtensions->currentItem());
}
