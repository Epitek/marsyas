#include "../Widgets/GridDisplay.h"

GridDisplay::GridDisplay(int winSize, Tracklist *tracklist, Grid* grid_, QWidget *parent)
: MyDisplay(tracklist, parent), _winSize(winSize)
{
	this->grid_ = grid_;
	_cellSize = grid_->getCellSize();
	setMinimumSize(winSize, winSize);
	setMouseTracking(true);
	setAcceptDrops(true);
	squareHasInitialized.resize(144);
	oldXPos = -1;
	oldYPos = -1;
	fullScreenMouseOn = false;
	fullScreenTimer = new QTimer(this);
	fullScreenTimer->setInterval(150);

	connect(this, SIGNAL(clearMode()), grid_, SLOT(clearMode()));
	connect(this, SIGNAL(extractMode()), grid_, SLOT(setExtractMode()));
	connect(this, SIGNAL(trainMode()), grid_, SLOT(setTrainMode()));
	connect(this, SIGNAL(predictMode()), grid_, SLOT(setPredictMode()));
	connect(this, SIGNAL(initMode()), grid_, SLOT(setInitMode()));
	connect(this, SIGNAL(savePredictionGridSignal(QString)), grid_, SLOT(savePredictionGrid(QString)));
	connect(this, SIGNAL(openPredictionGridSignal(QString)), grid_, SLOT(openPredictionGrid(QString)));
	connect(grid_, SIGNAL(repaintSignal()), this, SLOT(repaintSlot()));
	connect(this, SIGNAL(cancelButtonPressed()), grid_, SLOT(cancelPressed()));
	connect(this, SIGNAL(normHashLoadPressed()), grid_, SLOT(openNormHash()));
	connect(fullScreenTimer, SIGNAL(timeout()), this, SLOT(fullScreenMouseMove()));
}

GridDisplay::~GridDisplay()
{
	// TODO: DELETE STUFF
}
// ******************************************************************
//
// SLOTS

void GridDisplay::clear()
{

	emit clearMode();

}
void GridDisplay::train()
{
	emit trainMode();
	grid_->buttonPressed.wakeAll();
}
void GridDisplay::extract()
{
	cout << "extract" <<endl;
	emit extractMode();
	grid_->buttonPressed.wakeAll();
}
void GridDisplay::predict()
{
	emit predictMode();
	grid_->buttonPressed.wakeAll();
}
void GridDisplay::init()
{
	emit initMode();
	grid_->buttonPressed.wakeAll();
}
void GridDisplay::savePredictionGrid(QString fname)
{
	emit savePredictionGridSignal(fname);
}
void GridDisplay::openPredictionGrid(QString fname)
{
	emit openPredictionGridSignal(fname);
}
void GridDisplay::playModeChanged()
{
	grid_->setContinuous( !grid_->isContinuous() );
}
void GridDisplay::repaintSlot()
{
	repaint( );
}
void GridDisplay::cancelButton()
{
	emit cancelButtonPressed();
}

void GridDisplay::normHashLoad()
{
	emit normHashLoadPressed();
}

void GridDisplay::fullScreenMouse()
{
	if( fullScreenMouseOn = !fullScreenMouseOn )
	{
		fullScreenTimer->start();
		grabMouse();
	}
	else
	{
		fullScreenTimer->stop();
		releaseMouse();
	}
	emit fullScreenMode(fullScreenMouseOn);
	setMouseTracking(!fullScreenMouseOn);
	
}
void GridDisplay::fullScreenMouseMove()
{
	QRect screenSize = (QApplication::desktop())->screenGeometry();
	
	// map mouse position on screen to a grid location
	int gridX = mouseCursor->pos().x() / (screenSize.width() / (grid_->getWidth() - 1));
	int gridY = mouseCursor->pos().y() / (screenSize.height() / (grid_->getHeight() - 1));
	
	updateXYPosition(gridX, gridY);
	if(grid_->getXPos() != oldXPos || oldYPos != grid_->getYPos()  ) 
	{
		playNextTrack();
		oldXPos = grid_->getXPos();
	    oldYPos = grid_->getYPos();
		repaint();
	}
}


// ******************************************************************
//
// FUNCTIONS
void GridDisplay::midiXYEvent(unsigned char xaxis, unsigned char yaxis) 
{
	int x = (int)(xaxis / 128.0 * grid_->getWidth());
	int y = grid_->getHeight() - 1 - (int)(yaxis / 128.0 * grid_->getHeight());

	std::cout << "midi xy event (" << x << "," << y << ")\n";
	updateXYPosition(x, y);
	playNextTrack();
}
void GridDisplay::updateXYPosition(int x, int y)
{
	grid_->setXPos(x);
	grid_->setYPos(y);
}

void GridDisplay::midiPlaylistEvent(bool next) 
{
	if ( next ) {
		std::cout << "midi playlist event\n";
		playNextTrack();	
	}
}

void GridDisplay::reload() 
{

	//TODO:  Figure out if reload is needed
}


void GridDisplay::playNextTrack() 
{

	if( !grid_->getCurrentFiles().isEmpty() ) 
	{


		int counterSize = grid_->getGridCounterSizes(grid_->getCurrentIndex());
		if (counterSize > 0) 
			grid_->setGridCounter( grid_->getCurrentIndex() , (grid_->getGridCounter(grid_->getCurrentIndex()) + 1) % counterSize );  
		int counter = grid_->getGridCounter(grid_->getCurrentIndex());

		QList<std::string> playlist = grid_->getCurrentFiles();
		cout << "Currently Playing: " + playlist[counter] << endl;
		cout << "Playlist: " << endl;
		for(int i = 0; i < counterSize; i++ ) {
		cout << playlist[i] << endl;
		}
		grid_->playTrack(counter);
	} else
	{
		grid_->stopPlaying();
	}
}


/*
* -----------------------------------------------------------------------------
* Mouse Events
* -----------------------------------------------------------------------------
*/
void GridDisplay::mousePressEvent(QMouseEvent *event) 
{
	std::cout << "mouse Press Event" << std::endl;

	if(fullScreenMouseOn)
	{
		QRect screenSize = (QApplication::desktop())->screenGeometry();
		int gridX = mouseCursor->pos().x() / (screenSize.width() / (grid_->getWidth() - 1));
		int gridY = mouseCursor->pos().y() / (screenSize.height() / (grid_->getHeight() - 1));

		updateXYPosition(gridX, gridY);
		if(grid_->getXPos() != oldXPos || oldYPos != grid_->getYPos()  ) 
		{
			oldXPos = grid_->getXPos();
			oldYPos = grid_->getYPos();
		}
	}
	else
	{
		updateXYPosition(event->pos().x() / _cellSize, event->pos().y() / _cellSize);
	}
	//TODO: REMOVE OR KEEP?
	//getCurrentSquare()->nextTrack();
	//_tracklist->listTracks(&getCurrentSquare()->getTracks());
	playNextTrack();
	repaint();
}

void GridDisplay::mouseMoveEvent(QMouseEvent *event) 
{
	if ( (event->pos().x() >= _winSize) || (event->pos().y() >= _winSize) ) {
		return;
	}
	updateXYPosition(event->pos().x() / _cellSize, event->pos().y() / _cellSize);

	if(grid_->isContinuous() && (grid_->getXPos() != oldXPos || oldYPos != grid_->getYPos() ) ) 
	{
		playNextTrack();
		oldXPos = grid_->getXPos();
	    oldYPos = grid_->getYPos();
	}
	repaint();
}

/*
* -----------------------------------------------------------------------------
* Mouse Events
* -----------------------------------------------------------------------------
*/
void GridDisplay::dragMoveEvent(QDragMoveEvent* event ) 
{


	//TODO: NEED TO ADD IN FOR INIT

	//qDebug() << "Drag Move";
}

void GridDisplay::dragEnterEvent(QDragEnterEvent* event)
{
	if ( event->proposedAction() == Qt::CopyAction ) {
		event->acceptProposedAction();
	}
}

void GridDisplay::dropEvent(QDropEvent *event) 
{

	qDebug() << "drop!!" ;
	if ( event->proposedAction() == Qt::CopyAction ) 
	{
		//Position of drop event
		int x = event->pos().x() / _cellSize;
		int y = event->pos().y() / _cellSize;
		int k = y * grid_->getHeight() + x;
		squareHasInitialized[k] = true; 


		const QMimeData* data = event->mimeData();

		QString trackName = data->text();

		QByteArray trackLocation = data->data("application/track-location");
		QString foobar = trackLocation;

		if ( data->hasFormat("application/track-id") ) {
			cout << "Track" << endl;
			grid_->addInitFile(trackLocation, x, y);
		} else if ( data->hasFormat("application/playlist-id") ) {
			cout << "Play List" << endl;
		}	
	}
	repaint();
}

void GridDisplay::paintEvent(QPaintEvent* /* event */) 
{

	QPainter painter;
	painter.begin(this);

	//Find density
	int maxDensity = 0;
	int minDensity = 100;
	for (int i=0; i < grid_->getFiles().size(); i++) {
		if(grid_->getFilesAt(i).size() > maxDensity)
		{
			maxDensity = grid_->getFilesAt(i).size();
		}
		else if (grid_->getFilesAt(i).size() < minDensity) 
		{
			minDensity = grid_->getFilesAt(i).size();
		}
	}

	Colormap *map = Colormap::factory(Colormap::GreyScale);
	for (int i=0; i < grid_->getHeight(); i++) {
		for (int j=0; j < grid_->getWidth(); j++) {
			int k = i * grid_->getHeight() + j;

			QRect	 myr(i*_cellSize,j*_cellSize,_cellSize,_cellSize);
			QLine	 myl1(i*_cellSize,j*_cellSize, i*_cellSize, j*_cellSize + _cellSize);
			QLine	 myl2(i*_cellSize,j*_cellSize, i*_cellSize+_cellSize, j*_cellSize );

			if ( grid_->getFilesAt(k).size() == 0 ) {
				QColor color(map->getRed(125), map->getGreen(0), map->getBlue(0));
				painter.setBrush(color);
			} else {
				int c = int(grid_->getFilesAt(k).size() / float(maxDensity) * (map->getDepth()-1));
				QColor color(map->getRed(c), map->getGreen(c), map->getBlue(c));
				painter.setBrush(color);
			}

			painter.setPen(Qt::NoPen);
			painter.drawRect(myr);

			painter.setPen(Qt::black);
			painter.drawLine(myl1);
			painter.drawLine(myl2);

			painter.setBrush(Qt::red);
			QRect newr( grid_->getXPos() * _cellSize + _cellSize / 4,
				grid_->getYPos() * _cellSize + _cellSize / 4,
				_cellSize - _cellSize / 2,
				_cellSize-_cellSize / 2);
			painter.drawRect(newr);
		}
	}
	// Draw an 'i' in all initilized squares
	for(int i = 0; i < squareHasInitialized.size(); i++)
	{
		if(squareHasInitialized[i])
		{
			int y = i / grid_->getHeight();
			int x = i % grid_->getHeight();
			int cellSize = grid_->getCellSize();
			/*cout << "y";
			cout << y << endl;
			cout << "x";
			cout << x << endl;*/
			painter.setBrush(Qt::green);
			QFont *font = new QFont();
			font->setPointSize(16);
			font->setStyleHint(QFont::OldEnglish);
			painter.setFont(*font);
			painter.drawText(x * cellSize, y * cellSize, cellSize, cellSize, Qt::AlignCenter, "i");
		}
	}

	delete map;
	painter.end();
}
