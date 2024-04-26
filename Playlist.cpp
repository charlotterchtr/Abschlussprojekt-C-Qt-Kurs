
#include "Playlist.h"

PlaylistDialog::PlaylistDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Your Playlist");

    layout = new QVBoxLayout(this);

    playlistWidget = new QListWidget(this);
    playlistWidget->setSelectionMode(QAbstractItemView::SingleSelection); // Allow selecting only one item at a time

    CurrentIndex = new int;
    *CurrentIndex = 0;
    connect(this, &PlaylistDialog::CurrentIndexChanged, this, &PlaylistDialog::highlightCurrentIndex);

    layout->addWidget(playlistWidget);
    connect(this, &PlaylistDialog::show_playlist, this, &PlaylistDialog::handleShowPlaylist);

    playlistWidget->setMouseTracking(true);
    connect(playlistWidget, &QListWidget::itemDoubleClicked, this, &PlaylistDialog::deleteSelectedItem);

    setLayout(layout);
}

//maybe leave drag and drop out of it! OK
//display title - done!
//instead: highlight current  (not working) - now mostly working
//shuffle/no shuffle - done
//delete song (not working) -done
//save playlist after closing the player - don't know


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
    *CurrentIndex = *i;
    emit CurrentIndexChanged();
}

void PlaylistDialog::highlightCurrentIndex() {
    if (!playlist->isEmpty()){
        // Check if CurrentIndex is valid
        if (CurrentIndex != nullptr && *CurrentIndex >= 0 && *CurrentIndex < playlistWidget->count()) {

            // Reset the background color of all items
            for (int i = 0; i < playlistWidget->count(); ++i) {
                QListWidgetItem * item = playlistWidget->item(i);
                item->setData(Qt::BackgroundRole, QVariant()); // Reset background color
            }

            // Set the background color of the current item using a delegate
            QPalette palette = QApplication::palette();
            palette.setColor(QPalette::Highlight, Qt::green);
            playlistWidget->item(*CurrentIndex)->setData(Qt::BackgroundRole, palette.color(QPalette::Highlight));

        }
    }
}

void PlaylistDialog::deleteSelectedItem(){
    QList<QListWidgetItem *> selectedItems = playlistWidget->selectedItems();
    if (!selectedItems.isEmpty()) { //Check if Empty!!
        // Get the selected item
        QListWidgetItem *selectedItem = selectedItems.first();
        int index = playlistWidget->row(selectedItem);

        playlistWidget->takeItem(index);
        playlist->removeAt(index);

        if(playlist->isEmpty()){
            QString text = "No Song in Playlist. Please add a song";
            QListWidgetItem * item = new QListWidgetItem(text); // Create a new item with the file name
            playlistWidget->addItem(item); // Add the item to the playlistWidget
        }
        emit ItemDeleted(index);
    }
}
