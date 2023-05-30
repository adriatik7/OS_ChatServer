# OS_ChatServer




OS_ChatServer është një server i thjeshtë i mesazheve i implementuar në C duke përdorur programimin e soketave (sockets) dhe multithreading. Ai lejon klientët të lidhen me serverin dhe të shkëmbejnë mesazhe me njëri-tjetrin. Serveri mbështet komanda të tilla si listimi i klientëve të lidhur, dërgimi i mesazheve private dhe shkëputja nga serveri.

## Veçoritë

- Mundësohet lidhja e shumë klientëve me serverin njëkohësisht (10 tani, por mund të ndryshohet).
- Klientët mund të dërgojnë mesazhe te të gjithë klientët e tjerë të lidhur ose privat te një marrës i caktuar.
- Serveri ruan një listë të klientëve të lidhur.
- Serveri mbështet komanda të thjeshta si listimi i klientëve të lidhur, dërgimi i mesazheve private dhe shkëputja nga serveri.


## Kërkesat

- Compiler for C (p.sh., gcc)
- Sistemi i bazuar në POSIX (Linux, Unix, macOS)

## Përdorimi

1. Kompiloni programin e serverit duke përdorur komandën vijuese:

   ```bash
   gcc chserver.c -o chserver 
   ```

2. "Run" serverin duke përdorur komandën vijuese:

   ```bash
   ./chserver
   ```

3. Serveri do të fillojë të punojë dhe të dëgjojë për lidhjet e klientëve në portën 8888 (mund të ndryshohet).

4. Klientët mund të lidhen me serverin duke compile file-n chclient dhe pastaj të bëhet run 

   ```bash
   gcc chclient.c -o chclient
   ```
   ```
   ./chclient
   ```

   Sapo të lidhur, klientët mund të fillojnë të shkëmbejnë mesazhe me klientët e tjerë të lidhur.

5. Serveri ofron komandat vijuese:

   - `/list`: Liston të gjithë klientët e lidhur në atë moment.
   - `/msg <marrës> <mesazh>`: Dërgon një mesazh privat te një marrës i caktuar.
   - `/quit`: Ndalon lidhjen me serverin.
   - `/help`: Shfaq listën e komandave të disponueshme.

   Shënim: Komandat duhet të jenë të shkruara duke filluar me një slash (" / ").

## Detajet e Implementimit

Serveri i mesazheve është i realizuar duke përdorur komponentët vijues:

- `pthread`: Serveri përdor threads POSIX për të trajtuar paralelisht lidhjet e
 shumë klientëve.
- `struct Client`: Paraqet një klient të lidhur me soketën dhe emrin e tij.
- `clients[MAX_CLIENTS]`: Një varg i strukturave `Client` për të ruajtur klientët e lidhur.
- `pthread_mutex_t`: Një mbyllëse mutex për të mbrojtur të dhënat e përbashkëta (p.sh., vargu i klientëve) gjatë qasjes paralele.
- `send_message_to_all()`: Dërgon një mesazh te të gjithë klientët e lidhur, përveç dërguesit.
- `send_private_message()`: Dërgon një mesazh privat te një klientë i caktuar.
- `handle_client()`: Loop e thred-it që merret me komunikimin me një klient, përfshirë marrjen dhe përpunimin e mesazheve.
- `main()`: Funksioni kryesor i serverit që konfiguron socket e serverit, pranon lidhjet e klientëve dhe krijon threads për të trajtuar çdo klient.

## Kontribuimi

Kontributet janë të mirëpritura! Nëse gjeni çfarëdo problemi ose keni sugjerime për përmirësim, ju lutemi hapni një çështje (issue) ose paraqisni një kërkesë për "pull request".

