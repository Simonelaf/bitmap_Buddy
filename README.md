# bitmap_Buddy
Progetto per il corso di Sistemi Operativi,
corso di Ingegneria Informatica alla Sapienza dell'a.a.2021/2022.

Implementazione di un Buddy Allocator che fa utilizzo di una BitMap per ragioni di bookkeeping.
L'allocatore funziona nel seguente modo: divide ricorsivamente a metà la memoria disponibile e alloca il più piccolo spazio libero in grado di soddisfare l’iniziale richiesta di memoria da allocare. Come esempio supponiamo di avere a disposizione un buffer di 512 kB e che arrivi una richiesta per 60 kB, la memoria verrà divisa in due partizioni da 256 kB, dopodichè poichè ancora troppo grandi una di queste verrà divisa in due ulteriori partizioni da 128 kB e poi ancora in partizioni da 64 kB; a questo punto ulteriori partizionamenti porterebbero ad avere blocchi di memoria(chiamati buddy) troppo piccoli per contenere i 60 kB quindi ci fermiamo e comunichiamo che è possibile soddisfare la richiesta.

In questa implementazione il partizionamento è astratto: la memoria totale (e le sue partizioni) possono essere quindi rappresentate mediante un albero binario, in cui ciascun nodo rappresenta un blocco di memoria, la cui dimensione dipende dal livello dell’albero in cui si trova. Maggiore il livello, maggiore la profondità dell’albero e più piccoli i blocchi di memoria che si possono allocare.

Ci avvaliamo dunque di una Bitmap per gestire la rappresentazione sottoforma di struttura ad albero binario completo della memoria, realizzata mediante un array, dove ciascuna cella rappresenta un nodo della struttura: viene assegnato il valore 0 alle celle dell’array corrispondenti a blocchi di memoria liberi e 1 per quelli allocati; questo ci aiuterà inoltre a individuare, gestire e restituire gli indirizzi dei blocchi di memoria utili soddisfare le richieste. Il blocco di memoria puntato dall'indirizzo sarà abbinato ad un nodo dell'albero e quindi ad un indice nella bitmap.<br><br>

Per associare i nodi dell'albero alla bitmap, ogni livello inizia con indice 2^(livello della bitmap) e finisce con indice (2^(livello+1)-1). 

Tramite la funzione <code>BuddyAllocator_malloc</code>, dopo un piccolo controllo iniziale per verificare che la memoria richiesta sia effettivamente disponibile, viene stabilito il livello a cui si trova la memoria da allocare, arrotondando la richiesta alla dimensione del livello più grande che può contenere tale dimensione (non è possibile allocare una zona di memoria la cui dimensione sia una via di mezzo tra le dimensioni di due livelli successivi). A questo punto, partendo dal primo indice del livello viene scandita la bitmap alla ricerca di un blocco libero (quindi con valore 0): se questo viene trovato, vengono impostati ricorsivamente i padri e i figli del nodo scelto come occupati, in quanto rappresentano blocchi fittizzi non piu disponibili; se invece non è disponibile un blocco libero si continua la ricerca ma ad un livello superiore e quindi con blocchi di taglia doppia.
A questo punto la funzione restituirà il puntatore all’area di memoria associata a tale nodo. Tuttavia, verranno sprecati i primi byte della memoria per salvarvi l’indice del nodo corrispondente.

Tramite la funzione <code>BuddyAllocator_free</code> è possibile rilasciare un blocco di memoria precedentemente occupato; a tale scopo come prima cosa viene ricavato l’indice del nodo che era stato precedentemente salvato nell'area della memoria puntata dall'indirizzo e successivamente cambia il bit corrispondente al nodo nella bit-map da 0 a 1, avendo cura di impostare di nuovo i "figli" del nodo stesso a 0 se ne ha.
A questo punto, ricorsivamente, se anche il buddy, cioè il fratello del nodo appena rilasciato è libero (cioè è uguale a 0) allora devono essere "riuniti", liberando anche il nodo genitore.

Le altre funzioni implementate servono come ausilio a quelli appena descritti e sono:

<code>set_parent</code> che imposta ricorsivamente il bit dei genitori nella bitmap

<code>set_children</code> che imposta ricorsivamente il bit dei figli nella bitmap


