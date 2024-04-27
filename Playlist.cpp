#include "Playlist.h"

PlaylistDialog::PlaylistDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Your Playlist");
    layout = new QVBoxLayout(this);

    playlistWidget = new QListWidget(this);
    playlistWidget->setSelectionMode(QAbstractItemView::SingleSelection); // Allow selecting one item for deletion

    layout->addWidget(playlistWidget);
    connect(this, &PlaylistDialog::show_playlist, this, &PlaylistDialog::handleShowPlaylist);

    playlistWidget->setMouseTracking(true);
    connect(playlistWidget, &QListWidget::itemDoubleClicked, this, &PlaylistDialog::deleteSelectedItem);
    connect(playlistWidget, &QListWidget::itemClicked, this, &PlaylistDialog::handleItemClicked);

    CurrentIndex = new int; // Current Index used for highligthing
    *CurrentIndex = 0;
    connect(this, &PlaylistDialog::CurrentIndexChanged, this, &PlaylistDialog::highlightCurrentIndex);

    setLayout(layout);
}

//add items of playlist to QListWidget to show
void PlaylistDialog::handleShowPlaylist(){

    if (playlistWidget->count() == 0){
        if (!playlist->isEmpty()) {
            for (const QUrl & song : *playlist) {
                QString fileName = song.fileName();
                QListWidgetItem * item = new QListWidgetItem(fileName);
                playlistWidget->addItem(item);
            }
        }
        else{
            QString text = "No Song in Playlist. Please add a song";
            QListWidgetItem * item = new QListWidgetItem(text);
            playlistWidget->addItem(item);
        }
    }
}

//get copy of original Player-Playlist (only for display and delete-selection)
void PlaylistDialog::getPlaylist(QList<QUrl> * orig_playlist){
    if (playlist) {
        delete playlist;
    }
    playlist = new QList<QUrl>(*orig_playlist); //copy playlist
    emit show_playlist();
}

//Current Index to highlight current song
void PlaylistDialog::getCurrentIndex(int * i){
    *CurrentIndex = *i;
    emit CurrentIndexChanged();
}

//to avoid interference with highlighting
void PlaylistDialog::handleItemClicked(){
    playlistWidget->setCurrentItem(nullptr);
}

//highlight current song
void PlaylistDialog::highlightCurrentIndex() {
    if (!playlist->isEmpty()){
        if (CurrentIndex != nullptr && *CurrentIndex >= 0 && *CurrentIndex < playlistWidget->count()) {

            // Reset the background color of all items
            for (int i = 0; i < playlistWidget->count(); ++i) {
                QListWidgetItem * item = playlistWidget->item(i);
                item->setData(Qt::BackgroundRole, QVariant());
            }

            // Set the background color of the current item
            QPalette palette = QApplication::palette();
            palette.setColor(QPalette::Highlight, Qt::green);
            playlistWidget->item(*CurrentIndex)->setData(Qt::BackgroundRole, palette.color(QPalette::Highlight));
        }
    }
}

//select song to be deleted from playlist, pass selected index to Player
void PlaylistDialog::deleteSelectedItem(){
    QList<QListWidgetItem*> selectedItems = playlistWidget->selectedItems();
    if (!selectedItems.isEmpty()) { //Check if Empty!!
        // Get the selected item
        QListWidgetItem * selectedItem = selectedItems.first();
        if(selectedItem->text() != "No Song in Playlist. Please add a song"){
            int index = playlistWidget->row(selectedItem);

            playlistWidget->takeItem(index);
            playlist->removeAt(index);

            // Explicitly set the current item to prevent interference with highligthing
            playlistWidget->setCurrentItem(nullptr);

            if(playlist->isEmpty()){
                QString text = "No Song in Playlist. Please add a song";
                QListWidgetItem * item = new QListWidgetItem(text);
                playlistWidget->addItem(item);
            }
            emit ItemDeleted(index);
        }
        selectedItems.clear();
    }
}

PlaylistDialog::~PlaylistDialog(){
    delete CurrentIndex;
}
