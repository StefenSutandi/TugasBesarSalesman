Seorang pedagang ingin menjual barang dagangannya ke sejumlah kota. Untuk menghemat waktu dan uang, dia perlu menentukan rute perjalanan terpendek yang membawanya mengunjungi setiap kota dalam daftar tujuannya tepat satu kali dan kembali ke kota keberangkatannya.
Masalah ini dikenal sebagai the Travelling Salesman Problem (TSP). Dari himpunan kota beserta lokasinya dan sebuah kota keberangkatan, anda ditugaskan mencari rute perjalanan terpendek yang melalui setiap kota tepat satu kali dan kembali ke kota keberangkatan. Sebagai contoh, bila kita berada di kota Bandung, dan ingin pergi ke Bali, Semarang, dan Surabaya. Maka, rute perjalanan terpendeknya adalah Bandung-Semarang-Surabaya-Bali-Bandung.

Pemecahan Masalah pada soal ini dapat diselesaikan dengan beberapa metode alternatif :
1. Algoritma Greedy
2. Algoritma Brute Force (Exhaustive Search)
3. Breadth First Search (BFS)
4. Depth First Search (DFS)
5. Algoritma Branch and Bound
6. Algoritma Genetika
7. Algoritma Ant Colony Optimization
8. Algoritma Particle Swarm Optimization

Pada soal ini, rute yang dihasilkan diharapkan untuk memenuhi sifat berikut :
1. Memberikan rute yang memenuhi syarat : Setiap kota dikunjungi tepat satu kali, tidak kurang ataupun lebih, kecuali kota keberangkatan. Kemudian, Rute diawali dan diakhiri di kota keberangkatan.
2. Memberikan rute dengan jarak total terpendek dari seluruh kemungkinan rute yang ada.
3. Kota yang dipilih adalah kota – kota yang terdapat di wilayah Indonesia
4. Program dapat digunakan untuk mencari rute untuk minimal 6 kota dan maksimal 15 kota.
