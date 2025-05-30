#include <iostream>
#include <string>
using namespace std;

struct Video
{
    string judul;
    int durasi;
    string status;
    Video *left;
    Video *right;
};

struct PlaylistNode
{
    Video *video;
    PlaylistNode *next;
};

struct Riwayat
{
    Video *video;
    Riwayat *next;
};

struct Undo
{
    string aksi;
    Video *video;
    Undo *next;
};

Video *root = nullptr;
PlaylistNode *awalPlaylist = nullptr;
Riwayat *awalRiwayat = nullptr;
Undo *atasUndo = nullptr;

Video *buatVideo(string judul, int durasi)
{
    Video *newNode = new Video;
    newNode->judul = judul;
    newNode->durasi = durasi;
    newNode->status = "tersedia";
    newNode->left = nullptr;
    newNode->right = nullptr;
    return newNode;
}

void simpanUndo(string aksi, Video *video)
{
    Undo *newNode = new Undo;
    newNode->aksi = aksi;
    newNode->video = video;
    newNode->next = atasUndo;
    atasUndo = newNode;
}

bool cariJudul(Video *root, string judul)
{
    if (root == nullptr)
        return false;
    if (root->judul == judul)
        return true;
    if (judul < root->judul)
        return cariJudul(root->left, judul);
    return cariJudul(root->right, judul);
}

Video *tambahKePohon(Video *root, Video *newNode)
{
    if (root == nullptr)
        return newNode;
    if (newNode->judul < root->judul)
        root->left = tambahKePohon(root->left, newNode);
    else
        root->right = tambahKePohon(root->right, newNode);
    return root;
}

void tampilInorder(Video *root)
{
    if (root == nullptr)
        return;
    tampilInorder(root->left);
    cout << "Judul: " << root->judul << ", Durasi: " << root->durasi << " menit, Status: " << root->status << endl;
    tampilInorder(root->right);
}

Video *cariVideo(Video *root, string judul)
{
    if (root == nullptr)
        return nullptr;
    if (judul == root->judul)
        return root;
    if (judul < root->judul)
        return cariVideo(root->left, judul);
    return cariVideo(root->right, judul);
}

Video *hapusDariPohon(Video *root, string judul, bool &ditemukan, Video *&disimpan)
{
    if (root == nullptr)
        return nullptr;
    if (judul < root->judul)
        root->left = hapusDariPohon(root->left, judul, ditemukan, disimpan);
    else if (judul > root->judul)
        root->right = hapusDariPohon(root->right, judul, ditemukan, disimpan);
    else
    {
        ditemukan = true;
        disimpan = root;
        if (root->left == nullptr)
            return root->right;
        else if (root->right == nullptr)
            return root->left;
        Video *pengganti = root->right;
        while (pengganti->left != nullptr)
            pengganti = pengganti->left;
        root->judul = pengganti->judul;
        root->durasi = pengganti->durasi;
        root->status = pengganti->status;
        root->right = hapusDariPohon(root->right, pengganti->judul, ditemukan, disimpan);
    }
    return root;
}

void tambahVideo()
{
    string judul;
    int durasi;
    cout << "Masukkan judul video: ";
    getline(cin, judul);
    cout << "Masukkan durasi video (menit): ";
    cin >> durasi;
    cin.ignore();
    if (cariJudul(root, judul))
    {
        cout << "Video dengan judul " << judul << " sudah ada!" << endl;
        return;
    }
    Video *newNode = buatVideo(judul, durasi);
    root = tambahKePohon(root, newNode);
    simpanUndo("tambah", newNode);
    cout << "Video dengan judul " << judul << " berhasil ditambahkan." << endl;
}

void tampilkanVideo()
{
    cout << "\nDaftar Video :" << endl;
    tampilInorder(root);
    cout << "\nApakah Anda ingin mencari video? (y/t): ";
    char answer;
    cin >> answer;
    cin.ignore();
    if (answer == 'y' || answer == 'Y')
    {
        string cari;
        cout << "Masukkan judul video: ";
        getline(cin, cari);
        Video *hasil = cariVideo(root, cari);
        if (hasil != nullptr)
        {
            cout << "Ditemukan: " << hasil->judul << ", Durasi: " << hasil->durasi << ", Status: " << hasil->status << endl;
        }
        else
        {
            cout << "Video tidak ditemukan!" << endl;
        }
    }
}

void tambahKePlaylist()
{
    string judul;
    cout << "Masukkan judul video yang ingin ditambahkan ke playlist: ";
    getline(cin, judul);
    Video *vid = cariVideo(root, judul);
    if (!vid)
    {
        cout << "Video tidak ditemukan." << endl;
        return;
    }
    if (vid->status != "tersedia")
    {
        cout << "Video tidak tersedia!" << endl;
        return;
    }
    PlaylistNode *newNode = new PlaylistNode;
    newNode->video = vid;
    newNode->next = nullptr;
    if (!awalPlaylist)
    {
        awalPlaylist = newNode;
        vid->status = "sedang diputar";
    }
    else
    {
        PlaylistNode *temp = awalPlaylist;
        while (temp->next)
            temp = temp->next;
        temp->next = newNode;
        vid->status = "dalam antrean";
    }
    simpanUndo("playlist", vid);
    cout << "Video berhasil ditambahkan ke playlist." << endl;
}

void tontonVideo()
{
    if (!awalPlaylist)
    {
        cout << "Playlist kosong." << endl;
        return;
    }
    PlaylistNode *tonton = awalPlaylist;
    Video *vid = tonton->video;
    cout << "Menonton: " << vid->judul << "..." << endl;
    vid->status = "tersedia";
    Riwayat *riw = new Riwayat;
    riw->video = vid;
    riw->next = awalRiwayat;
    awalRiwayat = riw;
    awalPlaylist = awalPlaylist->next;
    delete tonton;
    simpanUndo("tonton", vid);
    cout << "Video selesai ditonton" << endl;
}

void tampilHistory()
{
    cout << "\nRiwayat video yang telah ditonton:" << endl;
    Riwayat *temp = awalRiwayat;
    while (temp)
    {
        cout << temp->video->judul << " (" << temp->video->durasi << " menit)" << endl;
        temp = temp->next;
    }
}

void hapusVideo()
{
    string judul;
    cout << "Masukkan judul video yang ingin dihapus: ";
    getline(cin, judul);
    Video *target = cariVideo(root, judul);
    if (!target)
    {
        cout << "Video tidak ditemukan!" << endl;
        return;
    }
    if (target->status != "tersedia")
    {
        cout << "Video sedang diputar/masih dalam antrean. Yakin ingin menghapus? (y/t): ";
        char yakin;
        cin >> yakin;
        cin.ignore();
        if (yakin != 'y' && yakin != 'Y')
            return;
    }
    bool ditemukan = false;
    Video *disimpan = nullptr;
    root = hapusDariPohon(root, judul, ditemukan, disimpan);
    simpanUndo("hapus", disimpan);
    cout << "Video dengan judul " << judul << " berhasil dihapus!" << endl;
}

void batalAksiAkhir()
{
    if (!atasUndo)
    {
        cout << "Tidak ada aksi terakhir yang bisa dibatalkan." << endl;
        return;
    }
    string aksi = atasUndo->aksi;
    Video *video = atasUndo->video;
    Undo *hapus = atasUndo;
    atasUndo = atasUndo->next;
    delete hapus;

    if (aksi == "tambah")
    {
        bool dummy = false;
        Video *dummyVid = nullptr;
        root = hapusDariPohon(root, video->judul, dummy, dummyVid);
        cout << "Tambah video dibatalkan." << endl;
    }
    else if (aksi == "hapus")
    {
        root = tambahKePohon(root, video);
        cout << "Hapus video dibatalkan." << endl;
    }
    else if (aksi == "playlist")
    {
        video->status = "tersedia";
        if (awalPlaylist && awalPlaylist->video == video)
        {
            PlaylistNode *hapus = awalPlaylist;
            awalPlaylist = awalPlaylist->next;
            delete hapus;
        }
        else
        {
            PlaylistNode *temp = awalPlaylist;
            while (temp->next && temp->next->video != video)
                temp = temp->next;
            if (temp->next)
            {
                PlaylistNode *hapus = temp->next;
                temp->next = hapus->next;
                delete hapus;
            }
        }
        cout << "Tambahkan ke playlist dibatalkan." << endl;
    }
    else if (aksi == "tonton")
    {
        Riwayat *hapus = awalRiwayat;
        awalRiwayat = awalRiwayat->next;
        delete hapus;
        PlaylistNode *newNode = new PlaylistNode;
        newNode->video = video;
        newNode->next = awalPlaylist;
        awalPlaylist = newNode;
        video->status = "sedang diputar";
        cout << "Tonton video dibatalkan." << endl;
    }
}

int main()
{
    int pilihan;
    do
    {
        cout << "\n===== WELCOME TO IDLIX TUBE =====" << endl;
        cout << "1. Tambah Video" << endl;
        cout << "2. Tampilkan Daftar Video" << endl;
        cout << "3. Tambah ke Playlist" << endl;
        cout << "4. Tonton Video" << endl;
        cout << "5. Riwayat Tontonan" << endl;
        cout << "6. Hapus Video" << endl;
        cout << "7. Undo Tindakan Terakhir" << endl;
        cout << "8. Keluar" << endl;
        cout << "Pilih menu: ";
        cin >> pilihan;
        cin.ignore();
        switch (pilihan)
        {
        case 1:
            tambahVideo();
            break;
        case 2:
            tampilkanVideo();
            break;
        case 3:
            tambahKePlaylist();
            break;
        case 4:
            tontonVideo();
            break;
        case 5:
            tampilHistory();
            break;
        case 6:
            hapusVideo();
            break;
        case 7:
            batalAksiAkhir();
            break;
        case 8:
            cout << "Terimakasi telah menggunakan IDLIX Tube!" << endl;
            cin.get();
            break;
        default:
            cout << "Pilihan tidak valid!" << endl;
        }
    } while (pilihan != 8);
    return 0;
}