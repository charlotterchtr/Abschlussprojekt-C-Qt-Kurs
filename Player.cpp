#include "Player.h"


MusicPlayer::MusicPlayer(QWidget *parent)
    : QWidget(parent)
{

    QWidget::setWindowTitle("Music Player");

    // Player setup
    layout = new QVBoxLayout(this);
    Player = new QMediaPlayer(this);
    playlist = new  QList<QPair<QUrl, int>>;
    CurrentIndex = new int;
    *CurrentIndex = 0;
    status = new QMediaPlayer::PlaybackState;

    audioOutput = new QAudioOutput;
    Player->setAudioOutput(audioOutput);

    //Shuffle
    Shuffle = new bool;
    *Shuffle = false;

    // Track title
    trackLayout = new QVBoxLayout;
    titleLabel = new QLabel("Please add a song", this); // Initial text when playlist is empty
    authorLabel = new QLabel("", this); // Initially empty
    authorLabel->setStyleSheet("font-style: italic;");
    trackLayout->addWidget(titleLabel, 0, Qt::AlignCenter);
    trackLayout->addWidget(authorLabel, 0, Qt::AlignCenter);

    // duration slider and label
    QHBoxLayout * durationLayout = new QHBoxLayout;
    slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(0, Player->duration());
    connect(slider, &QSlider::sliderMoved, this, &MusicPlayer::seek);
    durationLayout->addWidget(slider);

    duration = new QLabel();
    duration->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    durationLayout->addWidget(duration);

    //connect MediaPlayer-signals to Slots
    connect(Player, &QMediaPlayer::durationChanged, this, &MusicPlayer::durationChanged);
    connect(Player, &QMediaPlayer::positionChanged, this, &MusicPlayer::positionChanged);
    connect(this, &MusicPlayer::FirstSongAdded, this, &MusicPlayer::startPlayer);
    connect(Player, &QMediaPlayer::mediaStatusChanged, this, &MusicPlayer::handleStateChanged);
    connect(Player, &QMediaPlayer::mediaStatusChanged, this, &MusicPlayer::handleCursor);
    connect(this, &MusicPlayer::mediaLoaded, this, &MusicPlayer::metaDataChanged);
    connect(Player, &QMediaPlayer::errorOccurred, this, &MusicPlayer::displayErrorMessage);

    // add controls and connect
    controlLayout = new QHBoxLayout;
    controlLayout->setContentsMargins(0, 0, 0, 0);
    PlayerControl = new Controls();
    PlayerControl->setState(Player->playbackState());
    PlayerControl->setVolume(audioOutput->volume());
    PlayerControl->setMuted(PlayerControl->isMuted());

    connect(PlayerControl, &Controls::play, this, &MusicPlayer::playClicked);
    connect(PlayerControl, &Controls::opened, this, &MusicPlayer::open);
    connect(PlayerControl, &Controls::pause, Player, &QMediaPlayer::pause);
    connect(PlayerControl, &Controls::stop, Player, &QMediaPlayer::stop);
    connect(PlayerControl, &Controls::next, this, &MusicPlayer::nextClicked);
    connect(this, &MusicPlayer::ClickNext, this, &MusicPlayer::nextClicked);
    connect(PlayerControl, &Controls::previous, this, &MusicPlayer::previousClicked);
    connect(this, &MusicPlayer::CurrentIndexChanged, this, &MusicPlayer::handleCurrentIndexChanged);
    connect(PlayerControl, &Controls::changeVolume, audioOutput, &QAudioOutput::setVolume);
    connect(PlayerControl, &Controls::changeMuting, audioOutput, &QAudioOutput::setMuted);
    connect(PlayerControl, &Controls::shuffle, this, &MusicPlayer::handleShuffle);
    connect(PlayerControl, &Controls::changeRate, Player, &QMediaPlayer::setPlaybackRate);
    connect(Player, &QMediaPlayer::playbackStateChanged, PlayerControl, &Controls::setState);
    connect(audioOutput, &QAudioOutput::volumeChanged, PlayerControl, &Controls::setVolume);
    connect(audioOutput, &QAudioOutput::mutedChanged, PlayerControl, &Controls::setMuted);
    controlLayout->addWidget(PlayerControl);
    controlLayout->addStretch(1);

    //Playlist handling
    ViewPlaylist = new QPushButton(tr("View Playlist"), this);
    controlLayout->addWidget(ViewPlaylist);
    connect(ViewPlaylist, &QPushButton::clicked, this, &MusicPlayer::handleViewPlaylist);
    connect(this, &MusicPlayer::SongsAdded, this, &MusicPlayer::handlePlaylistUpdated);
    connect(this, &MusicPlayer::FirstSongAdded, this, &MusicPlayer::handlePlaylistUpdated);
    connect(this, &MusicPlayer::FirstSongAdded, this, &MusicPlayer::startPlayer);
    connect(this, &MusicPlayer::PlaylistChanged, this, &MusicPlayer::handlePlaylistUpdated);

    //compose layout
    layout->addLayout(trackLayout);
    layout->addLayout(durationLayout);
    layout->addLayout(controlLayout);

    this->setLayout(layout);
}

MusicPlayer::~MusicPlayer() {}

void MusicPlayer::startPlayer(){
    if (!playlist->isEmpty()) {
        Player->setSource(playlist->at(0).first);
        * CurrentIndex = playlist->at(0).second;
        * status = QMediaPlayer::StoppedState;
        emit CurrentIndexChanged(status);
    }
}

void MusicPlayer::playClicked(){
    if (!playlist->isEmpty()) {
        Player->play();
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
}

void MusicPlayer::handleViewPlaylist(){
    if (!playlistDialog){
        playlistDialog = new PlaylistDialog(this);
        connect(playlistDialog, &PlaylistDialog::ItemDeleted, this, &MusicPlayer::handleItemDeleted);
        playlistDialog->getPlaylist(playlist);
        playlistDialog->getCurrentIndex(CurrentIndex);
        playlistDialog->show();
        connect(playlistDialog, &PlaylistDialog::finished, this, &MusicPlayer::cleanupPlaylistDialog);
    }
    else{
        playlistDialog->raise();
    }
}

void MusicPlayer::cleanupPlaylistDialog() {
    if (playlistDialog) {
        delete playlistDialog;
        playlistDialog = nullptr;
    }
}

void MusicPlayer::handlePlaylistUpdated(){

    if (playlistDialog){
        delete playlistDialog;
        playlistDialog = new PlaylistDialog(this);
        connect(playlistDialog, &PlaylistDialog::ItemDeleted, this, &MusicPlayer::handleItemDeleted);
        playlistDialog->getPlaylist(playlist);
        playlistDialog->getCurrentIndex(CurrentIndex);
        playlistDialog->show();
        connect(playlistDialog, &PlaylistDialog::finished, this, &MusicPlayer::cleanupPlaylistDialog);
    }
}


void MusicPlayer::handleItemDeleted(int index){

    playlist->removeAt(index);

    // Update the current index if needed
    if (!playlist->isEmpty()) {
        if (index == (*CurrentIndex)) {
            if(*Shuffle == false){
                if(*CurrentIndex < (playlist->size() -1)) {
                    (*CurrentIndex)++;
                    * status = Player->playbackState();
                    Player->setSource(playlist->at(*CurrentIndex).first);
                    emit CurrentIndexChanged(status);
                }
                else{
                    *CurrentIndex = 0;
                    * status = Player->playbackState();
                    Player->setSource(playlist->at(*CurrentIndex).first);
                    emit CurrentIndexChanged(status);
                }
            } else {
                int length = playlist->length();
                int x = rand() % length;
                *CurrentIndex = x;
                * status = Player->playbackState();
                Player->setSource(playlist->at(*CurrentIndex).first);
                emit CurrentIndexChanged(status);
            }
        } else if (*CurrentIndex > 0 && index <= (*CurrentIndex)) {   //Check!
            (*CurrentIndex)--;
            * status = Player->playbackState();
            emit CurrentIndexChanged(status);
        }
    }
    else{               //Empty Playlist
        Player->stop();
        titleLabel->setText("No Title loaded");
        authorLabel->setText("No Artist loaded");
    }

}

void MusicPlayer::setStatus(QString const & status){
    QWidget::setWindowTitle("Music Player || " + status);
}

void MusicPlayer::displayErrorMessage(QMediaPlayer::Error error, const QString & errorString){
    QWidget::setWindowTitle("Music Player || Error: " + errorString);
}


void MusicPlayer::handleStateChanged(){
    std::cout << Player->mediaStatus() << std::endl;
    switch(Player->mediaStatus()) {
    case QMediaPlayer::NoMedia:
        setStatus("No Media");
        break;
    case QMediaPlayer::LoadedMedia:
        emit mediaLoaded();
        setStatus("Media Loaded");
        break;
    case QMediaPlayer::LoadingMedia:
        setStatus("Loading...");
        break;
    case QMediaPlayer::BufferingMedia:
        setStatus("Buffering...");
        break;
    case QMediaPlayer::BufferedMedia:
        setStatus("Media Buffered");
        break;
    case QMediaPlayer::StalledMedia:
        setStatus("Stalled Media");
        break;
    case QMediaPlayer::EndOfMedia:
        setStatus("End of Media");
        emit ClickNext();
        break;
    case QMediaPlayer::InvalidMedia:
        setStatus("Invalid Media");
        break;
    }
}

void MusicPlayer::metaDataChanged(){

        auto data = Player->metaData();
        QString Title = data.stringValue(data.Title);
        QString Artist = data.stringValue(data.AlbumArtist);

        if (Title.trimmed().isEmpty()){
            titleLabel->setText("Unknown Title");
        } else{
            titleLabel->setText(Title);
        }

        if (Artist.trimmed().isEmpty()){
            authorLabel->setText("Unknown Artist");
        } else{
            authorLabel->setText(Artist);
        }

};

void MusicPlayer::nextClicked(){
    if (!playlist->isEmpty()) {
        if(*Shuffle == false){
            if(*CurrentIndex < (playlist->size() -1)) {
                (*CurrentIndex)++;
                * status = Player->playbackState();
                Player->setSource(playlist->at(*CurrentIndex).first);
                emit CurrentIndexChanged(status);
            }
            else{
                *CurrentIndex = 0;
                * status = Player->playbackState();
                Player->setSource(playlist->at(*CurrentIndex).first);
                emit CurrentIndexChanged(status);
            }
        } else {
            int length = playlist->length();
            int x = rand() % length;
            *CurrentIndex = x;
            * status = Player->playbackState();
            Player->setSource(playlist->at(*CurrentIndex).first);
            emit CurrentIndexChanged(status);
        }
    }
};

void MusicPlayer::previousClicked(){
    if (!playlist->isEmpty()) {
        if (*Shuffle == false){
            if(*CurrentIndex > 0) {
                (*CurrentIndex)--;
                * status = Player->playbackState();
                Player->setSource(playlist->at(*CurrentIndex).first);
                emit CurrentIndexChanged(status);
            }
            else{
                *CurrentIndex = (playlist->size() -1);
                * status = Player->playbackState();
                Player->setSource(playlist->at(*CurrentIndex).first);
                emit CurrentIndexChanged(status);
            }
        } else {
            int length = playlist->length();
            int x = rand() % length;
            *CurrentIndex = x;
            * status = Player->playbackState();
            Player->setSource(playlist->at(*CurrentIndex).first);
            emit CurrentIndexChanged(status);
        }
    }

};

void MusicPlayer::handleShuffle(bool IsShuffled){
    if (!playlist->isEmpty()) {
        *Shuffle = IsShuffled;
    }
}

void MusicPlayer::handleCurrentIndexChanged(QMediaPlayer::PlaybackState * state){
    if(*state == QMediaPlayer::PlayingState){
        Player->play();
    }
    else if(*state == QMediaPlayer::PausedState || *state == QMediaPlayer::StoppedState){
        Player->stop();
    }

    if (playlistDialog){
        playlistDialog->getCurrentIndex(CurrentIndex);
    }
}

void MusicPlayer::seek(int position){
    // Calculate the position in milliseconds based on the slider's value
    qint64 newPosition = static_cast<qint64>((position * Player->duration()) / slider->maximum());
    // Set the position of the media player
    Player->setPosition(newPosition);
};

void MusicPlayer::handleCursor(QMediaPlayer::MediaStatus status) {

#ifndef QT_NO_CURSOR
    if (status == QMediaPlayer::LoadingMedia || status == QMediaPlayer::BufferingMedia
        || status == QMediaPlayer::StalledMedia)
        setCursor(QCursor(Qt::BusyCursor));
    else
        unsetCursor();
#endif

};
