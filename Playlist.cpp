
#include "Playlist.h"

PlaylistDialog::PlaylistDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Your Playlist");

    layout = new QVBoxLayout(this);

    playlistWidget = new QListWidget(this);
    //playlistWidget->setStyleSheet("QListWidget::item:!selected { background-color: black; }");


    CurrentIndex = new int;
    connect(this, &PlaylistDialog::CurrentIndexChanged, this, &PlaylistDialog::highlightCurrentIndex);

    playlistWidget->setDragDropMode(QAbstractItemView::InternalMove);
    playlistWidget->setAcceptDrops(true);

    layout->addWidget(playlistWidget);
    connect(this, &PlaylistDialog::show_playlist, this, &PlaylistDialog::handleShowPlaylist);

    playlistWidget->setMouseTracking(true);
    connect(this, &PlaylistDialog::createMenu, this, &PlaylistDialog::createContextMenu);

    setLayout(layout);
}

//maybe leave drag and drop out of it! OK
//display title - done!
//instead: highlight current  (not working)
//shuffle/no shuffle
//delete song (not working)
//save playlist after closing the player


void PlaylistDialog::handleShowPlaylist(){

    if (playlistWidget->count() == 0){
        if (!playlist->isEmpty()) {
            for (const QPair<QUrl, int>& song : *playlist) {
                QString fileName = song.first.fileName(); // Get the file name
                QListWidgetItem * item = new QListWidgetItem(fileName); // Create a new item with the file name
                playlistWidget->addItem(item); // Add the item to the playlistWidget
            }
        }
        else{
            QString text = "No Song in Playlist. Please add a song";
            QListWidgetItem * item = new QListWidgetItem(text); // Create a new item with the file name
            playlistWidget->addItem(item); // Add the item to the playlistWidget
        }
    }
};

void PlaylistDialog::getPlaylist(QList<QPair<QUrl, int>> * orig_playlist){
    if (playlist) {
        delete playlist;
    }
    playlist = new QList<QPair<QUrl, int>>(* orig_playlist);
    emit show_playlist();
}

void PlaylistDialog::getCurrentIndex(int * i){
    CurrentIndex = i;
    emit CurrentIndexChanged();
}

void PlaylistDialog::highlightCurrentIndex() {
    // Check if CurrentIndex is valid
    if (CurrentIndex != nullptr && * CurrentIndex >= 0 && *CurrentIndex < playlistWidget->count()) {
        //playlistWidget->item(*CurrentIndex)->setForeground(Qt::yellow);
        std::cout << *CurrentIndex << std::endl;
        QListWidgetItem* currentItem = playlistWidget->item(*CurrentIndex);
        currentItem->setForeground(Qt::yellow);
        /*
         * QListWidgetItem * currentItem = playlistWidget->item(*CurrentIndex);
        // Set the background color of the item
        QPalette palette = currentItem->palette();
        palette.setColor(QPalette::Base, Qt::yellow); // You can set any color you want here
        currentItem->setPalette(palette);
        */
    }
}

void PlaylistDialog::createContextMenu(QMouseEvent * event) {
    const QPoint & pos = event->pos();
    std::cout << "mouse event" << std::endl;
    QListWidgetItem *selectedItem = playlistWidget->itemAt(pos);
    if (selectedItem) {
        QMenu contextMenu(tr("Context Menu"), this);
        QAction *deleteAction = new QAction(tr("Delete"), this);
        connect(deleteAction, &QAction::triggered, this, [this, selectedItem]() {
            // Delete the selected item from the playlist
            int index = playlistWidget->row(selectedItem);
            playlistWidget->takeItem(index);
            playlist->removeAt(index);
        });
        contextMenu.addAction(deleteAction);
        contextMenu.exec(playlistWidget->mapToGlobal(pos));
    }
}

void PlaylistDialog::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        std::cout << "mouse event" << std::endl;
        emit createMenu(event);
        //createContextMenu(event->pos());
    }
    //QDialog::mousePressEvent(event);
}

void PlaylistDialog::dropEvent(QDropEvent *event){
    /*
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
*/
};
