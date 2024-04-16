#include "Playlist.h"

PlaylistDialog::PlaylistDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Your Playlist");

    layout = new QVBoxLayout(this);

    playlistWidget = new QListWidget(this);

    playlistWidget->setDragDropMode(QAbstractItemView::InternalMove);
    playlistWidget->setAcceptDrops(true);

    layout->addWidget(playlistWidget);
    connect(this, &PlaylistDialog::show_playlist, this, &PlaylistDialog::handleShowPlaylist);

    setLayout(layout);
}

//maybe leave drag and drop out of it!
//instead: highlight current song
//shuffle/no shuffle
//delete song
//save playlist after closing the player


void PlaylistDialog::handleShowPlaylist(){

    if (playlistWidget->count() == 0){
        if (!playlist->isEmpty()) {
            for (const QPair<QUrl, int>& song : *playlist) {
                QString fileName = song.first.fileName(); // Get the file name
                QListWidgetItem* item = new QListWidgetItem(fileName); // Create a new item with the file name
                playlistWidget->addItem(item); // Add the item to the playlistWidget
            }
        }
        else{
            QString text = "No Song in Playlist. Please add a song";
            QListWidgetItem* item = new QListWidgetItem(text); // Create a new item with the file name
            playlistWidget->addItem(item); // Add the item to the playlistWidget
        }
    }
};

void PlaylistDialog::getPlaylist(QList<QPair<QUrl, int>> * orig_playlist){
    if(playlist == nullptr){
        playlist = new QList<QPair<QUrl, int>>;
    }
    else{
        delete playlist;
    }
    * playlist = * orig_playlist;
    emit show_playlist();
}

void PlaylistDialog::dropEvent(QDropEvent *event){
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        // Extract the dropped items
        QList<QUrl> urls = mimeData->urls();

        // Extract file names from dropped URLs
        QStringList fileNames;
        for (const QUrl &url : urls) {
            fileNames << url.toLocalFile();
        }

        // Calculate the new index where the items were dropped
        int newIndex = playlistWidget->indexAt(event->position().toPoint()).row();

        // Update the order of the playlist
        QList<QPair<QUrl, int>> newPlaylist;
        for (int i = 0; i < playlist->size(); ++i) {
            if (i == newIndex) {
                // Insert the dropped items at the new index
                for (const QString &fileName : fileNames) {
                    newPlaylist.append(qMakePair(QUrl::fromLocalFile(fileName), i));
                }
            }
            if (!fileNames.contains(playlist->at(i).first.toLocalFile())) {
                newPlaylist.append(playlist->at(i));
            }
        }
        *playlist = newPlaylist;

        // Update CurrentIndex if the currently playing song was moved
        for (int i = 0; i < playlist->size(); ++i) {
            if (playlist->at(i).second == *CurrentIndex) {
                *CurrentIndex = i;
                break;
            }
        }
    }
    event->acceptProposedAction();
};
