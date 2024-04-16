#include "Player.h"

MusicPlayer::MusicPlayer(QWidget *parent)
    : QWidget(parent)
{

    layout = new QVBoxLayout(this);


    Player = new QMediaPlayer(this);
    playlist = new  QList<QPair<QUrl, int>>;
    CurrentIndex = new int;
    *CurrentIndex = 0;
    //Player->setSource(QUrl::fromLocalFile("C:/Users/richt/OneDrive - Microsoft Cloud - Bereich Universität Regensburg/Studium/MASTER/Programmieren in C und C++/Projekt/test.mp3"));
    //std::cout << "loaded? " << Player->mediaStatus() << std::endl;

    audioOutput = new QAudioOutput;
    Player->setAudioOutput(audioOutput);

    // Track title
    trackLayout = new QVBoxLayout(this);
    titleLabel = new QLabel("Please add a song", this); // Initial text when playlist is empty
    authorLabel = new QLabel("", this); // Initially empty
    trackLayout->addWidget(titleLabel);
    trackLayout->addWidget(authorLabel);

    // duration slider and label
    QHBoxLayout * durationLayout = new QHBoxLayout(this);
    slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(0, Player->duration());
    connect(slider, &QSlider::sliderMoved, this, &MusicPlayer::seek);
    durationLayout->addWidget(slider);

    duration = new QLabel();
    duration->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    durationLayout->addWidget(duration);

    //connect MediaPlayersignals to Slots
    connect(Player, &QMediaPlayer::durationChanged, this, &MusicPlayer::durationChanged);
    connect(Player, &QMediaPlayer::positionChanged, this, &MusicPlayer::positionChanged);

    //connect(Player, QOverload<>::of(&QMediaPlayer::metaDataChanged), this, &MusicPlayer::metaDataChanged);
    connect(Player, &QMediaPlayer::mediaStatusChanged, this, &MusicPlayer::statusChanged);
    connect(this, &MusicPlayer::FirstSongAdded, this, &MusicPlayer::startPlayer);
    connect(this, &MusicPlayer::CurrentIndexChanged, this, &MusicPlayer::metaDataChanged);

    connect(Player, &QMediaPlayer::bufferProgressChanged, this, &MusicPlayer::bufferingProgress);
    connect(Player, &QMediaPlayer::errorChanged, this, &MusicPlayer::displayErrorMessage);
    connect(Player, &QMediaPlayer::tracksChanged, this, &MusicPlayer::tracksChanged);


    // add controls
    controlLayout = new QHBoxLayout;
    controlLayout->setContentsMargins(0, 0, 0, 0);

    QPushButton *openButton = new QPushButton(tr("Open"), this);
    connect(openButton, &QPushButton::clicked, this, &MusicPlayer::open);
    controlLayout->addWidget(openButton);
    controlLayout->addStretch(1);

    PlayerControl = new Controls();
    PlayerControl->setState(Player->playbackState());
    PlayerControl->setVolume(audioOutput->volume());
    PlayerControl->setMuted(PlayerControl->isMuted());

    connect(PlayerControl, &Controls::play, this, &MusicPlayer::playClicked);
    connect(PlayerControl, &Controls::pause, Player, &QMediaPlayer::pause);
    connect(PlayerControl, &Controls::stop, Player, &QMediaPlayer::stop);
    connect(PlayerControl, &Controls::next, this, &MusicPlayer::nextClicked);
    connect(PlayerControl, &Controls::previous, this, &MusicPlayer::previousClicked);
    connect(this, &MusicPlayer::CurrentIndexChanged, this, &MusicPlayer::handleCurrentIndexChanged);
    connect(PlayerControl, &Controls::changeVolume, audioOutput, &QAudioOutput::setVolume);
    connect(PlayerControl, &Controls::changeMuting, audioOutput, &QAudioOutput::setMuted);
    connect(PlayerControl, &Controls::changeRate, Player, &QMediaPlayer::setPlaybackRate);
    connect(Player, &QMediaPlayer::playbackStateChanged, PlayerControl, &Controls::setState);
    connect(audioOutput, &QAudioOutput::volumeChanged, PlayerControl, &Controls::setVolume);
    connect(audioOutput, &QAudioOutput::mutedChanged, PlayerControl, &Controls::setMuted);
    controlLayout->addWidget(PlayerControl);
    controlLayout->addStretch(1);

    ViewPlaylist = new QPushButton(tr("View Playlist"), this);
    controlLayout->addWidget(ViewPlaylist);
    connect(ViewPlaylist, &QPushButton::clicked, this, &MusicPlayer::handleViewPlaylist);
    connect(this, &MusicPlayer::playlistUpdated, this, &MusicPlayer::handlePlaylistUpdated);

    //playlistWidget = new QListWidget(this);
    //playlistWidget->setDragDropMode(QAbstractItemView::InternalMove);
    //playlistWidget->setAcceptDrops(true);
    connect(this, &MusicPlayer::SongsAdded, this, &MusicPlayer::updatePlaylist);
    connect(this, &MusicPlayer::FirstSongAdded, this, &MusicPlayer::updatePlaylist);

    //compose layout
    layout->addLayout(trackLayout);
    layout->addLayout(durationLayout);
    layout->addLayout(controlLayout);
    //layout->addWidget(playlistWidget);

    this->setLayout(layout);
}

MusicPlayer::~MusicPlayer() {}

void MusicPlayer::startPlayer(){
    if (!playlist->isEmpty()) {
        Player->setSource(playlist->at(0).first);
        *CurrentIndex = playlist->at(0).second;
        emit CurrentIndexChanged();
    }
}

void MusicPlayer::playClicked(){
    if (!playlist->isEmpty()) {
        //Player->setSource(playlist->at(0).first);
        Player->play();
        //*CurrentIndex = playlist->at(0).second;
    }
}

void MusicPlayer::open(){
    // Select multiple MP3 files
    QStringList fileNames = QFileDialog::getOpenFileNames(nullptr, "Select MP3 Files", "", "MP3 files (*.mp3)");

    int orig_i = playlist->size();
    int i = playlist->size();
    // Convert file paths to QUrls and add them to the list
    for(const QString &fileName : fileNames) {
        playlist->append(qMakePair(QUrl::fromLocalFile(fileName), i));
        i ++;
    }

    if(orig_i == 0){
        emit FirstSongAdded();
    }
    else{
        emit SongsAdded();
    }

};

void MusicPlayer::durationChanged(qint64 new_duration){
    c_duration = new_duration / 1000;
    slider->setMaximum(c_duration);

};
void MusicPlayer::positionChanged(qint64 progress){
    if (!slider->isSliderDown()) {
        qint64 duration = Player->duration();
        if (duration > 0) {
            int newPosition = static_cast<int>((progress * slider->maximum()) / duration);
            slider->setValue(newPosition);
        }
    }
    updateDurationInfo(progress / 1000);

};

void MusicPlayer::updateDurationInfo(qint64 currentInfo)
{
    QString tStr;
    if (currentInfo || c_duration) {
        QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60, currentInfo % 60,
                          (currentInfo * 1000) % 1000);
        QTime totalTime((c_duration / 3600) % 60, (c_duration / 60) % 60, c_duration % 60,
                        (c_duration * 1000) % 1000);
        QString format = "mm:ss";
        if (c_duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    duration->setText(tStr);
};

void MusicPlayer::handleViewPlaylist(){
    if (!playlistDialog){
        playlistDialog = new PlaylistDialog(this);
        playlistDialog->getPlaylist(playlist);
        playlistDialog->show();
    }
    else{
        playlistDialog->raise();
    }
}

void MusicPlayer::updatePlaylist(){         //save code!
    emit playlistUpdated();
};

void MusicPlayer::handlePlaylistUpdated(){
    if (playlistDialog){
        delete playlistDialog;                      //memory leak???
        playlistDialog = new PlaylistDialog(this);
        playlistDialog->getPlaylist(playlist);
        playlistDialog->show();
    }
}

void MusicPlayer::metaDataChanged(){
    if (Player->mediaStatus() == QMediaPlayer::LoadedMedia || Player->mediaStatus() == QMediaPlayer::PlayingState) {
        auto data = Player->metaData();
        QString Title = data.stringValue(data.Title);
        QString Artist = data.stringValue(data.AlbumArtist);

        if (Title == ""){                           //fix!
            titleLabel->setText("Unknown Title");
        } else{
            titleLabel->setText(Title);
        }

        if (Artist == ""){
            authorLabel->setText("Unknown Artist");
        } else{
            authorLabel->setText(Artist);
        }

    }
    else{
        titleLabel->setText("Title not loaded");
        authorLabel->setText("Artist not loaded");
    }
};

void MusicPlayer::tracksChanged(){

};

void MusicPlayer::nextClicked(){
    if (!playlist->isEmpty()) {
        if(*CurrentIndex != (playlist->size() -1)) {
            (*CurrentIndex)++;
            Player->setSource(playlist->at(*CurrentIndex).first);
            emit CurrentIndexChanged();
        }
        else{
            *CurrentIndex = 0;
            Player->setSource(playlist->at(*CurrentIndex).first);
            emit CurrentIndexChanged();
        }
    }
};

void MusicPlayer::previousClicked(){
    if (!playlist->isEmpty()) {
        if(*CurrentIndex != 0) {
            (*CurrentIndex)--;
            Player->setSource(playlist->at(*CurrentIndex).first);
            emit CurrentIndexChanged();
        }
        else{
            *CurrentIndex =  (playlist->size() -1);
            Player->setSource(playlist->at(*CurrentIndex).first);
            emit CurrentIndexChanged();
        }
    }

};

void MusicPlayer::handleCurrentIndexChanged(){
    if(Player->mediaStatus() == QMediaPlayer::PlayingState){
        Player->play();
    }
    else if(Player->mediaStatus() == QMediaPlayer::PausedState || Player->mediaStatus() == QMediaPlayer::StoppedState){
        Player->stop();
    }
}

void MusicPlayer::seek(int position){
    // Calculate the position in milliseconds based on the slider's value
    qint64 newPosition = static_cast<qint64>((position * Player->duration()) / slider->maximum());
    // Set the position of the media player
    Player->setPosition(newPosition);
};



void MusicPlayer::statusChanged(QMediaPlayer::MediaStatus status){
}
/*
    handleCursor(status);
    // handle status message
    switch (status) {
    case QMediaPlayer::NoMedia:
    case QMediaPlayer::LoadedMedia:
        setStatusInfo(QString());
        break;
    case QMediaPlayer::LoadingMedia:
        setStatusInfo(tr("Loading..."));
        break;
    case QMediaPlayer::BufferingMedia:
    case QMediaPlayer::BufferedMedia:
        setStatusInfo(tr("Buffering %1%").arg(qRound(player->bufferProgress() * 100.)));
        break;
    case QMediaPlayer::StalledMedia:
        setStatusInfo(tr("Stalled %1%").arg(qRound(player->bufferProgress() * 100.)));
        break;
    case QMediaPlayer::EndOfMedia:
        QApplication::alert(this);
        //playlist->next();
        break;
    case QMediaPlayer::InvalidMedia:
        displayErrorMessage();
        break;
    }
};

void Player::handleCursor(QMediaPlayer::MediaStatus status) {
#ifndef QT_NO_CURSOR
    if (status == QMediaPlayer::LoadingMedia || status == QMediaPlayer::BufferingMedia
        || status == QMediaPlayer::StalledMedia)
        setCursor(QCursor(Qt::BusyCursor));
    else
        unsetCursor();
#endif
}

void Player::setStatusInfo(const QString &info)
{
    statusInfo = info;

    if (m_statusBar) {
        statusBar->showMessage(m_trackInfo);
        statusLabel->setText(m_statusInfo);
    } else {
        if (!m_statusInfo.isEmpty())
            setWindowTitle(QStringLiteral("%1 | %2").arg(trackInfo).arg(statusInfo));
        else
            setWindowTitle(trackInfo);
    }
}
*/

void MusicPlayer::bufferingProgress(float progress){

};

void MusicPlayer::selectAudioStream(){

};

void MusicPlayer::selectSubtitleStream(){

};

void MusicPlayer::displayErrorMessage(){

};

void MusicPlayer::audioOutputChanged(int){

};
