/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

void ConfigDialog::init() 
{
    tabDatabase->raise();
    
    QSettings settings;
    settings.setPath( "mp3car", "headunit" );    
    
    // load file extensions
    QString extensions = settings.readEntry( "/headunit/extensions", "mp3;wma;avi;mpg" );
    QStringList list = QStringList::split(";", extensions);
    listExtensions->insertStringList(list);
    
    // load media paths
    lineMusicPath->setText(settings.readEntry("/headunit/musicpath", ""));
    lineVideoPath->setText(settings.readEntry("/headunit/videopath", ""));  
    
    // load skins
    listBoxSkins->clear();
    QDir dir("skins", QString::null, QDir::DirsFirst);
    dir.setMatchAllDirs(true);
    if (dir.isReadable()) { 	
	QFileInfoList *entries = (QFileInfoList *)dir.entryInfoList();
	QFileInfo *fi;
	for ( fi = entries->first(); fi; fi = entries->next() )
	    if (fi->fileName().at(0) != '.')
		listBoxSkins->insertItem(fi->fileName());
    }
    listBoxSkins->setSelected(0, TRUE);
    
    // setup db
    db = new DBHandler();
    connect( db, SIGNAL(dbStatus(int,const QString&)), this, SLOT(currentStatus(int,const QString&)) );
}


void ConfigDialog::currentStatus( int numFiles, const QString & path )
{
    lcdNumFiles->display(numFiles);
    if (checkShowFiles->isChecked())
	textPaths->append(QDir::convertSeparators(path));
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


void ConfigDialog::synchDBClicked()
{
    QString extensions;
    for (int i=0; i<listExtensions->count(); i++)
		extensions+="*."+listExtensions->text(i)+" "; 
    textPaths->clear();
    lcdNumFiles->display(0);  
	db->populateDB(lineMusicPath->text(), lineVideoPath->text(), extensions);
}


void ConfigDialog::okClicked()
{
    QSettings settings;
    settings.setPath( "mp3car", "headunit" );    
    
    // save file extensions
    QString extensions;
    for (int i=0; i<listExtensions->count(); i++)
	extensions+=listExtensions->text(i)+";"; 
    settings.writeEntry( "/headunit/extensions", extensions);
    
    // save media paths
    settings.writeEntry("/headunit/musicpath", lineMusicPath->text());
    settings.writeEntry("/headunit/videopath", lineVideoPath->text());
    accept();
}


void ConfigDialog::skinSelect( int item )
{
    QString skinName("skins/");
    skinName.append(listBoxSkins->text(item));
    skinName.append("/MENU_OFF.jpg");
    QPixmap preview(skinName);
    pixmapPreview->setPixmap(preview);
}


void ConfigDialog::setSkinClicked()
{
    QSettings settings;
    settings.setPath( "mp3car", "headunit" );    
    QString skin = listBoxSkins->currentText();
    if (!skin.isNull() && !skin.isEmpty()) {
	settings.writeEntry( "/headunit/skin", skin);
    }
}
