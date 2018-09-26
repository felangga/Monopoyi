// Monopoyi by edo putra sanjaya & felix angga e. 
// Copyright 2012. 

#include <stdio.h>
#include <conio.h>
#include <iostream.h>
#include <string.h>
#include <windows.h>
#include <time.h>

struct Player {
   int nomor;                           // Nomor Player
   char nama[20];                       // Nama Player
   int zeny;                            // Jumlah Zeny milik player default 2000 zeny
   int posisi_kotak;                    // Posisi pemain di kotak ?
   int putaran;                         // Sudah berapa putaran ?
   bool bebas_penjara;                  // apakah punya kartu bebas penjara ?
   int jml_dadu;                        // Jumlah dadu yang dilempar
   bool dipenjara;                      // Sedang dipenjara ?
} player[6];                            // 6 untuk backup

struct Kotak {
   int nomor_kotak;                     // Nomor Kotak
   int warna;                           // Warna Kotak
   int pemilik_kotak;                   // Pemilik Kotak 
   int terakhir_upgrade;                // Upgrade terakhir dari kotak
   int harga;                           // Harga Kotak
   
   Kotak *next;
   Kotak *prev; 
} kotak[22];

struct Kartu {
   int head;
   int id_kartu[35];
   int tail;    
};

Kartu kartu;

Kotak *head;
Kotak *info_kotak;
Kotak *bantu;

int kotak_warna[16] = {0,0,0,0,14,14,14,14,2,2,2,0xD,0xD,1,1,12};               // Inisialisasi warna kotak default 
int jml_pemain      = 0;                                                        // Jumlah pemain yang akan bermain


void setcolor(unsigned short color) 
{ 
  HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE); 
  SetConsoleTextAttribute(hCon,color); 
}

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
COORD CursorPosition;

void gotoxy(int x, int y) {
  CursorPosition.X = x;
  CursorPosition.Y = y;
  SetConsoleCursorPosition(console, CursorPosition);    
}

void Inisialisasi_antrian () {
  // FIFO
  kartu.head = kartu.tail = -1;
}

void antrian_tambah (int data) {
  if (kartu.tail == -1) {                                                       // jika masih kosong
    kartu.head = kartu.tail = 0;
    kartu.id_kartu[kartu.tail] = data;
  } else {
    kartu.tail ++;
    kartu.id_kartu[kartu.tail] = data;                                          // tambah data terakhir
  }
}

int antrian_kurang () {
  int id_kartu = kartu.id_kartu[kartu.head];
  for (int i = kartu.head; i < kartu.tail - 1; i++) {
    kartu.id_kartu[i] = kartu.id_kartu[i+1];
  }
  
  if (id_kartu == 31) {                                                         // Jika nemu kartu bebas penjara
    kartu.id_kartu[kartu.tail] = -1;
  } else {
    kartu.id_kartu[kartu.tail] = id_kartu;      
  }
  
  return id_kartu;
}

void Inisialisasi_awal() {
   bool bener = false;
   while (bener == false){   
   system("cls");

   // Input nama pemain 
   printf(" __  __                                     _ \n");
   printf("|  \\/  |                                   (_)\n");
   printf("| \\  / | ___  _ __   ___  _ __   ___  _   _ _ \n");
   printf("| |\\/| |/ _ \\| '_ \\ / _ \\| '_ \\ / _ \\| | | | |\n");
   printf("| |  | | (_) | | | | (_) | |_) | (_) | |_| | |\n");
   printf("|_|  |_|\\___/|_| |_|\\___/| .__/ \\___/ \\__, |_|\n");
   printf("                         | |           __/ |  \n");
   printf("                         |_|          |___/   \n");
   printf("-----------------------------------------------\n");
   printf("| Edo Putra Sanjaya & Felix Angga Erlandhita  |\n");
   printf("-----------------------------------------------\n");
   
   printf("\nBerapa orang yang akan bermain ? (3-5) = "); scanf("%d", &jml_pemain); fflush(stdin);
   printf("\n=====================================================\n");
   if (jml_pemain >= 3 && jml_pemain <= 5) { 
     for (int i = 0; i < jml_pemain; i++) {
      player[i].nomor = i+1;
      printf("Pemain %d, nama pemain ? ", i+1); gets(player[i].nama); fflush(stdin);   
     }
     printf("=====================================================\n");
     bener = true;
   
   } else {
      printf("\n\n\tInputan tidak sesuai !");
      getch();
  }
  }
  int acak, acak2;
  printf ("\n - Pengacakan Dadu - \n\n");
  for (int i = 0; i < jml_pemain; i ++) {
    printf("%s\t: ", player[i].nama);     
    getch();
    acak = (rand()%4)+1;
    acak2 = (rand()%4)+1;
    printf("%d %d = %d", acak, acak2, acak+acak2); 
    printf("\n");
    getch();
    player[i].jml_dadu = acak+acak2;
  }
  
  
  // Sorting pemain yang punya dadu lebih banyak
  for (int sort = 0; sort < jml_pemain; sort ++) {
    for (int i = sort + 1; i < jml_pemain; i ++) {
      if (player[sort].jml_dadu < player[i].jml_dadu) {
         player[5]     = player[sort];
         player[sort]  = player[i];
         player[i]     = player[5];
      }    
    }  
  }
  printf("\n - Urutan pemain -\n\n");
 
  for (int i = 0; i < jml_pemain; i++){
   printf("%d. %s", i+1, player[i].nama);
    printf("\n");    
  }
  getch();

  Kotak *bantu, *baru;
  
  for (int q = 0; q < 16; q ++) {
    int acak  = rand()%16;
    int acak2 = rand()%16;
    
    int temp = kotak_warna[acak];
    kotak_warna[acak]  = kotak_warna[acak2];
    kotak_warna[acak2] = temp;
  }
  
  int count = 0;
  for (int i = 0; i < 21; i ++) {                                               // Inisialisasi DLLC
      
      baru = new Kotak;
      baru -> nomor_kotak = i+1;                                                // Nomor Kotak
      baru -> harga = 0;
      if (i != 0 && i != 5 && i != 10 && i != 15) {
         baru -> warna = kotak_warna[count];                                    // Warna Kotak
         switch(kotak_warna[count]) {                                           // set harga kotak
          case 0 : baru -> harga = 1000; break;                                 // warna hitam
          case 14: baru -> harga = 2000; break;                                 // warna kuning;
          case 2 : baru -> harga = 4000; break;                                 // warna hijau;                       
          case 1 : baru -> harga = 7000; break;                                 // warna biru
          case 12: baru -> harga = 1000; break;                                 // warna merah
          default: baru -> harga = 0; break;
         }  
         count ++;
      } else baru -> warna = 15;                                                // Pojok - pojok set warna 15 (putih)
      baru->pemilik_kotak = -2;                                                 // Mengapa -2, sukasuka :P
      baru->terakhir_upgrade = 0;
     
      baru -> next = NULL;
      baru -> prev = NULL;
      
      if (head == NULL) {
         head = baru;
         head -> next = head;
         head -> prev = head;
      } else {
         baru -> next = head;
         head -> prev -> next = baru;
         baru -> prev = head -> prev;
         head -> prev = baru;    
      }
  }
  
  // Inisialisasi Pemain
  for (int pemain = 0; pemain <= jml_pemain; pemain++) {
    player[pemain].zeny = 2000;
    player[pemain].putaran = 0;
    player[pemain].posisi_kotak = 1;
    player[pemain].bebas_penjara = false;
    player[pemain].dipenjara = false;
  }
  
  Inisialisasi_antrian();
  
  // Inisialisasi tumpukan kartu
  int temp [36] = {0};
  srand(time(NULL));
  for (int kartu = 0; kartu < 35; kartu ++) temp [kartu] = kartu;
    
  for (int acak = 0; acak < 35; acak ++) {
      int waton   = rand() % 34;
      int tampung = temp[acak];
      temp[acak]  = temp[waton];
      temp[waton] = tampung;
          
      antrian_tambah(temp[acak]);
  }
  
}

void informasi_kotak(int nomor_kotak) {
   bantu = head;
   if (nomor_kotak == bantu->nomor_kotak) 
     info_kotak = bantu;
  else {
   bantu = bantu -> next; 
   while (bantu != head)  {
     if (bantu->nomor_kotak == nomor_kotak) {
       info_kotak = bantu;
       break;              
     }
     bantu = bantu -> next;    
   }    
 }   
}

void tulis_di_kotak (char isi[20], int posisi, int xc, int yc) {                // x dan y dimulai dari kordinat dalam kotak
  if (posisi == 0) posisi = 1;
  int x = (((6 - posisi) * 13) + 3) + xc;                                       // 6 menunjukan jumlah kotak dalam 1 baris
  int y = 27 + yc;                                                              // 28 fix sebagai posisi awal pemain di kotak 1
  
  if (posisi <= 6) { 
    gotoxy(x,y);        
  } else if (posisi <= 11) {
    y = (5 * (11-posisi)) + 2+yc;
    gotoxy(2+xc, y);
  } else if (posisi <= 16) {
    y = 2+yc;
    x = (((posisi-11) * 13) + 3) + xc;
    gotoxy(x,y);
  } else if (posisi <= 21) {
    x = 68 + xc;
    y = ((5 * (posisi-16)) + 2) + yc;
    gotoxy(x, y);
  }
  informasi_kotak(posisi);  
  setcolor((info_kotak->warna * 16)+15);                                        // ambil warna background dan sesuaikan
  printf("%s",isi);  
}

/* Fungsi untuk menggambar posisi orang yang ada di
   papan permainan */
void pos_pemain (int pemain, int posisi) {  
  posisi = posisi % 20;
  if (posisi == 0) posisi = 1;
  int x = (((6 - posisi) * 13) + 3)+ pemain;                                    // 6 menunjukan jumlah kotak dalam 1 baris
  int y = 28;                                                                   // 28 fix sebagai posisi awal pemain di kotak 1
  
  if (posisi <= 6) { 
    gotoxy(x,y);        
  } else if (posisi <= 11) {
    y = (5 * (11-posisi)) + 3;
    gotoxy(2+pemain, y);
  } else if (posisi <= 16) {
    y = 3;
    x = (((posisi-11) * 13) + 3) + pemain;
    gotoxy(x,y);
  } else if (posisi <= 21) {
    x = 69 + pemain;
    y = (5 * (posisi-16)) + 3;
    gotoxy(x, y);
  }
  
  setcolor(9+pemain);
  printf("%d",player[pemain-1].nomor);
}

void papan() {  
  printf("\n");
  gotoxy(0,0);
  setcolor(7);
  printf("%c", 201);
  for (int i = 0; i < 78; i++) if (i % 13 == 0 && i != 0) printf("%c", 209); else printf("%c",205);   
  printf("%c", 187);

  for (int j = 0; j < 4; j++) {
    printf("%c",186);
    for (int i = 0; i < 78; i++) { 
        switch(i/13) {
           case 0 : setcolor(15); break;
           case 1 : setcolor(kotak_warna[8]); break;
           case 2 : setcolor(kotak_warna[9]); break;
           case 3 : setcolor(kotak_warna[10]); break;
           case 4 : setcolor(kotak_warna[11]); break;
           case 5 : setcolor(15); break;
         } 
        if (i % 13 == 0 && i != 0) { setcolor(7); printf("%c", 179);} else printf ("%c", 219); 
    }
    setcolor(7);
    printf("%c",186);
  }
  
  setcolor(7); printf("%c", 186);
  for (int i = 0; i < 78; i++) if ((i == 13 || i == 65 ) && i != 0) printf("%c", 197); else printf("%c",196);  setcolor(7); printf("%c", 186);   
  
  for (int j = 1; j < 20; j ++) {
    setcolor(7);
    printf("%c", 186);
    
    if (j % 5 == 0 && j != 0) { 
      for (int i = 0; i < 78; i++) 
        if ((i == 13 || i == 65 ) && i != 0) {
          if (i == 13) printf("%c", 180); 
          else if (i==65) { setcolor(7); printf("%c",195); }
         } 
         else 
          if (i>13 && i<=64) { 
            setcolor(0); 
            printf("%c",196);
          } else { 
            setcolor(7); 
            printf("%c",196);
          }
      setcolor(7); printf("%c", 186);
    } else {    
    for (int i = 0; i < 78; i++) {
       switch (j) {
         case 1: case 2: case 3: case 4:
         switch(i/13) {
          case 0 : setcolor(kotak_warna[7]); break;
          case 1 : setcolor(0); break;
          case 2 : setcolor(0); break;
          case 3 : setcolor(0); break;
          case 4 : setcolor(0); break;
          case 5 : setcolor(kotak_warna[12]); break;
         } break;
         case 5: case 6: case 7: case 8: case 9:
          switch(i/13) {
          case 0 : setcolor(kotak_warna[6]); break;
          case 1 : setcolor(0); break;
          case 2 : setcolor(0); break;
          case 3 : setcolor(0); break;
          case 4 : setcolor(0); break;
          case 5 : setcolor(kotak_warna[13]); break;
         } break;    
         case 10: case 11: case 12: case 13: case 14:
          switch(i/13) {
          case 0 : setcolor(kotak_warna[5]); break;
          case 1 : setcolor(0); break;
          case 2 : setcolor(0); break;
          case 3 : setcolor(0); break;
          case 4 : setcolor(0); break;
          case 5 : setcolor(kotak_warna[14]); break;
         } break;    
         case 15: case 16: case 17: case 18: case 19:
           switch(i/13) {
          case 0 : setcolor(kotak_warna[4]); break;
          case 1 : setcolor(0); break;
          case 2 : setcolor(0); break;
          case 3 : setcolor(0); break;
          case 4 : setcolor(0); break;
          case 5 : setcolor(kotak_warna[15]); break;
         } break;       
        }
               
        if ((i == 13 || i == 65) && i != 0) { setcolor(7); printf("%c", 179);} else printf ("%c", 219); 
    }    
    setcolor(7); printf("%c", 186);
    }
   
  }
  setcolor(7); printf("%c", 186);
  for (int i = 0; i < 78; i++) if ((i == 13 || i == 65 ) && i != 0) printf("%c", 197); else printf("%c",196);  setcolor(7); printf("%c", 186);
  for (int j = 0; j < 4; j++) {
    printf("%c",186);
    for (int i = 0; i < 78; i++) { 
        switch(i/13) {
           case 0 : setcolor(15); break;
           case 1 : setcolor(kotak_warna[3]); break;
           case 2 : setcolor(kotak_warna[2]); break;
           case 3 : setcolor(kotak_warna[1]); break;
           case 4 : setcolor(kotak_warna[0]); break;
           case 5 : setcolor(15); break;
         } 
        if (i % 13 == 0 && i != 0) { setcolor(7); printf("%c", 179);} else printf ("%c", 219); 
    }
    setcolor(7);
    printf("%c",186);
  }
  printf("%c", 200); for (int i = 0; i < 78; i++) if (i % 13 == 0 && i != 0) printf("%c", 207); else printf("%c",205);   
  printf("%c", 188);
  setcolor(0xF1);
  gotoxy(69,27); printf("<- START");
  gotoxy(2, 27); printf("Msk Penjara");
  gotoxy(69, 2); printf("PENJARA !");
  gotoxy(2, 2); printf("Park. Bebas");
  
  for (int kertu = 1; kertu < 21; kertu ++) {                                   // Tulis tulisan kartu di kotak pink
    if (kertu != 1 && kertu != 6 && kertu != 11 && kertu != 16) {
      informasi_kotak(kertu);
      if (info_kotak -> warna == 0xD) {
        tulis_di_kotak("KARTU",kertu,2,0);
      }
    }   
  }
  
  gotoxy(0, 31); setcolor(10);
  for (int i = 0; i < jml_pemain; i ++) printf("%s (%d) : %d zeny\t", player[i].nama, player[i].nomor, player[i].zeny);
  setcolor(0); for(int i = 0; i < 80; i++) printf(" ");
  // gambar posisi pemain
  for (int i = 0; i < jml_pemain; i ++) {
     pos_pemain(i+1, player[i].posisi_kotak);    
  }  
  
}

// Hapus pemain yang sudah gugur
void kill_pemain(int nomor_pemain) {
   for (int i = 0; i < jml_pemain; i++) {
     if(player[i].nomor == nomor_pemain) {
        
        player[5] = player[i];                   
        player[i] = player[i+1];
        player[i+1] = player[5];
     }
   }
   jml_pemain--;
}

// Fungsi untuk mendenda pemain
// 1 Jika lunas, 0 jika belum
int denda(int giliran, int harga, int pemilik_kotak) {
 if (player[giliran].zeny >= harga) {
   setcolor(10);
   gotoxy(16,13); printf("Anda mempunyai %d zeny untuk ", player[giliran].zeny); 
   gotoxy(16,14); printf("membayar denda, tekan ENTER ");
   gotoxy(16,15); printf("untuk membayar.");
   if (pemilik_kotak < 5) player[pemilik_kotak].zeny += harga;                  // tambah uang ke pemilik kotak
   player[giliran].zeny -= harga;
   getch();                                                                      // kurangi uang yang kena denda
   return 1;
 } else {
   int kotak_milik[20];
   // Searching 
   bantu = head;
   bantu = bantu -> next;
   int count = 0;
   while(bantu != head) {
     if (bantu->pemilik_kotak == giliran) {
        kotak_milik[count] = bantu->nomor_kotak;
        count ++;                     
     }           
     bantu = bantu -> next;
   }
   
   if (count > 0) {
     gotoxy(16,13); printf("Zeny anda tidak cukup untuk ");
     gotoxy(16,14); printf("membayar denda, silahkan jual kotak anda");
            
     for (int i = 0; i < (count%4); i ++) {                            // tampilkan batasi sampai 4, mengingat tempat sempit
       setcolor(14);
       informasi_kotak(kotak_milik[i]);
       gotoxy(16,15+i); printf("- kotak %d, %d zeny", kotak_milik[i], info_kotak->harga);    
     }
          
   bool jual = false;
   int input;
             
   while (!jual) {
     setcolor(0); gotoxy(16,21); printf("%d bukan kotak milik anda !", input);
     setcolor(14);
     gotoxy(16,20); for(int i=0; i<34; i++) printf(" ");             // hapus background
     gotoxy(16,20); printf("Kotak yang akan anda jual : ");  scanf("%d", &input); fflush(stdin);
     for (int cari = 0; cari < count; cari ++) if (input == kotak_milik[cari]) jual = true;                
              
     if (jual == false) { setcolor(12); gotoxy(16,21); printf("%d bukan kotak milik anda !", input); }
   }
              
   // Jual kotak
   informasi_kotak(input);
   info_kotak->pemilik_kotak = -2;
   player[giliran].zeny      = info_kotak->harga;
   return 0;
  } else {
   setcolor(12);
   gotoxy(16,13); printf("Zeny anda tidak cukup untuk ");
   gotoxy(16,14); printf("membayar denda, anda juga tidak punya");
   gotoxy(16,15); printf("kotak, maaf anda KALAH!!"); 
   kill_pemain(giliran);
   getch();
   return 1;
  }
 }    
}

void masuk_penjara(int giliran) {
  setcolor(14); gotoxy(16,6);
  printf("- PENJARA -");
  
  if (player[giliran].bebas_penjara != true){                                   // tidak punya kartu bebas penjara
    setcolor(14); gotoxy(16,9);
    printf("Whoops, bukan hari keberuntungan anda, ");
    gotoxy(16,10); printf("anda harus masuk penjara :(");
    
    if (player[giliran].zeny >= 2000) {
       setcolor(10); gotoxy(16,11);
       printf("Bayar 2000 zeny atau tunggu %d giliran ? [B] : ", jml_pemain-1);
       char input; input = getchar(); fflush(stdin);
       if (input == 'B' || input == 'b') {
          player[giliran].zeny -= 2000;         
          player[giliran].dipenjara = false;
       } else {
         player[giliran].dipenjara = true;
         player[giliran].posisi_kotak = 16;       
       }
    } else {
      setcolor(14); gotoxy(16,11);
      printf("Anda tidak mempunyai uang, silahkan tunggu %d giliran.", jml_pemain-1);
      player[giliran].dipenjara = true;  
      player[giliran].posisi_kotak = 16;    
    }
  } else {  
    setcolor(10); gotoxy(16,7);
    printf("Anda terselamatkan oleh kartu bebas penjara :)");       
    player[giliran].bebas_penjara = false;
    getch();
  }
}


int main () {

  head = NULL;
  srand(time(NULL));
  Inisialisasi_awal();
  bool rampung = false;
  int giliran = 0;
  system("cls");
  papan();
  // gambar posisi pemain
  for (int i = 0; i < jml_pemain; i ++) {
     pos_pemain(i+1, player[i].posisi_kotak);    
  }   

  
  while (jml_pemain > 1 || player[giliran].zeny > 50000) {
      giliran = giliran % jml_pemain;
      if (giliran == 0) {
        for (int i = 0; i < jml_pemain; i++) player[i].dipenjara = false;           
      }
      setcolor(10); gotoxy(65-strlen(player[giliran].nama),0); printf("Giliran : %s", player[giliran].nama);
      // section acak dadu 
      setcolor(14);
      gotoxy(16,6); printf("- Acak Dadu -\n");  
      bool stop = false;
      int acak  = 0;
      int acak2 = 0;
          
      gotoxy(16,8); setcolor(14); printf("%s\t:", player[giliran].nama);
      stop = false;
      while (!stop) {                                                           // ngacak dadu
        
        gotoxy(28,8);
        acak = (rand()%4)+1;
        acak2 = (rand()%4)+1;
        
        printf("%d, %d       ( Tekan sembarang tombol )", acak, acak2);   
      
        if(kbhit()) stop = true;
      }
      // handle sudah berapa kali putaran
      player[giliran].posisi_kotak += (acak+acak2); 
           
      getch();
      if (acak == acak2) { gotoxy (25,15); printf ("Binggo ! Lempar sekali lagi!"); }
      getch();
      
      papan();
      // ambil informasi kotak 
      informasi_kotak (player[giliran].posisi_kotak);      
      if (info_kotak->harga == 0 && info_kotak -> warna != 15) {
         int ambilkartu = antrian_kurang();
         setcolor(14);
         gotoxy(16, 6); printf("- KARTU -");
         gotoxy(16, 8); printf("Anda mendapatkan kartu nomor %d, " , ambilkartu+1);
         
         switch(ambilkartu) {
           case 0 : case 1 : case 2 : case 3 : 
           case 4 : { 
                      setcolor(10); gotoxy(16, 9); printf("Selamat, anda maju %d kotak!", ambilkartu + 1);
                      player[giliran].posisi_kotak += ambilkartu+1;
                      
                    } break;
           case 5 : case 6 : case 7 : case 8 :
           case 9 : { 
                      setcolor (14); gotoxy(16, 9); printf("Whoops, anda mundur %d kotak!", ambilkartu - 4);
                      player[giliran].posisi_kotak -= ambilkartu - 4;
                    } break;
           case 10: case 11: case 12: case 13: case 14: 
           case 15: case 16: case 17: case 18: case 19: 
                    {
                      setcolor(14); gotoxy(16, 9); 
                      if (ambilkartu <= 14) {
                        printf("Whoops, anda terkena denda 100 zeny!");
                        while (denda(giliran,100, 6) != 1);
                      } else if (ambilkartu <= 17) {
                        printf("Whoops, anda terkena denda 500 zeny!");
                        while (denda(giliran,500, 6) != 1);
                      } else if (ambilkartu <= 19) {
                        printf("Whoops, anda terkena denda 1000 zeny!");
                        while (denda(giliran,1000, 6) != 1);
                      }
                      
                    } break;
           case 20: case 21: case 22: case 23: case 24:
           case 25: case 26: case 27: case 28: case 29:
                    {
                      setcolor(10); gotoxy(16, 9);
                      if (ambilkartu <= 24) {
                        printf("Selamat, anda mendapatkan bonus 100 zeny!");
                        player[giliran].zeny += 100;           
                      } else if (ambilkartu <= 27) {
                        printf("Selamat, anda mendapatkan bonus 500 zeny!");
                        player[giliran].zeny += 500;
                      } else if (ambilkartu <= 29) {
                        printf("Selamat, anda mendapatkan bonus 1000 zeny!");
                        player[giliran].zeny += 1000;       
                      } 
                    } break;
           case 30: {
                      setcolor(10); gotoxy(16,9); 
                      printf("Selamat anda mendapatkan kartu bebas parkir,");
                      gotoxy(16,10); printf("pindah ke kotak : "); 
                      int input; scanf("%d", &input); fflush(stdin);
                      if (input < 1 || input > 20) {
                        setcolor(12); gotoxy(16,11); printf("Kesempatan anda gagal !");
                        getch();
                    } else
                       player[giliran].posisi_kotak = input;
                     
                    } break;
           case 31: {
                      setcolor(10); gotoxy(16,9);
                      printf("Selamat, anda mendapatkan kartu bebas rumah ");
                      gotoxy(16,10); printf("tahanan! Simpan baik - baik jika diperlukan.");
                      player[giliran].bebas_penjara = true;
                    } break;
           case 32: {
                      setcolor(14); gotoxy(16,9);
                      masuk_penjara(giliran);
                      papan();
                    } break;
           case 33: {
                      setcolor(14); gotoxy(16,9);
                      printf("Whoops, anda harus membayar masing - masing");
                      gotoxy(16,10); printf("pemain sebesar 100 zeny.");
                      for (int i = 0; i < jml_pemain; i ++) {
                        if (i != giliran) {
                          player[i].zeny += 100;
                        }     
                      }         
                      player[giliran].zeny -= 100*(jml_pemain-1);
                     } break;
           case 34: {
                      setcolor(10); gotoxy(16,9);
                      printf("Selamat, masing - masing pemain akan membayar");
                      gotoxy(16,10); printf("anda sebesar 100 zeny.");
                      for (int i = 0; i < jml_pemain; i ++) {
                        if (i != giliran) {
                          player[i].zeny -= 100;
                        }     
                      }         
                      player[giliran].zeny += 100*(jml_pemain-1);
                     } break;
           } 
         gotoxy(25,16); printf("Tekan ENTER untuk lanjut...");
         getch();
         papan();                                                               // refresh man !
      }
      
      if (player[giliran].posisi_kotak > 20) {
        player[giliran].posisi_kotak = player[giliran].posisi_kotak - 20;
        player[giliran].putaran ++;   
        player[giliran].zeny += 1000;    
        papan();                         
      }
      
      switch(player[giliran].posisi_kotak) {
        case 6 : {
                   masuk_penjara(giliran);
                   papan();
                 } break;                             
        case 11 : {
                    gotoxy(16, 6); printf("- PARKIR BEBAS -");
                    setcolor(10); gotoxy(16,9); 
                    printf("Selamat anda mendapatkan kartu bebas parkir,");
                    gotoxy(16,10); printf("pindah ke kotak (1-20): "); 
                    int input; scanf("%d", &input); fflush(stdin);
                    if (input < 1 || input > 20) {
                      setcolor(12); gotoxy(16,11); printf("Kesempatan anda gagal !");
                      getch();
                    }
                    else
                      player[giliran].posisi_kotak = input;
                      
                    papan();
                  } break;
      }
      
      informasi_kotak (player[giliran].posisi_kotak);      
      if ((player[giliran].putaran > 0 && info_kotak->harga > 0) && (player[giliran].posisi_kotak != 1 && player[giliran].posisi_kotak != 6 && player[giliran].posisi_kotak != 11 && player[giliran].posisi_kotak != 16)) {
         informasi_kotak(player[giliran].posisi_kotak);
         if (info_kotak->pemilik_kotak == -2) {                                 // Jika kotak masih kosong / netral
           setcolor(14);
           gotoxy(16, 6); printf("- TRANSAKSI -");
           setcolor(7);
           gotoxy(16, 8); printf("# Nomor kotak\t: %d", info_kotak->nomor_kotak);
           gotoxy(16, 9); printf("# Pemilik kotak\t: Belum ada");
           gotoxy(16,10); printf("# Upgrade\t: %d", info_kotak->terakhir_upgrade);
           setcolor(10);
           gotoxy(16,12); printf("# Harga : %d zeny", info_kotak->harga);
           
           gotoxy(20,15); printf("Apakah anda mau membeli kotak ? [Y] : "); char input; scanf("%c", &input); fflush(stdin);
           if (input == 'Y' || input == 'y') {
             if (player[giliran].zeny >= info_kotak->harga) {
               info_kotak->pemilik_kotak = giliran;
               player[giliran].zeny     = player[giliran].zeny - info_kotak->harga;                        
             } else {
               setcolor(12); gotoxy(25,16); printf("Maaf, zeny anda tidak cukup :("); 
               getch();       
             }
           }
           papan();
         } else if (info_kotak->pemilik_kotak != giliran) {                     // Jika kotak telah dibeli orang
           setcolor(14); gotoxy(16,6); printf("- DENDA -");
           setcolor(7); gotoxy(16,8); printf("# Nomor kotak\t: %d", info_kotak->nomor_kotak);
           gotoxy(16,9); printf("# Pemilik kotak\t: %s", player[info_kotak->pemilik_kotak].nama);
           gotoxy(16,11); printf("# Denda : %d / 2 : %d", info_kotak->harga, (info_kotak->harga)/2);
           setcolor(10);
           informasi_kotak(giliran);
           while (denda(giliran, info_kotak->harga/2, info_kotak->pemilik_kotak) != 1);
          } else if (info_kotak->pemilik_kotak == giliran) {                 // Jika milik sendiri, maka upgrade (jika mau)
               setcolor(14); gotoxy(16,6); printf("- UPGRADE -");
               setcolor(7); gotoxy(16,8); printf("# Nomor kotak\t: %d", info_kotak->nomor_kotak);
               gotoxy(16,9); printf("# Pemilik kotak\t: %s", player[info_kotak->pemilik_kotak].nama);
               gotoxy(16,11); printf("# Harga upgrade : %d * 2 : %d", info_kotak->harga, (info_kotak->harga)*2);
               
               if (player[giliran].zeny >= (info_kotak->harga*2)) {
                 setcolor(10); gotoxy(16,13); printf("Apakah akan diupgrade ? [Y] : "); char input; scanf("%c", &input); fflush(stdin);
                 if (input == 'Y' || input == 'y') {
                   player[giliran].zeny -= info_kotak->harga*2;
                   info_kotak -> harga = info_kotak->harga*2;
                   info_kotak -> terakhir_upgrade ++;
                 }
               } else {
                 setcolor(12); gotoxy(22,16); printf("Zeny tidak cukup, ENTER untuk lewat..."); 
                 getch();
               }
             }
           papan();
      }
      
    if (acak != acak2) giliran ++;
    if (player[giliran].dipenjara == true) giliran ++;
     
  }
  system("cls");
  setcolor(10);
  gotoxy(23,10); printf("Selamat kepada %s, anda pemenangnya !",player[giliran].nama);
  getch();
}
