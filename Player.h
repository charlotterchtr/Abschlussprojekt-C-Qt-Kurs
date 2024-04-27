#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMediaMetaData>
#include <QPushButton>
#include <QGridLayout>
#include <QFileDialog>
#include <QLabel>
#include <QSlider>
#include <QListWidget>

#include "Playlist.h"
#include "Control.h"

class MusicPlayer : public QWidget
{
    Q_OBJECT

    //Layouts
    QVBoxLayout * layout = nullptr;
    QBoxLayout * controlLayout = nullptr;
    QVBoxLayout * trackLayout = nullptr;
    QHBoxLayout * durationLayout = nullptr;

    //Player
    QMediaPlayer * Player = nullptr;
    QAudioOutput * audioOutput = nullptr;
    QMediaPlayer::PlaybackState * status = nullptr;
    int * CurrentIndex = nullptr;

    //Playlist
    QList<QUrl> * playlist = nullptr;
    QPushButton * ViewPlaylist = nullptr;
    PlaylistDialog * playlistDialog = nullptr;

    //Controls and Labels
    QSlider * slider = nullptr;
    QLabel * duration = nullptr;
    QLabel * titleLabel = nullptr;
    QLabel * authorLabel = nullptr;
    Controls * PlayerControl = nullptr;

    bool isPlayerAvailable() const;
    bool * Shuffle = nullptr;
    qint64 c_duration;

private slots:
    void open();
    void startPlayer();
    void playClicked();
    void previousClicked();
    void nextClicked();
    void metaDataChanged();
    void handleShuffle(bool IsShuffled);
    void handleCurrentIndexChanged(QMediaPlayer::PlaybackState * state);
    void handleItemDeleted(int index);

    void handleViewPlaylist();
    void handlePlaylistUpdated();
    void cleanupPlaylistDialog();

    void setStatus(QString const & status);
    void handleStateChanged();
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
    void seek(int mseconds);
    void updateDurationInfo(qint64 currentInfo);

    void handleCursor(QMediaPlayer::MediaStatus status);
    void displayErrorMessage(QMediaPlayer::Error error, const QString & errorString);

signals:
    void CurrentIndexChanged(QMediaPlayer::PlaybackState * state);
    void FirstSongAdded();
    void SongsAdded();
    void mediaLoaded();
    void PlaylistChanged();
    void deleteItemsSelected(QList<QListWidgetItem *> selectedItems);
    void ClickNext();

public:
    MusicPlayer(QWidget * parent = nullptr);
    ~MusicPlayer();
};
#endif // PLAYER_H
